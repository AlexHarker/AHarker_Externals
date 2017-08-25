
#ifndef READWRITELOCK_H
#define READWRITELOCK_H

#include <algorithm>
#include <thread>

struct ReadWriteLock
{
    struct Lock
    {
        Lock() : mAtomicLock(0) {}
        ~Lock() { acquire(); }
        
        void acquire() { while(!attempt()); }
        bool attempt() { return ATOMIC_COMPARE_SWAP32(0, 1, &mAtomicLock); }
        void release() { ATOMIC_COMPARE_SWAP32(1, 0, &mAtomicLock); }
        
    private:
        
        Lock(const Lock&) = delete;
        Lock& operator=(const Lock&) = delete;
        
        t_int32_atomic mAtomicLock;
    };
    
    ReadWriteLock() : mReadCount(0) {}
    
    void acquireRead()
    {
        mReadLock.acquire();
        if (mReadCount++ == 0)
            mWriteLock.acquire();
        mReadLock.release();
    }
    
    void acquireWrite()
    {
        mWriteLock.acquire();
    }
    
    void promoteReadToWrite()
    {
        using namespace std::chrono_literals;
        long maxIterationsBeforeSleep = 5;
        bool notAcquired = true;
        
        for (long i = 0; notAcquired; i = std::min(i++, maxIterationsBeforeSleep))
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
    
    void release()
    {
        mReadLock.acquire();
        if (mReadCount < 1 || --mReadCount == 0)
            mWriteLock.release();
        mReadLock.release();
    }
    
private:
    
    ReadWriteLock(const ReadWriteLock&) = delete;
    ReadWriteLock& operator=(const ReadWriteLock&) = delete;
    
    Lock mWriteLock;
    Lock mReadLock;
    long mReadCount;
};

struct HoldWriteLock
{
    HoldWriteLock() : mLock(NULL) {}
    HoldWriteLock(ReadWriteLock *lock) : mLock(lock)    { mLock->acquireWrite(); }
    ~HoldWriteLock()                                    { if (mLock) mLock->release(); }
    
private:
    
    HoldWriteLock(const HoldWriteLock&) = delete;
    HoldWriteLock& operator=(const HoldWriteLock&) = delete;
    
    ReadWriteLock *mLock;
};


#endif
