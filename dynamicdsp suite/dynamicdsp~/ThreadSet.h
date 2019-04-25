
#ifndef __THREADSET__
#define __THREADSET__

#include <vector>

#include <ext.h>

#include <AH_Atomic.h>
#include <SIMDSupport.hpp>
#include <stdint.h>

#ifdef __APPLE__

#include <libkern/OSAtomic.h>
#include <pthread.h>
#include <mach/semaphore.h>
#include <mach/task.h>

// Mac OS specific definitions

typedef pthread_t OSThreadType;
typedef semaphore_t OSSemaphoreType;
typedef void *OSThreadFunctionType(void *arg);

#else

// Windows OS specific definitions

#include <windows.h>

typedef HANDLE OSThreadType;
typedef HANDLE OSSemaphoreType;
DWORD WINAPI OSThreadFunctionType(LPVOID arg);

#endif


// Lightweight thread

struct Thread
{
    
    typedef void ThreadFunctionType(void *);
    
public:
    
    Thread(ThreadFunctionType *threadFunction, void *arg);
    ~Thread();
    
    void close();
    
private:
    
    // Deleted
    
    Thread(const Thread&);
    Thread& operator=(const Thread&);
    
    // threadStart is a quick OS-style wrapper to call the object which calls the relevant static function
    
    static OSThreadFunctionType threadStart;
    void call() { mThreadFunction(mArg); }
    
    // Data
    
    OSThreadType mInternal;
    ThreadFunctionType *mThreadFunction;
    void *mArg;
    bool mValid;
};


// Semaphore (note that you should most likely close() before the destructor is called

class Semaphore
{
    
public:
    
    Semaphore(long maxCount);
    ~Semaphore();
    
    void close();
    void signal(long n);
    bool wait();
    
private:
    
    // Deleted
    
    Semaphore(const Semaphore&);
    Semaphore& operator=(const Semaphore&);
    
    // Data
    
    OSSemaphoreType mInternal;
    bool mValid;
};


class ThreadSet
{
    
public:
    
    typedef void procFunc(t_object *, void **, void *, t_ptr_uint, long, long, long);
    
    struct ThreadSlot
    {
        ThreadSlot(void *owner, long idx, long numTempOuts) : mOwner(owner), mIdx(idx), mProcessed(1), mTempMemory(NULL), mTempMemSize(0)
        {
            mTempBuffers.resize(numTempOuts);
        }
        
        void **getBuffers() { return &mTempBuffers[0]; }

        void *mOwner;
        long mIdx;
        t_int32_atomic mProcessed;
        void *mTempMemory;
        t_ptr_uint mTempMemSize;
        std::vector<void *> mTempBuffers;
    };
    
    ThreadSet(t_object *owner, procFunc *process, long numThreads, long numTempOuts) : mOwner(owner), mProcess(process), mSemaphore(numThreads - 1),  mVecSize(0), mActive(numThreads), mTemporaryBufferSize(0)
    {
        numThreads = numThreads < 1 ? 1 : numThreads;

        for (long i = 0; i < numThreads; i++)
            mThreadSlots.push_back(ThreadSlot(this, i, numTempOuts));
        
        for (long i = 0; i < numThreads - 1; i++)
            mThreads.push_back(new Thread(&threadEntry, &mThreadSlots[i + 1]));

    }
    
    ~ThreadSet()
    {
        // Close sempahore and threads
        
        mSemaphore.close();
        for (std::vector<Thread *>::iterator it = mThreads.begin(); it != mThreads.end(); it++)
            (*it)->close();
        
        // Free threads
        
        for (std::vector<Thread *>::iterator it = mThreads.begin(); it != mThreads.end(); it++)
            delete (*it);
        
        mThreads.clear();
        
        // Free temporary buffers
        
        for (std::vector<ThreadSlot>::iterator it = mThreadSlots.begin(); it != mThreadSlots.end(); it++)
            for (std::vector<void *>::iterator jt = it->mTempBuffers.begin(); jt != it->mTempBuffers.end(); jt++)
                deallocate_aligned(*jt);
    }
        
    inline void tick(long vecSize, long numThreads, void **sig_outs)
    {
        // Set number active threads
        
        // FIX - safety
        
        mActive = numThreads;
        mVecSize = vecSize;
        
        if (numThreads == 1)
        {
            mProcess(mOwner, sig_outs, mThreadSlots[0].mTempMemory, mThreadSlots[0].mTempMemSize, vecSize, 0, 1);
            return;
        }
            
        // Set flags
        
        for (long i = 0; i < numThreads; i++)
            mThreadSlots[i].mProcessed = 0;
        
        mSemaphore.signal(numThreads - 1);
        
        // Process thread
        
        mProcess(mOwner, mThreadSlots[0].getBuffers(), mThreadSlots[0].mTempMemory, mThreadSlots[0].mTempMemSize, vecSize, 0, numThreads);
    
        // Wait for all the other threads to return
        
        for (long i = 1; i < numThreads; i++)
            while (mThreadSlots[i].mProcessed != 1);
    }
    
    static unsigned long getNumProcessors()
    {
#ifdef __APPLE__
        return MPProcessors();
#else
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        
        return sysinfo.dwNumberOfProcessors;
#endif
    }
    
    void setTempMemory(long thread, void *memory, t_ptr_uint size)
    {
        mThreadSlots[thread].mTempMemory = memory;
        mThreadSlots[thread].mTempMemSize = size;
    }
    
    void *getThreadOut(long thread, long index)
    {
        return mThreadSlots[thread].mTempBuffers[index];
    }
        
    bool resizeTempBuffers(t_ptr_int size)
    {
        if (size != mTemporaryBufferSize)
        {
            for (std::vector<ThreadSlot>::iterator it = mThreadSlots.begin(); it != mThreadSlots.end(); it++)
            {
                for (std::vector<void *>::iterator jt = it->mTempBuffers.begin(); jt != it->mTempBuffers.end(); jt++)
                {
                    deallocate_aligned(*jt);
                    *jt = allocate_aligned<u_int8_t>(size);
                    if (!*jt)
                    {
                        object_error((t_object *) mOwner, "not enough memory");
                        mTemporaryBufferSize = 0;
                        return true;
                    }
                }
            }
        }
        
        mTemporaryBufferSize = size;
        
        return false;
    }
    
    void threadClassEntry(long threadNum)
    {
        while (mSemaphore.wait())
        {
            for (long i = threadNum; i < threadNum + mActive - 1; i++)
            {
                // N.B. Get values from thread each time in case they have been changed
                
                long idx = (i % (mActive - 1)) + 1;
                
                if (Atomic_Compare_And_Swap(0, 2, &mThreadSlots[idx].mProcessed))
                {
                    mProcess(mOwner, mThreadSlots[idx].getBuffers(), mThreadSlots[idx].mTempMemory, mThreadSlots[idx].mTempMemSize, mVecSize, idx, mActive);
                    mThreadSlots[idx].mProcessed = 1;
                }
            }
        }
    }
    
    static void threadEntry(void *arg)
    {
        ThreadSet::ThreadSlot *slot = static_cast<ThreadSet::ThreadSlot *>(arg);
        ThreadSet *set =  static_cast<ThreadSet *>(slot->mOwner);
        
        set->threadClassEntry(slot->mIdx);
    }
    
    long getNumThreads() const { return mThreadSlots.size(); }
    
private:
    
    t_object *mOwner;

    procFunc *mProcess;

    Semaphore mSemaphore;
    std::vector<Thread *> mThreads;
    std::vector<ThreadSlot> mThreadSlots;
    
    long mVecSize;
    long mActive;
    t_ptr_int mTemporaryBufferSize;
};

#endif /* defined(__THREADSET__) */
