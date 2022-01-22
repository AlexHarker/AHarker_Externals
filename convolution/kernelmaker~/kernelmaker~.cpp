
/*
 *  kernelmaker~
 *
 *  kernelmaker~ copies samples from a buffer modifying them by samples from another buffer (window functions etc.).
 *  kernelmaker~ can also be used to create simple bandpass filter kernels for partconvolve~'s eq mode.
 *
 *  It is intended to create short impulse responses to be used with timeconvolve~ and/or partconvolve~.
 *  Those externals copy the buffer so you can dispose of the buffer / reuse it once setting the impulse response.
 *  This means you can repeatedly use the same kernelmaker~ object and buffer for creating impulses.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <algorithm>

#include <ibuffer_access.hpp>


// Globals and Object Structure

t_class *this_class;

struct t_kernelmaker
{
    t_pxobject x_obj;
    
    t_atom_long fades;
};

t_symbol *ps_dirty;

// Function Protoypes

void *kernelmaker_new(t_atom_long fades);
void kernelmaker_assist(t_kernelmaker *x, void *b, long m, long a, char *s);

void kernelmaker_int(t_kernelmaker *x, t_atom_long fades);

void kernelmaker_window(t_kernelmaker *x, t_symbol *msg, long argc, t_atom *argv);
void kernelmaker_window_internal(t_kernelmaker *x, t_symbol *target_name, t_symbol *source_name, t_symbol *window_name, t_atom_long offset, t_atom_long length);

void kernelmaker_env(t_kernelmaker *x, t_symbol *msg, long argc, t_atom *argv);
void kernelmaker_env_internal(t_kernelmaker *x, t_symbol *target_name, t_symbol *source_name, t_symbol *window_name, t_atom_long offset, t_atom_long slide);

void kernelmaker_ring_mod(t_kernelmaker *x, t_symbol *msg, long argc, t_atom *argv);
void kernelmaker_ring_mod_internal(t_kernelmaker *x, t_symbol *target_name, t_symbol *source_name, t_symbol *window_name, t_atom_long offset);

void kernelmaker_trap(t_kernelmaker *x, t_symbol *msg, long argc, t_atom *argv);
void kernelmaker_trap_internal(t_kernelmaker *x, t_symbol *target_name, double env1, double env2, double env3, double env4, t_atom_long length);

// Main

int C74_EXPORT main()
{
    this_class = class_new("kernelmaker~",
                            (method) kernelmaker_new,
                            (method) nullptr,
                            sizeof(t_kernelmaker),
                            0L,
                            A_DEFLONG,
                            0);
    
    class_addmethod(this_class, (method) kernelmaker_int, "int", A_LONG, 0L);
    class_addmethod(this_class, (method) kernelmaker_window, "makekernel_wind", A_GIMME, 0L);
    class_addmethod(this_class, (method) kernelmaker_env, "makekernel_env", A_GIMME, 0L);
    class_addmethod(this_class, (method) kernelmaker_ring_mod, "makekernel_ring", A_GIMME, 0L);
    class_addmethod(this_class, (method) kernelmaker_trap, "makekernel_trap", A_GIMME, 0L);
    class_addmethod(this_class, (method) kernelmaker_assist, "assist", A_CANT, 0L);
    
    class_register(CLASS_BOX, this_class);
        
    ps_dirty = gensym("dirty");
    
    return 0;
}

// New / Assist

void *kernelmaker_new(t_atom_long fades)
{
    t_kernelmaker *x = (t_kernelmaker *)object_alloc (this_class);
    
    x->fades = !fades ? 40 : fades;
    
    return x;
}

void kernelmaker_assist(t_kernelmaker *x, void *b, long m, long a, char *s)
{
    sprintf(s,"Instructions To Build Kernel");
}

// Int (set fades)

void kernelmaker_int(t_kernelmaker *x, t_atom_long fades)
{
    if (fades >= 0)
        x->fades = fades;
}

// Normalisation Utility

void normalise_kernel(float *out_samps, t_ptr_int length, t_ptr_int num_chans, double peak_amp, t_atom_long fades)
{
    if (peak_amp)
    {
        const double amp_recip = 1.0 / peak_amp;
        const double fade_recip = amp_recip / ((fades > 0) ? fades : 1);

        t_ptr_int i;

        for (i = 0; i < fades; i++)
            out_samps[i * num_chans] *= static_cast<float>(fade_recip * i);
        for (; i < length - fades; i++)
            out_samps[i * num_chans] *= static_cast<float>(amp_recip);
        for (; i < length; i++)
            out_samps[i * num_chans] *= static_cast<float>(fade_recip * (length - i));
    }
}

// Window Method (makekernel_wind)

// A kernel windowed by a function stored in a second buffer

void kernelmaker_window(t_kernelmaker *x, t_symbol *msg, long argc, t_atom *argv)
{
    if (argc < 5)
        error ("kernelmaker~: not enough argments to message makekernel_wind");

    kernelmaker_window_internal(x, atom_getsym(argv + 0), atom_getsym(argv + 1), atom_getsym(argv + 2), atom_getlong(argv + 3), atom_getlong(argv + 4));
}

void kernelmaker_window_internal(t_kernelmaker *x, t_symbol *target_name, t_symbol *source_name, t_symbol *window_name, t_atom_long offset, t_atom_long length)
{
    ibuffer_data target(target_name);
    ibuffer_data source(source_name);
    ibuffer_data window(window_name);
            
    if (target.get_type() == kBufferMaxBuffer && target.get_length() && source.get_length() && window.get_length())
    {
        float *out_samps = reinterpret_cast<float *>(target.get_samples());

        offset = (offset < 0) ? 0 : offset;
        
        // Calculate and clip length

        length = std::min(length, target.get_length());
        length = std::min(length, source.get_length() - offset);
        
        // Zero length if the window is shorter than 513 samples
        
        length = (window.get_length() < 513) ? 0 : length;
        
        const double length_recip = 512.0 / length;
        
        double peak_amp = 0.0;

        for (t_ptr_int i = 0; i < length; i++)
        {
            double current_samp = ibuffer_get_samp(source, i + offset, 0);
            
            // Window the next sample
            
            const double pos = i * length_recip;
            const t_ptr_int ipos = (t_ptr_int) pos;
            const double fract = pos - ipos;
            
            const double lower = ibuffer_get_samp(window, ipos, 0);
            const double upper = ibuffer_get_samp(window, ipos + 1, 0);
            current_samp *= lower - (fract * (lower - upper));
            
            // Store the result
            
            out_samps[i * target.get_num_chans()] = static_cast<float>(current_samp);
            
            // Store the peak abs sample value
            
            current_samp = fabs(current_samp);
            if (current_samp > peak_amp)
                peak_amp = current_samp;
        }
        
        // Now normalise the kernel and set the buffer as dirty
        
        normalise_kernel(out_samps, length, target.get_num_chans(), peak_amp, 0);
        target.set_dirty();
    }
}

// Envelope Method (makekernel_env)

// Create a kernel windowed by an envelope derived from lowpass filtering the absolute vals of another buffer

void kernelmaker_env(t_kernelmaker *x, t_symbol *msg, long argc, t_atom *argv)
{
    if (argc < 5)
        error ("kernelmaker~: not enough argments to message makekernel_env");
    
    kernelmaker_env_internal(x, atom_getsym(argv + 0), atom_getsym(argv + 1), atom_getsym(argv + 2), atom_getlong(argv + 3), atom_getlong(argv + 4));
}

void kernelmaker_env_internal(t_kernelmaker *x, t_symbol *target_name, t_symbol *source_name, t_symbol *window_name, t_atom_long offset, t_atom_long slide)
{
    ibuffer_data target(target_name);
    ibuffer_data source(source_name);
    ibuffer_data window(window_name);
        
    if (target.get_type() == kBufferMaxBuffer && target.get_length() && source.get_length() && window.get_length())
    {
        float *out_samps = reinterpret_cast<float *>(target.get_samples());
        
        offset = (offset < 0) ? 0 : offset;
        
        // Calculate and clip length

        t_ptr_int length = window.get_length();
        length = std::min(length, target.get_length());
        length = std::min(length, source.get_length() - offset);
        
        double peak_amp = 0.;
        double slide_register = 0.0;
       
        const double slide_recip = 1.0 / static_cast<double>((slide < 1) ? 1 : slide);        // slideup;
        
        for (t_ptr_int i = 0; i < length; i++)
        {
            // Get samples from the buffers
            
            double current_samp = ibuffer_get_samp(source, i + offset, 0);
            double wind_val = fabs(ibuffer_get_samp(window, i, 0));
            
            // Window the sample (using slide filter on the window buffer)

            slide_register += slide_recip * (wind_val - slide_register);
            current_samp *= slide_register;
            
            // Store the result
            
            out_samps[i * target.get_num_chans()] = static_cast<float>(current_samp);
            
            // Store the peak abs sample value
            
            peak_amp = std::max(peak_amp, fabs(current_samp));
        }
        
        // Now normalise the kernel and set the buffer as dirty
        
        normalise_kernel(out_samps, length, target.get_num_chans(), peak_amp, x->fades);
        target.set_dirty();
    }
}

// Ring Modulated Method (makekernel_ring)

// Create a kernel from a buffer ring modulated using a second buffer

void kernelmaker_ring_mod(t_kernelmaker *x, t_symbol *msg, long argc, t_atom *argv)
{
    if (argc < 4)
        error ("kernelmaker~: not enough argments to message makekernel_ring");
    
    kernelmaker_ring_mod_internal(x, atom_getsym(argv + 0), atom_getsym(argv + 1), atom_getsym(argv + 2), atom_getlong(argv + 3));
}

void kernelmaker_ring_mod_internal(t_kernelmaker *x, t_symbol *target_name, t_symbol *source_name, t_symbol *window_name, t_atom_long offset)
{
    ibuffer_data target(target_name);
    ibuffer_data source(source_name);
    ibuffer_data window(window_name);
            
    if (target.get_type() == kBufferMaxBuffer && target.get_length() && source.get_length() && window.get_length())
    {
        float *out_samps = reinterpret_cast<float *>(target.get_samples());
        
        offset = (offset < 0) ? 0 : offset;
        
        // Calculate and clip length

        t_ptr_int length = window.get_length();
        length = std::min(length, target.get_length());
        length = std::min(length, source.get_length() - offset);
        
        double peak_amp = 0.0;
        
        for (t_ptr_int i = 0; i < length; i++)
        {
            // Ring modulate the two inputs sample and store the result

            double current_samp = ibuffer_get_samp(source, i + offset, 0) * ibuffer_get_samp(window, i, 0);
            out_samps[i * target.get_num_chans()] = static_cast<float>(current_samp);

            // Store the peak abs sample value
            
            peak_amp = std::max(peak_amp, fabs(current_samp));
        }
        
        // Now normalise the kernel and set the buffer as dirty
        
        normalise_kernel(out_samps, length, target.get_num_chans(), peak_amp, x->fades);
        target.set_dirty();
    }
}

// Trapezoid Method (makekernel_trap)

// Create a of a trapezioid shape with amplitude 1
// Points are set using the range 0-1 which is scaled over the length of the kernel
// This mode is intended to be used to create simple bandpass filter kernels for driving partconvolve~ in eq mode

void kernelmaker_trap(t_kernelmaker *x, t_symbol *msg, long argc, t_atom *argv)
{
    if (argc < 6)
        error ("kernelmaker~: not enough argments to message makekernel_trap");
    
    kernelmaker_trap_internal(x, atom_getsym(argv + 0), atom_getfloat(argv + 1), atom_getfloat(argv + 2), atom_getfloat(argv + 3), atom_getfloat(argv + 4), atom_getlong(argv + 5));
}

void kernelmaker_trap_internal(t_kernelmaker *x, t_symbol *target_name, double env1, double env2, double env3, double env4, t_atom_long length)
{
    ibuffer_data target(target_name);
            
    if (target.get_type() == kBufferMaxBuffer)
    {
        if (target.get_length() < length)
            length = target.get_length();
        
        if (length < 1)
            return;
        
        float *out_samps = reinterpret_cast<float *>(target.get_samples());

        // Clip envelope points and make sure they are correctly ordered
        
        env1 = std::min(env1, 1.0);
        env2 = std::min(std::max(env2, env1), 1.0);
        env3 = std::min(std::max(env3, env2), 1.0);
        env4 = std::min(std::max(env4, env3), 1.0);
        
        // Create trapezoid
        
        double length_recip = 1.0 / length;
        t_ptr_int i;

        for (i = 0; i < env1 * length; i++)
            out_samps[i * target.get_num_chans()] = 0.f;
        for (; i < env2 * length; i++)
            out_samps[i * target.get_num_chans()] = static_cast<float>(((i * length_recip) - env1) / (env2 - env1));
        for (; i < env3 * length; i++)
            out_samps[i * target.get_num_chans()] = 1.f;
        for (; i < env4 * length; i++)
            out_samps[i * target.get_num_chans()] = static_cast<float>(1.f - ((i * length_recip) - env3) / (env4 - env3));

        for (; i < length; i++)
            out_samps[i * target.get_num_chans()] = 0.f;
        
        // Set the buffer as dirty
        
        target.set_dirty();
    }
}
