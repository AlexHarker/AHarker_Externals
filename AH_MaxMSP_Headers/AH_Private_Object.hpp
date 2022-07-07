
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

// Private Count Getter

template <class T>
using private_count_getter = t_private_count&(T&);

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
void private_object_release(T *x, private_count_getter<T> get)
{
    if (!x)
        return;
                
    // Decrease the count with the lock held
        
    safe_lock_hold(&get(*x).lock);
    
    // If this is the last client then unregister and free the object (deferlow in case the pointer is hanging around)
    
    if (--get(*x).count <= 0)
    {
        object_unregister(x);
        defer_low(x, (method) private_object_deferred_deletion<T>, nullptr, 0, nullptr);
    }
}

// Find and Retain an Object

template <class T>
T *private_object_find_retain(T *prev, t_symbol *name, t_symbol *s_namespace, private_count_getter<T> get)
{
    // See if an object is registered with this name that is still active and if so increase the count
    
    bool found = false;
    T *x = reinterpret_cast<T*>(object_findregistered(s_namespace, name));

    if (x && x != prev)
    {
        // Check that the object is still valid

        safe_lock_hold(&get(*x).lock);
        
        if ((found = get(*x).count > 0))
            get(*x).count++;
    }
    
    // If there is no object found (it has just been deleted or it is the same as prev) return the old object
    
    return found ? x : prev;
}

#endif /* _AH_PRIVATE_OBJECTS_HPP_ */
