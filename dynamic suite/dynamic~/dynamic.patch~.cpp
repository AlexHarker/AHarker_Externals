
/*
 *  dynamic.patch~
 *
 *  dynamic.patch~ is a legacy attempt at an equivalent to thispoly~ for dynamicdsp~.
 *  It has now been superceeded by dynamic.this~.
 *
 *  It is included for backwards compatibilty purposes.
 *  dynamic.this~ has a more similar functionality to thispoly~ in comparison to dynamic.patch~.
 *  dynamic.patch~ has less flexible control over audio muting and voice busy state.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <dynamic~.hpp>


// Globals and Object Structure

t_class *this_class;

t_symbol *ps_deletepatch = gensym("deletepatch");

struct t_dynamic_patch
{
    t_object x_obj;
    
    void *m_outlet;
    t_clock *m_clock;
    
    long index;
    
    void *dynamic_parent;
};

// Function Prototypes

void *dynamic_patch_new(t_atom_long state);
void dynamic_patch_free(t_dynamic_patch *x);
void dynamic_patch_assist(t_dynamic_patch *x, void *b, long m, long a, char *s);
void dynamic_patch_int(t_dynamic_patch *x, t_atom_long intin);
void dynamic_patch_bang(t_dynamic_patch *x);
void dynamic_patch_delete(t_dynamic_patch *x);
void clock_delete(t_dynamic_patch *x);

// Main

int C74_EXPORT main()
{
    this_class = class_new("dynamic.patch~",
                           (method) dynamic_patch_new,
                           (method) dynamic_patch_free,
                           sizeof(t_dynamic_patch),
                           (method) nullptr,
                           A_DEFLONG,
                           0);
    
    class_addmethod(this_class, (method) dynamic_patch_assist, "assist", A_CANT, 0);
    
    class_addmethod(this_class, (method) dynamic_patch_int, "int", A_LONG, 0);
    
    class_addmethod(this_class, (method) dynamic_patch_bang, "bang", 0);
    class_addmethod(this_class, (method) dynamic_patch_bang, "loadbang", A_CANT, 0);
    
    class_addmethod(this_class, (method) dynamic_patch_delete, "delete", 0);
    
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / Free / Assist

void *dynamic_patch_new(t_atom_long state)
{
    t_dynamic_patch *x = (t_dynamic_patch *) object_alloc(this_class);
    
    x->m_outlet = intout(x);
    x->m_clock = clock_new(x, (method) *clock_delete);
    
    x->dynamic_parent = dynamic_get_parent();
    x->index = dynamic_get_patch_index(x->dynamic_parent);
    
    dynamic_patch_int(x, state);
    
    return x;
}

void dynamic_patch_free(t_dynamic_patch *x)
{
    object_free(x->m_clock);
}

void dynamic_patch_assist(t_dynamic_patch *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
        sprintf(s,"Patch Index");
    else
        sprintf(s,"Busy / On (0 or 1), Bang for Index");
}

// State

void dynamic_patch_int(t_dynamic_patch *x, t_atom_long intin)
{
    dynamic_set_patch_busy(x->dynamic_parent, x->index, intin);
    dynamic_set_patch_on(x->dynamic_parent, x->index, intin);
}

// Get State

void dynamic_patch_bang(t_dynamic_patch *x)
{
    if (x->index)
        outlet_int(x->m_outlet, x->index);
}

// Delete

void dynamic_patch_delete(t_dynamic_patch *x)
{
    clock_set(x->m_clock, 0);
}

void clock_delete(t_dynamic_patch *x)
{
    if (x->dynamic_parent)
        object_method(x->dynamic_parent, ps_deletepatch, x->index);
}
