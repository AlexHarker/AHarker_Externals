
#include "PatchSlot.h"

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
    t_fourcc valid_types[3];
    
    // Set the valid types to test for
    
    valid_types[0] = FOUR_CHAR_CODE('maxb');
    valid_types[1] = FOUR_CHAR_CODE('TEXT');
    valid_types[2] = FOUR_CHAR_CODE('JSON');
    
    // Set all flags off / free old patch
    
    mValid = mOn = mBusy = false;
    freePatch();
    
    // Try to locate a file of the given name that is of the correct type

    // FIX - make this nicer
    
    char name[512];
    strcpy(name, mName.c_str());
    
    if (locatefile_extended(name, &mPath, &type, valid_types, 3))
        return kFileNotFound;
    
    // FIX - this is a stop gap (needs better attention to figure out how to do this...)
    
    t_symbol *ps_dynamicdsp = gensym("___DynamicDSP~___");
    t_symbol *ps_patch_index = gensym("___DynamicPatchIndex___");
    t_symbol *ps_inhibit_subpatcher_vis = gensym("inhibit_subpatcher_vis");
    //t_symbol *ps_PAT = gensym("#P");
    
    // Store the old loading symbols
    
    t_object *previous = ps_dynamicdsp->s_thing;
    t_object *previous_index = ps_patch_index->s_thing;
    t_object *save_inhibit_state = ps_inhibit_subpatcher_vis->s_thing;
    //t_patcher *saveparent = (t_patcher *)ps_PAT->s_thing;

    // Bind to the loading symbols

    ps_dynamicdsp->s_thing = mOwner;
    ps_patch_index->s_thing = (t_object *) mUserIndex;
    ps_inhibit_subpatcher_vis->s_thing = (t_object *) -1;
    //ps_PAT->s_thing = (t_object *) x->parent_patch;

    // Load the patch (don't interrupt dsp and set to allow Modify Read-Only)
    
    short save_loadupdate = dsp_setloadupdate(false);
    loadbang_suspend();
    mPatch = (t_patcher *)intload(name, mPath, 0 , mArgc, mArgv, false);
    object_method(mPatch, gensym("setclass"));
    loadbang_resume();
    dsp_setloadupdate(save_loadupdate);
    
    // Restore previous loading symbol bindings
    
    ps_dynamicdsp->s_thing = previous;
    ps_patch_index->s_thing = previous_index;
    ps_inhibit_subpatcher_vis->s_thing = save_inhibit_state;
    //ps_PAT->s_thing = (t_object *) saveparent;
    
    // Check something has loaded and that it is a patcher
    
    if (!mPatch)
        return kNothingLoaded;
    
    if (!ispatcher((t_object *)mPatch))
    {
        freePatch();
        return kNotPatcher;
    }
    
    // Find ins and link outs
    
    patcherTraverse<&PatchSlot::findIns>(mPatch, NULL, mOwner);
    patcherTraverse<&PatchSlot::linkOutlets>(mPatch, NULL, mOwner);
    
    // Set window name / set associations / compile DSP if the owning patch is on
    
    setWindowName();
    long result = 0;
    object_method(mPatch, gensym("traverse"), setSubAssoc, mOwner, &result);
    
    if (sys_getdspobjdspstate(mOwner))
        compileDSP(vecSize, samplingRate, true);
    
    // The patch is valid and ready to go
    
    mOn = true;
    mValid = true;
    
    return kNone;
}

PatchSlot::~PatchSlot()
{
    freePatch();
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

bool PatchSlot::process(void *tempMem, void **outputs, t_ptr_uint tempMemSize)
{
    if (checkProcess(tempMem, outputs, tempMemSize))
    {
        processTick(tempMem, outputs);
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

void PatchSlot::openWindow()
{
    if (mPatch)
        mess0((t_object *)mPatch, gensym("front"));
}

void PatchSlot::closeWindow()
{
    if (mPatch)
        object_method(mPatch, gensym("wclose"));
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
        if (jbox_get_maxclass(b)  == ps_out)
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
        freeobject((t_object *)mPatch);
    }
    
    mPatch = NULL;
    mDSPChain = NULL;
}

// ThreadedPatchSlot Additions

bool ThreadedPatchSlot::processIfUnprocessed(void *tempMem, void **outputs, t_ptr_uint tempMemSize)
{
    if (PatchSlot::checkProcess(tempMem, outputs, tempMemSize) && checkProcess())
    {
        processTick(tempMem, outputs);
        return true;
    }
    
    return false;
}

bool ThreadedPatchSlot::processIfThreadMatches(void *tempMem, void **outputs, t_ptr_uint tempMemSize, long thread, long availableThreads)
{
    if ((mThreadCurrent % availableThreads) == thread)
        return process(tempMem, outputs, tempMemSize);
    
    return false;
}
