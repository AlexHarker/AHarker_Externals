
#include "PatchSlot.hpp"

#include <algorithm>
#include <jpatcher_api.h>


#ifndef __APPLE__
#define snprintf _snprintf
#endif

struct SymbolBinding
{
    SymbolBinding(const char *str, void *item) : mSymbol(gensym(str)), mPrevious(mSymbol->s_thing)
    {
        mSymbol->s_thing = reinterpret_cast<t_object *>(item);
    }
    
    ~SymbolBinding()
    {
        mSymbol->s_thing = mPrevious;
    }
    
    t_symbol *mSymbol;
    t_object *mPrevious;
};

// Deferred patch deletion

void deletePatch(t_object *patch, t_symbol *s, short argc, t_atom *argv)
{
    freeobject(patch);
}

// Set subpatcher association

int setSubpatcherAssoc(t_patcher *p, t_object *x)
{
    t_object *assoc;
    
    object_method(p, gensym("getassoc"), &assoc);
    if (!assoc)
        object_method(p, gensym("setassoc"), x);
    
    return 0;
}

// Patcher Traversal

template <void (PatchSlot::*Method)(t_patcher *p)>
int PatchSlot::patcherTraverse(t_patcher *p, void *arg, t_object *owner)
{
    // Avoid recursion into a poly / pfft / dynamicdsp~
    
    t_object *assoc = 0;
    object_method(p, gensym("getassoc"), &assoc);
    if (assoc && assoc != owner)
        return 0;
    
    // Call function
    
    (this->*Method)(p);
    
    // Continue search for subpatchers and traverse them if found
    
    for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
    {
        t_patcher *p2;
        long index = 0;
        
        while (b && (p2 = reinterpret_cast<t_patcher *>(object_subpatcher(jbox_get_object(b), &index, arg))))
            if (patcherTraverse<Method>(p2, arg, owner))
                return 1;
    }
    
    return 0;
}

PatchSlot::~PatchSlot()
{
    freePatch();
}

// Loading

PatchSlot::LoadError PatchSlot::load(t_symbol *path, long argc, t_atom *argv, long vecSize, long samplingRate)
{
    // Copy Arguments
    
    mPathSymbol = path;
    mName = std::string(path->s_name);
    mPath = 0;
    mArgc = std::min(argc, (long) MAX_ARGS);
    
    std::copy_n(argv, mArgc, mArgv);
    
    // Load
    
    return load(vecSize, samplingRate, true);
}

PatchSlot::LoadError PatchSlot::load(long vecSize, long samplingRate, bool initialise)
{
    t_fourcc type;
    t_fourcc validTypes[3];
    
    // Set the valid types to test for
    
    validTypes[0] = FOUR_CHAR_CODE('maxb');
    validTypes[1] = FOUR_CHAR_CODE('TEXT');
    validTypes[2] = FOUR_CHAR_CODE('JSON');
    
    // Set flags off / free old patch / set on ready to be changed at load time
    
    bool on = initialise ? true : mOn;
    mValid = mBusy = mOn = false;
    freePatch();
    mOn = on;
    
    // Try to locate a file of the given name that is of the correct type
    
    char name[512];
    strcpy(name, mName.c_str());
    
    if (locatefile_extended(name, &mPath, &type, validTypes, 3))
        return kFileNotFound;
    
    // Store the binding symbols with RAII to restore once done
    
    SymbolBinding owner("__dynamic.host_object__", mOwner);
    SymbolBinding vis("inhibit_subpatcher_vis", reinterpret_cast<void *>(-1));
    SymbolBinding PAT("#P", mParent);

    // Load the patch (don't interrupt dsp and use )
    
    short savedLoadUpdate = dsp_setloadupdate(false);
    loadbang_suspend();
    mPatch = reinterpret_cast<t_patcher *>(intload(name, mPath, 0 , mArgc, mArgv, false));

    // Check something has loaded and that it is a patcher
    
    if (!mPatch)
        return loadFinished(kNothingLoaded, savedLoadUpdate);
    
    if (!ispatcher(reinterpret_cast<t_object *>(mPatch)))
        return loadFinished(kNotPatcher, savedLoadUpdate);

    // Use setclass to allow Modify Read-Only / Set associations
    
    long result = 0;
    object_method(mPatch, gensym("setclass"));
    object_method(mPatch, gensym("traverse"), setSubpatcherAssoc, mOwner, &result);

    // Find ins and link outs
    
    patcherTraverse<&PatchSlot::findIns>(mPatch, nullptr, mOwner);
    patcherTraverse<&PatchSlot::linkOutlets>(mPatch, nullptr, mOwner);
    
    // Set window name and compile DSP if the owning patch is on
    
    setWindowName();
    
    if (sys_getdspobjdspstate(mOwner))
        compileDSP(vecSize, samplingRate, true);
    
    // Finish loading (which fires loadbang and sets the valid flag)
    
    return loadFinished(kNone, savedLoadUpdate);
}

PatchSlot::LoadError PatchSlot::loadFinished(LoadError error, short savedLoadUpdate)
{
    loadbang_resume();
    dsp_setloadupdate(savedLoadUpdate);
    mValid = error == kNone ? true : false;
    if (!mValid)
        freePatch();
    return error;
}

// Messages

void PatchSlot::message(long inlet, t_symbol *msg, long argc, t_atom *argv)
{
    if (inlet < 0 || inlet >= getNumIns())
        return;
    
    for (std::vector<void *>::iterator it = mInTable[inlet].begin(); it != mInTable[inlet].end(); it++)
        outlet_anything(*it, msg, argc, argv);
}

// Processing and DSP

bool PatchSlot::process(void **outputs)
{
    if (mDSPChain && mValid && mOn)
    {
        mOutputs = outputs;
        dspchain_tick(mDSPChain);
        return true;
    }
    
    return false;
}

void PatchSlot::compileDSP(long vecSize, long samplingRate, bool forceWhenInvalid)
{
    if (mValid || forceWhenInvalid)
    {
        // Free the old dspchain
    
        if (mDSPChain)
            freeobject(reinterpret_cast<t_object *>(mDSPChain));
    
        // Recompile
    
        mDSPChain = dspchain_compile(mPatch, vecSize, samplingRate);
        
        // This won't work for gen~ the first time round, so we have to check and compile again
        // Hopefully c74 will sort it, but this code needs to remain for old versions of Max
        
        if (mDSPChain && mDSPChain->c_broken)
        {
            if (mDSPChain)
                freeobject(reinterpret_cast<t_object *>(mDSPChain));
            
            mDSPChain = dspchain_compile(mPatch, vecSize, samplingRate);
        }
    }
}

// Window Management

void PatchSlot::setWindowName()
{
    char indexString[16];
    
    snprintf(indexString, 15, " (%ld)", mUserIndex);
    
    std::string windowName = std::string(mName);
    windowName.append(indexString);
    jpatcher_set_title(mPatch, gensym(windowName.c_str()));
}

void PatchSlot::openWindow() const
{
    if (mPatch)
        object_method(mPatch, gensym("front"));
}

void PatchSlot::closeWindow() const
{
    if (mPatch)
        object_method(mPatch, gensym("wclose"));
}

// Error strings

const char *PatchSlot::getError(LoadError error)
{
    switch (error)
    {
        case kNone:             return "";
        case kFileNotFound:     return "file not found";
        case kNothingLoaded:    return "nothing was loaded";
        case kNotPatcher:       return "file is not a patcher";
    }
}

// Inlet linking and unlinking

void PatchSlot::handleIO(t_patcher *p, const char *type, long maxIndex, std::function<void(IO *, long)> func)
{
    t_symbol *ps_io = gensym(type);
    
    for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
    {
        if (jbox_get_maxclass(b) == ps_io)
        {
            IO *io = reinterpret_cast<IO *>(jbox_get_object(b));
            long index = io->s_index - 1;
            if (index >= 0 && index < maxIndex)
                func(io, index);
        }
    }
}

void PatchSlot::findIns(t_patcher *p)
{
    auto store = [&](IO *io, long index) { mInTable[index].push_back(io->s_obj.o_outlet); };

    handleIO(p, "in", getNumIns(), store);
}

void PatchSlot::linkOutlets(t_patcher *p)
{
    auto link = [&](IO *io, long index) { outlet_add(io->s_outlet, (*mOutTable)[index]); };
    
    handleIO(p, "out", getNumOuts(), link);
}

void PatchSlot::unlinkOutlets(t_patcher *p)
{
    auto unlink = [&](IO *io, long index) { outlet_rm(io->s_outlet, (*mOutTable)[index]); };
    
    handleIO(p, "out", getNumOuts(), unlink);
}

// Free patch

void PatchSlot::freePatch()
{
    if (mDSPChain)
        freeobject(reinterpret_cast<t_object *>(mDSPChain));

    if (mPatch)
    {
        // Remove pointers to ins, unlink outlets, close window and free the patch
        
        for (std::vector< std::vector<void *> >::iterator it = mInTable.begin(); it != mInTable.end(); it++)
            it->clear();
        patcherTraverse<&PatchSlot::unlinkOutlets>(mPatch, NULL, mOwner);
        closeWindow();

        // Defer patch deletion for speed and safety
        
        defer(mPatch, (method) deletePatch, NULL, 0, NULL);
    }
    
    mPatch = nullptr;
    mDSPChain = nullptr;
}
