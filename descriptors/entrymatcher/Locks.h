
#ifndef LOCKS_H
#define LOCKS_H

#include <algorithm>
#include <thread>

// Basic Spin Lock

struct Lock
{
    Lock() : mAtomicLock(0) {}
    Lock(const Lock&) = delete;
    Lock& operator=(const Lock&) = delete;
    ~Lock() { acquire(); }
    
    // Acquire, Attempt and Release
    
    void acquire() { while(!attempt()); }
    bool attempt() { return ATOMIC_COMPARE_SWAP32(0, 1, &mAtomicLock); }
    void release() { ATOMIC_COMPARE_SWAP32(1, 0, &mAtomicLock); }
    
private:
    
   // Data
    
    t_int32_atomic mAtomicLock;
};

// For Holding a Spin Lock using RAII

struct HoldLock
{
    HoldLock() : mLock(NULL) {}
    HoldLock(Lock *lock) : mLock(lock)    { mLock->acquire(); }
    ~HoldLock()                           { if (mLock) mLock->release(); }
    
    HoldLock(const HoldLock&) = delete;
    HoldLock& operator=(const HoldLock&) = delete;
    
    
private:
    
    // Data
    
    Lock *mLock;
};

// A Multiple Reader Single Writer Lock

struct ReadWriteLock
{
    const static int maxIterationsBeforeSleep = 5;

    ReadWriteLock() : mReadCount(0) {}
    ReadWriteLock(const ReadWriteLock&) = delete;
    ReadWriteLock& operator=(const ReadWriteLock&) = delete;
    
    // Acquires the Read Lock
    
    void acquireRead()
    {
        mReadLock.acquire();
        if (mReadCount++ == 0)
            mWriteLock.acquire();
        mReadLock.release();
    }
    
    // Acquires the Write Lock

    void acquireWrite()
    {
        using namespace std::chrono_literals;
        bool notAcquired = true;
        
        for (long i = 0; notAcquired; i = std::min(i++, (long) maxIterationsBeforeSleep))
        {
            if (mWriteLock.attempt())
                notAcquired = false;
            
            if (i == maxIterationsBeforeSleep)
                std::this_thread::sleep_for(0.1ms);
        }
    }
    
    // Acquires the Write Lock *Once* the Read Lock is Held

    void promoteReadToWrite()
    {
        using namespace std::chrono_literals;
        bool notAcquired = true;
        
        for (long i = 0; notAcquired; i = std::min(i++, (long) maxIterationsBeforeSleep))
        {
            mReadLock.acquire();
            if (mReadCount == 1)
            {
                mReadCount = 0;
                notAcquired = false;
            }
            mReadLock.release();
            
            if (i == maxIterationsBeforeSleep)
                std::this_thread::sleep_for(0.1ms);
        }
    }
    
    // Releases Write or Read Locks
    
    void release()
    {
        mReadLock.acquire();
        if (mReadCount < 1 || --mReadCount == 0)
            mWriteLock.release();
        mReadLock.release();
    }
    
private:

    // Data
    
    Lock mWriteLock;
    Lock mReadLock;
    long mReadCount;
};

// For Holding the Write Lock of a Read Write Lock using RAII

struct HoldWriteLock
{
    HoldWriteLock() : mLock(NULL) {}
    HoldWriteLock(ReadWriteLock *lock) : mLock(lock)    { mLock->acquireWrite(); }
    ~HoldWriteLock()                                    { if (mLock) mLock->release(); }
    
    HoldWriteLock(const HoldWriteLock&) = delete;
    HoldWriteLock& operator=(const HoldWriteLock&) = delete;

private:
    
    // Data
    
    ReadWriteLock *mLock;
};


#endif
