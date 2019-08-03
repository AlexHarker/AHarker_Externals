
#ifndef AH_LOCKS_H
#define AH_LOCKS_H

#include <algorithm>
#include <atomic>
#include <chrono>
#include <thread>

// Basic lock (With spin and periodic sleep variants)

struct thread_lock
{
    const static int max_iter_before_sleep = 5;

    thread_lock() : m_flag(0) {}
    thread_lock(const thread_lock&) = delete;
    thread_lock& operator=(const thread_lock&) = delete;
    ~thread_lock() { acquire(); }
    
    // Acquire, Attempt and Release
    
    void acquire()
    {
        bool expected = false;
        while (!m_flag.compare_exchange_weak(expected, true));
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
    
    void acquire_periodic_sleep()
    {
        using namespace std::chrono_literals;
        bool acquired = false;
        
        for (int i = 0; !acquired; i = std::min(i++, max_iter_before_sleep))
        {
            if (attempt())
                acquired = true;
            
            if (i == max_iter_before_sleep)
                std::this_thread::sleep_for(0.1ms);
        }
    }
    
private:
    
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
            m_write_lock.acquire_periodic_sleep();
        m_read_lock.release();
    }
    
    // Acquires the write lock

    void acquire_write()
    {
        m_write_lock.acquire_periodic_sleep();
    }
    
    // Acquires the write Lock *once* the read lock is held

    void promote()
    {
        using namespace std::chrono_literals;
        bool acquired = false;
        
        for (int i = 0; !acquired; i = std::min(i++, thread_lock::max_iter_before_sleep))
        {
            m_read_lock.acquire();
            if (m_read_count == 1)
            {
                m_read_count = 0;
                acquired = true;
            }
            m_read_lock.release();
            
            if (i == thread_lock::max_iter_before_sleep)
                std::this_thread::sleep_for(0.1ms);
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
    lock_hold(thread_lock *lock) : m_lock(lock)    { m_lock->acquire_method(); }
    ~lock_hold()                                   { destroy(); }
    
    void destroy()
    {
        if (m_lock)
        {
            m_lock->release_method();
            m_lock = nullptr;
        }
    }
    
    lock_hold(const lock_hold&) = delete;
    lock_hold& operator=(const lock_hold&) = delete;
    
private:
    
    T *m_lock;
};

// Specific lock holds

using spin_lock_hold = lock_hold<thread_lock, &thread_lock::acquire, &thread_lock::release>;
using read_lock_hold = lock_hold<read_write_lock, &read_write_lock::acquire_read, &read_write_lock::release_read>;
using write_lock_hold = lock_hold<read_write_lock, &read_write_lock::acquire_write, &read_write_lock::release_write>;



#endif
