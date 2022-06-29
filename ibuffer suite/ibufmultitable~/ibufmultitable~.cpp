
/*
 *  ibufmultitable~
 *
 *  ibufmultitable~ is an efficient object designed for table lookup (for window functions etc.) for multiple functions.
 *  The functions stored in a single buffer.
 *  It is an extended version of the ibufmultitable~ object.
 *
 *  ibufmultitable~ features SIMD optimisation and four types of interpolation.
 *  Linear interpolation and three different kinds of cubic interpolation are supported.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <ibuffer_access.hpp>

#include <algorithm>


t_class *this_class;

struct t_ibufmultitable
{
    t_pxobject x_obj;
    
    t_symbol *buffer_name;
    
    InterpType interp_type;
    
    t_atom_long chan;
    t_atom_long start_samp;
    t_atom_long end_samp;
};

// Function Prototypes

void *ibufmultitable_new(t_symbol *s, long argc, t_atom *argv);
void ibufmultitable_free(t_ibufmultitable *x);
void ibufmultitable_assist(t_ibufmultitable *x, void *b, long m, long a, char *s);

void ibufmultitable_set(t_ibufmultitable *x, t_symbol *s);

void ibufmultitable_perform64(t_ibufmultitable *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void ibufmultitable_dsp64(t_ibufmultitable *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags);

// Main

int C74_EXPORT main()
{
    this_class = class_new("ibufmultitable~",
                           (method) ibufmultitable_new,
                           (method) ibufmultitable_free,
                           sizeof(t_ibufmultitable),
                           (method) nullptr,
                           A_GIMME,
                           0);
    
    class_addmethod(this_class, (method) ibufmultitable_set, "set", A_SYM, 0);
    class_addmethod(this_class, (method) ibufmultitable_assist, "assist", A_CANT, 0);
    class_addmethod(this_class, (method) ibufmultitable_dsp64, "dsp64", A_CANT, 0);
    
    // Add Attributes
    
    add_ibuffer_interp_attribute<t_ibufmultitable, InterpType::Linear>(this_class, "interp");
    
    CLASS_ATTR_LONG(this_class, "chan", 0L, t_ibufmultitable, chan);
    CLASS_ATTR_FILTER_MIN(this_class, "chan", 1);
    CLASS_ATTR_LABEL(this_class, "chan", 0L, "Buffer Channel");
    
    CLASS_ATTR_LONG(this_class, "startsamp", 0L, t_ibufmultitable, start_samp);
    CLASS_ATTR_FILTER_MIN(this_class, "startsamp", 0);
    CLASS_ATTR_LABEL(this_class, "startsamp", 0L, "Start Sample");
    
    CLASS_ATTR_LONG(this_class, "endsamp", 0L, t_ibufmultitable, end_samp);
    CLASS_ATTR_FILTER_MIN(this_class, "endsamp", 0);
    CLASS_ATTR_LABEL(this_class, "endsamp", 0L, "End Sample");
    
    CLASS_ATTR_ORDER(this_class, "interp", 0L, "1");
    CLASS_ATTR_ORDER(this_class, "chan", 0L, "2");
    CLASS_ATTR_ORDER(this_class, "startsamp", 0L, "3");
    CLASS_ATTR_ORDER(this_class, "endsamp", 0L, "4");
    
    class_dspinit(this_class);
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / Free / Assist

void *ibufmultitable_new(t_symbol *s, long argc, t_atom *argv)
{
    t_ibufmultitable *x = (t_ibufmultitable *) object_alloc(this_class);
    
    dsp_setup((t_pxobject *) x, 2);
    outlet_new((t_object *) x, "signal");
    
    // Default variables
    
    t_symbol *buffer_name = nullptr;
    t_atom_long start_samp = 0;
    t_atom_long end_samp = 512;
    t_atom_long chan = 1;
    
    // Arguments
    
    long non_attr_argc = attr_args_offset(static_cast<short>(argc), argv);
    
    x->buffer_name = non_attr_argc > 0 ? atom_getsym(argv + 0) : buffer_name;
    x->start_samp = non_attr_argc > 1 ? atom_getlong(argv + 1) : std::max(start_samp, static_cast<t_atom_long>(0));
    x->end_samp = non_attr_argc > 2 ? atom_getlong(argv + 2) : std::max(end_samp, static_cast<t_atom_long>(0));
    x->chan = non_attr_argc > 3 ? atom_getlong(argv + 3) : std::max(chan, static_cast<t_atom_long>(1));
    
    x->interp_type = InterpType::Linear;
    
    // Set attributes from arguments
    
    attr_args_process(x, static_cast<short>(argc), argv);
    
    return x;
}

void ibufmultitable_free(t_ibufmultitable *x)
{
    dsp_free(&x->x_obj);
}

void ibufmultitable_assist(t_ibufmultitable *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
    {
        sprintf(s,"(signal) Output");
    }
    else
    {
        switch (a)
        {
            case 0:
                sprintf(s,"(signal) Position Input (0-1)");
                break;
                
            case 1:
                sprintf(s,"(signal) Sample Offset");
                break;
        }
    }
}

// Buffer Setting

void ibufmultitable_set(t_ibufmultitable *x, t_symbol *s)
{
    ibuffer_data buffer(s);
    
    x->buffer_name = s;
    
    if (buffer.get_type() == kBufferNone)
        object_error((t_object *) x, "ibufmultitable~: no buffer %s", s->s_name);
}

// Core Perform Routine

template <int N, class T>
void perform_positions(T *positions, const T *in, const T *offsets, long n_samps, double start, double end, double last)
{
    SIMDType<T, N> *v_positions = reinterpret_cast<SIMDType<T, N> *>(positions);
    const SIMDType<T, N> *v_in = reinterpret_cast<const SIMDType<T, N> *>(in);
    const SIMDType<T, N> *v_offsets = reinterpret_cast<const SIMDType<T, N> *>(offsets);
    
    const SIMDType<T, N> mul(static_cast<T>(end - start));
    const SIMDType<T, N> add(static_cast<T>(start));
    const SIMDType<T, N> limit(static_cast<T>(last));
    const SIMDType<T, N> zero(static_cast<T>(0));
    const SIMDType<T, N> one(static_cast<T>(1));
    
    long n_vecs = n_samps / N;
    
    for (long i = 0; i < n_vecs; i++)
    {
        SIMDType<T, N> position = min(one, max(zero, *v_in++)) * mul + add + *v_offsets++;
        v_positions[i] = min(limit, max(zero, position));
    }
}

template <typename T>
T clip(const t_ptr_int in, const t_ptr_int max)
{
    return static_cast<T>(std::min(max, std::max(t_ptr_int(0), in)));
}

template <class T>
void perform_core(t_ibufmultitable *x, const T *in, const T *offset_in, T *out, long vec_size)
{
    // Check if the buffer is set / valid and get the length information
    
    ibuffer_data buffer(x->buffer_name);
    
    double start_samp = clip<double>(x->start_samp, buffer.get_length() - 1);
    double end_samp = clip<double>(x->end_samp, buffer.get_length() - 1);
    long chan = clip<long>(x->chan - 1, buffer.get_num_chans() - 1);
    double last_samp = static_cast<double>(buffer.get_length() - 1);
    
    // Calculate output
    
    if (buffer.get_length() && (buffer.get_length() - start_samp) >= 1)
    {
        // Positions
        
        constexpr int N = SIMDLimits<T>::max_size;
        
        const long v_count = vec_size / N;
        const long S = v_count * N;
        
        // Read from buffer
        
        perform_positions<N>(out + 0, in + 0, offset_in + 0, v_count, start_samp, end_samp, last_samp);
        perform_positions<1>(out + S, in + S, offset_in + S, (v_count - S), start_samp, end_samp, last_samp);
        
        ibuffer_read(buffer, out, out, vec_size, chan, 1.f, x->interp_type);
    }
    else
        std::fill_n(out, vec_size, T(0));
}

// Perform and DSP

void ibufmultitable_perform64(t_ibufmultitable *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    perform_core(x, ins[0], ins[1], outs[0], vec_size);
}

void ibufmultitable_dsp64(t_ibufmultitable *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags)
{
    // Set buffer again in case it is no longer valid / extant
    
    ibufmultitable_set(x, x->buffer_name);
    
    object_method(dsp64, gensym("dsp_add64"), x, ibufmultitable_perform64);
}
