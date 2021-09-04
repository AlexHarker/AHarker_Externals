
/*
 *  ibufmultitable~
 *
 *  ibufmultitable~ is an efficient object designed for table lookup (for window functions etc.) with multiple functions stored in a single buffer. It is an extended version of the ibufmultitable~ object.
 *
 *  ibufmultitable~ features SIMD optimisation and four types of interpolation (linear interpolation and three different kinds of cubic interpolation which can be requested as desired.
 *
 *  Copyright 2010-21 Alex Harker. All rights reserved.
 *
 */

#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Denormals.h>
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


void *ibufmultitable_new(t_symbol *s, long argc, t_atom *argv);
void ibufmultitable_free(t_ibufmultitable *x);
void ibufmultitable_assist(t_ibufmultitable *x, void *b, long m, long a, char *s);

void ibufmultitable_set(t_ibufmultitable *x, t_symbol *msg, long argc, t_atom *argv);
void ibufmultitable_set_internal(t_ibufmultitable *x, t_symbol *s);

t_int *ibufmultitable_perform(t_int *w);
void ibufmultitable_dsp(t_ibufmultitable *x, t_signal **sp, short *count);

void ibufmultitable_perform64(t_ibufmultitable *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void ibufmultitable_dsp64(t_ibufmultitable *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);


int C74_EXPORT main()
{
    this_class = class_new("ibufmultitable~",
                           (method)ibufmultitable_new,
                           (method)ibufmultitable_free,
                           sizeof(t_ibufmultitable),
                           NULL,
                           A_GIMME,
                           0);
    
    class_addmethod(this_class, (method)ibufmultitable_set, "set", A_GIMME, 0);
    class_addmethod(this_class, (method)ibufmultitable_assist, "assist", A_CANT, 0);
    class_addmethod(this_class, (method)ibufmultitable_dsp, "dsp", A_CANT, 0);
    class_addmethod(this_class, (method)ibufmultitable_dsp64, "dsp64", A_CANT, 0);
    
    // Add Attributes
    
    add_ibuffer_interp_attribute<t_ibufmultitable, kInterpLinear>(this_class, "interp");
    
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

void *ibufmultitable_new(t_symbol *s, long argc, t_atom *argv)
{
    t_ibufmultitable *x = (t_ibufmultitable *)object_alloc(this_class);
    
    dsp_setup((t_pxobject *)x, 2);
    outlet_new((t_object *)x, "signal");
    
    // Default variables
    
    t_symbol *buffer_name = NULL;
    t_atom_long start_samp = 0;
    t_atom_long end_samp = 512;
    t_atom_long chan = 1;
    
    // Arguments
    
    long non_attr_argc = attr_args_offset(argc, argv);
    
    x->buffer_name = non_attr_argc > 0 ? atom_getsym(argv + 0) : buffer_name;
    x->start_samp = non_attr_argc > 1 ? atom_getlong(argv + 1) : std::max(start_samp, static_cast<t_atom_long>(0));
    x->end_samp = non_attr_argc > 2 ? atom_getlong(argv + 2) : std::max(end_samp, static_cast<t_atom_long>(0));
    x->chan = non_attr_argc > 3 ? atom_getlong(argv + 3) : std::max(chan, static_cast<t_atom_long>(1));
    
    x->interp_type = kInterpLinear;
    
    // Set attributes from arguments
    
    attr_args_process(x, argc, argv);
    
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

void ibufmultitable_set(t_ibufmultitable *x, t_symbol *msg, long argc, t_atom *argv)
{
    ibufmultitable_set_internal(x, argc ? atom_getsym(argv) : 0);
}

void ibufmultitable_set_internal(t_ibufmultitable *x, t_symbol *s)
{
    ibuffer_data buffer(s);
    
    x->buffer_name = s;
    
    if (buffer.get_type() == kBufferNone && s)
        object_error((t_object *) x, "ibufmultitable~: no buffer %s", s->s_name);
}

// Core Perform Routine

template <int N, class T>
void perform_positions(T *positions, T *in, T *offset_in, long num_samps, const double start_samp, const double end_samp, const double last_samp)
{
    SIMDType<T, N> *v_positions = reinterpret_cast<SIMDType<T, N> *>(positions);
    SIMDType<T, N> *v_in = reinterpret_cast<SIMDType<T, N> *>(in);
    SIMDType<T, N> *v_offset_in = reinterpret_cast<SIMDType<T, N> *>(offset_in);
    
    const SIMDType<T, N> mul(end_samp - start_samp);
    const SIMDType<T, N> add(start_samp);
    const SIMDType<T, N> end(last_samp);
    const SIMDType<T, N> zero(static_cast<T>(0));
    const SIMDType<T, N> one(static_cast<T>(1));
    
    long num_vecs = num_samps / N;
    
    for (long i = 0; i < num_vecs; i++)
    {
        SIMDType<T, N> position = min(one, max(zero, *v_in++)) * mul + add + *v_offset_in++;
        v_positions[i] = min(end, max(zero, position));
    }
}

long clip(const long in, const long max)
{
    return std::min(max, std::max(0L, in));
}

template <class T>
void perform_core(t_ibufmultitable *x, T *in, T *offset_in, T *out, long vec_size)
{
    // Check if the buffer is set / valid and get the length information
    
    ibuffer_data buffer(x->buffer_name);
    
    long start_samp = clip(x->start_samp, buffer.get_length() - 1);
    long end_samp = clip(x->end_samp, buffer.get_length() - 1);
    long chan = clip(x->chan - 1, buffer.get_num_chans() - 1);
    double last_samp = buffer.get_length() - 1;
    
    // Calculate output
    
    if (buffer.get_length() && (buffer.get_length() - start_samp) >= 1)
    {
        // Positions
        
        const int N = SIMDLimits<T>::max_size;
        const long v_count = vec_size / N;
        const long S = v_count * N;
        
        // Read from buffer
        
        perform_positions<N>(out + 0, in + 0, offset_in + 0, v_count, start_samp, end_samp, last_samp);
        perform_positions<1>(out + S, in + S, offset_in + S, (v_count - S), start_samp, end_samp, last_samp);
        
        ibuffer_read(buffer, out, out, vec_size, chan, 1.f, x->interp_type);
    }
    else
        std::fill_n(out, vec_size, 0);
}

// Perform and DSP for 32-bit signals

t_int *ibufmultitable_perform(t_int *w)
{
    // Ignore the copy of this function pointer (due to denormal fixer)
    
    // Set pointers
    
    float *in = (float *) w[2];
    float *offset_in = (float *) w[3];
    float *out = (float *)(w[4]);
    long vec_size = w[5];
    t_ibufmultitable *x = (t_ibufmultitable *) w[6];
    
    if (!x->x_obj.z_disabled)
        perform_core(x, in, offset_in, out, vec_size);
    
    return w + 7;
}

void ibufmultitable_dsp(t_ibufmultitable *x, t_signal **sp, short *count)
{
    // Set buffer again in case it is no longer valid / extant
    
    ibufmultitable_set_internal(x, x->buffer_name);
    
    if (count[0] && count[1])
        dsp_add(denormals_perform, 6, ibufmultitable_perform, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n, x);
}

// Perform and DSP for 64-bit signals

void ibufmultitable_perform64(t_ibufmultitable *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    perform_core(x, ins[0], ins[1], outs[0], vec_size);
}

void ibufmultitable_dsp64(t_ibufmultitable *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    // Set buffer again in case it is no longer valid / extant
    
    ibufmultitable_set_internal(x, x->buffer_name);
    
    if (count[0] && count[1])
        object_method(dsp64, gensym("dsp_add64"), x, ibufmultitable_perform64);
}
