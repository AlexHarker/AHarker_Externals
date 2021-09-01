
#include <assert.h>

#include "thread_set.hpp"

#ifdef __APPLE__

// Mac thread implementation

thread::thread(thread_function *function, void *arg)
: m_thread_function(function), m_arg(arg), m_valid(true)
{
    pthread_attr_t thread_attributes;
    sched_param scheduling_parameters;

    // Get default attributes and scheduling parameters

    pthread_attr_init(&thread_attributes);
    pthread_attr_getschedparam(&thread_attributes, &scheduling_parameters);

    // Set detach state and policy

    pthread_attr_setdetachstate(&thread_attributes, PTHREAD_CREATE_JOINABLE);

    if (maxversion() >= 0x800)
        pthread_attr_setschedpolicy(&thread_attributes, SCHED_RR);
    else
        pthread_attr_setschedpolicy(&thread_attributes, SCHED_FIFO);

    // Set the priority of the thread before we create it

    if (maxversion() >= 0x800)
        scheduling_parameters.sched_priority = 43;
    else
        scheduling_parameters.sched_priority = 63;

    // Set the scheduling attributes and create the thread

    pthread_attr_setschedparam(&thread_attributes, &scheduling_parameters);
    pthread_create(&m_internal, &thread_attributes, thread_start, this);
}

thread::~thread()
{
    assert(!m_valid && "Thread not closed before deletion");
}

void thread::join()
{
    if (m_valid)
    {
        m_valid = false;
        std::atomic_thread_fence(std::memory_order_seq_cst);

        // Wait for thread to join before we allow the program to continue

        pthread_join(m_internal, nullptr);
    }
}

void *thread::thread_start(void *arg)
{
    static_cast<thread *>(arg)->call();

    return nullptr;
}


// Mac semaphore implementation

semaphore::semaphore(long max_count) : m_valid(true)
{
    semaphore_create(mach_task_self(), &m_internal, SYNC_POLICY_FIFO, 0);
}

semaphore::~semaphore()
{
    assert(!m_valid && "Semaphore not closed before deletion");
    semaphore_destroy(mach_task_self(), m_internal);
}

void semaphore::close()
{
    if (m_valid)
    {
        m_valid = false;
        std::atomic_thread_fence(std::memory_order_seq_cst);
        semaphore_signal_all(m_internal);
    }
}

void semaphore::signal(long n)
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
    for (long i = 0; i < n; i++)
        semaphore_signal(m_internal);
}

bool semaphore::wait()
{
    if (m_valid)
        semaphore_wait(m_internal);

    return m_valid;
}

#else

// Windows thread implementation

thread::thread(thread_function *function, void *arg)
: m_thread_function(function), m_arg(arg), m_valid(true)
{
    // Create thread

    m_internal = CreateThread(nullptr, 0, thread_start, this, 0, nullptr);

    // Set priority

   SetThreadPriority(m_internal, THREAD_PRIORITY_TIME_CRITICAL);
}

thread::~thread()
{
    assert(!m_valid && "Thread not closed before deletion");
    CloseHandle(m_internal);
}

void thread::join()
{
    if (m_valid)
    {
        m_valid = false;
        std::atomic_thread_fence(std::memory_order_seq_cst);

        // Wait for thread to join before we allow the program to continue

        WaitForSingleObject(m_internal, INFINITE);
    }
}

DWORD WINAPI Thread::thread_start(LPVOID arg)
{
    static_cast<thread *>(arg)->call();

    return 0;
}

// Windows semaphore implementation

semaphore::semaphore(long max_count) : m_valid(true)
{
    m_internal.m_handle = CreateSemaphore(nullptr, 0, max_count, nullptr);
    m_internal.m_max_count = max_count;
}

semaphore::~semaphore()
{
    assert(!m_valid && "Semaphore not closed before deletion");
    CloseHandle(m_internal.m_handle);
}

void semaphore::close()
{
    if (m_valid)
    {
        m_valid = false;
        std::atomic_thread_fence(std::memory_order_seq_cst);

        // Signal maximum count to ensure all threads are released, and check for completion

        for (long n = m_internal.m_max_count; n > 0; n--)
        {
            if (ReleaseSemaphore(m_internal.m_handle, n, nullptr))
                break;
        }
    }
}

void semaphore::signal(long n)
{
    // N.B. - signalling is unsafe after the semaphore has been closed

    std::atomic_thread_fence(std::memory_order_seq_cst);
    ReleaseSemaphore(m_internal.m_handle, n, nullptr);
}

bool semaphore::wait()
{
    if (m_valid)
        WaitForSingleObject(m_internal.m_handle, INFINITE);

    return m_valid;
}

#endif


// thread_set class

thread_set::thread_set(t_object *owner, process_function *process, long num_threads, long num_outs)
: m_owner(owner), m_process(process)
, m_active(std::max(1L, num_threads)), m_vec_size(0), m_buffer_size(0)
, m_semaphore(m_active - 1), m_thread_slots(num_threads, thread_slot(this, num_outs))
{
    // Create threads

    for (long i = 0; i < num_threads - 1; i++)
        m_threads.emplace_back(new thread(&thread_entry, m_thread_slots.data() + i + 1));
}

thread_set::~thread_set()
{
    // Close semaphore and join threads

    m_semaphore.close();

    for (auto it = m_threads.begin(); it != m_threads.end(); it++)
        (*it)->join();

    // Free temporary buffers

    for (auto it = m_thread_slots.begin(); it != m_thread_slots.end(); it++)
        for (auto jt = it->m_buffers.begin(); jt != it->m_buffers.end(); jt++)
            deallocate_aligned(*jt);
}

void thread_set::tick(long vec_size, long num_threads, void **outs)
{
    // Set number active threads

    m_active = std::min(num_threads, get_num_threads());
    m_vec_size = vec_size;

    if (num_threads == 1)
    {
        m_process(m_owner, outs, vec_size, 0, 1);
        return;
    }

    // Set flags

    for (long i = 0; i < num_threads; i++)
        m_thread_slots[i].m_processed = 0;

    m_semaphore.signal(num_threads - 1);

    // Process thread

    m_process(m_owner, m_thread_slots[0].get_buffers(), vec_size, 0, num_threads);

    // Wait for all the other threads to return

    for (long i = 1; i < num_threads; i++)
        while (m_thread_slots[i].m_processed != 1);
}

bool thread_set::resize_buffers(t_ptr_int size)
{
    if (size != m_buffer_size)
    {
        for (auto it = m_thread_slots.begin(); it != m_thread_slots.end(); it++)
        {
            for (auto jt = it->m_buffers.begin(); jt != it->m_buffers.end(); jt++)
            {
                deallocate_aligned(*jt);
                *jt = allocate_aligned<u_int8_t>(size);
                if (!*jt)
                {
                    object_error((t_object *) m_owner, "not enough memory");
                    m_buffer_size = 0;
                    return true;
                }
            }
        }
    }

    m_buffer_size = size;

    return false;
}

// Main thread loop methods

void thread_set::thread_entry(void *arg)
{
    thread_set::thread_slot *slot = static_cast<thread_set::thread_slot *>(arg);
    thread_set *set = static_cast<thread_set *>(slot->m_owner);

    set->processing_loop(slot - set->m_thread_slots.data());
}

void thread_set::processing_loop(long thread_idx)
{
    while (m_semaphore.wait())
    {
        // Loop through possible threads in case we are woken multiple times

        for (long i = thread_idx; i < thread_idx + m_active - 1; i++)
        {
            // N.B. Get values from thread each time in case they have been changed

            long idx = (i % (m_active - 1)) + 1;
            int expected = 0;

            if (m_thread_slots[idx].m_processed.compare_exchange_strong(expected, 2))
            {
                m_process(m_owner, m_thread_slots[idx].get_buffers(), m_vec_size, idx, m_active);
                m_thread_slots[idx].m_processed = 1;
            }
        }
    }
}
