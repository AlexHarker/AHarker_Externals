
/*
 *  vlog~
 *
 *	vlog~ is a vectorised version of log~.
 *
 *	This code is heavily based on Tempory_Unary.h which is slightly more commented in the case that anything here is unclear.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_VectorOpsExtended.h>
#include <AH_Denormals.h>
#include "Helpers.h"


#define MIN_INPUT_VAL 0.0000000001

vFloat min_input_val_3232 = {MIN_INPUT_VAL, MIN_INPUT_VAL, MIN_INPUT_VAL, MIN_INPUT_VAL};

#ifdef VECTOR_F64_128BIT
vDouble min_input_val_3264 = {MIN_INPUT_VAL, MIN_INPUT_VAL};
#endif

void *this_class;


typedef struct _vlog
{
    t_pxobject x_obj;
	
	float float_val;

	long proxy_id;
	void *base_input;
	
} t_vlog;


void *vlog_new();
void vlog_free(t_vlog *x);


void vlog_float(t_vlog *x, double datain);
void vlog_int(t_vlog *x, long datain);

void vlog_dsp(t_vlog *x, t_signal **sp, short *count);
#if (defined F32_VEC_LOG_OP || defined F32_VEC_LOG_ARRAY)
t_int *vlog_perform(t_int *w);
t_int *vlog_perform_misaligned(t_int *w);
#endif
t_int *vlog_perform_scalar(t_int *w);

void vlog_dsp64 (t_vlog *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
#if defined VECTOR_F64_128BIT && (defined F64_VEC_LOG_OP || defined F64_VEC_LOG_ARRAY)
void vlog_perform64 (t_vlog *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
#endif
void vlog_perform_scalar64 (t_vlog *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

void vlog_assist(t_vlog *x, void *b, long m, long a, char *s);


__inline float log_scalar_32 (float a, float base_const)
{
	a = a < MIN_INPUT_VAL ? MIN_INPUT_VAL : a;
	
	return logf(a) * base_const;
}


__inline double log_scalar_64 (double a, double base_const)
{
	a = a < MIN_INPUT_VAL ? MIN_INPUT_VAL : a;
	
	return log(a) * base_const;
}


int main(void)
{
    this_class = class_new ("vlog~", (method) vlog_new, (method)vlog_free, (short)sizeof(t_vlog), NULL, A_DEFFLOAT, 0);
	
	class_addmethod(this_class, (method)vlog_float, "float", A_FLOAT, 0);
	class_addmethod(this_class, (method)vlog_int, "int", A_LONG, 0);
	class_addmethod(this_class, (method)vlog_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)vlog_dsp64, "dsp64", A_CANT, 0);
	class_addmethod(this_class, (method)vlog_assist, "assist", A_CANT, 0);
	
	class_dspinit(this_class);
	class_register(CLASS_BOX, this_class);
	
	post ("vlog~ - using vector version by Alex Harker");
	
	return 0;
}


void vlog_free(t_vlog *x)
{
	dsp_free(&x->x_obj);
}


void *vlog_new(double base)
{
    t_vlog *x = (t_vlog *) object_alloc (this_class);
	
	x->base_input = proxy_new(x, 1, &x->proxy_id);
    dsp_setup((t_pxobject *)x, 1);
    outlet_new((t_object *)x,"signal");
	
	if (base)
		vlog_float(x, base);
	else 
		x->float_val = 1.f;

    return (x);
}


void vlog_float(t_vlog *x, double datain)
{
	x->float_val = 1.0 / log(datain);
}


void vlog_int(t_vlog *x, long datain)
{
	vlog_float(x,  (float) datain);
}


void vlog_dsp(t_vlog *x, t_signal **sp, short *count)
{
	// Default to scalar routine
	
	method current_perform_routine = (method) vlog_perform_scalar; 
	long vec_size_val = sp[0]->s_n;
	
#if (defined F32_VEC_LOG_OP || defined F32_VEC_LOG_ARRAY)
	if ((vec_size_val >> 2) > 0 || !SSE2_check())
	{
		vec_size_val >>= 2;
		
		// Check memory alignment of all relevant vectors
		
		if ((long) sp[0]->s_vec % 16 || (long) sp[1]->s_vec % 16)
		{
			current_perform_routine = (method) vlog_perform_misaligned;
			post ("vlog~: handed a misaligned signal vector - update to Max 5.1.3 or later");
		}
		else
			current_perform_routine = (method) vlog_perform;
	}
#endif
	
	dsp_add(denormals_perform, 5, current_perform_routine, sp[0]->s_vec, sp[1]->s_vec, vec_size_val, x);
}


#if (defined F32_VEC_LOG_OP || defined F32_VEC_LOG_ARRAY)

t_int *vlog_perform(t_int *w)
{
#ifndef F32_VEC_LOG_OP
	float *in1 = (float *)(w[2]);
	float *out1 = (float *)(w[3]);
    long vec_size = (long) w[4] << 2;
    t_vlog *x = (t_vlog *) w[5];
	
	vFloat base_const = float2vector(x->float_val);

	min_clip_array_32(out1, in1, vec_size, min_input_val_3232);
	F32_VEC_LOG_ARRAY(out1, out1, vec_size);
	mul_const_array_32(out1, out1, vec_size, base_const);
#else
    vFloat *in1 = (vFloat *)(w[2]);
	vFloat *out1 = (vFloat *)(w[3]);
    long vec_size = w[4];
    t_vlog *x = (t_vlog *) w[5];

	vFloat base_const = float2vector(x->float_val);

	while (vec_size--)
		*out1++ = F32_VEC_MUL_OP(base_const, F32_VEC_LOG_OP(F32_VEC_MAX_OP(min_input_val_3232, *in1++)));
#endif
	
	return w + 6;
}


t_int *vlog_perform_misaligned(t_int *w)
{
#ifndef F32_VEC_LOG_OP
	return vlog_perform(w);
#else
	vFloat *in1 = (vFloat *)(w[2]);
	vFloat *out1 = (vFloat *)(w[3]);
	long vec_size = w[4];
    t_vlog *x = (t_vlog *) w[5];
	
	vFloat Temp;
	vFloat base_const = float2vector(x->float_val);

	while (vec_size--)
	{
		Temp = F32_VEC_MUL_OP(base_const, F32_VEC_LOG_OP(F32_VEC_MAX_OP(min_input_val_3232, F32_VEC_ULOAD((float *) in1++))));
		F32_VEC_USTORE((float *) out1++, Temp);
	}
	
	return w + 6;
#endif		
}

#endif


t_int *vlog_perform_scalar(t_int *w)
{
    float *in1 = (float *)(w[2]);
	float *out1 = (float *)(w[3]);
    long vec_size = w[4];
    t_vlog *x = (t_vlog *) w[5];
	
	float base_const = x->float_val;
	
	while (vec_size--)
		*out1++ = log_scalar_32(*in1++, base_const);
	
	return w + 6;
}


// 64 bit dsp routine

void vlog_dsp64 (t_vlog *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{	
	// Default to scalar routine
	
	method current_perform_routine = (method) vlog_perform_scalar64; 
	
#if defined VECTOR_F64_128BIT && (defined F64_VEC_LOG_OP || defined F64_VEC_LOG_ARRAY)
	// Use SIMD routine if possible
	
	if ((maxvectorsize >> 1) > 0 && SSE2_check())
		current_perform_routine = (method) vlog_perform64;
#endif
	
	object_method(dsp64, gensym("dsp_add64"), x, current_perform_routine, 0, 0);
}


#if defined VECTOR_F64_128BIT && (defined F64_VEC_LOG_OP || defined F64_VEC_LOG_ARRAY)

// 64 bit perform routine (SIMD)

void vlog_perform64 (t_vlog *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
	vDouble base_const = double2vector(x->float_val);

#ifndef F64_VEC_LOG_OP
	min_clip_array_64(outs[0], ins[0], vec_size, min_input_val_3264);	
	F64_VEC_LOG_ARRAY(outs[0], outs[0], vec_size);
	mul_const_array_64(outs[0], outs[0], vec_size, base_const);
#else
	vDouble *in1 = (vDouble *) ins[0];
	vDouble *out1 = (vDouble *) outs[0];
	
	vec_size >>= 1;
	
	while (vec_size--)
		*out1++ = F64_VEC_MUL_OP(base_const, F64_VEC_LOG_OP(F64_VEC_MAX_OP(min_input_val_3264, *in1++)));;
#endif
}

#endif


// 64 bit perform routine (scalar calculations for small block sizes)

void vlog_perform_scalar64 (t_vlog *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
	double *in1 = ins[0];
	double *out1 = outs[0];
	
	double base_const = x->float_val;

	while (vec_size--)
		*out1++ = log_scalar_64(*in1++, base_const);
}


void vlog_assist(t_vlog *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) {
		sprintf(s,"(signal) In");
	}
    else {
		sprintf(s,"(signal) Out");
	}
}
