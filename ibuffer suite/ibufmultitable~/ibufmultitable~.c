
/*
 *  ibufmultitable~
 *
 *	ibufmultitable~ is an efficient object designed for table lookup (for window functions etc.) with multiple functions stored in a single buffer. It is an extended version of the ibufmultitable~ object.
 *
 *	ibufmultitable~ features SIMD optimisation and four types of interpolation (linear interpolation and three different kinds of cubic interpolation which can be requested as desired.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */



#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_VectorOps.h>
#include <AH_Denormals.h>
#include <ibuffer_access.h>
#include <dynamicdsp~.h>


void *this_class;


typedef struct _ibufmultitable
{
    t_pxobject x_obj;
	
	t_object *buffer_pointer;
	t_symbol *buffer_name;
	
	long start_samp;
	long end_samp;
	long chan;	
	long interp_mode;
	
	void *default_mem;
	void **temp_mem;
	
	void *dynamicdsp_parent;
	long dynamicdsp_index;
	
} t_ibufmultitable;


void *ibufmultitable_new (t_symbol *the_buffer, long start_samp, long end_samp, long chan);
void ibufmultitable_free (t_ibufmultitable *x);
void ibufmultitable_assist (t_ibufmultitable *x, void *b, long m, long a, char *s);

void ibufmultitable_interp (t_ibufmultitable *x, t_symbol *msg, short argc, t_atom *argv);
void ibufmultitable_set(t_ibufmultitable *x, t_symbol *msg, short argc, t_atom *argv);
void ibufmultitable_set_internal (t_ibufmultitable *x, t_symbol *s);
void ibufmultitable_startsamp (t_ibufmultitable *x, long startsamp);
void ibufmultitable_endsamp (t_ibufmultitable *x, long endsamp);
void ibufmultitable_chan (t_ibufmultitable *x, long chan);

__inline float ibufmultitable_clip_zero_one (float in);
t_int *ibufmultitable_perform (t_int *w);
t_int *ibufmultitable_perform_small (t_int *w);
void ibufmultitable_dsp (t_ibufmultitable *x, t_signal **sp, short *count);

__inline double ibufmultitable_clip_zero_one64 (double in);
void ibufmultitable_perform64 (t_ibufmultitable *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void ibufmultitable_perform_small64 (t_ibufmultitable *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void ibufmultitable_dsp64 (t_ibufmultitable *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);


int main (void)
{	
	this_class = class_new("ibufmultitable~",
						   (method)ibufmultitable_new,
						   (method)ibufmultitable_free,
						   sizeof(t_ibufmultitable), 
						   NULL, 
						   A_SYM, 
						   A_DEFLONG, 
						   A_DEFLONG, 
						   A_DEFLONG, 
						   0);
	
	class_addmethod(this_class, (method)ibufmultitable_interp, "interp", A_GIMME, 0);
	class_addmethod(this_class, (method)ibufmultitable_set, "set", A_GIMME, 0);	
	class_addmethod(this_class, (method)ibufmultitable_startsamp, "startsamp", A_LONG, 0);	
	class_addmethod(this_class, (method)ibufmultitable_endsamp, "endsamp", A_LONG, 0);	
	class_addmethod(this_class, (method)ibufmultitable_chan, "chan", A_LONG, 0);	
	class_addmethod(this_class, (method)ibufmultitable_assist, "assist", A_CANT, 0);
	class_addmethod(this_class, (method)ibufmultitable_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)ibufmultitable_dsp64, "dsp64", A_CANT, 0);
	
	class_dspinit(this_class);
	class_register(CLASS_BOX, this_class);
	
	ibuffer_init ();
	
	return 0;
}


void *ibufmultitable_new(t_symbol *the_buffer, long start_samp, long end_samp, long chan)
{
    t_ibufmultitable *x = (t_ibufmultitable *)object_alloc(this_class);
    
    dsp_setup((t_pxobject *)x, 2);
	outlet_new((t_object *)x, "signal");
	
	// Default variables
	
	x->buffer_pointer = 0;
	x->buffer_name = the_buffer;
	
	x->default_mem = 0;
	x->temp_mem = 0;
	
	// Arguments
	
	if (start_samp < 0) 
		start_samp = 0;
	if (end_samp < 0) 
		end_samp = 0;
	if (end_samp == 0) 
		end_samp = 512;
	if (end_samp < start_samp) 
		end_samp = start_samp;
	if (chan < 1) 
		chan = 1;
	
	if (start_samp > end_samp)
	{
		long swap = end_samp;
		end_samp = start_samp;
		start_samp = swap;
	}
	
	x->start_samp = start_samp;
	x->end_samp = end_samp;
	x->chan = chan;
	x->interp_mode = INTERP_TYPE_LIN;
	
	// Get dynamicdsp~ host if it exists (for temporary memory)
	
	x->dynamicdsp_parent = Get_Dynamic_Object();
	x->dynamicdsp_index = Get_Dynamic_Patch_Index(x->dynamicdsp_parent);
	
	return (x);
}


void ibufmultitable_free(t_ibufmultitable *x)
{
	dsp_free(&x->x_obj);
	ALIGNED_FREE(x->default_mem);
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


void ibufmultitable_interp (t_ibufmultitable *x, t_symbol *msg, short argc, t_atom *argv)
{
	t_symbol *mode = argc ? atom_getsym(argv) : ps_linear;
						
	if (mode == ps_linear)
		x->interp_mode = INTERP_TYPE_LIN;
	if (mode == ps_bspline)
		x->interp_mode = INTERP_TYPE_CUBIC_BSPLINE;
	if (mode == ps_hermite)
		x->interp_mode = INTERP_TYPE_CUBIC_HERMITE;
	if (mode == ps_lagrange)
		x->interp_mode = INTERP_TYPE_CUBIC_LAGRANGE;
	
	if (mode != ps_linear && mode != ps_bspline &&  mode != ps_hermite && mode != ps_lagrange)
		error ("ibufmultitable~: no interpolation mode %s", mode->s_name);
}
							

void ibufmultitable_set(t_ibufmultitable *x, t_symbol *msg, short argc, t_atom *argv)
{
	ibufmultitable_set_internal(x, argc ? atom_getsym(argv) : 0);
}


void ibufmultitable_set_internal (t_ibufmultitable *x, t_symbol *s)
{
	void *b = ibuffer_get_ptr (s);
	
	if (b) 
	{	
		x->buffer_pointer = b;
		x->buffer_name = s;
	}
	else 
	{
		x->buffer_pointer = 0;
		x->buffer_name = 0;
		if (s)
			error ("ibufmultitable~: no buffer %s", s->s_name);
	}
}


void ibufmultitable_startsamp (t_ibufmultitable *x, long start_samp)
{
	if (start_samp < 0)
		start_samp = 0;
	
	x->start_samp = start_samp;
}


void ibufmultitable_endsamp (t_ibufmultitable *x, long end_samp)
{
	if (end_samp < 0)
		end_samp = 0;
	
	x->end_samp = end_samp;
}


void ibufmultitable_chan (t_ibufmultitable *x, long chan)
{
	if (chan < 1)
		chan = 1;
	
	x->chan = chan;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// Perform and DSP for 32-bit signals ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	


__inline float ibufmultitable_clip_zero_one (float in)
{
	in = in > 1.0 ? 1.0 : in;
	in = in < 0.0 ? 0.0 : in;
	
	return in;
}


t_int *ibufmultitable_perform (t_int *w)
{	
	// ignore the copy of this function pointer (due to denormal fixer)
	
	// Set pointers
	
	vFloat *in = (vFloat *) w[2];
	vFloat *offset_in = (vFloat *) w[3];
	vFloat *out = (vFloat *)(w[4]);
	long vec_size = w[5];
    t_ibufmultitable *x = (t_ibufmultitable *) w[6];
	
	vSInt32 *offsets = *x->temp_mem;
	long *l_offsets = (long *) offsets;
	vFloat *fracts =  (vFloat *) (l_offsets + vec_size);
	vFloat *temp_fracts = fracts;
	
	void *temp[4];
	
	// Standard ibuffer variables
	
	void *buffer_pointer = x->buffer_pointer;
	void *samps = 0;
	long format = -1;
	long n_chans = 0;
	long length = 0;
	
	// Object variables
	
	long start_samp = x->start_samp;
	long end_samp = x->end_samp;
	long chan = x->chan - 1;
	long interp_mode = x->interp_mode;
	
	// Local variables
	
	vFloat One = float2vector(1.f);
	vFloat Zero = float2vector(0.f);
	vFloat length_mult;
	vFloat offset_limit;
	vFloat vec_f;
	vSInt32 vec_i;
	long on = 0;	
	long i;
		
	temp[0] = (void *) out;
	temp[1] = (void *) (l_offsets + (2 * vec_size));
	temp[2] = (void *) (l_offsets + (3 * vec_size));
	temp[3] = (void *) (l_offsets + (4 * vec_size));
	
	if (x->x_obj.z_disabled) 
		goto bail;
	
	// Check if the buffer is set / valid and get the length information
	
	if (buffer_pointer && ibuffer_info (buffer_pointer, &samps, &length, &n_chans, &format))
	{		
		if (start_samp > length - 1) 
			start_samp = length - 1; 
		
		if (end_samp > length - 1) 
			end_samp = length - 1;  
		
		length_mult = float2vector(end_samp - start_samp);
		
		if (chan >= n_chans) 
			chan = 0;		
		
		if ((length - start_samp) >= 1)
		{
			offset_limit = float2vector((float) (length - (start_samp + 1)));
			on = 1;
		}
	}
	
	// Calculate output
	
	if (on)
	{
		ibuffer_increment_inuse (buffer_pointer);
		
		for (i = 0; i < vec_size >> 3; i++)
		{
			vec_f = F32_VEC_MUL_OP (length_mult, F32_VEC_MIN_OP (One, F32_VEC_MAX_OP (Zero, *in++)));
			vec_f = F32_VEC_MIN_OP (offset_limit, F32_VEC_MAX_OP (Zero, F32_VEC_ADD_OP(vec_f, *offset_in++)));
			vec_i = I32_VEC_FROM_F32_TRUNC (vec_f);
			*temp_fracts++ = F32_VEC_SUB_OP (vec_f, F32_VEC_FROM_I32(vec_i));
			*offsets++ = vec_i;
			
			vec_f = F32_VEC_MUL_OP (length_mult, F32_VEC_MIN_OP (One, F32_VEC_MAX_OP (Zero, *in++)));
			vec_f = F32_VEC_MIN_OP (offset_limit, F32_VEC_MAX_OP (Zero, F32_VEC_ADD_OP(vec_f, *offset_in++)));
			vec_i = I32_VEC_FROM_F32_TRUNC (vec_f);
			*temp_fracts++ = F32_VEC_SUB_OP (vec_f, F32_VEC_FROM_I32(vec_i));
			*offsets++ = vec_i;
		}
		
		// Preprocess ready to use with the routines below
		
		ibuffer_preprocess_offsets (l_offsets, vec_size, n_chans, format);
		
		// Get samples and interpolate as relevant
		
		switch (interp_mode)
		{
			case INTERP_TYPE_LIN:
				ibuffer_float_samps_simd_linear (ibuffer_offset (samps, start_samp, n_chans, format), out, l_offsets, fracts, temp, vec_size, n_chans, chan, format, 1.f);
				break;
			case INTERP_TYPE_CUBIC_BSPLINE:
				ibuffer_float_samps_simd_cubic_bspline (ibuffer_offset (samps, start_samp, n_chans, format), out, l_offsets, fracts, temp, vec_size, n_chans, chan, format, 1.f);
				break;
			case INTERP_TYPE_CUBIC_HERMITE:
				ibuffer_float_samps_simd_cubic_hermite (ibuffer_offset (samps, start_samp, n_chans, format), out, l_offsets, fracts, temp, vec_size, n_chans, chan, format, 1.f);
				break;
			case INTERP_TYPE_CUBIC_LAGRANGE:
				ibuffer_float_samps_simd_cubic_lagrange (ibuffer_offset (samps, start_samp, n_chans, format), out, l_offsets, fracts, temp, vec_size, n_chans, chan, format, 1.f);
				break;
		}
		
		ibuffer_decrement_inuse (buffer_pointer);
	}
	else
	{		
		for (i = 0; i < vec_size >> 2; i++)
			*out++ = Zero;
	}
	
bail:
	
	return w + 7;
}


t_int *ibufmultitable_perform_small (t_int *w)
{	
	// Ignore the copy of this function pointer (due to denormal fixer)
	
	// Set pointers
	
	float *in = (float *) w[2];
	float *offset_in = (float *) w[3];
	float *out = (float *)(w[4]);
	long vec_size = w[5];
    t_ibufmultitable *x = (t_ibufmultitable *) w[6];
		
	long *offsets = (long *) *x->temp_mem;
	float *fracts =  (float *) (offsets + vec_size);
	
	// Standard ibuffer variables
	
	void *buffer_pointer = x->buffer_pointer;
	void *samps = 0;
	long format = -1;
	long n_chans = 0;
	long length = 0;
	
	// Object variables
	
	long start_samp = x->start_samp;
	long end_samp = x->end_samp;
	long chan = x->chan - 1;
	long interp_mode = x->interp_mode;
	
	// Local variables
	
	float length_mult, f_temp;
	long offset_limit = 0;
	long o_temp;
	long on = 0;	
	long i;
	
	if (x->x_obj.z_disabled) 
		goto bail;
	
	// Check if the buffer is set / valid and get the length information
	
	if (buffer_pointer && ibuffer_info (buffer_pointer, &samps, &length, &n_chans, &format))
	{		
		if (start_samp > length - 1) 
			start_samp = length - 1; 
		
		if (end_samp > length - 1) 
			end_samp = length - 1; 
		
		length_mult = (float) (end_samp - start_samp);
		
		if (chan >= n_chans) 
			chan = 0;		
	
		if ((length - start_samp) >= 1)
		{
			offset_limit = (float) (length - (start_samp + 1));
			on = 1;
		}
	}
	
	// Calculate output
	
	if (on)
	{
		ibuffer_increment_inuse (buffer_pointer);
		samps = ibuffer_offset (samps, start_samp, n_chans, format);
		
		// Calculate offsets and fracts
		
		for (i = 0; i < vec_size; i++)
		{
			f_temp = length_mult * ibufmultitable_clip_zero_one64(*in++);
			f_temp += *offset_in++;
			f_temp = (f_temp > offset_limit) ? offset_limit : f_temp;		
			f_temp = f_temp < 0 ? 0 : f_temp;
			o_temp = (long) f_temp;
			offsets[i] = o_temp;
			fracts[i] = f_temp - (float) o_temp;
		}
		
		// Get samples and interpolate as relevant
		
		switch (interp_mode)
		{
			case INTERP_TYPE_LIN:
				ibuffer_float_samps_scalar_linear(samps, out, offsets, fracts, vec_size, n_chans, chan, format, 1.f);
				break;
				
			case INTERP_TYPE_CUBIC_BSPLINE:
				ibuffer_float_samps_scalar_cubic_bspline(samps, out, offsets, fracts, vec_size, n_chans, chan, format, 1.f);
				break;
				
			case INTERP_TYPE_CUBIC_HERMITE:
				ibuffer_float_samps_scalar_cubic_hermite(samps, out, offsets, fracts, vec_size, n_chans, chan, format, 1.f);
				break;
				
			case INTERP_TYPE_CUBIC_LAGRANGE:
				ibuffer_float_samps_scalar_cubic_lagrange(samps, out, offsets, fracts, vec_size, n_chans, chan, format, 1.f);
				break;
		}
		
		ibuffer_decrement_inuse (buffer_pointer);
	}
	else
	{		
		for (i = 0; i < vec_size; i++)
			*out++ = 0.f;
	}
	
bail:
	
	return w + 7;
}


void ibufmultitable_dsp(t_ibufmultitable *x, t_signal **sp, short *count)
{	
	long mem_size = sp[0]->s_n * (4 * sizeof(float) + sizeof(long));
	
	// Try to access dynamicdsp~ host tempory memory
	
	ALIGNED_FREE(x->default_mem);
	
	if (!Dynamic_Temp_Mem_Resize (x->dynamicdsp_parent, x->dynamicdsp_index, mem_size)) 
		x->default_mem = ALIGNED_MALLOC (mem_size);
	
	x->temp_mem = Dynamic_Get_TempMem (x->dynamicdsp_parent, x->dynamicdsp_index, &x->default_mem);
	
	// Set buffer again in case it is no longer valid / extant
	
	ibufmultitable_set_internal(x, x->buffer_name);
	
	if (sp[0]->s_n >= 8 && SSE2_check())
		dsp_add(denormals_perform, 6, ibufmultitable_perform, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n, x);
	else 
		dsp_add(denormals_perform, 6, ibufmultitable_perform_small, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n, x);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// Perform and DSP for 64-bit signals ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	


__inline double ibufmultitable_clip_zero_one64 (double in)
{
	in = in > 1.0 ? 1.0 : in;
	in = in < 0.0 ? 0.0 : in;
	
	return in;
}


#ifdef VECTOR_F64_128BIT

// FIX N.B. Limits only valid for a start samp of 0..... also offset could be negative!!!

void ibufmultitable_perform64 (t_ibufmultitable *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{		
	// Set pointers
	
	vDouble *in = (vDouble *) ins[0];
	vDouble *offset_in = (vDouble *) ins[1];
	vDouble *out = (vDouble *) outs[0];
	
	vDouble *fracts = *x->temp_mem;
	vDouble *temp_fracts = fracts;
	vSInt32 *offsets = (vSInt32 *) (fracts + (vec_size >> 1));
	long *l_offsets = (long *) offsets;
	
	void *temp[4];
	
	// Standard ibuffer variables
	
	void *buffer_pointer = x->buffer_pointer;
	void *samps = 0;
	long format = -1;
	long n_chans = 0;
	long length = 0;
	
	// Object variables
	
	long start_samp = x->start_samp;
	long end_samp = x->end_samp;
	long chan = x->chan - 1;
	long interp_mode = x->interp_mode;
	
	// Local variables
	
	vDouble One = double2vector(1.0);
	vDouble Zero = double2vector(0.0);
	vDouble offset_limit;
	vDouble length_mult;
	vDouble vec_f1;
	vSInt32 vec_i1, vec_i2;
	long on = 0;	
	long i;
		
	temp[0] = (void *) (out + (vec_size >> 2));
	temp[1] = (void *) (l_offsets + (1 * vec_size));
	temp[2] = (void *) (l_offsets + (2 * vec_size));
	temp[3] = (void *) (l_offsets + (3 * vec_size));
	
	if (x->x_obj.z_disabled) 
		return;
	
	// Check if the buffer is set / valid and get the length information
	
	if (buffer_pointer && ibuffer_info (buffer_pointer, &samps, &length, &n_chans, &format))
	{		
		if (start_samp > length - 1) 
			start_samp = length - 1; 
		
		if (end_samp > length - 1) 
			end_samp = length - 1;  
		
		length_mult = double2vector(end_samp - start_samp);
		
		if (chan >= n_chans) 
			chan = 0;		
		
		if ((length - start_samp) >= 1)
		{
			offset_limit = double2vector((double) (length - (start_samp + 1)));
			on = 1;
		}
	}
	
	// Calculate output
	
	if (on)
	{
		ibuffer_increment_inuse (buffer_pointer);
		
		for (i = 0; i < vec_size >> 2; i++)
		{
			vec_f1 = F64_VEC_MUL_OP (length_mult, F64_VEC_MIN_OP (One, F64_VEC_MAX_OP (Zero, *in++)));
			vec_f1 = F64_VEC_MIN_OP (offset_limit, F64_VEC_MAX_OP (Zero, F64_VEC_ADD_OP(vec_f1, *offset_in++)));
			vec_i1 = I32_VEC_FROM_F64_TRUNC (vec_f1);
			*temp_fracts++ = F64_VEC_SUB_OP (vec_f1, F64_VEC_FROM_I32(vec_i1));

			vec_f1 = F64_VEC_MUL_OP (length_mult, F64_VEC_MIN_OP (One, F64_VEC_MAX_OP (Zero, *in++)));
			vec_f1 = F64_VEC_MIN_OP (offset_limit, F64_VEC_MAX_OP (Zero, F64_VEC_ADD_OP(vec_f1, *offset_in++)));
			vec_i2 = I32_VEC_FROM_F64_TRUNC (vec_f1);
			*temp_fracts++ = F64_VEC_SUB_OP (vec_f1, F64_VEC_FROM_I32(vec_i2));
			
			*offsets++ = I32_VEC_OR_OP (vec_i1, I32_VEC_SHUFFLE_OP (vec_i2, 0x4E));
		}
		
		// Preprocess ready to use with the routines below
		
		ibuffer_preprocess_offsets (l_offsets, vec_size, n_chans, format);
		
		// Get samples and interpolate as relevant
		
		switch (interp_mode)
		{
			case INTERP_TYPE_LIN:
				ibuffer_double_samps_simd_linear(ibuffer_offset(samps, start_samp, n_chans, format), out, l_offsets, fracts, temp, vec_size, n_chans, chan, format, 1.f);
				break;
			case INTERP_TYPE_CUBIC_BSPLINE:
				ibuffer_double_samps_simd_cubic_bspline(ibuffer_offset(samps, start_samp, n_chans, format), out, l_offsets, fracts, temp, vec_size, n_chans, chan, format, 1.f);
				break;
			case INTERP_TYPE_CUBIC_HERMITE:
				ibuffer_double_samps_simd_cubic_hermite(ibuffer_offset(samps, start_samp, n_chans, format), out, l_offsets, fracts, temp, vec_size, n_chans, chan, format, 1.f);
				break;
			case INTERP_TYPE_CUBIC_LAGRANGE:
				ibuffer_double_samps_simd_cubic_lagrange(ibuffer_offset(samps, start_samp, n_chans, format), out, l_offsets, fracts, temp, vec_size, n_chans, chan, format, 1.f);
				break;
		}
		
		ibuffer_decrement_inuse (buffer_pointer);
	}
	else
	{		
		for (i = 0; i < vec_size >> 1; i++)
			*out++ = Zero;
	}
}

#else

void ibufmultitable_perform64(t_ibufmultitable *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
	ibufmultitable_perform_small64(x, dsp64, ins, numins, outs, numouts, vec_size, flags, userparam); 
}

#endif


void ibufmultitable_perform_small64 (t_ibufmultitable *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
	// Set pointers
	
	double *in = ins[0];
	double *offset_in = ins[1];
	double *out = outs[0];
	
	long *offsets = (long *) *x->temp_mem;
	double *fracts =  (double *) (offsets + vec_size);
	
	// Standard ibuffer variables
	
	void *buffer_pointer = x->buffer_pointer;
	void *samps = 0;
	long format = -1;
	long n_chans = 0;
	long length = 0;
	
	// Object variables
	
	long start_samp = x->start_samp;
	long end_samp = x->end_samp;
	long chan = x->chan - 1;
	long interp_mode = x->interp_mode;
	
	// Local variables

	double offset_limit = 0;
	double length_mult, f_temp;
	long o_temp;
	long on = 0;	
	long i;
	
	if (x->x_obj.z_disabled) 
		return;
	
	// Check if the buffer is set / valid and get the length information
	
	if (buffer_pointer && ibuffer_info (buffer_pointer, &samps, &length, &n_chans, &format))
	{
		if (start_samp > length - 1) 
			start_samp = length - 1; 
		
		if (end_samp > length - 1) 
			end_samp = length - 1;  
		
		length_mult = (double) (end_samp - start_samp);
		
		if (chan >= n_chans) 
			chan = 0;		
		
		if ((length - start_samp) >= 1)
		{
			offset_limit = (double) (length - (start_samp + 1));
			on = 1;
		}
	}
	
	// Calculate output
	
	if (on)
	{
		ibuffer_increment_inuse (buffer_pointer);
		samps = ibuffer_offset (samps, start_samp, n_chans, format);
		
		// Calculate offsets and fracts
		
		for (i = 0; i < vec_size; i++)
		{
			f_temp = length_mult * ibufmultitable_clip_zero_one64(*in++);
			f_temp += *offset_in++;
			f_temp = (f_temp > offset_limit) ? offset_limit : f_temp;		
			f_temp = f_temp < 0.0 ? 0.0 : f_temp;
			o_temp = (long) f_temp;
			offsets[i] = o_temp;
			fracts[i] = f_temp - (double) o_temp;
		}
		
		// Get samples and interpolate as relevant
		
		switch (interp_mode)
		{
			case INTERP_TYPE_LIN:
				ibuffer_double_samps_scalar_linear(samps, out, offsets, fracts, vec_size, n_chans, chan, format, 1.0);
				break;
				
			case INTERP_TYPE_CUBIC_BSPLINE:
				ibuffer_double_samps_scalar_cubic_bspline(samps, out, offsets, fracts, vec_size, n_chans, chan, format, 1.0);
				break;
				
			case INTERP_TYPE_CUBIC_HERMITE:
				ibuffer_double_samps_scalar_cubic_hermite(samps, out, offsets, fracts, vec_size, n_chans, chan, format, 1.0);
				break;
				
			case INTERP_TYPE_CUBIC_LAGRANGE:
				ibuffer_double_samps_scalar_cubic_lagrange(samps, out, offsets, fracts, vec_size, n_chans, chan, format, 1.0);
				break;
		}
		
		ibuffer_decrement_inuse (buffer_pointer);
	}
	else
	{		
		for (i = 0; i < vec_size; i++)
			*out++ = 0.0;
	}
}


void ibufmultitable_dsp64 (t_ibufmultitable *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{	
	long mem_size = maxvectorsize * (3 * sizeof(float) + sizeof(double) + sizeof(long));
	
	// Try to access dynamicdsp~ host tempory memory
	
	ALIGNED_FREE(x->default_mem);
	
	if (!Dynamic_Temp_Mem_Resize (x->dynamicdsp_parent, x->dynamicdsp_index, mem_size)) 
		x->default_mem = ALIGNED_MALLOC (mem_size);
	
	x->temp_mem = Dynamic_Get_TempMem (x->dynamicdsp_parent, x->dynamicdsp_index, &x->default_mem);
	
	// Set buffer again in case it is no longer valid / extant
	
	ibufmultitable_set_internal(x, x->buffer_name);
	
	if (maxvectorsize >= 8 && SSE2_check())
		object_method(dsp64, gensym("dsp_add64"), x, ibufmultitable_perform64);
	else 
		object_method(dsp64, gensym("dsp_add64"), x, ibufmultitable_perform_small64);
}



