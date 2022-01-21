
/*
 *  partconvolve~
 *
 *  partconvolve~ copies samples from a buffer to use as a impulse response for FFT-based partitioned convolution.
 *  It can also be used to perform a simple static FFT-based FIR filter on an incoming signal.
 *
 *  Typically partconvolve~ might be used in conjuction with timeconvolve~ for zero-latency convolution with longer impulses.
 *  The two objects have similar attributes / arguments and can be easily combined to design custom partitioning schemes.
 *
 *  Copyright 2010-2022 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Denormals.h>
#include <HISSTools_FFT/HISSTools_FFT.h>
#include <RandomGenerator.hpp>
#include <SIMDSupport.hpp>
#include <ibuffer_access.hpp>

#include <algorithm>

constexpr static int MIN_FFT_SIZE_LOG2          = 5;
constexpr static int MAX_FFT_SIZE_LOG2          = 20;
constexpr static int DEFAULT_MAX_FFT_SIZE_LOG2  = 16;
constexpr static int BUFFER_SIZE_DEFAULT        = 1323000;  // N.B. = 44100 * 30 or 30 seconds at 44.1kHz

// Random number generator

random_generator<> rand_gen;

// Utility

using VecType = SIMDType<float, 4>;

void update_split_complex_pointers(FFT_SPLIT_COMPLEX_F &complex1, const FFT_SPLIT_COMPLEX_F complex2, long offset)
{
    complex1.realp = complex2.realp + offset;
    complex1.imagp = complex2.imagp + offset;
}

long int_log2(long long in, long *inexact)
{
    long long temp = in;
    long out = 0;
    
    if (in <= 0)
        return - 1;
    
    while (temp)
    {
        temp >>= 1;
        out++;
    }
    
    if (in == 1 << (out - 1))
    {
        out--;
        *inexact = 0;
    }
    else
        *inexact = 1;
    
    return out;
}

// Class pointer and structure

t_class *this_class;

struct t_partconvolve
{
    t_pxobject x_obj;
    
    // FFT variables
    
    FFT_SETUP_F fft_setup_real;
    
    long max_fft_size;
    long max_fft_size_log2;
    long fft_size;
    long fft_size_log2;
    
    long rw_counter;
    
    // Scheduling variables
    
    long num_partitions;
    long valid_partitions;
    long partitions_done;
    long last_partition;
    long input_position;
    
    // Internal buffers
    
    float *fft_buffers[5];
    
    FFT_SPLIT_COMPLEX_F impulse_buffer;
    FFT_SPLIT_COMPLEX_F input_buffer;
    FFT_SPLIT_COMPLEX_F accum_buffer;
    FFT_SPLIT_COMPLEX_F partition_temp;
    
    long max_impulse_length;
    
    // Buffer name
    
    t_symbol *buffer_name;
    
    // Attributes
    
    t_atom_long chan;
    t_atom_long offset;
    t_atom_long length;
    
    // Flags
    
    bool reset_flag;                // reset fft data on next perform call
    bool memory_flag;                // memory was allocated correctly
    bool direct_flag;                // do not perform fft on impulse when partioning (direct/eq mode)
    bool eq_flag;                    // eq_flag mode on/off
};

void partconvolve_free(t_partconvolve *x);
void *partconvolve_new(t_symbol *s, long argc, t_atom *argv);

void partconvolve_max_fft_size_set(t_partconvolve *x, long max_fft_size);
t_max_err partconvolve_fft_size_set(t_partconvolve *x, t_object *attr, long argc, t_atom *argv);

t_max_err partconvolve_notify(t_partconvolve *x, t_symbol *s, t_symbol *msg, void *sender, void *data);

void partconvolve_direct(t_partconvolve *x, t_symbol *msg, long argc, t_atom *argv);
void partconvolve_eq(t_partconvolve *x,  t_symbol *msg, long argc, t_atom *argv);
void partconvolve_set(t_partconvolve *x,  t_symbol *msg, long argc, t_atom *argv);
void partconvolve_set_internal(t_partconvolve *x, t_symbol *s, bool direct_flag, bool eq_flag);

void partconvolve_partition(t_partconvolve *x, long direct_flag);

void partconvolve_perform_partition(FFT_SPLIT_COMPLEX_F in1, FFT_SPLIT_COMPLEX_F in2, FFT_SPLIT_COMPLEX_F out, long vec_size);
void partconvolve_perform_equaliser(FFT_SPLIT_COMPLEX_F in1, FFT_SPLIT_COMPLEX_F in2, FFT_SPLIT_COMPLEX_F out, long vec_size);
void partconvolve_perform_internal(t_partconvolve *x, float *in, float *out, long vec_size);

t_int *partconvolve_perform(t_int *w);
void partconvolve_dsp(t_partconvolve *x, t_signal **sp, short *count);

void partconvolve_perform64 (t_partconvolve *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void partconvolve_dsp64 (t_partconvolve *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

void partconvolve_assist(t_partconvolve *x, void *b, long m, long a, char *s);
void partconvolve_memoryusage(t_partconvolve *x);


int C74_EXPORT main()
{
    this_class = class_new("partconvolve~",
                           (method)partconvolve_new,
                           (method)partconvolve_free,
                           sizeof(t_partconvolve),
                           nullptr,
                           A_GIMME,
                           0);
    
    class_addmethod(this_class, (method)partconvolve_set, "set", A_GIMME, 0);
    class_addmethod(this_class, (method)partconvolve_direct, "direct", A_GIMME, 0);
    class_addmethod(this_class, (method)partconvolve_eq, "eq", A_GIMME, 0);
    
    class_addmethod(this_class, (method)partconvolve_memoryusage, "memoryusage", 0);
    class_addmethod(this_class, (method)partconvolve_assist, "assist", A_CANT, 0);
    class_addmethod(this_class, (method)partconvolve_notify, "notify", A_CANT, 0);
    class_addmethod(this_class, (method)partconvolve_dsp, "dsp", A_CANT, 0);
    class_addmethod(this_class, (method)partconvolve_dsp64, "dsp64", A_CANT, 0);
    
    class_addmethod(this_class, (method)object_obex_quickref, "quickref", A_CANT, 0);
    
    // Add Attributes
    
    CLASS_ATTR_LONG(this_class, "fftsize", 0L, t_partconvolve, fft_size);
    CLASS_ATTR_ACCESSORS(this_class, "fftsize", nullptr, partconvolve_fft_size_set);
    CLASS_ATTR_LABEL(this_class, "fftsize", 0L, "FFT Size");
    
    CLASS_ATTR_LONG(this_class, "length", 0L, t_partconvolve, length);
    CLASS_ATTR_FILTER_MIN(this_class, "length", 0);
    CLASS_ATTR_LABEL(this_class, "length", 0L, "Impulse Length");
    
    CLASS_ATTR_LONG(this_class, "offset", 0L, t_partconvolve, offset);
    CLASS_ATTR_FILTER_MIN(this_class, "offset", 0);
    CLASS_ATTR_LABEL(this_class,"offset", 0L, "Offset Into Buffer");
    
    CLASS_ATTR_LONG(this_class, "chan", 0L, t_partconvolve, chan);
    CLASS_ATTR_FILTER_CLIP(this_class, "chan", 1, 4);
    CLASS_ATTR_LABEL(this_class, "chan", 0L, "Buffer Read Channel");
    
    // Add dsp and register
    
    class_dspinit(this_class);
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

void partconvolve_free(t_partconvolve *x)
{
    dsp_free(&x->x_obj);
    hisstools_destroy_setup(x->fft_setup_real);
    deallocate_aligned(x->impulse_buffer.realp);
    deallocate_aligned(x->fft_buffers[0]);
}

void *partconvolve_new(t_symbol *s, long argc, t_atom *argv)
{
    long max_impulse_length = BUFFER_SIZE_DEFAULT;
    long max_fft_size;
    
    // Setup the object and make inlets / outlets
    
    t_partconvolve *x = (t_partconvolve *)object_alloc(this_class);
        
    dsp_setup((t_pxobject *)x, 1);
    outlet_new((t_object *)x,"signal");
    
    // Set default initial attributes and variables
    
    x->num_partitions = 0;
    
    x->max_fft_size_log2 = DEFAULT_MAX_FFT_SIZE_LOG2;
    x->max_fft_size = 1 << x->max_fft_size_log2;
    
    x->buffer_name = nullptr;
    
    x->fft_size_log2 = 0;
    x->fft_size = 0;
    x->length = 0;
    x->offset = 0;
    x->chan = 1;
    
    // Check arguments
    
    if (argc && atom_gettype(argv) == A_LONG)
    {
        max_impulse_length = atom_getlong(argv);
        
        if (max_impulse_length <= 0)
            max_impulse_length = BUFFER_SIZE_DEFAULT;
        if (max_impulse_length < 64)
        {
            object_error( (t_object *) x, "minimum internal buffer size is 64 samples");
            max_impulse_length = 64;
        }
        
        argv++;
        argc--;
    }
    
    x->max_impulse_length = max_impulse_length;
    
    if (argc && atom_gettype(argv) == A_LONG)
    {
        partconvolve_max_fft_size_set(x, atom_getlong(argv));
        argv++;
        argc--;
    }
    
    // Allocate impulse buffer and input buffer
    
    max_impulse_length = x->max_impulse_length;
    
    max_fft_size = 1 << x->max_fft_size_log2;
    
    // This is designed to make sure we can load the max impulse length, whatever the fft size
    
    if (max_impulse_length % (max_fft_size / 2))
    {
        max_impulse_length /= (max_fft_size / 2);
        max_impulse_length++;
        max_impulse_length *= (max_fft_size / 2);
    }
    
    x->impulse_buffer.realp = allocate_aligned<float>(max_impulse_length * 4);
    x->impulse_buffer.imagp = x->impulse_buffer.realp + max_impulse_length;
    x->input_buffer.realp = x->impulse_buffer.imagp + max_impulse_length;
    x->input_buffer.imagp = x->input_buffer.realp + max_impulse_length;
    
    // Allocate fft and temporary buffers
    
    x->fft_buffers[0] = allocate_aligned<float>(max_fft_size * 7);
    x->fft_buffers[1] = x->fft_buffers[0] + max_fft_size;
    x->fft_buffers[2] = x->fft_buffers[1] + max_fft_size;
    x->fft_buffers[3] = x->fft_buffers[2] + max_fft_size;
    
    x->accum_buffer.realp = x->fft_buffers[3] + max_fft_size;
    x->accum_buffer.imagp = x->accum_buffer.realp + (max_fft_size / 2);
    x->partition_temp.realp = x->accum_buffer.imagp + (max_fft_size / 2);
    x->partition_temp.imagp = x->partition_temp.realp + (max_fft_size / 2);
    
    x->fft_buffers[4] = x->partition_temp.imagp + (max_fft_size / 2);
    
    hisstools_create_setup(&x->fft_setup_real, x->max_fft_size_log2);
    
    x->memory_flag = x->fft_buffers[0] && x->impulse_buffer.realp && x->fft_setup_real;
    
    // Set attributes from arguments
    
    attr_args_process(x, argc, argv);
    
    // Check whether the fftsize attribute has been set (if not set it)
    
    if (x->fft_size == 0)
        object_attr_setlong(x, gensym("fftsize"), x->max_fft_size);
    
    if (!x->memory_flag)
        object_error((t_object *) x, "couldn't allocate enough memory.....");
    
    return x;
}

void partconvolve_max_fft_size_set(t_partconvolve *x, long max_fft_size)
{
    long inexact = 0;
    long max_fft_size_log2 = int_log2(max_fft_size, &inexact);
    
    if (max_fft_size_log2 < 0)
    {
        max_fft_size_log2 = DEFAULT_MAX_FFT_SIZE_LOG2;
        max_fft_size = 1 << max_fft_size_log2;
    }
    
    if (max_fft_size_log2 > MAX_FFT_SIZE_LOG2)
    {
        object_error( (t_object *) x, "maximum fft size too large - using %ld", 1 << MAX_FFT_SIZE_LOG2);
        max_fft_size_log2 = MAX_FFT_SIZE_LOG2;
    }
    
    if (max_fft_size_log2 && max_fft_size_log2 < MIN_FFT_SIZE_LOG2)
    {
        object_error( (t_object *) x, "maximum fft size too small - using %ld", 1 << MIN_FFT_SIZE_LOG2);
        max_fft_size_log2 = MIN_FFT_SIZE_LOG2;
    }
    
    if (inexact)
        object_error( (t_object *) x, "maximum fft size must be power of two - using %ld", 1 << max_fft_size_log2);
    
    x->max_fft_size_log2 = max_fft_size_log2;
    x->max_fft_size = 1 << max_fft_size_log2;
}

t_max_err partconvolve_fft_size_set(t_partconvolve *x, t_object *attr, long argc, t_atom *argv)
{
    float *window = x->fft_buffers[4];
    double window_gain = 0.;
    double window_scale;
    
    long inexact = 0;
    long fft_size_log2;
    long fft_size;
    
    if (!argc)
        return MAX_ERR_NONE;
    
    fft_size_log2 = int_log2(atom_getlong(argv), &inexact);
    
    if (fft_size_log2 < MIN_FFT_SIZE_LOG2 || fft_size_log2 > x->max_fft_size_log2)
    {
        object_error( (t_object *) x, "fft size out of range - should be between %ld and %ld", 1 << MIN_FFT_SIZE_LOG2, 1 << x->max_fft_size_log2);
        return MAX_ERR_NONE;
    }
    
    if (inexact)
        object_error( (t_object *) x, "fft size must be power of two - using %ld", 1 << fft_size_log2);
    
    // Set fft variables iff the fft size has actually actually changed
    
    if (fft_size_log2 != x->fft_size_log2 && x->memory_flag)
    {
        x->num_partitions = 0;
        
        // Initialise fft info
        
        fft_size = 1 << fft_size_log2;
        x->fft_size_log2 = fft_size_log2;
        x->fft_size = fft_size;
        
        // Make a hann window (and sqrt for overlap 2)
        
        for (long i = 0; i < fft_size; i++)
            window[i] = sqrt(0.5 - (0.5 * cos(M_PI * 2.0 * ((double) i / (double) fft_size))));
        
        // Calculate the gain of the window and the appropriate scaling and apply
        // Note that the scaling is split between input and output windowing for ease
        
        for (long i = 0; i < fft_size; i++)
            window_gain += (double) (window[i] * window[i]);
        
        window_scale = sqrt(1. / (4.0 * window_gain));
        
        for (long i = 0; i < fft_size; i++)
            window[i] *= window_scale;
        
        // Reload the impulse buffer if appropriate
        
        partconvolve_partition(x, x->direct_flag);
    }
    
    return MAX_ERR_NONE;
}

t_max_err partconvolve_notify(t_partconvolve *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
    if (msg == gensym("attr_modified"))
    {
        t_symbol *attrname = (t_symbol *) object_method((t_object *)data, gensym("getname"));
        
        if (attrname == gensym("fftsize"))
        {
            if (x->length > x->max_impulse_length)
            {
                object_error((t_object *) x, "requested length greater than internal buffer size - internal buffer length is set to %ld", x->max_impulse_length);
                x->length = x->max_impulse_length;
            }
        }
    }
    
    return MAX_ERR_NONE;
}

t_max_err partconvolve_length_set(t_partconvolve *x, t_object *attr, long argc, t_atom *argv)
{
    long length = atom_getlong(argv);
    long max_impulse_length = x->max_impulse_length;
    
    if (!argc)
        return MAX_ERR_NONE;
    
    if (length < 0)
        length = 0;
    if (length > max_impulse_length)
    {
        object_error( (t_object *) x, "requested length greater than internal buffer size - internal buffer length is set to %ld", max_impulse_length);
        length = max_impulse_length;
    }
    
    x->length = length;
    
    return MAX_ERR_NONE;
}

void partconvolve_direct(t_partconvolve *x, t_symbol *msg, long argc, t_atom *argv)
{
    partconvolve_set_internal(x, argc ? atom_getsym(argv) : nullptr, true, false);
}

void partconvolve_eq(t_partconvolve *x, t_symbol *msg, long argc, t_atom *argv)
{
    partconvolve_set_internal(x, argc ? atom_getsym(argv) : nullptr, true, true);
}

void partconvolve_set(t_partconvolve *x, t_symbol *msg, long argc, t_atom *argv)
{
    partconvolve_set_internal(x, argc ? atom_getsym(argv) : nullptr, false, false);
}

void partconvolve_set_internal(t_partconvolve *x, t_symbol *s, bool direct_flag, bool eq_flag)
{
    ibuffer_data buffer(s);
    
    // We store the buffer_name, as it may become valid later
    
    x->buffer_name = s;
    
    if (!eq_flag || !x->eq_flag)
        x->num_partitions = 0;
    
    if (buffer.get_length())
    {
        x->direct_flag = direct_flag;
        x->eq_flag = eq_flag;
        
        partconvolve_partition(x, direct_flag);
    }
    else
    {
        x->num_partitions = 0;
        x->reset_flag = 1;
        
        if (buffer.get_type() == kBufferNone && s)
            object_error( (t_object *) x, "%s is not a valid buffer", s->s_name);
    }
}

void partconvolve_partition(t_partconvolve *x, long direct_flag)
{
    // Standard ibuffer variables
    
    ibuffer_data buffer(x->buffer_name);
    
    // FFT variables / attributes
    
    FFT_SETUP_F fft_setup_real = x->fft_setup_real;
    
    long fft_size_halved = x->fft_size >> 1;
    long fft_size_log2 = x->fft_size_log2;
    
    // Attributes
    
    t_atom_long offset = x->offset;
    t_atom_long length = x->length;
    t_atom_long chan = x->chan - 1;
    
    // Partition variables
    
    float *buffer_temp1 = x->partition_temp.realp;
    FFT_SPLIT_COMPLEX_F impulse_buffer = x->impulse_buffer;
    FFT_SPLIT_COMPLEX_F buffer_temp2;
    
    t_ptr_int impulse_length;
    t_ptr_int buffer_pos;
    long num_partitions;
    
    // Access buffer
    
    if (!x->memory_flag || !buffer.get_length())
    {
        x->num_partitions = 0;
        return;
    }
    
    if (buffer.get_num_chans() < chan + 1)
        chan = chan % buffer.get_num_chans();
    
    // Calculate how much of the buffer to load
    
    impulse_length = std::max(static_cast<t_ptr_int>(0), buffer.get_length() - offset);
    if (length)
        impulse_length = std::min(static_cast<t_ptr_int>(length), impulse_length);
    if (x->eq_flag)
        impulse_length = std::min(static_cast<t_ptr_int>(fft_size_halved), impulse_length);
    if (length && impulse_length < length)
        object_error((t_object *) x, "buffer is shorter than requested length (after offset has been applied)");
    if (impulse_length > x->max_impulse_length)
    {
        impulse_length = x->max_impulse_length;
        object_error((t_object *) x, "internal buffer is not large enough to load entire buffer~ into memory");
    }
    
    // Partition / load the impulse
    
    if (direct_flag)
    {
        for (buffer_pos = offset, buffer_temp2 = impulse_buffer, num_partitions = 0; impulse_length > 0; buffer_pos += fft_size_halved, impulse_length -= fft_size_halved, num_partitions++)
        {
            // Get real values (zero pad if not enough data)
            
            long n_samps = std::min(impulse_length, static_cast<t_ptr_int>(fft_size_halved));
            ibuffer_get_samps(buffer, buffer_temp2.realp, buffer_pos, n_samps, chan);
            std::fill_n(buffer_temp2.realp + n_samps, fft_size_halved - n_samps, 0.f);
            impulse_length -= fft_size_halved;
            
            // Get imag values (zero pad if not enough data - note also the loop decrement to impulse_length)
            
            n_samps = std::min(impulse_length, static_cast<t_ptr_int>(fft_size_halved));
            ibuffer_get_samps(buffer, buffer_temp2.imagp, buffer_pos + fft_size_halved, n_samps, chan);
            std::fill_n(buffer_temp2.imagp + n_samps, fft_size_halved - n_samps, 0.f);
            
            update_split_complex_pointers(buffer_temp2, buffer_temp2, fft_size_halved);
        }
        
    }
    else
    {
        for (buffer_pos = offset, buffer_temp2 = impulse_buffer, num_partitions = 0; impulse_length > 0; buffer_pos += fft_size_halved, impulse_length -= fft_size_halved, num_partitions++)
        {
            // Get samples up to half the fft size, take FFT (with zero padding) and update the pointers
            
            long n_samps = std::min(impulse_length, static_cast<t_ptr_int>(fft_size_halved));
            ibuffer_get_samps(buffer, buffer_temp1, buffer_pos, n_samps, chan);
            hisstools_rfft(fft_setup_real, buffer_temp1, &buffer_temp2, n_samps, fft_size_log2);
            update_split_complex_pointers(buffer_temp2, buffer_temp2, fft_size_halved);
        }
    }
    
    // Set flags
    
    if (!x->num_partitions)
        x->reset_flag = 1;
    x->num_partitions = num_partitions;
}

void partconvolve_perform_partition(FFT_SPLIT_COMPLEX_F in1, FFT_SPLIT_COMPLEX_F in2, FFT_SPLIT_COMPLEX_F out, long vec_size)
{
    VecType *in_real1 = reinterpret_cast<VecType *>(in1.realp);
    VecType *in_imag1 = reinterpret_cast<VecType *>(in1.imagp);
    VecType *in_real2 = reinterpret_cast<VecType *>(in2.realp);
    VecType *in_imag2 = reinterpret_cast<VecType *>(in2.imagp);
    VecType *out_real = reinterpret_cast<VecType *>(out.realp);
    VecType *out_imag = reinterpret_cast<VecType *>(out.imagp);
    
    // Do Nyquist Calculation and then zero these bins
    
    float nyquist1 = in1.imagp[0];
    float nyquist2 = in2.imagp[0];
    
    in1.imagp[0] = 0.f;
    in2.imagp[0] = 0.f;
    
    out.imagp[0] += nyquist1 * nyquist2;
    
    long num_vecs = vec_size / VecType::size;
    
    // Do other bins
    
    for (long i = 0; i < num_vecs; i++)
    {
        out_real[i] += (in_real1[i] * in_real2[i]) - in_imag1[i] * in_imag2[i];
        out_imag[i] += (in_real1[i] * in_imag2[i]) + in_imag1[i] * in_real2[i];
    }
    
    // Replace nyquist bins
    
    in1.imagp[0] = nyquist1;
    in2.imagp[0] = nyquist2;
}

void partconvolve_perform_equaliser(FFT_SPLIT_COMPLEX_F in1, FFT_SPLIT_COMPLEX_F in2, FFT_SPLIT_COMPLEX_F out, long vec_size)
{
    VecType *in_real1 = reinterpret_cast<VecType *>(in1.realp);
    VecType *in_imag1 = reinterpret_cast<VecType *>(in1.imagp);
    VecType *in_real2 = reinterpret_cast<VecType *>(in2.realp);
    VecType *out_real = reinterpret_cast<VecType *>(out.realp);
    VecType *out_imag = reinterpret_cast<VecType *>(out.imagp);
    
    long num_vecs = vec_size / VecType::size;
    
    for (long i = 0; i < num_vecs; i++)
    {
        out_real[i] = in_real1[i] * in_real2[i];
        out_imag[i] = in_imag1[i] * in_real2[i];
    }
}

void partconvolve_perform_internal(t_partconvolve *x, float *in, float *out, long vec_size)
{
    FFT_SPLIT_COMPLEX_F impulse_buffer = x->impulse_buffer;
    FFT_SPLIT_COMPLEX_F input_buffer = x->input_buffer;
    FFT_SPLIT_COMPLEX_F accum_buffer = x->accum_buffer;
    FFT_SPLIT_COMPLEX_F impulse_temp, buffer_temp;
    
    long input_position = x->input_position;
    
    // Scheduling variables
    
    long num_partitions = x->num_partitions;
    long valid_partitions = x->valid_partitions;
    long partitions_done = x->partitions_done;
    long last_partition = x->last_partition;
    long num_partitions_to_do;
    long next_partition;
    
    // FFT variables
    
    FFT_SETUP_F fft_setup_real = x->fft_setup_real;
    
    float **fft_buffers = x->fft_buffers;
    
    long fft_size = x->fft_size;
    long fft_size_halved = x->fft_size >> 1;
    long fft_size_log2 = x->fft_size_log2;
    
    long rw_counter = x->rw_counter;
    long hop_mask = fft_size_halved - 1;
    
    long vec_remain = vec_size;
    
    bool reset_flag = x->reset_flag;
    bool eq_flag = x->eq_flag;
    
    if  (!num_partitions || x->x_obj.z_disabled || !x->memory_flag)
    {
        std::fill_n(out, vec_size, 0.f);
        return;
    }
    
    // If we need to reset everything we do that here - happens when the fft size changes, or a new buffer is loaded
    
    if (reset_flag)
    {
        // Reset fft buffers + accum buffer
        
        std::fill_n(fft_buffers[0], x->max_fft_size * 5, 0.f);
        
        // Reset fft offset (randomly)
        
        rw_counter = static_cast<long>(rand_gen.rand_int(static_cast<uint32_t>(fft_size_halved - 1)));
        
        // Reset scheduling variables
        
        input_position = 0;
        partitions_done = 0;
        last_partition = 0;
        valid_partitions = 1;
        
        // Set reset flag off
        
        x->reset_flag = 0;
    }
    
    // Main loop
    
    while (vec_remain > 0)
    {
        // How many samples to deal with this loop (depending on whether there is an fft to do before the end of the signal vector)
        
        long till_next_fft = (fft_size_halved - (rw_counter & hop_mask));
        long hi_counter = (rw_counter + fft_size_halved) & (fft_size - 1);
        long loop_size = std::min(vec_remain, till_next_fft);
        
        // Load input into buffer (twice) and output from the output buffer
        
        std::copy_n(in, loop_size, fft_buffers[0] + rw_counter);
        std::copy_n(in, loop_size, fft_buffers[1] + hi_counter);

        std::copy_n(fft_buffers[3] + rw_counter, loop_size, out);
        
        // Update pointers and counters
        
        vec_remain -= loop_size;
        rw_counter += loop_size;
        in += loop_size;
        out += loop_size;
        
        bool fft_now = !(rw_counter & hop_mask);
        
        // Work loop and scheduling - this is where most of the convolution is done
        // Calculate ow many partitions to do this loop
        // (We must make sure that all partitions are done before we need to do the next fft)
        
        if (fft_now)
            num_partitions_to_do = (valid_partitions - partitions_done) - 1;
        else
            num_partitions_to_do = (((valid_partitions - 1) * (rw_counter & hop_mask)) / fft_size_halved) - partitions_done;
        
        while (num_partitions_to_do > 0)
        {
            // Calculate buffer wraparounds
            // (If wraparound is in the middle of this set of partitions this loop will run again)
            
            next_partition = (last_partition < num_partitions) ? last_partition : 0;
            last_partition = (next_partition + num_partitions_to_do) > num_partitions ? num_partitions : next_partition + num_partitions_to_do;
            num_partitions_to_do -= last_partition - next_partition;
            
            // Calculate offsets and pointers
            
            update_split_complex_pointers(impulse_temp, impulse_buffer, (partitions_done + 1) * fft_size_halved);
            update_split_complex_pointers(buffer_temp, input_buffer, next_partition * fft_size_halved);
            
            // Do processing
            
            for (long i = next_partition; i < last_partition; i++)
            {
                partconvolve_perform_partition(buffer_temp, impulse_temp, accum_buffer, fft_size_halved);
                update_split_complex_pointers(impulse_temp, impulse_temp, fft_size_halved);
                update_split_complex_pointers(buffer_temp, buffer_temp, fft_size_halved);
                partitions_done++;
            }
        }
        
        // FFT processing - deal with the fft, any windowing, the first partition and overlapping
        
        if (fft_now)
        {
            bool fft_offset = rw_counter == fft_size_halved;
            
            // Calculate the position to do the fft from/to and calculate relevant pointers
            
            float *fft_input = ((!eq_flag) != fft_offset) ? fft_buffers[1] : fft_buffers[0];
            update_split_complex_pointers(buffer_temp, input_buffer, input_position * fft_size_halved);
            
            // For eq mode - window using hann window
            
            if (eq_flag)
            {
                VecType *v_fft_input = reinterpret_cast<VecType *>(fft_input);
                VecType *v_fft_buffer = reinterpret_cast<VecType *>(fft_buffers[4]);
                
                for (long i = 0; i < (fft_size / VecType::size); i++)
                    v_fft_input[i] *= v_fft_buffer[i];
            }
            
            // Do the fft and put into the input buffer
            
            hisstools_rfft(fft_setup_real, fft_input, &buffer_temp, fft_size, fft_size_log2);
            
            // Process first partition here and accumulate the output (we need it now!)
            
            if (eq_flag)
                partconvolve_perform_equaliser(buffer_temp, impulse_buffer, accum_buffer, fft_size_halved);
            else
                partconvolve_perform_partition(buffer_temp, impulse_buffer, accum_buffer, fft_size_halved);
            
            // Processing done - do inverse fft on the accumulation buffer
            
            hisstools_rifft(fft_setup_real, &accum_buffer, fft_buffers[2], fft_size_log2);
            
            // Store the result to the output buffer
            
            if (eq_flag)
            {
                // Window and overlap-add into output buffer (add first half, and owerwrite second)
                
                VecType *v_fft_window  = reinterpret_cast<VecType *>(fft_buffers[4]);
                VecType *v_fft_output1 = reinterpret_cast<VecType *>(fft_buffers[3] + (fft_offset ? fft_size_halved : 0));
                VecType *v_fft_output2 = reinterpret_cast<VecType *>(fft_buffers[3] + (fft_offset ? 0 : fft_size_halved));
                VecType *v_fft_buffer  = reinterpret_cast<VecType *>(fft_buffers[2]);
                
                for (long i = 0; i < (fft_size_halved / VecType::size); i++)
                    *v_fft_output1++ += *v_fft_buffer++ * *v_fft_window++;
                for (long i = 0; i < (fft_size_halved / VecType::size); i++)
                    *v_fft_output2++  = *v_fft_buffer++ * *v_fft_window++;
            }
            else
            {
                // Scale and store into output buffer (overlap-save)
                
                VecType v_scale_mult((float) (1.0 / (double) (fft_size << 2)));
                VecType *v_fft_output = reinterpret_cast<VecType *>(fft_buffers[3] + (fft_offset ? fft_size_halved : 0));
                VecType *v_fft_buffer = reinterpret_cast<VecType *>(fft_buffers[2]);
                
                for (long i = 0; i < (fft_size / VecType::size); i++)
                    *v_fft_output++ = *v_fft_buffer++ * v_scale_mult;
            }
            
            // Clear accumulation buffer
            
            std::fill_n(accum_buffer.realp, fft_size_halved, 0.f);
            std::fill_n(accum_buffer.imagp, fft_size_halved, 0.f);
            
            // Update rw_counter
            
            rw_counter = rw_counter & (fft_size - 1);
            
            // Set scheduling variables
            
            valid_partitions = (valid_partitions == num_partitions) ? valid_partitions : valid_partitions + 1;
            input_position = input_position ? input_position - 1 : num_partitions - 1;
            last_partition = input_position + 1;
            partitions_done = 0;
        }
    }
    
    // Write all variables back into the object struct
    
    x->input_position = input_position;
    x->rw_counter = rw_counter;
    
    x->valid_partitions = valid_partitions;
    x->partitions_done = partitions_done;
    x->last_partition = last_partition;
}

t_int *partconvolve_perform(t_int *w)
{
    // Miss denormal routine
    
    partconvolve_perform_internal((t_partconvolve *) w[5], (float *)(w[2]), (float *)(w[3]), (long) (w[4]));
    return w + 6;
}

void partconvolve_dsp(t_partconvolve *x, t_signal **sp, short *count)
{
    dsp_add(denormals_perform, 5, partconvolve_perform, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n, x);
}

template<class T, class U>
void typecast_copy(const T *in, U *out, long size)
{
    for (long i = 0; i < size; i++)
        out[i] = static_cast<U>(*in++);
}

void partconvolve_perform64(t_partconvolve *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    float *temp_in = reinterpret_cast<float *>(outs[0]);
    float *temp_out = temp_in + vec_size;
    
    // Copy in, process and copy out
    
    typecast_copy(ins[0], temp_in, vec_size);
    partconvolve_perform_internal(x, temp_in, temp_out, vec_size);
    typecast_copy(temp_out, outs[0], vec_size);
}

void partconvolve_dsp64(t_partconvolve *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    object_method(dsp64, gensym("dsp_add64"), x, partconvolve_perform64);
}

void partconvolve_memoryusage(t_partconvolve *x)
{
    long memory_size = (x->max_impulse_length * 4 * sizeof(float)) + (x->max_fft_size * 7 * sizeof(float));
    
    if (memory_size > 1024)
        object_post((t_object *)x, "using %.2lf MB", memory_size / 1048576.0);
    else
        object_post((t_object *)x, "using %.2lf KB", memory_size / 1024.0);
}

void partconvolve_assist(t_partconvolve *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
        sprintf(s,"(signal) Convolved Output");
    else
        sprintf(s,"(signal) Input");
}
