
#ifndef __THREADSET__
#define __THREADSET__

#include <atomic>
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

struct NumProcessors
{
    unsigned long operator()() { return MPProcessors(); }
};

#else

// Windows OS specific definitions

#include <windows.h>

typedef HANDLE OSThreadType;
typedef HANDLE OSSemaphoreType;
DWORD WINAPI OSThreadFunctionType(LPVOID arg);

struct NumProcessors
{
    unsigned long operator()() 
    {
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
    
        return sysinfo.dwNumberOfProcessors;
    }
};

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
    
    typedef void procFunc(t_object *, void **, long, long, long);
    
    struct ThreadSlot
    {
        ThreadSlot(void *owner, long idx, long numOuts) : mOwner(owner), mIdx(idx), mProcessed(1)
        {
            mBuffers.resize(numOuts);
        }
        
        void **getBuffers() { return mBuffers.data(); }

        void *mOwner;
        long mIdx;
        std::atomic<int> mProcessed;
        std::vector<void *> mBuffers;
    };
    
    ThreadSet(t_object *owner, procFunc *process, long numThreads, long numOuts);
    ~ThreadSet();
        
    void tick(long vecSize, long numThreads, void **sig_outs);
    bool resizeTempBuffers(t_ptr_int size);

    void threadClassEntry(long threadNum);
    static void threadEntry(void *arg);

    long getNumThreads() const { return mThreadSlots.size(); }

    template <typename T>
    T *getThreadOut(long thread, long index)
    {
        return (T *) mThreadSlots[thread].mBuffers[index];
    }
    
private:
    
    t_object *mOwner;
    procFunc *mProcess;
    Semaphore mSemaphore;
    std::vector<Thread *> mThreads;
    std::vector<ThreadSlot> mThreadSlots;
    
    long mActive;
    long mVecSize;

    t_ptr_int mBufferSize;
};

#endif /* defined(__THREADSET__) */
