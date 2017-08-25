
#ifndef READWRITELOCK_H
#define READWRITELOCK_H

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
        bool notAcquired = true;
        
        while (notAcquired)
        {
            mReadLock.acquire();
            if (mReadCount == 1)
            {
                mReadCount = 0;
                notAcquired = false;
            }
            mReadLock.release();
            
            // FIX - need sleep stuff here...
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
    
    ReadWriteLock *mLock;
};


#endif
