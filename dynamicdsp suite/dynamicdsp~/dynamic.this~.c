
/*
 *  dynamic.this~
 *
 *	dynamic.this~ is like thispoly~ but for patchers hosted inside a dynamicdsp~ object.
 *	
 *	It can be used to control muting and busy status, as well as to safely delete the patch it exists in (without interrupting general audio processing).
 *	It is also the mechanism for determining which slot of the host object the patch is loaded into (similar to voice number in poly~).
 * 
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

// FIX - don't output before loadbang!

#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <dynamicdsp~.h>


void *this_class;


typedef struct _dynamic_this
{
    t_object x_obj;
	
	void *a_outlet;
	void *b_outlet;
	void *c_outlet;
	void *m_clock;
	
	long index;
	
	void *dynamicdsp_parent;
	
} t_dynamic_this;


t_symbol *ps_deletepatch; 


void *dynamic_this_new(t_atom_long on, t_atom_long busy);
void dynamic_this_free(t_dynamic_this *x);
void dynamic_this_busy_internal(t_dynamic_this *x, t_atom_long arg_val);
void dynamic_this_busy(t_dynamic_this *x, t_symbol *msg, long argc, t_atom *argv);
void dynamic_this_mute(t_dynamic_this *x, t_symbol *msg, long argc, t_atom *argv);
void dynamic_this_flags(t_dynamic_this *x, t_symbol *msg, long argc, t_atom *argv);
void dynamic_this_loadbang(t_dynamic_this *x);
void dynamic_this_bang(t_dynamic_this *x);
void dynamic_this_delete(t_dynamic_this *x);
void clock_delete(t_dynamic_this *x);
void dynamic_this_assist(t_dynamic_this *x, void *b, long m, long a, char *s);


int C74_EXPORT main(void)
{
    this_class = class_new("dynamic.this~",
						   (method)dynamic_this_new, 
						   (method)dynamic_this_free, 
						   sizeof(t_dynamic_this), 
						   NULL, 
						   A_DEFLONG,
						   A_DEFLONG, 
						   0);
	
	class_addmethod(this_class, (method)dynamic_this_assist, "assist", A_CANT, 0);
	class_addmethod(this_class, (method)dynamic_this_loadbang, "loadbang", A_CANT, 0);
	class_addmethod(this_class, (method)dynamic_this_delete, "delete", 0);
	class_addmethod(this_class, (method)dynamic_this_busy_internal, "int", A_LONG, 0);
	
	class_addmethod(this_class, (method)dynamic_this_busy, "busy", A_GIMME, 0);
	class_addmethod(this_class, (method)dynamic_this_mute, "mute", A_GIMME, 0);
	class_addmethod(this_class, (method)dynamic_this_flags, "flags", A_GIMME, 0);
	
	class_addmethod(this_class, (method)dynamic_this_bang, "bang", 0);
	
	class_register(CLASS_BOX, this_class);
	
	ps_deletepatch = gensym("deletepatch");
	
	return 0;
}

void *dynamic_this_new(t_atom_long on, t_atom_long busy)
{
    t_dynamic_this *x = (t_dynamic_this *) object_alloc(this_class);
    
	x->c_outlet = intout(x);
	x->b_outlet = intout(x);  
    x->a_outlet = intout(x);
	x->m_clock = clock_new(x, (method)*clock_delete);
	
	x->dynamicdsp_parent = Get_Dynamic_Object();
	x->index = Get_Dynamic_Patch_Index(x->dynamicdsp_parent);
	
    Dynamic_Set_Patch_Busy(x->dynamicdsp_parent, x->index, busy);
	Dynamic_Set_Patch_On(x->dynamicdsp_parent, x->index, on);
	
	return (x);
}

void dynamic_this_free(t_dynamic_this *x)
{
	freeobject((t_object *)x->m_clock);
}

void dynamic_this_busy(t_dynamic_this *x, t_symbol *msg, long argc, t_atom *argv)
{	
	if (argc)
		dynamic_this_busy_internal(x, atom_getlong(argv));
}

void dynamic_this_busy_internal(t_dynamic_this *x, t_atom_long arg_val)
{	
    Dynamic_Set_Patch_Busy(x->dynamicdsp_parent, x->index, arg_val);
	outlet_int(x->c_outlet, Dynamic_Get_Patch_Busy(x->dynamicdsp_parent, x->index));
}

void dynamic_this_mute(t_dynamic_this *x, t_symbol *msg, long argc, t_atom *argv)
{	
	t_atom_long arg_val;
	
	if (!argc)
		return;
	
	arg_val = atom_getlong(argv);
	
	Dynamic_Set_Patch_On(x->dynamicdsp_parent, x->index, !arg_val);
	outlet_int(x->b_outlet, !Dynamic_Get_Patch_On(x->dynamicdsp_parent, x->index));
}

void dynamic_this_flags(t_dynamic_this *x, t_symbol *msg, long argc, t_atom *argv)
{	
    t_atom_long arg_val;
	
	if (!argc)
		return;
	
	arg_val = atom_getlong(argv);
	
    Dynamic_Set_Patch_Busy(x->dynamicdsp_parent, x->index, arg_val);
	Dynamic_Set_Patch_On(x->dynamicdsp_parent, x->index, arg_val);
	outlet_int(x->c_outlet, Dynamic_Get_Patch_Busy(x->dynamicdsp_parent, x->index));
	outlet_int(x->b_outlet, !Dynamic_Get_Patch_On(x->dynamicdsp_parent, x->index));
}

void dynamic_this_loadbang(t_dynamic_this *x)
{
	dynamic_this_bang(x);
}

void dynamic_this_bang(t_dynamic_this *x)
{
	outlet_int(x->c_outlet, Dynamic_Get_Patch_Busy(x->dynamicdsp_parent, x->index));
	outlet_int(x->b_outlet, !Dynamic_Get_Patch_On(x->dynamicdsp_parent, x->index));
	if (x->index)
		outlet_int (x->a_outlet, x->index);
}

void dynamic_this_delete(t_dynamic_this *x)
{
	clock_set(x->m_clock, 0L);
}

void clock_delete(t_dynamic_this *x)
{
	t_atom arg;
	
	atom_setlong(&arg, x->index);
	
	if (x->dynamicdsp_parent)
		typedmess(((t_object *)x->dynamicdsp_parent), ps_deletepatch, 1, &arg);
}

void dynamic_this_assist(t_dynamic_this *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET) 
	{
		switch (a)
		{
			case 0:
				sprintf(s,"Patch Index");
				break;
			case 1:
				sprintf(s,"Mute Status");
				break;
			case 2:
				sprintf(s,"Busy Status");
				break;
		}
	}
    else 
	{
		sprintf(s,"Set Mute / Busy / Flags, Bang for Report");
    }
}