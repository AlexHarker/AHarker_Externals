
#ifndef __PATCHSLOT__
#define __PATCHSLOT__

#include <ext.h>
#include <z_dsp.h>

#include <string>
#include <vector>

#include <AH_Atomic.h>


// Simple class

class PatchSlot
{
    
private:
 
    enum { MAX_ARGS = 16 };
 
public:
    
    enum LoadError { kNone, kFileNotFound, kNothingLoaded, kNotPatcher };
    
public:
    
    PatchSlot(t_object *owner, long numIns, std::vector<void *> *outTable) : mPatch(NULL), mPathSymbol(NULL), mPath(0), mDSPChain(NULL), mUserIndex(0), mArgc(0), mValid(false), mOn(false), mBusy(false), mOutputs(NULL), mTempMemSize(0), mTempMem(NULL), mOutTable(outTable), mOwner(owner)
    {
        mInTable.resize(numIns);
    }
    ~PatchSlot();
    
    LoadError load(long userIndex, t_symbol *path, long argc, t_atom *argv, long vecSize, long samplingRate);
    LoadError load(long vecSize, long samplingRate);
    
    void message(long inlet, t_symbol *msg, long argc, t_atom *argv);
    
    void compileDSP(long vecSize, long samplingRate, bool forceWhenInvalid = false);
    bool process(void *tempMem, void **outputs, t_ptr_uint tempMemSize);
    
    // Getters
   
    bool isEmpty() { return mPatch == NULL; }
    
    t_patcher *getPatch() { return mPatch; }
    bool getValid() { return mValid; }
    bool getOn() { return mOn; }
    bool getBusy() { return mBusy; }
    void ***getOutputsHandle() { return &mOutputs; }
    void **getTempMemHandle() { return &mTempMem; }
    
    // Setters
    
    void setOn(bool on) { mOn = on; }
    void setBusy(bool busy) { mBusy = busy; }
    void setInvalid() { mValid = false; }
    void setTempMemSize(t_ptr_uint size) { mTempMemSize = size; }

    // Number of ins and outs
    
    long getNumIns() { return mInTable.size(); }
    long getNumOuts() { return mOutTable->size(); }

    // Windows Managment
    
    void openWindow();
    void closeWindow();

protected:
    
    inline bool checkProcess(void *tempMem, void **outputs, t_ptr_uint tempMemSize)
    {
        return (mDSPChain && mOn && mValid && (tempMemSize >= mTempMemSize));
    }
    
    void processTick(void *tempMem, void **outputs)
    {
        mOutputs = outputs;
        mTempMem = tempMem;
        dspchain_tick(mDSPChain);
    }
    
private:
    
    template <void (PatchSlot::*Method)(t_patcher *p)> int patcherTraverse(t_patcher *p, void *arg, t_object *owner)
    {
        // Avoid recursion into a poly / pfft / dynamicdsp~
     
        t_object *assoc = 0;
        object_method(p, gensym("getassoc"), &assoc);
        if (assoc && assoc != owner)
            return 0;

        // Call function
        
        (this->*Method)(p);
        
        // Continue search for subpatchers
        
        for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
        {
            t_patcher *p2;
            long index = 0;
            
            while (b && (p2 = (t_patcher *)object_subpatcher(jbox_get_object(b), &index, arg)))
                if (patcherTraverse<Method>(p2, arg, owner))
                    return 1;
        }
        
        return 0;
    }
    
    void findIns(t_patcher *p);
    void linkOutlets(t_patcher *p);
    void unlinkOutlets(t_patcher *p);

    void setWindowName();
    void freePatch();

    // Patch and variables / dspchain
    
    t_patcher *mPatch;
    t_symbol *mPathSymbol;
    std::string mName;
    short mPath;
    t_dspchain *mDSPChain;
    long mUserIndex;
    
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
    
    // Inlet and Outlet Communication
    
    std::vector< std::vector<void *> > mInTable;
    std::vector<void *> *mOutTable;
    
    // Owner
    
    t_object *mOwner;
};


// Class with threading additions

class ThreadedPatchSlot : public PatchSlot
{
    
public:
    
    ThreadedPatchSlot(t_object *owner, long numIns, std::vector<void *> *outTable) : PatchSlot(owner, numIns, outTable), mProcessingFlag(0), mThreadCurrent(0), mThreadRequest(0)   {}
    
    void requestThread(long thread) { mThreadRequest = thread; }
    void updateThread() { mThreadCurrent = mThreadRequest; }
    void resetProcessed() { mProcessingFlag = 0; }
    
    bool processIfUnprocessed(void *tempMem, void **outputs, t_ptr_uint tempMemSize);
    bool processIfThreadMatches(void *tempMem, void **outputs, t_ptr_uint tempMemSize, long thread, long availableThreads);

private:

    bool checkProcess() { return Atomic_Compare_And_Swap(0, 1, &mProcessingFlag); }

    // Threading Variables
    
    t_int32_atomic mProcessingFlag;
    long mThreadCurrent;
    long mThreadRequest;
};

#endif /* defined(__PATCHSLOT__) */
