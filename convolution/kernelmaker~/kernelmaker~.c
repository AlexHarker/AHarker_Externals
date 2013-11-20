
/*
 *  kernelmaker~
 *
 *	kernelmaker~ copies samples from a buffer, in some kind of combination with samples from (an)other buffer(s) acting as modifiers (window functions etc.).
 *	kernelmaker~ can also be used to create simple bandpass filter kernels for partconvolve~'s eq mode - more shapes may be added later.
 *
 *	This is intended to allow the creation of small convolution kernels or impulse responses which can then be used with timeconvolve~ and/or partconvolve~.
 *	Because those externals copy the input of the buffer, you can dispose of the buffer, or change it once setting the convolution externals impulse response.
 *	In practice this means you can repeatedly use the same kernelmaker~ object, and buffer for creating impulses.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <ibuffer_access.h>


void *this_class;


typedef struct _kernelmaker
{
    t_pxobject x_obj;
	
	long fades;
	
} t_kernelmaker;


t_symbol *ps_dirty;



void *kernelmaker_new (long fades);
void kernelmaker_free (t_kernelmaker *x);
void kernelmaker_assist (t_kernelmaker *x, void *b, long m, long a, char *s);

void kernelmaker_int (t_kernelmaker *x, long fades);

void kernelmaker_normal (t_kernelmaker *x, t_symbol *msg, short argc, t_atom *argv);
void kernelmaker_normal_internal (t_kernelmaker *x, t_symbol *target_name, t_symbol *source_name, t_symbol *window_name, long offset, long length);

void kernelmaker_env (t_kernelmaker *x, t_symbol *msg, short argc, t_atom *argv);
void kernelmaker_env_internal (t_kernelmaker *x, t_symbol *target_name, t_symbol *source_name, t_symbol *window_name, long offset, long slide);

void kernelmaker_ring_mod (t_kernelmaker *x, t_symbol *msg, short argc, t_atom *argv);
void kernelmaker_ring_mod_internal (t_kernelmaker *x, t_symbol *target_name, t_symbol *source_name, t_symbol *window_name, long offset);

void kernelmaker_trap (t_kernelmaker *x, t_symbol *msg, short argc, t_atom *argv);
void kernelmaker_trap_internal (t_kernelmaker *x, t_symbol *target_name, double env1, double env2, double env3, double env4, long length);


int main (void)
{
    this_class = class_new ("kernelmaker~",
							(method) kernelmaker_new, 
							(method)kernelmaker_free, 
							sizeof(t_kernelmaker), 
							0L, 
							A_DEFLONG, 
							0);
	
	class_addmethod (this_class, (method)kernelmaker_int, "int", A_LONG, 0L);
	class_addmethod (this_class, (method)kernelmaker_normal, "makekernel_wind", A_GIMME, 0L);
	class_addmethod (this_class, (method)kernelmaker_env, "makekernel_env", A_GIMME, 0L);
	class_addmethod (this_class, (method)kernelmaker_ring_mod, "makekernel_ring", A_GIMME, 0L);
	class_addmethod (this_class, (method)kernelmaker_trap, "makekernel_trap", A_GIMME, 0L);
    class_addmethod (this_class, (method)kernelmaker_assist, "assist", A_CANT, 0L);
	
	class_register(CLASS_BOX, this_class);
	
	ibuffer_init ();
	
	ps_dirty = gensym("dirty");
	
	return 0;
}


void *kernelmaker_new (long fades)
{
    t_kernelmaker *x = (t_kernelmaker *)object_alloc (this_class);
	
	x->fades = !fades ? 40 : fades;
	
	return(x);
}


void kernelmaker_free(t_kernelmaker *x)
{
}


void kernelmaker_assist (t_kernelmaker *x, void *b, long m, long a, char *s)
{
	sprintf(s,"Instructions To Build Kernel");
}


void kernelmaker_int (t_kernelmaker *x, long fades)
{
	if (fades >= 0) 
		x->fades = fades;
}


// A kernel windowed by a function stored in a second buffer

void kernelmaker_normal (t_kernelmaker *x, t_symbol *msg, short argc, t_atom *argv)
{
	if (argc < 5)
		error ("kernelmaker~: not enough argments to message makekernel_env");

	kernelmaker_normal_internal(x, atom_getsym(argv + 0), atom_getsym(argv + 1), atom_getsym(argv + 2), atom_getlong(argv + 3), atom_getlong(argv + 4));
}


void kernelmaker_normal_internal (t_kernelmaker *x, t_symbol *target_name, t_symbol *source_name, t_symbol *window_name, long offset, long length)
{
	void *target = ibuffer_get_ptr(target_name);
	void *source = ibuffer_get_ptr(source_name);
	void *window = ibuffer_get_ptr(window_name);

	long t_length, s_length, w_length;
	long t_format, s_format, w_format;
	long t_chans, s_chans, w_chans;
	long ipos;
	long i;
	
	void *t_samps, *s_samps, *w_samps;
	float *out_samps;
	
	double lower, upper, pos, fract;
	double current_samp, length_recip, amp_recip;
	double peak_amp = 0.;
	
	if (target && ob_sym(target) == ps_buffer)
	{
		if (!ibuffer_info (target, &t_samps, &t_length, &t_chans, &t_format) ||
			!ibuffer_info (source, &s_samps, &s_length, &s_chans, &s_format) ||
			!ibuffer_info (window, &w_samps, &w_length, &w_chans, &w_format)) 
			return;
		
		ibuffer_increment_inuse(target);
		ibuffer_increment_inuse(source);
		ibuffer_increment_inuse(window);
		
		out_samps = (float *) t_samps;
		
		if (offset < 0) 
			offset = 0;
		
		if (t_length < length) 
			length = t_length;
		if (s_length < offset + length) 
			length = s_length - offset;
		if (w_length < 513) 
			length = 0;
		
		length_recip = 512.0 / length;
		
		for (i = 0; i < length; i++)
		{
			current_samp = ibuffer_double_get_samp (s_samps, i + offset, s_chans, 0, s_format); 
			
			// Window the next sample
			
			pos = i * length_recip;
			ipos = (long) pos;
			fract = pos - ipos;
			
			lower = ibuffer_double_get_samp (w_samps, ipos, w_chans, 0, w_format); 
			upper = ibuffer_double_get_samp (w_samps, ipos + 1, w_chans, 0, w_format);
			current_samp *= lower - (fract * (lower - upper));
			
			// Store the result
			
			out_samps[i * t_chans] = current_samp; 
			
			// Store the peak abs sample value
			
			current_samp = fabs(current_samp);
			if (current_samp > peak_amp) 
				peak_amp = current_samp;
		}
		
		// Now normalise the kernel
		
		if (peak_amp)
		{
			amp_recip = 1.0f / peak_amp;
			for (i = 0; i < length; i ++)
				out_samps[i * t_chans] *= amp_recip;
		}
		
		// Set the buffer as dirty
		
		object_method ((t_object *)target, ps_dirty);
		
		ibuffer_decrement_inuse(window);
		ibuffer_decrement_inuse(source);
		ibuffer_decrement_inuse(target);	
	}
}


// Create a kernel windowed by a function stored in another buffer and using an envelope derived from lowpass filtering the absolute vals of a third buffer

void kernelmaker_env (t_kernelmaker *x, t_symbol *msg, short argc, t_atom *argv)
{
	if (argc < 5)
		error ("kernelmaker~: not enough argments to message makekernel_env");
	
	kernelmaker_env_internal(x, atom_getsym(argv + 0), atom_getsym(argv + 1), atom_getsym(argv + 2), atom_getlong(argv + 3), atom_getlong(argv + 4));
}


void kernelmaker_env_internal (t_kernelmaker *x, t_symbol *target_name, t_symbol *source_name, t_symbol *window_name, long offset, long slide)
{
	void *target = ibuffer_get_ptr(target_name);
	void *source = ibuffer_get_ptr(source_name);
	void *window = ibuffer_get_ptr(window_name);
	
	void *t_samps, *s_samps, *w_samps;
	long t_length, s_length, w_length;
	long t_format, s_format, w_format;
	long t_chans, s_chans, w_chans;
	
	float *out_samps;
	
	double slide_register = 0.0;
	double slide_up_recip;
	double slide_down_recip;
	
	double current_samp, wind_val;
	double peak_amp = 0.;
	
	long fades = x->fades;
	long length, i;
	
	if (slide < 1)
		slide = 1;

	slide_up_recip = 1.0 / (double) slide;		// slideup;
	slide_down_recip = 1.0 / (double) slide;		// slidedown;

	if (target && ob_sym(target) == ps_buffer)
	{
		if (!ibuffer_info (target, &t_samps, &t_length, &t_chans, &t_format) ||
			!ibuffer_info (source, &s_samps, &s_length, &s_chans, &s_format) ||
			!ibuffer_info (window, &w_samps, &w_length, &w_chans, &w_format)) 
			return;
		
		ibuffer_increment_inuse(target);
		ibuffer_increment_inuse(source);
		ibuffer_increment_inuse(window);
		
		out_samps = (float *)t_samps;
		
		if (offset < 0) 
			offset = 0;
		
		length = w_length;
		
		if (t_length < length) 
			length = t_length;
		if (s_length < offset + length) 
			length = s_length - offset;
		
		for (i = 0; i < length; i++)
		{
			current_samp = ibuffer_double_get_samp (s_samps, i + offset, s_chans, 0, s_format); 
			
			// Window the next sample (using slide filter on the window buffer)
			
			wind_val = fabs (ibuffer_double_get_samp (w_samps, i, w_chans, 0, w_format));
			
			if (wind_val >= slide_register) 
				slide_register += slide_up_recip * (wind_val - slide_register);
			else 
				slide_register += slide_down_recip * (wind_val - slide_register);
			
			current_samp *= slide_register;
			
			// Store the result
			
			out_samps[i * t_chans] = current_samp; 
			
			// Store the peak abs sample value
			
			current_samp = fabs(current_samp);
			if (current_samp > peak_amp) 
				peak_amp = current_samp;
		}
		
		// Now normalise the kernel
		
		if (peak_amp)
		{
			double amp_recip = 1.0f / peak_amp;
			double faderecip = amp_recip / fades;
			
			for (i = 0; i < fades; i++)
				out_samps[i * t_chans] *= faderecip * i;
			for (; i < length - fades; i++)
				out_samps[i * t_chans] *= amp_recip;
			for (; i < length; i++)
				out_samps[i * t_chans] *= faderecip * (length - i);
		}
		
		// Set the buffer as dirty
		
		object_method ((t_object *)target, ps_dirty);
		
		ibuffer_decrement_inuse(window);
		ibuffer_decrement_inuse(source);
		ibuffer_decrement_inuse(target);	
	}
}


// Create a kernel windowed by a function stored in anohter buffer and ring modulated using a third buffer

void kernelmaker_ring_mod (t_kernelmaker *x, t_symbol *msg, short argc, t_atom *argv)
{
	if (argc < 4)
		error ("kernelmaker~: not enough argments to message makekernel_ring");
	
	kernelmaker_ring_mod_internal(x, atom_getsym(argv + 0), atom_getsym(argv + 1), atom_getsym(argv + 2), atom_getlong(argv + 3));
}


void kernelmaker_ring_mod_internal (t_kernelmaker *x, t_symbol *target_name, t_symbol *source_name, t_symbol *window_name, long offset)
{
	void *target = ibuffer_get_ptr(target_name);
	void *source = ibuffer_get_ptr(source_name);
	void *window = ibuffer_get_ptr(window_name);
	
	void *t_samps, *s_samps, *w_samps;
	long t_length, s_length, w_length;
	long t_format, s_format, w_format;
	long t_chans, s_chans, w_chans;
	
	float *out_samps;
	
	long fades = x->fades;
	long length, i;
	
	double current_samp;
	double peak_amp = 0.;
	
	if (target && ob_sym(target) == ps_buffer)
	{
		if (!ibuffer_info (target, &t_samps, &t_length, &t_chans, &t_format) ||
			!ibuffer_info (source, &s_samps, &s_length, &s_chans, &s_format) ||
			!ibuffer_info (window, &w_samps, &w_length, &w_chans, &w_format)) 
			return;
		
		ibuffer_increment_inuse(target);
		ibuffer_increment_inuse(source);
		ibuffer_increment_inuse(window);
		
		out_samps = (float *)t_samps;
		
		if (offset < 0) 
			offset = 0;
		
		length = w_length;
		if (t_length < length) 
			length = t_length;
		if (s_length < offset + length) 
			length = s_length - offset;
		
		for (i = 0; i < length; i++)
		{
			current_samp = ibuffer_double_get_samp (s_samps, i + offset, s_chans, 0, s_format); 
			
			// Ring modulate the next sample
			
			current_samp *= ibuffer_double_get_samp (w_samps, i, w_chans, 0, w_format); 
			
			// Store the result
			
			out_samps[i * t_chans] = current_samp; 
			
			// Store the peak abs sample value
			
			current_samp = fabs(current_samp);
			if (current_samp > peak_amp) 
				peak_amp = current_samp;
		}
		
		// Now normalise the kernel
		
		if (peak_amp)
		{
			double amp_recip = 1.0f / peak_amp;
			double faderecip = amp_recip / fades;
			
			for (i = 0; i < fades; i++)
				out_samps[i * t_chans] *= faderecip * i;
			for (; i < length - fades; i++)
				out_samps[i * t_chans] *= amp_recip;
			for (; i < length; i++)
				out_samps[i * t_chans] *= faderecip * (length - i);
		}
		
		// Set the buffer as dirty
		
		object_method ((t_object *)target, ps_dirty);
		
		ibuffer_decrement_inuse(window);
		ibuffer_decrement_inuse(source);
		ibuffer_decrement_inuse(target);	
	}
}


// Create a of a trapezioid shape with amplitude 1. with points at each of the normalised samples (0-1) given
// This mode is used to create simple bandpass filter kernels for driving partconvolve~ in eq mode

void kernelmaker_trap (t_kernelmaker *x, t_symbol *msg, short argc, t_atom *argv)
{
	if (argc < 6)
		error ("kernelmaker~: not enough argments to message makekernel_trap");
	
	kernelmaker_trap_internal(x, atom_getsym(argv + 0), atom_getfloat(argv + 1), atom_getfloat(argv + 2), atom_getfloat(argv + 3), atom_getfloat(argv + 4), atom_getlong(argv + 5));
}

void kernelmaker_trap_internal (t_kernelmaker *x, t_symbol *target_name, double env1, double env2, double env3, double env4, long length)
{
	void *target = ibuffer_get_ptr(target_name);
	
	double scaleval;
	double lengthrecip;
	
	void *t_samps;
	long t_length;
	long t_format;
	long t_chans;
	long i;
	
	float *out_samps;	
	
	if (target && ob_sym(target) == ps_buffer)
	{
		if (!ibuffer_info (target, &t_samps, &t_length, &t_chans, &t_format))
			return;
		
		ibuffer_increment_inuse(target);
		
		out_samps = (float *)t_samps;
		
		
		if (t_length < length) 
			length = t_length;
		
		if (length < 1)
		{
			ibuffer_decrement_inuse(target);	
			return;
		}
		
		if (env1 > 1.)
			env1 = 1.f;
		if (env2 > 1.)
			env2 = 1.f;
		if (env2 < env1)
			env2 = env1;
		if (env3 > 1.)
			env3 = 1.f;
		if (env3 < env2)
			env3 = env2;
		if (env4 > 1.)
			env4 = 1.f;
		if (env4 < env3)
			env4 = env3;
		
		lengthrecip = 1. / length;
		
		for (i = 0; i < env1 * length; i++)
			out_samps[i * t_chans] = 0.f;
		
		scaleval = env2 - env1;
		for (; i < env2 * length; i++)
			out_samps[i * t_chans] = ((i * lengthrecip) - env1) / scaleval;
		
		for (; i < env3 * length; i++)
			out_samps[i * t_chans] = 1.f;
		
		scaleval = env4 - env3;
		for (; i < env4 * length; i++)
			out_samps[i * t_chans] = 1.f - ((i * lengthrecip) - env3) / scaleval;
		
		for (; i < length; i++)
			out_samps[i * t_chans] = 0.f;
		
		// Set the buffer as dirty
		
		object_method ((t_object *)target, ps_dirty);
		
		ibuffer_decrement_inuse(target);	
	}
}
