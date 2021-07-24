
/*
 *  dynamic~ header
 *
 *	This header provides an interface for querying and seting the state of a parent dynamic object.
 *	All communication with a host object should be done using this interface.
 *	For uage examples see the dynamic set of objects (dynamicdsp.in~ / dynamicdsp.out~ / dynamicdsp.this~ etc.).
 *
 *  Copyright 2010-21 Alex Harker. All rights reserved.
 *
 */

#ifndef _DYNAMIC_INTERFACE_
#define _DYNAMIC_INTERFACE_

#include <algorithm>

#include <ext.h>

// Helper to check that an object that appears to be a host is of the correct type

static inline bool dynamic_is_dynamic_host(void *x)
{
    return x && (object_classname(x) == gensym("dynamicdsp~") ||
                 object_classname(x) == gensym("dynamicserial~"));
}

// Helper to allow traversal through parent patchers

static inline t_object *dynamic_traverse_assoc(t_object *p, void *obj)
{
    for (t_object *pp = p; pp; p = pp, pp = jpatcher_get_parentpatcher(p))
    {
        void *assoc = nullptr;
        object_method(pp, gensym("getassoc"), &assoc);
        
        if (assoc != obj)
        {
            if (!obj)
                p = pp;
            break;
        }
    }
    
    return p;
}

// Object Queries (*must* call these routines during new routines)

static inline void *dynamic_get_parent()
{
    void *obj = gensym("__dynamic.host_object__")->s_thing;
    
    if (!obj)
    {
        t_object *p = dynamic_traverse_assoc(gensym("#P")->s_thing, nullptr);
        object_method(p, gensym("getassoc"), &obj);
    }
    
    return dynamic_is_dynamic_host(obj) ? obj : nullptr;
}

static inline long dynamic_get_patch_index(void *obj)
{
    long index = 0;
    
	if (dynamic_is_dynamic_host(obj))
    {
        object_method(obj, gensym("loading_index"), &index);
        
        if (!index)
        {
            t_object *p = dynamic_traverse_assoc(gensym("#P")->s_thing, nullptr);
            p = dynamic_traverse_assoc(p, obj);
            index = std::max(0L, (long) object_method(obj,  gensym("getindex"), p));
        }
    }
	
    return index;
}

// Functions for querying the host object

void dynamic_object_method(void *obj, const char *str, void *ptr1, void *ptr2 = nullptr)
{
    if (dynamic_is_dynamic_host(obj))
        object_method(obj, gensym(str), ptr1, ptr2);
}

template <typename T>
void dynamic_object_method(void *obj, const char *str, long index, T value)
{
    if (index > 0)
        dynamic_object_method(obj, str, reinterpret_cast<void *>(index), reinterpret_cast<void *>(value));
}

template <typename T, typename... Args>
T dynamic_object_query(void *obj, const char *str, Args...args)
{
    T value = T(0);
    dynamic_object_method(obj, str, args..., &value);
    return value;
}

// Signal IO Queries

static inline long dynamic_get_num_sig_ins(void *obj)
{
    return dynamic_object_query<long>(obj, "query_num_sigins");
}

static inline long dynamic_get_num_sig_outs(void *obj)
{
    return dynamic_object_query<long>(obj, "query_num_sigouts");
}

static inline void **dynamic_get_sig_in_ptrs(void *obj)
{
    return dynamic_object_query<void **>(obj, "query_sigins");
}

static inline void ***dynamic_get_sig_out_handle(void *obj, long index)
{
    return dynamic_object_query<void ***>(obj, "query_sigouts", index);
}

// State Queries

static inline t_atom_long dynamic_get_patch_on(void *obj, long index)
{
    return dynamic_object_query<t_atom_long>(obj, "client_get_patch_on", index);
}

static inline t_atom_long dynamic_get_patch_busy(void *obj, long index)
{
    return dynamic_object_query<t_atom_long>(obj, "client_get_patch_busy", index);
}

static inline void dynamic_set_patch_busy(void *obj, long index, bool state)
{
    dynamic_object_method(obj, "client_set_patch_busy", index, state);
}

static inline void dynamic_set_patch_on(void *obj, long index, bool state)
{
    dynamic_object_method(obj, "client_set_patch_on", index, state);
}

#endif  /* _DYNAMIC_INTERFACE_	*/
