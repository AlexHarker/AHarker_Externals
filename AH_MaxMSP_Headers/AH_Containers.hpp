
#include <algorithm>
#include <atomic>
#include <chrono>
#include <vector>

// N.B. the list owns the pointers

template <class T>
class threadsafe_pointer_list
{
    enum { max_iter_before_sleep = 5 };

public:
    
    // Can be created from any thread
    // It is then safe to act on any of the contents, but not store pointers/defer on any pointer
    
    struct access
    {
        access(threadsafe_pointer_list& list) : m_list(list), m_current(m_list.acquire())
        {}
        
        ~access()
        {
            m_list.release();
        }
        
        std::vector<T *> &get() { return m_current; }
        
    private:
        
        std::vector<T *>& m_current;
        threadsafe_pointer_list& m_list;
    };
    
    threadsafe_pointer_list() : m_current(m_vec_a), m_users(0) {}
    
    ~threadsafe_pointer_list()
    {
        clear();
    }
    
    // These can be called from the low priority thread only
    
    void add(T * ptr, size_t idx)
    {
        auto add_to_vector = [](std::vector<T *> &v, T * ptr, size_t idx)
        {
            if (v.size() < idx)
                v.resize(idx, nullptr);
            
            v[idx] = ptr;
        };
        
        add_to_vector(next());
        swap();
        wait_till_safe_to_modify();
        add_to_vector(next());
    }

    void remove(size_t idx)
    {
        auto ptr = next()[idx];
        next()[idx] = nullptr;
        swap();
        
        wait_till_safe_to_modify();
                     
        next()[idx] = nullptr;
        delete ptr;
    }

    void clear()
    {
        next().clear();
        swap();
        wait_till_safe_to_modify();
        empty();
    }

private:
    
    // Safe from any thread
    
    std::vector<T *> &acquire()
    {
        m_users++;
        return current();
    }
    
    void release()
    {
        m_users--;
    }
    
    std::vector<T *>& current()
    {
        return m_current.load();
    }
    
    // Only to be called from the low priority thread
    
    void empty()
    {
        auto &v = next();
        
        for (auto it = v.begin(); it != v.end(); it++)
            delete (*it);
        
        next().clear();
    }
    
    std::vector<T *>& next()
    {
        if (current() == m_vec_b)
            return m_vec_a;
        else
            return m_vec_b;
    }
    
    void swap()
    {
        m_current.store(next());
    }

    void wait_till_safe_to_modify()
    {
        using namespace std::chrono_literals;

        int i = 0;
        
        while (m_users++ != 0)
        {
            m_users--;
    
            i = std::min(i++, max_iter_before_sleep);
            
            if (i == max_iter_before_sleep)
                std::this_thread::sleep_for(0.1ms);
        }
    }
    
    std::vector<T *> m_vec_a;
    std::vector<T *> m_vec_b;
    
    std::atomic<std::vector<T *>&> m_current;
    
    std::atomic<long> m_users;
};
