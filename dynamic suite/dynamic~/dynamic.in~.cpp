
/*
 *  dynamic.in~
 *
 *  dynamic.in~ acts like in~ but for patchers loaded inside a dynamic~ host object.
 *
 *  Unlike in~ you can change the signal inlet which the object refers to by sending an int to the object.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <dynamic~.hpp>


// Class and object structure

t_class *this_class;

struct t_dynamic_in
{
    t_pxobject x_obj;
    
    long num_sig_ins;
    void **sig_ins;
    
    t_atom_long inlet_num;
    bool valid;
};

// Function prototypes

void *dynamic_in_new(t_atom_long inlet_num);
void dynamic_in_assist(t_dynamic_in *x, void *b, long m, long a, char *s);
void dynamic_in_free(t_dynamic_in *x);

void dynamic_in_int(t_dynamic_in *x, t_atom_long inlet_num);

t_int *dynamic_in_perform(t_int *w);
void dynamic_in_perform64(t_dynamic_in *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

void dynamic_in_dsp(t_dynamic_in *x, t_signal **sp, short *count);
void dynamic_in_dsp64(t_dynamic_in *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

// Main

int C74_EXPORT main()
{
    this_class = class_new("dynamic.in~",
                           (method) dynamic_in_new,
                           (method) dynamic_in_free,
                           sizeof(t_dynamic_in),
                           nullptr,
                           A_DEFLONG,
                           0);
    
    class_addmethod(this_class, (method) dynamic_in_dsp, "dsp", A_CANT, 0);
    class_addmethod(this_class, (method) dynamic_in_dsp64, "dsp64", A_CANT, 0);
    class_addmethod(this_class, (method) dynamic_in_assist, "assist", A_CANT, 0);
    class_addmethod(this_class, (method) dynamic_in_int, "int", A_LONG, 0);
    
    class_dspinit(this_class);
    
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / Free / Assist

void *dynamic_in_new(t_atom_long inlet_num)
{
    t_dynamic_in *x = (t_dynamic_in *) object_alloc(this_class);
    void *dynamic_parent = dynamic_get_parent();
    
    x->num_sig_ins = dynamic_get_num_sig_ins(dynamic_parent);
    x->sig_ins = dynamic_get_sig_in_ptrs(dynamic_parent);
    x->inlet_num = -1;
    x->valid = false;
    
    dsp_setup((t_pxobject *)x, 1);
    outlet_new((t_object *)x, "signal");
    
    dynamic_in_int(x, inlet_num);
    
    return x;
}

void dynamic_in_free(t_dynamic_in *x)
{
    dsp_free(&x->x_obj);
}

void dynamic_in_assist(t_dynamic_in *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
        sprintf(s,"(signal) Signal Input %ld of Patcher", (long) x->inlet_num);
    else
        sprintf(s,"(int) Inlet Number");
}

// Int Handler

void dynamic_in_int(t_dynamic_in *x, t_atom_long inlet_num)
{
    x->valid = false;
    
    if (inlet_num >= 1 && inlet_num <= x->num_sig_ins)
    {
        x->inlet_num = inlet_num;
        x->valid = true;
    }
}

// Perform / DSP

t_int *dynamic_in_perform(t_int *w)
{
    float *out = (float *)(w[1]);
    long vec_size = w[2];
    t_dynamic_in *x = (t_dynamic_in *)(w[3]);
    
    if (x->valid)
        memcpy(out, x->sig_ins[x->inlet_num - 1], vec_size * sizeof(float));
    else
        memset(out, 0, vec_size * sizeof(float));
    
    return w + 4;
}

void dynamic_in_perform64(t_dynamic_in *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    if (x->valid)
        memcpy(outs[0], x->sig_ins[x->inlet_num - 1], vec_size * sizeof(double));
    else
        memset(outs[0], 0, vec_size * sizeof(double));
}

void dynamic_in_dsp(t_dynamic_in *x, t_signal **sp, short *count)
{
    dsp_add(dynamic_in_perform, 3, sp[1]->s_vec, sp[0]->s_n, x);
}

void dynamic_in_dsp64(t_dynamic_in *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    object_method(dsp64, gensym("dsp_add64"), x, dynamic_in_perform64, 0, nullptr);
}

