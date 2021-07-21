
#include <assert.h>

#include "ThreadSet.hpp"

#ifdef __APPLE__

// Thread Mac OS implementation

Thread::Thread(ThreadFunctionType *threadFunction, void *arg) : mThreadFunction(threadFunction), mArg(arg), mValid(true)
{
    pthread_attr_t threadAttributes;
    sched_param schedulingParameters;
    
    // Get default attributes and scheduling parameters
    
    pthread_attr_init(&threadAttributes);
    pthread_attr_getschedparam(&threadAttributes, &schedulingParameters);
    
    // Set detach state and policy
    
    pthread_attr_setdetachstate(&threadAttributes, PTHREAD_CREATE_JOINABLE);
    
    if (maxversion() >= 0x800)
        pthread_attr_setschedpolicy(&threadAttributes, SCHED_RR);
    else
        pthread_attr_setschedpolicy(&threadAttributes, SCHED_FIFO);
    
    // Set the priority of the thread before we create it
    
    if (maxversion() >= 0x800)
        schedulingParameters.sched_priority = 43;
    else
        schedulingParameters.sched_priority = 63;
    
    // Set the scheduling attributes and create the thread
    
    pthread_attr_setschedparam(&threadAttributes, &schedulingParameters);
    pthread_create(&mInternal, &threadAttributes, threadStart, this);
}

Thread::~Thread()
{
    assert(!mValid && "Thread not closed before deletion");
}

void Thread::join()
{
    if (mValid)
    {
        mValid = false;
        std::atomic_thread_fence(std::memory_order_seq_cst);
        
        // Wait for thread to join before we allow the program to continue
        
        pthread_join(mInternal, nullptr);
    }
}

void *Thread::threadStart(void *arg)
{
    static_cast<Thread *>(arg)->call();
    
    return nullptr;
}


// Semaphore Mac OS implementation

Semaphore::Semaphore(long maxCount) : mValid(true)
{
    semaphore_create(mach_task_self(), &mInternal, SYNC_POLICY_FIFO, 0);
}

Semaphore::~Semaphore()
{
    assert(!mValid && "Semaphore not closed before deletion");
    semaphore_destroy(mach_task_self(), mInternal);
}

void Semaphore::close()
{
    if (mValid)
    {
        mValid = false;
        std::atomic_thread_fence(std::memory_order_seq_cst);
        semaphore_signal_all(mInternal);
    }
}

void Semaphore::signal(long n)
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
    for (long i = 0; i < n; i++)
        semaphore_signal(mInternal);
}

bool Semaphore::wait()
{
    if (mValid)
        semaphore_wait(mInternal);
    
    return mValid;
}

#else

// Thread Windows OS implementation

Thread::Thread(ThreadFunctionType *threadFunction, void *arg) : mThreadFunction(threadFunction), mArg(arg), mValid(true)
{
    // Create thread
    
    mInternal = CreateThread(nullptr, 0, threadStart, this, 0, nullptr);
    
    // Set priority
    
   SetThreadPriority(mInternal, THREAD_PRIORITY_TIME_CRITICAL);
}

Thread::~Thread()
{
    assert(!mValid && "Thread not closed before deletion");
    CloseHandle(mInternal);
}

void Thread::join()
{
    if (mValid)
    {
        mValid = false;
        std::atomic_thread_fence(std::memory_order_seq_cst);
        
        // Wait for thread to join before we allow the program to continue
        
        WaitForSingleObject(mInternal, INFINITE);
    }
}

DWORD WINAPI Thread::threadStart(LPVOID arg)
{
    static_cast<Thread *>(arg)->call();
    
    return 0;
}


// Semaphore Windows OS implementation

Semaphore::Semaphore(long maxCount) : mValid(true)
{
    mInternal.mHandle = CreateSemaphore(nullptr, 0, maxCount, nullptr);
    mInternal.mMaxCount = maxCount;
}

Semaphore::~Semaphore()
{
    assert(!mValid && "Semaphore not closed before deletion");
    CloseHandle(mInternal.mHandle);
}

void Semaphore::close()
{
    if (mValid)
    {
        mValid = false;
        std::atomic_thread_fence(std::memory_order_seq_cst);
        
        // Signal maximum count to ensure all threads are released, and check for completion
        
        for (long n = mInternal.mMaxCount; n > 0; n--)
        {
            if (ReleaseSemaphore(mInternal.mHandle, n, nullptr))
                break;
        }
    }
}

void Semaphore::signal(long n)
{
    // N.B. - signalling is unsafe after the semaphore has been closed
    
    std::atomic_thread_fence(std::memory_order_seq_cst);
    ReleaseSemaphore(mInternal.mHandle, n, nullptr);
}

bool Semaphore::wait()
{
    if (mValid)
        WaitForSingleObject(mInternal.mHandle, INFINITE);
    
    return mValid;
}

#endif


// ThreadSet Class

ThreadSet::ThreadSet(t_object *owner, procFunc *process, long numThreads, long numOuts)
: mOwner(owner), mProcess(process)
, mActive(std::max(1L, numThreads)), mVecSize(0), mBufferSize(0)
, mSemaphore(mActive - 1)
{
    // Create slots and threads

    for (long i = 0; i < numThreads; i++)
        mThreadSlots.emplace_back(this, i, numOuts);
    
    for (long i = 0; i < numThreads - 1; i++)
        mThreads.emplace_back(new Thread(&threadEntry, &mThreadSlots[i + 1]));
    
}

ThreadSet::~ThreadSet()
{
    // Close semaphore and join threads
    
    mSemaphore.close();
    
    for (auto it = mThreads.begin(); it != mThreads.end(); it++)
        (*it)->join();
    
    // Free temporary buffers
    
    for (auto it = mThreadSlots.begin(); it != mThreadSlots.end(); it++)
        for (auto jt = it->mBuffers.begin(); jt != it->mBuffers.end(); jt++)
            deallocate_aligned(*jt);
}

void ThreadSet::tick(long vecSize, long numThreads, void **outs)
{
    // Set number active threads
    
    mActive = std::min(numThreads, getNumThreads());
    mVecSize = vecSize;
    
    if (numThreads == 1)
    {
        mProcess(mOwner, outs, vecSize, 0, 1);
        return;
    }
    
    // Set flags
    
    for (long i = 0; i < numThreads; i++)
        mThreadSlots[i].mProcessed = 0;
    
    mSemaphore.signal(numThreads - 1);
    
    // Process thread
    
    mProcess(mOwner, mThreadSlots[0].getBuffers(), vecSize, 0, numThreads);
    
    // Wait for all the other threads to return
    
    for (long i = 1; i < numThreads; i++)
        while (mThreadSlots[i].mProcessed != 1);
}

bool ThreadSet::resizeBuffers(t_ptr_int size)
{
    if (size != mBufferSize)
    {
        for (auto it = mThreadSlots.begin(); it != mThreadSlots.end(); it++)
        {
            for (auto jt = it->mBuffers.begin(); jt != it->mBuffers.end(); jt++)
            {
                deallocate_aligned(*jt);
                *jt = allocate_aligned<u_int8_t>(size);
                if (!*jt)
                {
                    object_error((t_object *) mOwner, "not enough memory");
                    mBufferSize = 0;
                    return true;
                }
            }
        }
    }
    
    mBufferSize = size;
    
    return false;
}

// Main thread loop methods

void ThreadSet::threadEntry(void *arg)
{
    ThreadSet::ThreadSlot *slot = static_cast<ThreadSet::ThreadSlot *>(arg);
    ThreadSet *set = static_cast<ThreadSet *>(slot->mOwner);
    
    set->processingLoop(slot->mIdx);
}

void ThreadSet::processingLoop(long threadNum)
{
    while (mSemaphore.wait())
    {
        // Loop through possible threads in case we are woken multiple times
        
        for (long i = threadNum; i < threadNum + mActive - 1; i++)
        {
            // N.B. Get values from thread each time in case they have been changed
            
            long idx = (i % (mActive - 1)) + 1;
            int expected = 0;
            
            if (mThreadSlots[idx].mProcessed.compare_exchange_strong(expected, 2))
            {
                mProcess(mOwner, mThreadSlots[idx].getBuffers(), mVecSize, idx, mActive);
                mThreadSlots[idx].mProcessed = 1;
            }
        }
    }
}
