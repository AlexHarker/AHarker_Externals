
#include "PatchSlot.hpp"

#include <algorithm>
#include <jpatcher_api.h>


#ifndef __APPLE__
#define snprintf _snprintf
#endif


// Generic in/out structure

struct IO
{
    t_object s_obj;
    
    long s_index;
    void *s_outlet;
    
};

// Deferred patch deletion

void deletePatch(t_object *patch, t_symbol *s, short argc, t_atom *argv)
{
    freeobject(patch);
}

// Set subpatcher association

// FIX - could make this use inbuilt traverse (and hence part of class)

int setSubAssoc(t_patcher *p, t_object *x)
{
    t_object *assoc;
    
    object_method(p, gensym("getassoc"), &assoc);
    if (!assoc)
        object_method(p, gensym("setassoc"), x);
    
    return 0;
}

PatchSlot::~PatchSlot()
{
    freePatch();
}

// Loading

PatchSlot::LoadError PatchSlot::load(long userIndex, t_symbol *fileName, long argc, t_atom *argv, long vecSize, long samplingRate)
{
    // Copy Arguments
    
    mPathSymbol = fileName;
    mName = std::string(fileName->s_name);
    mPath = 0;
    mUserIndex = userIndex;
    mArgc = std::min(argc, (long) MAX_ARGS);
    if (mArgc)
        memcpy(mArgv, argv, mArgc * sizeof(t_atom));
    
    // Load
    
    return load(vecSize, samplingRate);
}

PatchSlot::LoadError PatchSlot::load(long vecSize, long samplingRate)
{
    t_fourcc type;
    t_fourcc validTypes[3];
    
    // Set the valid types to test for
    
    validTypes[0] = FOUR_CHAR_CODE('maxb');
    validTypes[1] = FOUR_CHAR_CODE('TEXT');
    validTypes[2] = FOUR_CHAR_CODE('JSON');
    
    // Set flags off / free old patch / set on ready to be changed at load time
    
    mValid = mBusy = mOn = false;
    freePatch();
    mOn = true;
    
    // Try to locate a file of the given name that is of the correct type

    // FIX - make this nicer
    
    char name[512];
    strcpy(name, mName.c_str());
    
    if (locatefile_extended(name, &mPath, &type, validTypes, 3))
        return kFileNotFound;
        
    t_symbol *ps_dynamicdsp = gensym("___DynamicDSP~___");
    t_symbol *ps_patch_index = gensym("___DynamicPatchIndex___");
    t_symbol *ps_inhibit_subpatcher_vis = gensym("inhibit_subpatcher_vis");
    t_symbol *ps_PAT = gensym("#P");
    
    // Store the old loading symbols
    
    t_object *previous = ps_dynamicdsp->s_thing;
    t_object *previous_index = ps_patch_index->s_thing;
    t_object *save_inhibit_state = ps_inhibit_subpatcher_vis->s_thing;
    t_patcher *saveparent = (t_patcher *)ps_PAT->s_thing;

    // Bind to the loading symbols

    ps_dynamicdsp->s_thing = mOwner;
    ps_patch_index->s_thing = (t_object *) mUserIndex;
    ps_inhibit_subpatcher_vis->s_thing = (t_object *) -1;
    ps_PAT->s_thing = (t_object *)mParent;

    // Load the patch (don't interrupt dsp and use setclass to allow Modify Read-Only)
    
    short savedLoadUpdate = dsp_setloadupdate(false);
    loadbang_suspend();
    mPatch = (t_patcher *)intload(name, mPath, 0 , mArgc, mArgv, false);
    object_method(mPatch, gensym("setclass"));
    
    // Restore previous loading symbol bindings
    
    ps_dynamicdsp->s_thing = previous;
    ps_patch_index->s_thing = previous_index;
    ps_inhibit_subpatcher_vis->s_thing = save_inhibit_state;
    ps_PAT->s_thing = (t_object *) saveparent;
    
    // Check something has loaded and that it is a patcher
    
    if (!mPatch)
        return loadFinished(kNothingLoaded, savedLoadUpdate);
    
    if (!ispatcher((t_object *)mPatch))
    {
        freePatch();
        return loadFinished(kNotPatcher, savedLoadUpdate);
    }
    
    // Find ins and link outs
    
    patcherTraverse<&PatchSlot::findIns>(mPatch, NULL, mOwner);
    patcherTraverse<&PatchSlot::linkOutlets>(mPatch, NULL, mOwner);
    
    // Set window name / set associations / compile DSP if the owning patch is on
    
    long result = 0;
    setWindowName();
    object_method(mPatch, gensym("traverse"), setSubAssoc, mOwner, &result);
    
    if (sys_getdspobjdspstate(mOwner))
        compileDSP(vecSize, samplingRate, true);
    
    // Loadbang before turning the patch on
    
    loadFinished(kNone, savedLoadUpdate);
    
    // The patch is valid and ready to go
    
    mValid = true;
    
    return kNone;
}

PatchSlot::LoadError PatchSlot::loadFinished(LoadError error, short savedLoadUpdate)
{
    loadbang_resume();
    dsp_setloadupdate(savedLoadUpdate);
    
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
            freeobject((t_object *)mDSPChain);
    
        // Recompile
    
        mDSPChain = dspchain_compile(mPatch, vecSize, samplingRate);
        
        // This won't work for gen~ the first time round, so we have to check and compile again
        // Hopefully c74 will sort it, but this code needs to remain for old versions of Max
        
        if (mDSPChain && mDSPChain->c_broken)
        {
            if (mDSPChain)
                freeobject((t_object *)mDSPChain);
            
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

void PatchSlot::findIns(t_patcher *p)
{
    t_symbol *ps_in = gensym("in");
    
    for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
    {
        if (jbox_get_maxclass(b) == ps_in)
        {
            IO *io = (IO *)jbox_get_object(b);
            long inlet = io->s_index - 1;
            if (inlet >= 0 && inlet < getNumIns())
                mInTable[inlet].push_back(io->s_obj.o_outlet);
        }
    }
}

void PatchSlot::linkOutlets(t_patcher *p)
{
    t_symbol *ps_out = gensym("out");
    
    for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
    {
        if (jbox_get_maxclass(b) == ps_out)
        {
            IO *io = (IO *)jbox_get_object(b);
            long inlet = io->s_index - 1;
            if (inlet >= 0 && inlet < getNumOuts())
                outlet_add(io->s_outlet, (*mOutTable)[io->s_index - 1]);
        }
    }
}

void PatchSlot::unlinkOutlets(t_patcher *p)
{
    t_symbol *ps_out = gensym("out");
    
    for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
    {
        if (jbox_get_maxclass(b) == ps_out)
        {
            IO *io = (IO *)jbox_get_object(b);
            long inlet = io->s_index - 1;
            if (inlet >= 0 && inlet < getNumOuts())
                outlet_rm(io->s_outlet, (*mOutTable)[io->s_index - 1]);
        }
    }
}

void PatchSlot::freePatch()
{
    if (mDSPChain)
        freeobject((t_object *)mDSPChain);

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
