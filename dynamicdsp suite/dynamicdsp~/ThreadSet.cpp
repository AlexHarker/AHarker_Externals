
#include "ThreadSet.h"


#ifdef __APPLE__

// Thread mac implementation

Thread::Thread(threadFunc *threadFunction, void *arg)
    {
        pthread_attr_t tattr;
        struct sched_param param;
        
        pthread_attr_init(&tattr);
        pthread_attr_getschedparam (&tattr, &param);
        param.sched_priority = 63;
        pthread_attr_setschedparam (&tattr, &param);
        pthread_create(&mPth, &tattr, threadFunction, arg);
    }

Thread::~Thread() {}

// Semaphore mac implementations

Semaphore::Semaphore(long size)
{
    mTask = mach_task_self();
    semaphore_create(mTask, &mInternal, 0, 0);
}
    
Semaphore::~Semaphore()
{
    OSMemoryBarrier();
    semaphore_destroy(mTask, mInternal);
}
    
void Semaphore::tick(long n)
{
    OSMemoryBarrier();
    for (long i = 0; i < n; i++)
        semaphore_signal(mInternal);
}
    
bool Semaphore::wait()
{
    return semaphore_wait(mInternal) == KERN_TERMINATED;
}

#else

// Thread windows implementation

Thread::Thread(threadFunc *threadFunction, void *arg) : mExiting(false)
{
    mPth = CreateThread(NULL, 0, threadFunction, arg, 0, NULL);
    SetThreadPriority(mPth, THREAD_PRIORITY_TIME_CRITICAL);
}
    
Thread::~Thread
{
    mExciting = true;
}

// Semaphore windows implementations

Semaphore::Semaphore(long size) : mSize(size)
{
    mInternal = CreateSemaphore(NULL, 0, mSize, NULL);
}
    
Semaphore::~Semaphore()
{
    MemoryBarrier();
    ReleaseSemaphore(mInternal, mSize, NULL);
    CloseHandle(mInternal);
    
    // FIX - How to wait for threads?!!!
}
    
void Semaphore::tick(long n)
{
    MemoryBarrier();
    ReleaseSemaphore(mInternal, n, NULL);
}
    
bool Semaphore::wait()
{
    WaitForSingleObject(mInternal, INFINITE);
    return constant_thread->exiting;
}

#endif

// Thread starting functions

#ifdef __APPLE__

void *threadStart(void *arg)
{
    ThreadSet::ThreadSlot *slot = (ThreadSet::ThreadSlot *)arg;
    ThreadSet *set = (ThreadSet *)slot->mOwner;
    
    set->threadEntry(slot->mIdx);
    
    return NULL;
}

#else

DWORD WINAPI threadStart(LPVOID arg)
{
    ThreadSet::ThreadSlot *slot = (ThreadSet::ThreadSlot *)arg;
    ThreadSet *set = (ThreadSet *)slot->mOwner;
    
    set->threadEntry(slot->mIdx);
    
    return 0;
}

#endif
