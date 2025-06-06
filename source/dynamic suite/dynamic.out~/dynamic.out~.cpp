
/*
 *  dynamic.out~
 *
 *  dynamic.out~ acts like out~ but for patchers loaded inside a dynamicdsp~ object.
 *
 *  Unlike out~ you can change the signal outlet which the object refers to by sending an int to the object.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <z_dsp.h>
#include <ext_obex.h>

#include <simd_support.hpp>

#include <dynamic~.hpp>


// Globals and Object Structure

t_class *this_class;

struct t_dynamic_out
{
    t_pxobject x_obj;
    
    double ***out_handle;
    long num_sig_outs;
    
    t_atom_long outlet_num;
};

// Function prototypes

void *dynamic_out_new(t_atom_long out_num);
void dynamic_out_free(t_dynamic_out *x);
void dynamic_out_assist(t_dynamic_out *x, void *b, long m, long a, char *s);

void dynamic_out_int(t_dynamic_out *x, t_atom_long intin);

void dynamic_out_perform_scalar64(t_dynamic_out *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void dynamic_out_perform64(t_dynamic_out *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

void dynamic_out_dsp64(t_dynamic_out *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags);

// Main

int C74_EXPORT main()
{
    this_class = class_new("dynamic.out~",
                           (method) dynamic_out_new,
                           (method) dynamic_out_free,
                           sizeof(t_dynamic_out),
                           (method) nullptr,
                           A_DEFLONG,
                           0);
    
    class_addmethod(this_class, (method) dynamic_out_int, "int", A_LONG, 0);
    
    class_addmethod(this_class, (method) dynamic_out_dsp64, "dsp64", A_CANT, 0);
    class_addmethod(this_class, (method) dynamic_out_assist, "assist", A_CANT, 0);

    class_dspinit(this_class);
    
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / Free / Assist

void *dynamic_out_new(t_atom_long outlet_num)
{
    t_dynamic_out *x = (t_dynamic_out *) object_alloc(this_class);
    void *dynamicdsp_parent = dynamic_get_parent();
    
    x->out_handle = dynamic_get_sig_out_handle(dynamicdsp_parent, dynamic_get_patch_index(dynamicdsp_parent));
    x->num_sig_outs = dynamic_get_num_sig_outs(dynamicdsp_parent);
    
    x->outlet_num = outlet_num;
    
    dsp_setup((t_pxobject *) x, 1);
    
    return x;
}

void dynamic_out_free(t_dynamic_out *x)
{
    dsp_free(&x->x_obj);
}

void dynamic_out_assist(t_dynamic_out *x, void *b, long m, long a, char *s)
{
    static constexpr int maxAssist = 256;

    snprintf(s, maxAssist, "(signal) Signal Output %ld of Patcher / (int) Outlet Number",(long)  x->outlet_num);
}

// Int Handler

void dynamic_out_int (t_dynamic_out *x, t_atom_long intin)
{
    x->outlet_num = intin;
}

// Perform

// Template perform routine

template <typename T>
void dynamic_out_perform(t_dynamic_out *x, T *in, long N)
{
    T ***out_handle = reinterpret_cast<T ***>(x->out_handle);
    T *io_pointer;
    
    long num_sig_outs = x->num_sig_outs;
    t_atom_long outlet_num = x->outlet_num;
    
    if (!out_handle || !*out_handle)
        return;
    
    if (outlet_num >= 1 && outlet_num <= num_sig_outs)
    {
        if ((io_pointer = (*out_handle)[outlet_num - 1]))
        {
            for (long i = 0; i < N; i++, io_pointer++)
                *io_pointer = *io_pointer + *in++;
        }
    }
}

// Scalar Perform Routine

void dynamic_out_perform_scalar64(t_dynamic_out *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    dynamic_out_perform(x, ins[0], vec_size);
}

// SIMD Perform Routine

void dynamic_out_perform64(t_dynamic_out *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    using SIMD = htl::simd_type<double, htl::simd_limits<double>::max_size>;
    SIMD *in = reinterpret_cast<SIMD *>(ins[0]);
    
    dynamic_out_perform(x, in, vec_size / htl::simd_limits<double>::max_size);
}

// DSP

void dynamic_out_dsp64(t_dynamic_out *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags)
{
    constexpr int simd_width = htl::simd_limits<double>::max_size;
    
    // Use SIMD routines where possible
    
    if (max_vec >= simd_width)
        object_method(dsp64, gensym("dsp_add64"), x, dynamic_out_perform64, 0, nullptr);
    else
        object_method(dsp64, gensym("dsp_add64"), x, dynamic_out_perform_scalar64, 0, nullptr);
}
