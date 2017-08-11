
/*
 *  dynamicdsp header
 *
 *	This header file provides an interface for querying and seting the state of a parent dynamicdsp object.
 *	All communication with a host object should be done using this interface - the dynamicdsp.c file has some more information.
 *	For examples of use see the dynamicdsp set of objects (dynamicdsp.in~ / dynamicdsp.out~ / dynamicdsp.this~ etc.).
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#ifndef _DYNAMICDSP_
#define _DYNAMICDSP_ 

#include <ext.h>

#define DynamicIsValid(x) \
(x && ((((t_symbol *) ob_sym (x)) == gensym("dynamicdsp~")) || (ob_sym (x)) == gensym("dynamicserial~")))

// Object Queries

// These routines *must* be called these routines at loadbang - they are bogus at any other time

static __inline void *Get_Dynamic_Object() 
{
	void *obj = (void *) gensym("___DynamicDSP~___")->s_thing;

	if (DynamicIsValid(obj))
		return obj;
	else
		return NULL;
}

static __inline long Get_Dynamic_Patch_Index(void *obj)
{
	if (DynamicIsValid(obj))
		return (long) gensym("___DynamicPatchIndex___")->s_thing;
	else
		return 0L;
}

// Signal IO Queries

static __inline long Dynamic_Get_Num_Sig_Ins(void *obj)
{
	if (DynamicIsValid(obj))
		return (long) mess0((t_object *)obj, gensym("get_num_sigins"));
	else
		return 0L;
}

static __inline long Dynamic_Get_Num_Sig_Outs(void *obj)
{
	if (DynamicIsValid(obj))
		return (long) mess0((t_object *)obj, gensym ("get_num_sigouts"));
	else
		return 0L;
}

static __inline void **Dynamic_Get_Sig_In_Ptrs(void *obj)
{
	if (DynamicIsValid(obj))
		return (void **) mess0((t_object *)obj, gensym ("get_sigins"));
	else	
		return NULL;
}

static __inline void ***Dynamic_Get_Sig_Out_Handle(void *obj, long index)
{
	if (DynamicIsValid(obj) && index > 0)
		return (void ***) mess1((t_object *)obj, gensym("get_sigouts"), (void *)index);
	else
		return NULL;
}

// State Queries

static __inline t_atom_long Dynamic_Get_Patch_On(void *obj, long index)
{
	if (DynamicIsValid(obj) && index > 0)
		return (t_atom_long) mess1((t_object *)obj, gensym("get_patch_on"), (void *)index);
	
	return 0;
}

static __inline t_atom_long Dynamic_Get_Patch_Busy(void *obj, long index)
{
    if (DynamicIsValid(obj) && index > 0)
        return (t_atom_long) mess1((t_object *)obj, gensym("get_patch_busy"), (void *)index);
    
    return 0;
}

static __inline void Dynamic_Set_Patch_Busy(void *obj, long index, t_atom_long state)
{
	if (DynamicIsValid(obj) && index > 0)
		mess2((t_object *)obj, gensym("set_patch_busy"), (void *)index, (void *)state);
}

static __inline void Dynamic_Set_Patch_On (void *obj, long index, t_atom_long state)
{
	if (DynamicIsValid(obj) && index > 0)
		mess2((t_object *)obj, gensym("set_patch_on"), (void *)index, (void *)state);
}

// Temporary Memory Queries //

static __inline void **Dynamic_Get_Temp_Mem(void *obj, long index, void **default_memory)
{
	if (DynamicIsValid(obj) && index > 0)
		return (void **) mess1((t_object *)obj, gensym("get_temp_mem"), (void *)index);
	else
		return default_memory;
}

static __inline long Dynamic_Temp_Mem_Resize(void *obj, long index, t_ptr_uint size)
{
    if (DynamicIsValid(obj) && index > 0)
        return (long) mess2((t_object *)obj, gensym("temp_mem_resize"), (void *)index, (void *)size);
    else
        return 0;
}

#endif		/* _DYNAMICDSP_	*/