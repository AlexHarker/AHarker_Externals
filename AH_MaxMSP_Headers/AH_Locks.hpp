
#ifndef _AH_LOCKS_HPP_
#define _AH_LOCKS_HPP_

#include <algorithm>
#include <atomic>
#include <chrono>
#include <thread>

#if defined(__APPLE__)

inline void thread_nanosleep(long nanoseconds)
{
    std::this_thread::sleep_for(std::chrono::nanoseconds(nanoseconds));
}

#else

#include <windows.h>

inline void thread_nanosleep(long nanoseconds)
{
    SwitchToThread();
}

#endif

// Basic lock (With spin and periodic sleep variants)

struct thread_lock
{
    thread_lock() : m_flag(0) {}
    thread_lock(const thread_lock&) = delete;
    thread_lock& operator=(const thread_lock&) = delete;
    ~thread_lock() { acquire_low(); }
    
    // Acquire, Attempt and Release
    
    // This method is best for RT access
    
    void acquire()
    {
        acquire_impl(false);
    }
    
    // This method is less urgent and so best for write operations etc.
    
    void acquire_low()
    {
        acquire_impl(true);
    }
    
    bool attempt()
    {
        bool expected = false;
        return m_flag.compare_exchange_strong(expected, true);
    }
    
    void release()
    {
        bool expected = true;
        m_flag.compare_exchange_strong(expected, false);
    }
    
  
private:
    
    void acquire_impl(bool slow)
    {
        using clock = std::chrono::steady_clock;
        
        // Spin 10 times + 10000 nanoseconds
        
        for (int i = 0; i < 10; i++)
            if (attempt())
                return;
        
        auto timeOut = clock::now() + std::chrono::nanoseconds(10000);
        
        while (clock::now() < timeOut)
            if (attempt())
                return;
        
        // Ten short nanosleeps
        
        for (int i = 0; i < 10; i++)
        {
            if (attempt())
                return;
            thread_nanosleep(100);
        }
        
        // Continue with potentially longer nanosleeps
        
        while (!attempt())
            thread_nanosleep(slow ? 10000 : 100);
    }
    
    std::atomic<bool> m_flag;
};

// A multiple reader / single writer lock

struct read_write_lock
{
    read_write_lock() : m_read_count(0) {}
    read_write_lock(const read_write_lock&) = delete;
    read_write_lock& operator=(const read_write_lock&) = delete;
    
    // Acquires the read lock
    
    void acquire_read()
    {
        m_read_lock.acquire();
        if (m_read_count++ == 0)
            m_write_lock.acquire_low();
        m_read_lock.release();
    }
    
    // Acquires the write lock

    void acquire_write()
    {
        m_write_lock.acquire_low();
    }
    
    // Acquires the write Lock *once* the read lock is held

    void promote()
    {
        using namespace std::chrono_literals;
        constexpr int max_iter = 5;
        bool acquired = false;
        
        for (int i = 0; !acquired; i = std::min(i++, max_iter))
        {
            m_read_lock.acquire();
            if (m_read_count == 1)
            {
                m_read_count = 0;
                acquired = true;
            }
            m_read_lock.release();
            
            if (i == max_iter)
                thread_nanosleep(10000);
        }
    }
    
    // Releases Write or Read Locks
    
    void release_read()
    {
        m_read_lock.acquire();
        if (m_read_count < 1 || --m_read_count == 0)
            m_write_lock.release();
        m_read_lock.release();
    }
    
    void release_write()
    {
        m_write_lock.release();
    }
    
private:

    // Data
    
    thread_lock m_write_lock;
    thread_lock m_read_lock;
    long m_read_count;
};

// Lock holders using RAII

// A generic lock holder

template <class T, void (T::*acquire_method)(), void (T::*release_method)()>
struct lock_hold
{
    lock_hold() : m_lock(nullptr) {}
    lock_hold(T *lock) : m_lock(lock)    { (m_lock->*acquire_method)(); }
    ~lock_hold()                         { destroy(); }
    
    void destroy()
    {
        if (m_lock)
        {
            (m_lock->*release_method)();
            m_lock = nullptr;
        }
    }
    
    lock_hold(const lock_hold&) = delete;
    lock_hold& operator=(const lock_hold&) = delete;
    
private:
    
    T *m_lock;
};

// Specific lock holds

using safe_lock_hold = lock_hold<thread_lock, &thread_lock::acquire, &thread_lock::release>;
using read_lock_hold = lock_hold<read_write_lock, &read_write_lock::acquire_read, &read_write_lock::release_read>;
using write_lock_hold = lock_hold<read_write_lock, &read_write_lock::acquire_write, &read_write_lock::release_write>;

#endif
