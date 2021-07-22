
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

#include <ext.h>

static inline bool dynamic_is_dynamic_host(void *x)
{
    return x && (object_classname(x) == gensym("dynamicdsp~") ||
                 object_classname(x) == gensym("dynamicserial~"));
}

// FIX - timing of calls
// Object Queries (*must* be called these routines during patch loading)

static inline void *dynamic_get_parent()
{
	void *obj = gensym("__dynamic.host_object__")->s_thing;

    return dynamic_is_dynamic_host(obj) ? obj : nullptr;
}

static inline long dynamic_get_patch_index(void *obj)
{
	if (dynamic_is_dynamic_host(obj))
		return (long) gensym("__dynamic.patch_index__")->s_thing;
	else
		return 0L;
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
    return dynamic_object_query<void **>(obj, "query_num_sigins");
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
