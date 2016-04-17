
#include "PatchSlot.h"

#include <algorithm>
#include <jpatcher_api.h>

// Set subpatcher association

int set_sub_assoc(t_patcher *p, t_object *x)
{
    t_object *assoc;
    
    object_method(p, gensym("getassoc"), &assoc);
    if (!assoc)
        object_method(p, gensym("setassoc"), x);
    
    return 0;
}

// Blank PatchSlot Contructor

PatchSlot::PatchSlot() : mPatch(NULL), mPathSymbol(NULL), mPath(0), mDSPChain(NULL), mDisplayIndex(0), mArgc(0), mValid(false), mOn(false), mBusy(false), mOutputs(NULL), mTempMemSize(0), mTempMem(NULL) {}

// Loading PatchSlot Contructor

PatchSlot::LoadError PatchSlot::load(t_object *x, long displayIndex, t_symbol *fileName, long argc, t_atom *argv, long vecSize, long samplingRate)
{
    // Copy Arguments
    
    mPathSymbol = fileName;
    mName = std::string(fileName->s_name);
    mPath = 0;
    mDisplayIndex = displayIndex;
    mArgc = std::min(argc, (long) MAX_ARGS);
    if (mArgc)
        memcpy(mArgv, argv, mArgc * sizeof(t_atom));
    
    // Load
    
    return load(x, vecSize, samplingRate);
}

PatchSlot::LoadError PatchSlot::load(t_object *x, long vecSize, long samplingRate)
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
    
    // Load the patch (don't interrupt dsp and set to allow Modify Read-Only)
    
    short save_loadupdate = dsp_setloadupdate(false);
    loadbang_suspend();
    mPatch = (t_patcher *) intload(name, mPath, 0 , mArgc, mArgv, false);
    object_method(mPatch, gensym("setclass"));
    loadbang_resume();
    dsp_setloadupdate(save_loadupdate);
    
    // Check something has loaded and that it is a patcher
    
    if (!mPatch)
        return kNothingLoaded;
    
    if (!ispatcher((t_object *)mPatch))
    {
        freePatch();
        return kNotPatcher;
    }
    
    // Set window name / set associations / compile DSP if the owning patch is on
    
    setWindowName();
    long result = 0;
    object_method(mPatch, gensym("traverse"), set_sub_assoc, x, &result);
    
    if (sys_getdspobjdspstate(x))
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

bool PatchSlot::process(void *tempMem, void **outputs, t_ptr_uint tempMemSize)
{
    if (mDSPChain && mOn && mValid && (tempMemSize >= mTempMemSize))
    {
        mOutputs = outputs;
        mTempMem = tempMem;
    
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
        // Hopefully c74 will fix it, but this code needs to remain for old versions of Max
        
        if (mDSPChain && mDSPChain->c_broken)
        {
            if (mDSPChain)
                freeobject((t_object *)mDSPChain);
            
            mDSPChain = dspchain_compile(mPatch, vecSize, samplingRate);
        }
    }
}

void PatchSlot::setWindowName()
{
    char indexString[16];
    
    snprintf(indexString, 15, " (%ld)", mDisplayIndex);
    
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

void PatchSlot::freePatch()
{
    if (mDSPChain)
        freeobject((t_object *)mDSPChain);

    if (mPatch)
    {
        closeWindow();
        freeobject((t_object *)mPatch);
    }
    
    mPatch = NULL;
    mDSPChain = NULL;
}

