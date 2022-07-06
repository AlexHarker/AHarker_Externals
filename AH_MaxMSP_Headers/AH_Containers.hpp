
#include <algorithm>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

// N.B. the list owns the pointers

template <class T>
class threadsafe_pointer_list
{
public:
    
    using list_type = std::vector<T *>;
    
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
        
        list_type &operator()() { return m_current; }
        
    private:
        
        threadsafe_pointer_list& m_list;
        list_type& m_current;
    };
    
    threadsafe_pointer_list() : m_current(&m_vec_a), m_users(0), m_members(0) {}
    
    ~threadsafe_pointer_list()
    {
        clear();
    }
    
    // These can be called from the low priority thread only
    
    void add(T * ptr, size_t idx)
    {
        auto add_to_vector = [](list_type &v, T * ptr, size_t idx)
        {
            T *old_ptr = nullptr;
            
            if (v.size() < idx)
                v.resize(idx, nullptr);
            else
                ptr = v[idx];
            
            v[idx] = ptr;
            
            return old_ptr;
        };
        
        auto old_ptr = add_to_vector(next(), ptr, idx);
        
        if (!old_ptr)
            m_members++;
        
        swap();
        wait_till_safe_to_modify();
        add_to_vector(next(), ptr, idx);
        delete old_ptr;
    }

    void remove(size_t idx)
    {
        auto ptr = next()[idx];
        next()[idx] = nullptr;
        swap();
        
        wait_till_safe_to_modify();
                     
        if (!ptr)
            m_members--;
        
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

    size_t members() { return m_members.load(); }
    
private:
    
    // Safe from any thread
    
    list_type &acquire()
    {
        m_users++;
        return current();
    }
    
    void release()
    {
        m_users--;
    }
    
    list_type& current()
    {
        return *m_current.load();
    }
    
    // Only to be called from the low priority thread
    
    void empty()
    {
        auto &v = next();
        
        for (auto it = v.begin(); it != v.end(); it++)
            delete (*it);
        
        next().clear();
    }
    
    list_type& next()
    {
        if (current() == m_vec_b)
            return m_vec_a;
        else
            return m_vec_b;
    }
    
    void swap()
    {
        m_current.store(&next());
    }

    void wait_till_safe_to_modify()
    {
        using namespace std::chrono_literals;

        constexpr int max_iter_before_sleep = 5;
        int i = 0;
        
        while (m_users++ != 0)
        {
            m_users--;
    
            i = std::min(i++, max_iter_before_sleep);
            
            if (i == max_iter_before_sleep)
                std::this_thread::sleep_for(0.1ms);
        }
    }
    
    list_type m_vec_a;
    list_type m_vec_b;
    
    std::atomic<list_type *> m_current;
    
    std::atomic<long> m_users;
    std::atomic<size_t> m_members;
};
