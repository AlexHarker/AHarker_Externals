
#ifndef __PATCHSLOT__
#define __PATCHSLOT__

#include <ext.h>
#include <z_dsp.h>

#include <string>


class PatchSlot
{
    
private:
 
    enum { MAX_ARGS = 16 };
 
public:
    
    enum LoadError { kNone, kFileNotFound, kNothingLoaded, kNotPatcher };
    
public:
    
    PatchSlot();
    ~PatchSlot();

    LoadError load(t_object *x, long displayIndex, t_symbol *path, long argc, t_atom *argv, long vecSize, long samplingRate);
    LoadError load(t_object *x, long vecSize, long samplingRate);
    void compileDSP(long vecSize, long samplingRate, bool forceWhenInvalid = false);
    bool process(void *tempMem, void **outputs, t_ptr_uint tempMemSize);
    
    t_patcher *getPatch() { return mPatch; }
    bool getValid() { return mValid; }
    bool getOn() { return mOn; }
    bool getBusy() { return mBusy; }
    void ***getOutputsHandle() { return &mOutputs; }
    void **getTempMemHandle() { return &mTempMem; }
    void setOn(bool on) { mOn = on; }
    void setBusy(bool busy) { mBusy = busy; }
    void setInvalid() { mValid = false; }
    void setTempMemSize(t_ptr_uint size) { mTempMemSize = size; }
    
    bool isEmpty() { return mPatch == NULL; }

    void openWindow();
    void closeWindow();

private:
    
    void setWindowName();
    void freePatch();

    // Patch and variables / dspchain
    
    t_patcher *mPatch;
    t_symbol *mPathSymbol;
    std::string mName;
    short mPath;
    t_dspchain *mDSPChain;
    long mDisplayIndex;
    
    // Arguments (stored in case of reload / update)
    
    long mArgc;
    t_atom mArgv[MAX_ARGS];
    
    // Flags
    
    bool mValid;
    bool mOn;
    bool mBusy;
    
    // Pointer to Array of Audio Out Buffers (which are thread dependent)
    
    void **mOutputs;
    
    // Temporary Memory Variables
    
    t_ptr_uint mTempMemSize;
    void *mTempMem;
};


#endif /* defined(__PATCHSLOT__) */
