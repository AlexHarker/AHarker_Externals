
#ifndef _THREADSET_H_
#define _THREADSET_H_

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
using OSSemaphoreType = struct WinSemaphore { HANDLE m_handle; long m_max_count; };
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

struct thread
{
    typedef void ThreadFunctionType(void *);

public:

    thread(ThreadFunctionType *thread_function, void *arg);
    ~thread();

    // Non-copyable

    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;

    void join();

private:

    // threadStart is a quick OS-style wrapper to call the object which calls the relevant static function

    static OSThreadFunctionType thread_start;
    void call() { m_thread_function(m_arg); }

    // Data

    OSThreadType m_internal;
    ThreadFunctionType *m_thread_function;
    void *m_arg;
    bool m_valid;
};


// Semaphore (note that you should most likely close() before the destructor is called

class semaphore
{
public:

    semaphore(long max_count);
    ~semaphore();

    // Non-copyable

    semaphore(const semaphore&) = delete;
    semaphore& operator=(const semaphore&) = delete;

    void close();
    void signal(long n);
    bool wait();

private:

    // Data

    OSSemaphoreType m_internal;
    bool m_valid;
};


// ThreadSet class

class thread_set
{

public:

    typedef void ProcFunc(t_object *, void **, long, long, long);

    struct thread_slot
    {
        thread_slot(void *owner, long num_outs)
        : m_owner(owner), m_processed(1)
        {
            m_buffers.resize(num_outs, nullptr);
        }

        thread_slot(const thread_slot& other)
        : m_owner(other.m_owner), m_processed(1), m_buffers(other.m_buffers) {}

        void **get_buffers() { return m_buffers.data(); }

        void *m_owner;
        std::atomic<int> m_processed;
        std::vector<void *> m_buffers;
    };

    thread_set(t_object *owner, ProcFunc *process, long num_threads, long num_outs);
    ~thread_set();

    long get_num_threads() const { return m_thread_slots.size(); }

    void tick(long vec_size, long num_threads, void **outs);
    bool resize_buffers(t_ptr_int size);

    template <typename T>
    T *get_thread_buffer(long thread, long index)
    {
        return (T *) m_thread_slots[thread].m_buffers[index];
    }

    static void thread_entry(void *arg);

private:

    void processing_loop(long thread_idx);

    t_object *m_owner;
    ProcFunc *m_process;

    long m_active;
    long m_vec_size;
    t_ptr_int m_buffer_size;

    semaphore m_semaphore;
    std::vector<std::unique_ptr<thread>> m_threads;
    std::vector<thread_slot> m_thread_slots;
};

#endif  /* _THREADSET_H_ */
