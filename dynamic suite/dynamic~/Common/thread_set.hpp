
#ifndef _THREADSET_HPP_
#define _THREADSET_HPP_

#include <atomic>
#include <thread>
#include <vector>

#include <ext.h>
#include <stdint.h>

#include <simd_support.hpp>

#ifdef __APPLE__

#include <libkern/OSAtomic.h>
#include <pthread.h>
#include <mach/semaphore.h>
#include <mach/task.h>

// Mac OS specific definitions

using os_thread = pthread_t;
using os_semaphore = semaphore_t;
using os_thread_function = void *(void *arg);

#else

// Windows OS specific definitions

#include <windows.h>

using os_thread = HANDLE;
using os_semaphore = struct win_semaphore { HANDLE m_handle; long m_max_count; };
using os_thread_function = DWORD WINAPI(LPVOID arg);

#endif


// Lightweight thread

struct thread
{
    typedef void thread_function(void *);

public:

    thread(thread_function *function, void *arg);
    ~thread();

    // Non-copyable

    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;

    void join();

private:

    // thread_start is a OS-specific wrapper to call the object which calls the relevant static function

    static os_thread_function thread_start;
    void call() { m_thread_function(m_arg); }

    // Data

    os_thread m_internal;
    thread_function *m_thread_function;
    void *m_arg;
    bool m_valid;
};


// semaphore (note that you should call close() before the destructor is called)

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

    os_semaphore m_internal;
    bool m_valid;
};


// thread_set class

class thread_set
{

public:

    typedef void process_function(t_object *, double **, long, long, long);

    struct thread_slot
    {
        thread_slot(void *owner, long num_outs)
        : m_owner(owner), m_processed(1)
        {
            m_buffers.resize(num_outs, nullptr);
        }

        thread_slot(const thread_slot& other)
        : m_owner(other.m_owner), m_processed(1), m_buffers(other.m_buffers) {}

        double **get_buffers() { return m_buffers.data(); }

        void *m_owner;
        std::atomic<int> m_processed;
        std::vector<double *> m_buffers;
    };

    thread_set(t_object *owner, process_function *process, long num_threads, long num_outs);
    ~thread_set();

    long get_num_threads() const { return static_cast<long>(m_thread_slots.size()); }

    void tick(long vec_size, long num_threads, double **outs);
    bool resize_buffers(t_ptr_int size);

    double *get_thread_buffer(long thread, long index)
    {
        return m_thread_slots[thread].m_buffers[index];
    }

    static void thread_entry(void *arg);

private:

    void processing_loop(long thread_idx);

    t_object *m_owner;
    process_function *m_process;

    long m_active;
    long m_vec_size;
    t_ptr_int m_buffer_size;

    semaphore m_semaphore;
    std::vector<std::unique_ptr<thread>> m_threads;
    std::vector<thread_slot> m_thread_slots;
};

#endif /* _THREADSET_HPP_ */
