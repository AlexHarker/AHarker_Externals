
/*
 *  dynamic.patch~
 *
 *	dynamic.patch~ is a legacy attempt at an equivalent to thispoly~ for dynamicdsp~. It has now been superceeded by dynamic.this~.
 *	
 *	It is included for backwards compatibilty purposes, and provides similar functionaily to dynamic.this~.
 *	dynamic.this~ has a more similar message / object structure to thispoly~ and allows more flexible control over audio muting and voice busy state.
 * 
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <dynamic~.hpp>


t_class *this_class;


struct t_dynamic_patch
{
    t_object x_obj;
	
	void *m_outlet;
	void *m_clock;
	
	long index;
	
	void *dynamic_parent;
};


t_symbol *ps_deletepatch; 


void *dynamic_patch_new(t_atom_long state);
void dynamic_patch_free(t_dynamic_patch *x);
void dynamic_patch_loadbang(t_dynamic_patch *x);
void dynamic_patch_bang(t_dynamic_patch *x);
void dynamic_patch_delete(t_dynamic_patch *x);
void clock_delete(t_dynamic_patch *x);
void dynamic_patch_int(t_dynamic_patch *x, t_atom_long intin);
void dynamic_patch_assist(t_dynamic_patch *x, void *b, long m, long a, char *s);


int C74_EXPORT main()
{
    this_class = class_new("dynamic.patch~",
						   (method)dynamic_patch_new, 
						   (method)dynamic_patch_free, 
						   sizeof(t_dynamic_patch), 
						   NULL, 
						   A_DEFLONG, 
						   0);
	
	class_addmethod(this_class, (method)dynamic_patch_assist, "assist", A_CANT, 0);
	class_addmethod(this_class, (method)dynamic_patch_loadbang, "loadbang", A_CANT, 0);
	class_addmethod(this_class, (method)dynamic_patch_delete, "delete", 0);
	class_addmethod(this_class, (method)dynamic_patch_int, "int", A_LONG, 0);
	class_addmethod(this_class, (method)dynamic_patch_bang, "bang", 0);
	
	class_register(CLASS_BOX, this_class);
	
	ps_deletepatch = gensym("deletepatch");
	
	return 0;
}

void *dynamic_patch_new(t_atom_long state)
{
    t_dynamic_patch *x = (t_dynamic_patch *) object_alloc(this_class);
	
    x->m_outlet = intout(x);
	x->m_clock = clock_new(x, (method)*clock_delete);
	
	x->dynamic_parent = dynamic_get_parent();
	x->index = dynamic_get_patch_index(x->dynamic_parent);
	
	dynamic_patch_int(x, state);
	
	return (x);
}

void dynamic_patch_free(t_dynamic_patch *x)
{
	freeobject((t_object *)x->m_clock);
}

void dynamic_patch_int(t_dynamic_patch *x, t_atom_long intin)
{	
    dynamic_set_patch_busy(x->dynamic_parent, x->index, intin);
	dynamic_set_patch_on(x->dynamic_parent, x->index, intin);
}

void dynamic_patch_loadbang(t_dynamic_patch *x)
{
	dynamic_patch_bang(x);
}

void dynamic_patch_bang(t_dynamic_patch *x)
{
	if (x->index)
		outlet_int(x->m_outlet, x->index);
}

void dynamic_patch_delete(t_dynamic_patch *x)
{
	clock_set(x->m_clock, 0L);
}

void clock_delete(t_dynamic_patch *x)
{
	t_atom arg;
	
	atom_setlong(&arg, x->index);
	
	if (x->dynamic_parent)
		typedmess(((t_object *)x->dynamic_parent), ps_deletepatch, 1, &arg);
}

void dynamic_patch_assist(t_dynamic_patch *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET) 
		sprintf(s,"Patch Index");
    else 
		sprintf(s,"Busy / On (0 or 1), Bang for Index");
}
