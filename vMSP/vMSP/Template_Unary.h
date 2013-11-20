
/*
 *  Template_Unary
 *
 *	This is a template file for a SIMD unary operator MSP object. 
 *	It should only be necessary to provide a few macro definitions before this code to comiple a fully functional object.
 *	See other source files for examples (e.g. vabs~.c).
 *
 *	The compiled object will support all block sizes, using SIMD where possible.
 *
 *	The following *MUST* be defined:
 *
 *	OBJNAME_STR			- The object name as a string (i.e. - with quotes)
 *	OBJNAME_FIRST(a)	- A macro to combine the object name fllowed by a given string
 *
 *		Should be defined like:
 *		#define OBJNAME_FIRST(a) objname ## a
 *	
 *	OBJNAME_SECOND(a)	- A macro to combine the object name followed by a given string
 *
 *		Should be defined like:
 *		#define OBJNAME_SECOND(a) a ## objname
 *
 *	F32_VEC_OP				- A unary 32 bit floating point SIMD operator (vFloat)
 *	F32_VEC_ARRAY			- A unary 32 bit floating point SIMD array function (float *out, float *out, long length)
 *	F32_SCALAR_OP			- A unary 32 bit floating point scalar operator (float)
 *
 *  To use the array version over the op define USE_F32_VEC_ARRAY
 *	To avoid using 32 bit vectors at all define NO_F32_SIMD
 *	It is best to define these based on the available functions (preferring the operation, then the array version, then no SIMD)
 *
 *	F64_VEC_OP				- A unary 64 bit floating point SIMD operator (vDouble)
 *	F64_VEC_ARRAY			- A unary 64 bit floating point SIMD array function (double *out, double *out, long length)
 *	F64_SCALAR_OP			- A unary 64 bit floating point scalar operator (double)
 *
 *  To use the array version over the op define USE_F32_VEC_ARRAY or USE_F64_VEC_ARRAY as appropriate 
 *	To avoid using vector processing at all define NO_F32_SIMD or NO_F64_SIMD
 *	It is best to define these based on the available functions (preferring the operation, then the array version, then no SIMD)
 *
 *	Optionally for objects requiring constants you can define:
 *
 *	SET_CONSTANTS		- Set constants in main routine (if necessary)
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_VectorOpsExtended.h>
#include <AH_Denormals.h>


void *this_class;


// Object structure

typedef struct OBJNAME_SECOND(_)
{
    t_pxobject x_obj;
			
} OBJNAME_SECOND(t_);


void *OBJNAME_FIRST(_new)();
void OBJNAME_FIRST(_free)(OBJNAME_SECOND(t_) *x);

void OBJNAME_FIRST(_dsp)(OBJNAME_SECOND(t_) *x, t_signal **sp, short *count);
#ifndef NO_F32_SIMD
t_int *OBJNAME_FIRST(_perform)(t_int *w);
t_int *OBJNAME_FIRST(_perform_misaligned)(t_int *w);
#endif
t_int *OBJNAME_FIRST(_perform_scalar)(t_int *w);

void OBJNAME_FIRST(_dsp64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
#if (defined VECTOR_F64_128BIT) && !defined NO_F64_SIMD
void OBJNAME_FIRST(_perform64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
#endif
void OBJNAME_FIRST(_perform_scalar64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

void OBJNAME_FIRST(_assist)(OBJNAME_SECOND(t_) *x, void *b, long m, long a, char *s);



// Main routine

int main(void)
{
    this_class = class_new (OBJNAME_STR, (method) OBJNAME_FIRST(_new), (method)OBJNAME_FIRST(_free), (short)sizeof(OBJNAME_SECOND(t_)), NULL, 0);
	
	class_addmethod(this_class, (method)OBJNAME_FIRST(_dsp), "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)OBJNAME_FIRST(_dsp64), "dsp64", A_CANT, 0);
	class_addmethod(this_class, (method)OBJNAME_FIRST(_assist), "assist", A_CANT, 0);
    
	class_dspinit(this_class);
	class_register(CLASS_BOX, this_class);
	
	// Constants are set here (if they cannot be set in global code)
	
#ifdef SET_CONSTANTS
	SET_CONSTANTS
#endif

	post ("%s - using vector version by Alex Harker", OBJNAME_STR);
	
	return 0;
}


// Free routine

void OBJNAME_FIRST(_free)(OBJNAME_SECOND(t_) *x)
{
	dsp_free(&x->x_obj);
}


// New routine

void *OBJNAME_FIRST(_new)()
{
    OBJNAME_SECOND(t_) *x = (OBJNAME_SECOND(t_) *) object_alloc (this_class);
		
    dsp_setup((t_pxobject *)x, 1);
    outlet_new((t_object *)x,"signal");
    	
    return (x);
}


// 32 bit dsp routine

void OBJNAME_FIRST(_dsp)(OBJNAME_SECOND(t_) *x, t_signal **sp, short *count)
{	
	// Default to scalar routine
	
	method current_perform_routine = (method) OBJNAME_FIRST(_perform_scalar); 
	long vec_size_val = sp[0]->s_n;
	
#ifndef NO_F32_SIMD
	if ((vec_size_val >> 2) > 0 || !SSE2_check())
	{
		vec_size_val >>= 2;

		// Check memory alignment of all relevant vectors

		if ((long) sp[0]->s_vec % 16 || (long) sp[1]->s_vec % 16)
		{
			current_perform_routine = (method) OBJNAME_FIRST(_perform_misaligned);
			post ("%s: handed a misaligned signal vector - update to Max 5.1.3 or later", OBJNAME_STR);
		}
		else
			current_perform_routine = (method) OBJNAME_FIRST(_perform);
	}
#endif
	
	dsp_add(denormals_perform, 5, current_perform_routine, sp[0]->s_vec, sp[1]->s_vec, vec_size_val, x);
}


#ifndef NO_F32_SIMD

// 32 bit perform routine (SIMD)

t_int *OBJNAME_FIRST(_perform)(t_int *w)
{	
    int vec_size;
    OBJNAME_SECOND(t_) *x;
	
    vFloat *in1 = (vFloat *)(w[2]);
	vFloat *out1 = (vFloat *)(w[3]);
    vec_size = w[4];
    x = (OBJNAME_SECOND(t_) *) w[5];
	
#ifdef USE_F32_VEC_ARRAY
	F32_VEC_ARRAY((float *) out1, (float *) in1, vec_size << 2);
#else
	while (vec_size--)
		*out1++ = F32_VEC_OP(*in1++);
#endif

    return w + 6;
}


// 32 bit perform routine (misaligned SIMD - early versions of Max 5 only - slightly slower)

t_int *OBJNAME_FIRST(_perform_misaligned)(t_int *w)
{	
    int vec_size;
    OBJNAME_SECOND(t_) *x;
	
    vFloat *in1 = (vFloat *)(w[2]);
	vFloat *out1 = (vFloat *)(w[3]);
	vFloat Temp;
    vec_size = w[4];
    x = (OBJNAME_SECOND(t_) *) w[5];
	
#ifdef USE_F32_VEC_ARRAY
	F32_VEC_ARRAY((float *) out1, (float *) in1, vec_size << 2);
#else
	while (vec_size--)
	{
		Temp = F32_VEC_OP(F32_VEC_ULOAD((float *) in1++));
		F32_VEC_USTORE((float *) out1++, Temp);
	}
#endif
	
    return w + 6;
}

#endif


// 32 bit perform routine (scalar calculations for small block sizes)

t_int *OBJNAME_FIRST(_perform_scalar)(t_int *w)
{	
    int vec_size;
    OBJNAME_SECOND(t_) *x;
	float out_val;

    float *in1 = (float *)(w[2]);
	float *out1 = (float *)(w[3]);
    vec_size = w[4];
    x = (OBJNAME_SECOND(t_) *) w[5];
	
	while (vec_size--)
	{
		out_val = F32_SCALAR_OP(*in1++);
		*out1++ = AH_FIX_DENORM_FLOAT(out_val);
	}
	
    return w + 6;
}


// 64 bit dsp routine

void OBJNAME_FIRST(_dsp64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{	
	// Default to scalar routine
	
	method current_perform_routine = (method) OBJNAME_FIRST(_perform_scalar64); 
	
#if defined VECTOR_F64_128BIT && !defined NO_F64_SIMD
	// Use SIMD routine if possible
	
	if ((maxvectorsize >> 1) > 0 && SSE2_check())
		current_perform_routine = (method) OBJNAME_FIRST(_perform64);
#endif
		
	object_method(dsp64, gensym("dsp_add64"), x, current_perform_routine, 0, 0);
}


#if (defined VECTOR_F64_128BIT) && !defined NO_F64_SIMD

// 64 bit perform routine (SIMD)

void OBJNAME_FIRST(_perform64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
#ifdef USE_F64_VEC_ARRAY
	F64_VEC_ARRAY(outs[0], ins[0], vec_size);
#else
	vDouble *in1 = (vDouble *) ins[0];
	vDouble *out1 = (vDouble *) outs[0];
	
	vec_size >>= 1;
	
	while (vec_size--)
		*out1++ = F64_VEC_OP(*in1++);
#endif
}

#endif


// 64 bit perform routine (scalar calculations for small block sizes)

void OBJNAME_FIRST(_perform_scalar64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
	double *in1 = ins[0];
	double *out1 = outs[0];
	
	while (vec_size--)
		*out1++ = F64_SCALAR_OP(*in1++);
}


// Assist routine

void OBJNAME_FIRST(_assist)(OBJNAME_SECOND(t_) *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) {
		sprintf(s,"(signal) In");
	}
    else {
		sprintf(s,"(signal) Out");
	}
}
