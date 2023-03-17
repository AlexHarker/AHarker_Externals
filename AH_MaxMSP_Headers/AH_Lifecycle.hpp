
#ifndef _AH_LIFECYCLE_HPP_
#define _AH_LIFECYCLE_HPP_

#include <new>

template <typename T, typename ...Args>
void create_object(T& object, Args... args)
{
    new (&object) T(args...);
}

template <typename T>
void destroy_object(T& object)
{
    object.~T();
}

#endif
