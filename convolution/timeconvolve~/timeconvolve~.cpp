
/*
 *  timeconvolve~
 *
 *  timeconvolve~ copies an impulse response from a buffer to use in real-time zero latency time-based convolution.
 *
 *  Typically timeconvolve~ is used in conjunction with partconvolve~ for zero-latency convolution with longer impulses.
 *  timeconvolve~ alone is limited to a maximum IR size of 2044 samples.
 *  The two objects have similar attributes / arguments and can be easily combined to design custom partitioning schemes.
 *  Note that the algorithms perform correlation with reversed impulse response coeffients (equivalent to convolution).
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Denormals.h>
#include <SIMDSupport.hpp>
#include <ibuffer_access.hpp>

#ifdef __APPLE__
#include <Accelerate/Accelerate.h>
#endif

#include <algorithm>

// Utility

using VecType = SIMDType<float, 4>;

#ifdef __APPLE__
t_ptr_int pad_length(t_ptr_int length)
{
    return length;
}
#else
t_ptr_int pad_length(t_ptr_int length)
{
    return ((length + 15) >> 4) << 4;
}
#endif

t_class *this_class;

struct t_timeconvolve
{
    t_pxobject x_obj;
    
    // Internal buffers
    
    float *impulse_buffer;
    float *input_buffer;
    float *output_buffer;
    
    long input_position;
    long impulse_length;
    
    // Attributes
    
    t_atom_long chan;
    t_atom_long offset;
    t_atom_long length;
    
    bool memory_flag;
};

void timeconvolve_free(t_timeconvolve *x);
void *timeconvolve_new(t_symbol *s, long argc, t_atom *argv);

void timeconvolve_set(t_timeconvolve *x, t_symbol *sym, long argc, t_atom *argv);

void time_domain_convolve_scalar(float *in, float *impulse, float *output, long N, long L);
void time_domain_convolve(float *in, VecType *impulse, float *output, long N, long L);

void timeconvolve_perform_internal(t_timeconvolve *x, float *in, float *out, long vec_size);
t_int *timeconvolve_perform(t_int *w);
void timeconvolve_dsp(t_timeconvolve *x, t_signal **sp, short *count);

void timeconvolve_perform64 (t_timeconvolve *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void timeconvolve_dsp64 (t_timeconvolve *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

void timeconvolve_assist(t_timeconvolve *x, void *b, long m, long a, char *s);

int C74_EXPORT main()
{
    this_class = class_new("timeconvolve~",
                           (method) timeconvolve_new,
                           (method) timeconvolve_free,
                           sizeof(t_timeconvolve),
                           nullptr,
                           A_GIMME,
                           0);
    
    class_addmethod(this_class, (method) timeconvolve_set, "set", A_GIMME, 0);
    
    class_addmethod(this_class, (method) timeconvolve_assist, "assist", A_CANT, 0);
    class_addmethod(this_class, (method) timeconvolve_dsp, "dsp", A_CANT, 0);
    class_addmethod(this_class, (method) timeconvolve_dsp64, "dsp64", A_CANT, 0);
    
    class_addmethod(this_class, (method) object_obex_quickref, "quickref", A_CANT, 0);
    
    // Add Attributes
            
    CLASS_ATTR_LONG(this_class, "length", 0L, t_timeconvolve, length);
    CLASS_ATTR_FILTER_CLIP(this_class, "length", 0, 2044);
    CLASS_ATTR_LABEL(this_class, "length", 0L, "Impulse Length");
    
    CLASS_ATTR_LONG(this_class, "offset", 0L, t_timeconvolve, offset);
    CLASS_ATTR_FILTER_MIN(this_class, "offset", 0);
    CLASS_ATTR_LABEL(this_class,"offset", 0L, "Offset Into Buffer");
    
    CLASS_ATTR_LONG(this_class, "chan", 0L, t_timeconvolve, chan);
    CLASS_ATTR_FILTER_MIN(this_class, "chan", 1);
    CLASS_ATTR_LABEL(this_class, "chan", 0L, "Buffer Read Channel");

    // Add dsp and register
    
    class_dspinit(this_class);
    class_register(CLASS_BOX, this_class);
        
    return 0;
}

void timeconvolve_free(t_timeconvolve *x)
{
    dsp_free(&x->x_obj);
    deallocate_aligned(x->impulse_buffer);
    deallocate_aligned(x->input_buffer);
}

void *timeconvolve_new(t_symbol *s, long argc, t_atom *argv)
{
    // Setup the object and make inlets / outlets
    
    t_timeconvolve *x = (t_timeconvolve *) object_alloc(this_class);
    
    dsp_setup((t_pxobject *)x, 1);
    outlet_new((t_object *)x,"signal");
    
    // Set default initial attributes and variables
    
    x->offset = 0;
    x->length = 0;
    x->chan = 1;
    
    x->input_position = 0;
    x->impulse_length = 0;
    
    // Set attributes from arguments
    
    attr_args_process(x, argc, argv);
    
    // Allocate impulse buffer and input buffer
    
    x->impulse_buffer = allocate_aligned<float>(2048);
    x->input_buffer = allocate_aligned<float>(8192);
    
    std::fill_n(x->impulse_buffer, 2048, 0.f);
    std::fill_n(x->input_buffer, 8192, 0.f);

    x->memory_flag = (x->impulse_buffer && x->input_buffer);
    
    if (!x->memory_flag)
        object_error ((t_object *) x, "couldn't allocate enough memory.....");
    
    return x;
}

void timeconvolve_set(t_timeconvolve *x, t_symbol *sym, long argc, t_atom *argv)
{
    // Standard ibuffer variables
    
    t_symbol *buffer_name = argc ? atom_getsym(argv) : 0;
    
    ibuffer_data buffer(buffer_name);
    
    // Attributes
    
    t_atom_long chan = x->chan - 1;
    t_atom_long offset = x->offset;
    t_atom_long length = x->length;
    
    float *impulse_buffer = x->impulse_buffer;
    
    t_ptr_int impulse_length;
    
    if (!x->memory_flag)
        return;
    
    if (buffer.get_length())
    {
        if (buffer.get_num_chans() < chan + 1)
            chan = chan % buffer.get_num_chans();
        
        // Calculate impulse length
        
        impulse_length = buffer.get_length() - offset;
        if (length && length < impulse_length)
            impulse_length = length;
        if (length && impulse_length < length)
            object_error ((t_object *) x, "buffer is shorter than requested length (after offset has been applied)");
        
        impulse_length = std::min(std::max(impulse_length, static_cast<t_ptr_int>(0)), static_cast<t_ptr_int>(2044));
        
        if (impulse_length)
        {
            // Padding is used to simplify processing under windows
            
            t_ptr_int impulse_offset = pad_length(impulse_length) - impulse_length;
            std::fill_n(impulse_buffer, impulse_offset, 0.f);
            ibuffer_get_samps(buffer, impulse_buffer + impulse_offset, offset, impulse_length, chan, true);
        }
        
        x->impulse_length = (long) impulse_length;
    }
    else
    {
        x->impulse_length = 0;

        if (buffer.get_type() == kBufferNone && buffer_name)
            object_error ((t_object *) x, "%s is not a valid buffer", buffer_name->s_name);
    }
}

#ifndef __APPLE__
void time_domain_convolve(float *in, VecType *impulse, float *output, long N, long L)
{
    VecType output_accum;
    float *input;
    float results[4];
        
    L = pad_length(L);
                   
    for (long i = 0; i < N; i++)
    {
        output_accum = VecType(0.f);
        input = in - L + 1 + i;
        
        for (long j = 0; j < L >> 2; j += 4)
        {
            // Load vals
            
            output_accum += impulse[j + 0] * VecType(input);
            input += 4;
            output_accum += impulse[j + 1] * VecType(input);
            input += 4;
            output_accum += impulse[j + 2] * VecType(input);
            input += 4;
            output_accum += impulse[j + 3] * VecType(input);
            input += 4;
        }
        
        output_accum.store(results);
        
        *output++ = results[0] + results[1] + results[2] + results[3];
    }
}
#else
void time_domain_convolve(float *in, VecType *impulse, float *output, long N, long L)
{
    vDSP_conv(in + 1 - L, 1, reinterpret_cast<float *>(impulse), 1, output, 1, N, L);
}
#endif

void timeconvolve_perform_internal(t_timeconvolve *x, float *in, float *out, long vec_size)
{
    float *impulse_buffer = x->impulse_buffer;
    float *input_buffer = x->input_buffer;
    long input_position = x->input_position;
    long impulse_length = x->impulse_length;
    
    // Copy input twice (allows us to read input out in one go)
    
    std::copy_n(in, vec_size, input_buffer + input_position);
    std::copy_n(in, vec_size, input_buffer + input_position + 4096);
   
    // Advance pointer
    
    input_position += vec_size;
    if (input_position >= 4096)
        input_position = 0;
    x->input_position = input_position;
    
    // Do convolution

    time_domain_convolve(input_buffer + 4096 + (input_position - vec_size), (VecType *) impulse_buffer, out, vec_size, impulse_length);
}

t_int *timeconvolve_perform(t_int *w)
{
    // Miss perform routine for denormal handling (w[2] onwards)
    
    timeconvolve_perform_internal((t_timeconvolve *)(w[5]), (float *)(w[2]), (float *)(w[3]), (long) (w[4]));
    
    return w + 6;
}

void timeconvolve_dsp(t_timeconvolve *x, t_signal **sp, short *count)
{
    dsp_add(denormals_perform, 5, timeconvolve_perform, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n, x);
}

template<class T, class U>
void typecast_copy(const T *in, U *out, long size)
{
    for (long i = 0; i < size; i++)
        out[i] = static_cast<U>(*in++);
}

void timeconvolve_perform64(t_timeconvolve *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    float *temp_in = reinterpret_cast<float *>(outs[0]);
    float *temp_out = temp_in + vec_size;
    
    // Copy in, process and copy out
    
    typecast_copy(ins[0], temp_in, vec_size);
    timeconvolve_perform_internal(x, temp_in, temp_out, vec_size);
    typecast_copy(temp_out, outs[0], vec_size);
}

void timeconvolve_dsp64(t_timeconvolve *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    object_method(dsp64, gensym("dsp_add64"), x, timeconvolve_perform64);
}

void timeconvolve_assist(t_timeconvolve *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
        sprintf(s,"(signal) Convolved Output");
    else
        sprintf(s,"(signal) Input");
}
