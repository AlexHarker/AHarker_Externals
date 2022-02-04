
#ifndef _AH_PRIVATE_OBJECTS_HPP_
#define _AH_PRIVATE_OBJECTS_HPP_


#include <ext.h>

#include <AH_Locks.hpp>


// Structure used for threadsaftey and counting (must be constructed / destructed)

struct t_private_count
{
    thread_lock lock;
    long count = 1;
};

// Create a Private Object

template <class T>
T *private_object_create(t_symbol *object_name, t_symbol *name, t_symbol *s_namespace, long argc, t_atom *argv)
{
    T *x = static_cast<T *>(object_new_typed(CLASS_NOBOX, object_name, argc, argv));
    return static_cast<T *>(object_register(s_namespace, name, x));
}

// Delete Private Object (in the low-priority thread)

template <class T>
void private_object_deferred_deletion(T *x, t_symbol *msg, long argc, t_atom *argv)
{
    object_free(x);
}

// Release a Private Object (deleting if necessary)

template <class T>
void private_object_release(T *x)
{
    if (!x)
        return;
                
    // Decrease the count with the lock held
        
    spin_lock_hold(&x->m_count.lock);
    
    // If this is the last client then unregister and free the object (deferlow in case the pointer is hanging around)
    
    if (--x->m_count.count <= 0)
    {
        object_unregister(x);
        defer_low(x, (method) private_object_deferred_deletion<T>, nullptr, 0, nullptr);
    }
}

// Find and Retain an Object

template <class T>
T *private_object_find_retain(T *prev, t_symbol *name, t_symbol *s_namespace)
{
    // See if an object is registered with this name that is still active and if so increase the count
    
    bool found = false;
    T *x = reinterpret_cast<T*>(object_findregistered(s_namespace, name));

    if (x && x != prev)
    {
        // Check that the object is still valid

        spin_lock_hold(&x->m_count.lock);
        
        if ((found = x->m_count.count > 0))
            x->m_count.count++;
    }
    
    // If there is no object found, it's just been deleted or it's the same as prev return the old object
    
    return found ? x : prev;
}

#endif /* _AH_PRIVATE_OBJECTS_HPP_ */
