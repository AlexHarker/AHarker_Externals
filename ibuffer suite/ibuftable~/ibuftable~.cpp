
/*
 *  ibuftable~
 *
 *  ibuftable~ is an efficient object designed for table lookup (for window functions etc.).
 *
 *  ibuftable~ features SIMD optimisation and four types of interpolation.
 *  Linear interpolation and three different kinds of cubic interpolation are supported.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Denormals.h>
#include <ibuffer_access.hpp>

#include <algorithm>


// Globals and Object Structure

t_class *this_class;

struct t_ibuftable
{
    t_pxobject x_obj;
    
    t_symbol *buffer_name;
    
    InterpType interp_type;
    
    t_atom_long chan;
    t_atom_long start_samp;
    t_atom_long end_samp;
};

// Function Prototypes

void *ibuftable_new(t_symbol *s, long argc, t_atom *argv);
void ibuftable_free(t_ibuftable *x);
void ibuftable_assist(t_ibuftable *x, void *b, long m, long a, char *s);

void ibuftable_set(t_ibuftable *x, t_symbol *s, long argc, t_atom *argv);
void ibuftable_set_internal(t_ibuftable *x, t_symbol *s);

t_int *ibuftable_perform(t_int *w);
void ibuftable_dsp(t_ibuftable *x, t_signal **sp, short *count);

void ibuftable_perform64(t_ibuftable *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void ibuftable_dsp64(t_ibuftable *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags);

// Main

int C74_EXPORT main()
{
    this_class = class_new("ibuftable~",
                           (method) ibuftable_new,
                           (method) ibuftable_free,
                           sizeof(t_ibuftable),
                           (method) nullptr,
                           A_GIMME,
                           0);
    
    class_addmethod(this_class, (method) ibuftable_set, "set", A_GIMME, 0);
    class_addmethod(this_class, (method) ibuftable_assist, "assist", A_CANT, 0);
    class_addmethod(this_class, (method) ibuftable_dsp, "dsp", A_CANT, 0);
    class_addmethod(this_class, (method) ibuftable_dsp64, "dsp64", A_CANT, 0);
    
    // Add Attributes
    
    add_ibuffer_interp_attribute<t_ibuftable, InterpType::Linear>(this_class, "interp");
    
    CLASS_ATTR_ATOM_LONG(this_class, "chan", 0L, t_ibuftable, chan);
    CLASS_ATTR_FILTER_MIN(this_class, "chan", 1);
    CLASS_ATTR_LABEL(this_class, "chan", 0L, "Buffer Channel");
    
    CLASS_ATTR_ATOM_LONG(this_class, "startsamp", 0L, t_ibuftable, start_samp);
    CLASS_ATTR_FILTER_MIN(this_class, "startsamp", 0);
    CLASS_ATTR_LABEL(this_class, "startsamp", 0L, "Start Sample");
    
    CLASS_ATTR_ATOM_LONG(this_class, "endsamp", 0L, t_ibuftable, end_samp);
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

void *ibuftable_new(t_symbol *s, long argc, t_atom *argv)
{
    t_ibuftable *x = (t_ibuftable *) object_alloc(this_class);
    
    dsp_setup((t_pxobject *) x, 1);
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

void ibuftable_free(t_ibuftable *x)
{
    dsp_free(&x->x_obj);
}

void ibuftable_assist(t_ibuftable *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
        sprintf(s,"(signal) Output");
    else
        sprintf(s,"(signal) Position Input (0-1)");
}

// Buffer Setting

void ibuftable_set(t_ibuftable *x, t_symbol *msg, long argc, t_atom *argv)
{
    ibuftable_set_internal(x, argc ? atom_getsym(argv) : 0);
}

void ibuftable_set_internal(t_ibuftable *x, t_symbol *s)
{
    ibuffer_data buffer(s);
    
    x->buffer_name = s;
    
    if (buffer.get_type() == kBufferNone && s)
        object_error((t_object *) x, "ibuftable~: no buffer %s", s->s_name);
}

// Core Perform Routines

template <int N, class T>
void perform_positions(T *positions, const T *in, long n_vecs, double start, double end)
{
    SIMDType<T, N> *v_positions = reinterpret_cast<SIMDType<T, N> *>(positions);
    const SIMDType<T, N> *v_in = reinterpret_cast<const SIMDType<T, N> *>(in);
    
    const SIMDType<T, N> mul(static_cast<T>(end - start));
    const SIMDType<T, N> add(static_cast<T>(start));
    const SIMDType<T, N> zero(static_cast<T>(0));
    const SIMDType<T, N> one(static_cast<T>(1));
    
    for (long i = 0; i < n_vecs; i++)
        v_positions[i] = (mul * min(one, max(zero, *v_in++))) + add;
}

template <typename T>
T clip(const t_ptr_int in, const t_ptr_int max)
{
    return static_cast<T>(std::min(max, std::max(t_ptr_int(0), in)));
}

template <class T>
void perform_core(t_ibuftable *x, const T *in, T *out, long vec_size)
{
    // Check if the buffer is set / valid and get the length information
    
    ibuffer_data buffer(x->buffer_name);
    
    double start_samp = clip<double>(x->start_samp, buffer.get_length() - 1);
    double end_samp = clip<double>(x->end_samp, buffer.get_length() - 1);
    long chan = clip<long>(x->chan - 1, buffer.get_num_chans() - 1);
    
    // Calculate output
    
    if (buffer.get_length())
    {
        // Positions
        
        constexpr int N = SIMDLimits<T>::max_size;
        
        const long v_count = vec_size / N;
        const long S = v_count * N;
        
        perform_positions<N>(out + 0, in + 0, v_count, start_samp, end_samp);
        perform_positions<1>(out + S, in + S, (vec_size - S), start_samp, end_samp);
        
        // Read from buffer
        
        ibuffer_read(buffer, out, out, vec_size, chan, 1.f, x->interp_type);
    }
    else
        std::fill_n(out, vec_size, T(0));
}

// Perform and DSP for 32-bit signals

t_int *ibuftable_perform(t_int *w)
{
    // Ignore the copy of this function pointer (due to denormal fixer)
    
    // Set pointers
    
    const float *in = (float *) w[2];
    float *out = (float *)(w[3]);
    long vec_size = (long) w[4];
    t_ibuftable *x = (t_ibuftable *) w[5];
    
    if (!x->x_obj.z_disabled)
        perform_core(x, in, out, vec_size);
    
    return w + 6;
}

void ibuftable_dsp(t_ibuftable *x, t_signal **sp, short *count)
{
    // Set buffer again in case it is no longer valid / extant
    
    ibuftable_set_internal(x, x->buffer_name);
    
    dsp_add(denormals_perform, 5, ibuftable_perform, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n, x);
}

// Perform and DSP for 64-bit signals

void ibuftable_perform64(t_ibuftable *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    perform_core(x, ins[0], outs[0], vec_size);
}

void ibuftable_dsp64(t_ibuftable *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags)
{
    // Set buffer again in case it is no longer valid / extant
    
    ibuftable_set_internal(x, x->buffer_name);
    
    object_method(dsp64, gensym("dsp_add64"), x, ibuftable_perform64);
}
