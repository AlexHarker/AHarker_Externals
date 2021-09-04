
/*
 *  dynamic.this~
 *
 *  dynamic.this~ is like thispoly~ but for patchers hosted inside a dynamic~ object.
 *
 *  It can be used to control muting and busy status for the given patch.
 *  It can also be used to safely delete the patch it exists in (without interrupting other audio processing).
 *  Finally, it is the mechanism for determining which slot of the host object the patch is loaded into.
 *
 *  Copyright 2010-21 Alex Harker. All rights reserved.
 *
 */

#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <dynamic~.hpp>


// Globals and Object Structure

t_class *this_class;

t_symbol *ps_deletepatch;

struct t_dynamic_this
{
    t_object x_obj;
    
    void *a_outlet;
    void *b_outlet;
    void *c_outlet;
    void *m_clock;
    
    long index;
    
    void *dynamic_parent;
};

// Function Prototypes

void *dynamic_this_new(t_atom_long on, t_atom_long busy);
void dynamic_this_free(t_dynamic_this *x);
void dynamic_this_busy_internal(t_dynamic_this *x, t_atom_long arg_val);
void dynamic_this_busy(t_dynamic_this *x, t_symbol *msg, long argc, t_atom *argv);
void dynamic_this_mute(t_dynamic_this *x, t_symbol *msg, long argc, t_atom *argv);
void dynamic_this_flags(t_dynamic_this *x, t_symbol *msg, long argc, t_atom *argv);
void dynamic_this_bang(t_dynamic_this *x);
void dynamic_this_output_busy(t_dynamic_this *x);
void dynamic_this_output_mute(t_dynamic_this *x);
void dynamic_this_delete(t_dynamic_this *x);
void clock_delete(t_dynamic_this *x);
void dynamic_this_assist(t_dynamic_this *x, void *b, long m, long a, char *s);

// Main

int C74_EXPORT main()
{
    this_class = class_new("dynamic.this~",
                           (method) dynamic_this_new,
                           (method) dynamic_this_free,
                           sizeof(t_dynamic_this),
                           nullptr,
                           A_DEFLONG,
                           A_DEFLONG,
                           0);
    
    class_addmethod(this_class, (method) dynamic_this_assist, "assist", A_CANT, 0);
    
    class_addmethod(this_class, (method) dynamic_this_busy_internal, "int", A_LONG, 0);
    class_addmethod(this_class, (method) dynamic_this_busy, "busy", A_GIMME, 0);
    class_addmethod(this_class, (method) dynamic_this_mute, "mute", A_GIMME, 0);
    class_addmethod(this_class, (method) dynamic_this_flags, "flags", A_GIMME, 0);
    
    class_addmethod(this_class, (method) dynamic_this_bang, "bang", 0);
    class_addmethod(this_class, (method) dynamic_this_bang, "loadbang", A_CANT, 0);
    
    class_addmethod(this_class, (method) dynamic_this_output_busy, "changebusy", A_CANT, 0);
    class_addmethod(this_class, (method) dynamic_this_output_mute, "changeon", A_CANT, 0);
    
    class_addmethod(this_class, (method) dynamic_this_delete, "delete", 0);
    
    class_register(CLASS_BOX, this_class);
    
    ps_deletepatch = gensym("deletepatch");
    
    return 0;
}

// New / Free / Assist

void *dynamic_this_new(t_atom_long on, t_atom_long busy)
{
    t_dynamic_this *x = (t_dynamic_this *) object_alloc(this_class);
    
    x->c_outlet = intout(x);
    x->b_outlet = intout(x);
    x->a_outlet = intout(x);
    x->m_clock = clock_new(x, (method) *clock_delete);
    
    x->dynamic_parent = dynamic_get_parent();
    x->index = dynamic_get_patch_index(x->dynamic_parent);
    
    dynamic_set_patch_busy(x->dynamic_parent, x->index, busy);
    dynamic_set_patch_on(x->dynamic_parent, x->index, on);
    
    dynamic_register_listener(x->dynamic_parent, x->index, x);
    
    return x;
}

void dynamic_this_free(t_dynamic_this *x)
{
    freeobject((t_object *)x->m_clock);
    dynamic_unregister_listener(x->dynamic_parent, x->index, x);
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

// State

void dynamic_this_busy(t_dynamic_this *x, t_symbol *msg, long argc, t_atom *argv)
{
    if (argc)
        dynamic_this_busy_internal(x, atom_getlong(argv));
}

void dynamic_this_busy_internal(t_dynamic_this *x, t_atom_long arg_val)
{
    dynamic_set_patch_busy(x->dynamic_parent, x->index, arg_val);
}

void dynamic_this_mute(t_dynamic_this *x, t_symbol *msg, long argc, t_atom *argv)
{
    t_atom_long arg_val;
    
    if (!argc)
        return;
    
    arg_val = atom_getlong(argv);
    
    dynamic_set_patch_on(x->dynamic_parent, x->index, !arg_val);
}

void dynamic_this_flags(t_dynamic_this *x, t_symbol *msg, long argc, t_atom *argv)
{
    t_atom_long arg_val;
    
    if (!argc)
        return;
    
    arg_val = atom_getlong(argv);
    
    dynamic_set_patch_busy(x->dynamic_parent, x->index, arg_val);
    dynamic_set_patch_on(x->dynamic_parent, x->index, arg_val);
}

// Get State

void dynamic_this_bang(t_dynamic_this *x)
{
    dynamic_this_output_busy(x);
    dynamic_this_output_mute(x);
    if (x->index)
        outlet_int(x->a_outlet, x->index);
}

// Output State

void dynamic_this_output_busy(t_dynamic_this *x)
{
    outlet_int(x->c_outlet, dynamic_get_patch_busy(x->dynamic_parent, x->index));
}

void dynamic_this_output_mute(t_dynamic_this *x)
{
    outlet_int(x->b_outlet, !dynamic_get_patch_on(x->dynamic_parent, x->index));
}

// Delete

void dynamic_this_delete(t_dynamic_this *x)
{
    clock_set(x->m_clock, 0L);
}

void clock_delete(t_dynamic_this *x)
{
    if (x->dynamic_parent)
        object_method(x->dynamic_parent, ps_deletepatch, x->index);
}

