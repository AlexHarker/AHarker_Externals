
#ifndef _THREADSET_H_
#define _THREADSET_H_

#include <atomic>
#include <thread>
#include <vector>

#include <ext.h>
#include <stdint.h>

#include <SIMDSupport.hpp>

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
using OSSemaphoreType = struct WinSemaphore { HANDLE mHandle; long mMaxCount; };
DWORD WINAPI OSThreadFunctionType(LPVOID arg);

#endif


// Lightweight thread

struct Thread
{
    typedef void ThreadFunctionType(void *);
    
public:
    
    Thread(ThreadFunctionType *threadFunction, void *arg);
    ~Thread();
    
    // Non-copyable
    
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;
    
    void join();
    
private:
    
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
    
    // Non-copyable
    
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    
    void close();
    void signal(long n);
    bool wait();
    
private:
    
    // Data
    
    OSSemaphoreType mInternal;
    bool mValid;
};


// ThreadSet class

class ThreadSet
{
    
public:
    
    typedef void procFunc(t_object *, void **, long, long, long);
    
    struct ThreadSlot
    {
        ThreadSlot(void *owner, long numOuts)
        : mOwner(owner), mProcessed(1)
        {
            mBuffers.resize(numOuts, nullptr);
        }
        
        ThreadSlot(const ThreadSlot& other)
        : mOwner(other.mOwner), mProcessed(1), mBuffers(other.mBuffers) {}
        
        void **getBuffers() { return mBuffers.data(); }

        void *mOwner;
        std::atomic<int> mProcessed;
        std::vector<void *> mBuffers;
    };
    
    ThreadSet(t_object *owner, procFunc *process, long numThreads, long numOuts);
    ~ThreadSet();
    
    long getNumThreads() const { return mThreadSlots.size(); }

    void tick(long vecSize, long numThreads, void **outs);
    bool resizeBuffers(t_ptr_int size);

    template <typename T>
    T *getThreadBuffer(long thread, long index)
    {
        return (T *) mThreadSlots[thread].mBuffers[index];
    }

    static void threadEntry(void *arg);

private:
    
    void processingLoop(long threadIdx);

    t_object *mOwner;
    procFunc *mProcess;

    long mActive;
    long mVecSize;
    t_ptr_int mBufferSize;

    Semaphore mSemaphore;
    std::vector<std::unique_ptr<Thread>> mThreads;
    std::vector<ThreadSlot> mThreadSlots;
};

#endif  /* _THREADSET_H_ */
