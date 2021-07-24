
#ifndef _PATCHSLOT_H_
#define _PATCHSLOT_H_

#include <ext.h>
#include <z_dsp.h>

#include <functional>
#include <string>
#include <vector>

#include <AH_Locks.hpp>


// PatchSlot Class

class PatchSlot
{
    // Generic in/out structure
    
    struct IO
    {
        t_object s_obj;
        
        long s_index;
        void *s_outlet;
    };
    
    enum { MAX_ARGS = 16 };
 
public:
    
    enum LoadError { kNone, kFileNotFound, kNothingLoaded, kNotPatcher };
    
    PatchSlot(t_object *owner, t_object *parent, t_atom_long index, long numIns, std::vector<void *> *outTable)
    : mPatch(nullptr), mDSPChain(nullptr), mPathSymbol(nullptr)
    , mPath(0), mUserIndex(index), mArgc(0)
    , mValid(false), mOn(false), mBusy(false)
    , mOutputs(nullptr), mOutTable(outTable)
    , mOwner(owner), mParent(parent)
    {
        mInTable.resize(numIns);
    }
    
    ~PatchSlot();
    
    LoadError load(t_symbol *path, long argc, t_atom *argv, long vecSize, long samplingRate);
    LoadError load(long vecSize, long samplingRate, bool initialise);
    
    void message(long inlet, t_symbol *msg, long argc, t_atom *argv);
    
    void compileDSP(long vecSize, long samplingRate, bool forceWhenInvalid);
    bool process(void **outputs);
    
    // Getters
       
    t_patcher *getPatch() const         { return mPatch; }
    t_atom_long getUserIndex() const    { return mUserIndex; }
    bool getValid() const               { return mValid; }
    bool getOn() const                  { return mOn; }
    bool getBusy() const                { return mBusy; }
    void ***getOutputHandle()           { return &mOutputs; }
    
    // Setters
    
    void setOn(bool on)         { mOn = on; }
    void setBusy(bool busy)     { mBusy = busy; }
    void setInvalid()           { mValid = false; }

    // Number of ins and outs
    
    long getNumIns() const      { return mInTable.size(); }
    long getNumOuts() const     { return mOutTable->size(); }

    // Windows Management
    
    void openWindow() const;
    void closeWindow() const;

    // Error string

    static const char *getError(LoadError error);
    
private:
    
    template <void (PatchSlot::*Method)(t_patcher *p)>
    int patcherTraverse(t_patcher *p, void *arg, t_object *owner);
    
    LoadError loadFinished(LoadError error, short savedLoadUpdate);
    
    void setWindowName();
    void freePatch();
    
    void handleIO(t_patcher *p, const char *type, long maxIndex, std::function<void(IO*, long)> func);
    
    void findIns(t_patcher *p);
    void linkOutlets(t_patcher *p);
    void unlinkOutlets(t_patcher *p);

    // Patch and variables / dspchain
    
    t_patcher *mPatch;
    t_dspchain *mDSPChain;
    t_symbol *mPathSymbol;
    std::string mName;
    short mPath;
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
    
    // Inlet and Outlet Communication
    
    std::vector<std::vector<void *>> mInTable;
    std::vector<void *> *mOutTable;
    
    // Owner
    
    t_object *mOwner;
    t_patcher *mParent;
};


// Class with threading additions

class ThreadedPatchSlot : public PatchSlot
{
    
public:
    
    ThreadedPatchSlot(t_object *owner, t_patcher *parent, t_atom_long index, long numIns, std::vector<void *> *outTable)
    : PatchSlot(owner, parent, index, numIns, outTable), mThreadCurrent(0), mThreadRequest(0) {}
    
    void requestThread(long thread)     { mThreadRequest = thread; }
    void updateThread()                 { mThreadCurrent = mThreadRequest; }
    void resetProcessed()               { mProcessingLock.release(); }
    
    bool processIfUnprocessed(void **outputs)
    {
        return mProcessingLock.attempt() ? process(outputs) : false;
    }
    
    bool processIfThreadMatches(void **outputs, long thread, long nThreads)
    {
        return ((mThreadCurrent % nThreads) == thread) ? process(outputs) : false;
    }

private:

    // Threading Variables
    
    thread_lock mProcessingLock;
    long mThreadCurrent;
    long mThreadRequest;
};

#endif  /* _PATCHSLOT_H_ */
