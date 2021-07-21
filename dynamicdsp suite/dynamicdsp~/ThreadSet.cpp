
#include <assert.h>

#include "ThreadSet.hpp"

#ifdef __APPLE__

// FIX - TODO
// Check windows

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
    pthread_attr_setschedpolicy(&threadAttributes, SCHED_FIFO);
    
    // Set the priority of the thread before we create it
    
    schedulingParameters.sched_priority = 75;
    
    // Set the scheduling attributes and creat the thread
    
    pthread_attr_setschedparam(&threadAttributes, &schedulingParameters);
    pthread_create(&mInternal, &threadAttributes, threadStart, this);
}

Thread::~Thread()
{
    assert(!mValid && "Thread not closed before deletion");
}

void Thread::close()
{
    if (mValid)
    {
        mValid = false;
        OSMemoryBarrier();
        
        // Wait for thread to join before we allow the program to continue
        
        pthread_join(mInternal, NULL);
    }
}

void *Thread::threadStart(void *arg)
{
    static_cast<Thread *>(arg)->call();
    
    return NULL;
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
        OSMemoryBarrier();
        semaphore_signal_all(mInternal);
    }
}

void Semaphore::signal(long n)
{
    OSMemoryBarrier();
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
    
    mInternal = CreateThread(NULL, 0, threadStart, this, 0, NULL);
    
    // Set priority
    
   SetThreadPriority(mInternal, THREAD_PRIORITY_TIME_CRITICAL);
}

Thread::~Thread()
{
    assert(!mValid && "Thread not closed before deletion");
    CloseHandle(mInternal);
}

void Thread::close()
{
    if (mValid)
    {
        mValid = false;
        MemoryBarrier();
        
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
    mInternal = CreateSemaphore(NULL, 0, maxCount, NULL);
}

Semaphore::~Semaphore()
{
    assert(!mValid && "Semaphore not closed before deletion");
    CloseHandle(mInternal);
}

void Semaphore::close()
{
    if (mValid)
    {
        mValid = false;
        MemoryBarrier();
        
        // Signal until the count is zero (only realiable way to signal all waiting threads
        
        for (long releaseCount = 1; releaseCount; --releaseCount)
            ReleaseSemaphore(mInternal, 1, &releaseCount);
    }
}

void Semaphore::signal(long n)
{
    // N.B. - signalling is unsafe after the semaphore has been closed
    
    MemoryBarrier();
    ReleaseSemaphore(mInternal, n, NULL);
}

bool Semaphore::wait()
{
    if (mValid)
        WaitForSingleObject(mInternal, INFINITE);
    
    return mValid;
}

#endif

// ThreadSet Class

ThreadSet::ThreadSet(t_object *owner, procFunc *process, long numThreads, long numOuts)
: mOwner(owner), mProcess(process), mSemaphore(numThreads - 1), mActive(numThreads), mVecSize(0), mBufferSize(0)
{
    numThreads = numThreads < 1 ? 1 : numThreads;
    
    for (long i = 0; i < numThreads; i++)
        mThreadSlots.emplace_back(this, i, numTempOuts);
    
    for (long i = 0; i < numThreads - 1; i++)
        mThreads.push_back(new Thread(&threadEntry, &mThreadSlots[i + 1]));
    
}

ThreadSet::~ThreadSet()
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
        for (std::vector<void *>::iterator jt = it->mBuffers.begin(); jt != it->mBuffers.end(); jt++)
            deallocate_aligned(*jt);
}

void ThreadSet::tick(long vecSize, long numThreads, void **sig_outs)
{
    // Set number active threads
    
    // FIX - safety
    
    mActive = numThreads;
    mVecSize = vecSize;
    
    if (numThreads == 1)
    {
        mProcess(mOwner, sig_outs, vecSize, 0, 1);
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

bool ThreadSet::resizeTempBuffers(t_ptr_int size)
{
    if (size != mBufferSize)
    {
        for (std::vector<ThreadSlot>::iterator it = mThreadSlots.begin(); it != mThreadSlots.end(); it++)
        {
            for (std::vector<void *>::iterator jt = it->mBuffers.begin(); jt != it->mBuffers.end(); jt++)
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

void ThreadSet::threadClassEntry(long threadNum)
{
    while (mSemaphore.wait())
    {
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

void ThreadSet::threadEntry(void *arg)
{
    ThreadSet::ThreadSlot *slot = static_cast<ThreadSet::ThreadSlot *>(arg);
    ThreadSet *set =  static_cast<ThreadSet *>(slot->mOwner);
    
    set->threadClassEntry(slot->mIdx);
}
