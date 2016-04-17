
#ifndef __THREADSLOT__
#define __THREADSLOT__

#include <vector>

#include <ext.h>

#include <AH_Atomic.h>
#include <AH_Types.h>

#ifdef __APPLE__
#include <pthread.h>
#include <mach/semaphore.h>
#include <mach/task.h>
#endif


#ifdef __APPLE__

struct Thread
{
    Thread(void *ref)
    {
        pthread_attr_t tattr;
        struct sched_param param;
        
        pthread_attr_init(&tattr);
        pthread_attr_getschedparam (&tattr, &param);
        param.sched_priority = 63;
        pthread_attr_setschedparam (&tattr, &param);
        pthread_create(&mPth, &tattr, dynamicdsp_threadwait, ref);
    }
    
private:
    
    pthread_t mPth;
};

struct Semaphore
{
    Semaphore()
    {
        mTask = mach_task_self();
        semaphore_create(mTask, &mInternal, 0, 0);
    }
    
    ~Semaphore()
    {
        semaphore_destroy(mTask, mInternal);
    }
    
private:
    
    task_t mTask;
    semaphore_t mInternal;
};

#else

struct Thread
{
    Thread(void *ref) : mExiting(false)
    {
        mPth = CreateThread(NULL, 0, dynamicdsp_threadwait, ref, 0, NULL);
        SetThreadPriority(mPth, THREAD_PRIORITY_TIME_CRITICAL);
    }
    
private:
    
    HANDLE mPth;
    AH_Boolean mExciting;
};

struct Semaphore
{
    Semaphore()
    {
        mInternal = CreateSemaphore(NULL, 0, max_obj_threads - 1, NULL);
    }
    
    ~Semaphore()
    {
        //for (long i = 1; i < x->max_obj_threads; i++)
        //    x->thread_space_ptr[i].exiting = true;
        //ReleaseSemaphore(x->tick_semaphore, x->max_obj_threads - 1, NULL);
        CloseHandle(mInternal);
    }
    
private:
    
    HANDLE mInternal;
};
#endif

class ThreadSet
{
    struct ThreadSlot
    {
        ThreadSlot(long numTempOuts) : mProcessed(1), mTempMemory(NULL), mTempMemSize(0)
        {
            mTempBuffers.resize(numTempOuts);
        }
        
        t_int32_atomic mProcessed;
        
        // Temporary Memory For Objects Within Patch
        
        void *mTempMemory;
        t_ptr_uint mTempMemSize;
        
        // Internal Buffers
        
        std::vector<void *> mTempBuffers;
    };

public:
    
    ThreadSet(void *parent, t_uint32 size) : mParent(parent)
    {
        mThreads.resize(size - 1);
        mThreadSlots.resize(size);
    }
    
private:
    
     void *mParent;
    
    std::vector<Thread> mThreads;
    std::vector<ThreadSlot> mThreadSlots;
};

#endif /* defined(__THREADSLOT__) */
