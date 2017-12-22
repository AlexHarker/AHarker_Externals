
#ifndef __THREADSET__
#define __THREADSET__

#include <vector>

#include <ext.h>

#include <AH_Atomic.h>
#include <AH_VectorOps.h>
#include <stdint.h>

#ifdef __APPLE__
#include <pthread.h>
#include <mach/semaphore.h>
#include <mach/task.h>
#else
#include <Windows.h>
#endif

// Lightweight high priority thread and semaphore (note you should destroy the thread before the semaphore)

struct Thread
{
    
#ifdef __APPLE__
    typedef void *threadFunc(void *arg);
#else
    DWORD WINAPI threadFunc(LPVOID arg);
#endif
    
    Thread(threadFunc *threadFunction, void *arg);
    ~Thread();
    
private:
    
#ifdef __APPLE__
    pthread_t mPth;
#else
    HANDLE mPth;
    bool mExciting;
#endif
};

struct Semaphore
{
    Semaphore(long size);
    ~Semaphore();

    void tick(long n);
    bool wait();
        
private:
    
#ifdef __APPLE__
    task_t mTask;
    semaphore_t mInternal;
#else
    long mSize;
    HANDLE mInternal;
#endif

};

// Thread starting functions

 #ifdef __APPLE__
 void *threadStart(void *arg);
 #else
 DWORD WINAPI threadStart(LPVOID arg);
 #endif

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
            mThreads.push_back(new Thread(&threadStart, &mThreadSlots[i + 1]));

    }
    
    ~ThreadSet()
    {
        // Free threads
        
        for (std::vector<Thread *>::iterator it = mThreads.begin(); it != mThreads.end(); it++)
            delete (*it);
        
        mThreads.clear();
        
        // Free temporary buffers
        
        for (std::vector<ThreadSlot>::iterator it = mThreadSlots.begin(); it != mThreadSlots.end(); it++)
            for (std::vector<void *>::iterator jt = it->mTempBuffers.begin(); jt != it->mTempBuffers.end(); jt++)
                ALIGNED_FREE(*jt);
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
        
        mSemaphore.tick(numThreads - 1);
        
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
                    ALIGNED_FREE(*jt);
                    *jt = ALIGNED_MALLOC(size);
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
    
    void threadEntry(long threadNum)
    {
        while(1)
        {
            if (mSemaphore.wait())
                break;
            
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
