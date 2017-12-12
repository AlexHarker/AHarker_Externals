
/*
 *  Template_Binary
 *
 *	This is a template file for a SIMD binary (either commutative or non-commutative) operator MSP object. 
 *	It should only be necessary to provide a few macro definitions before this code to comiple a fully functional object.
 *	See other source files for examples (e.g. vdiv~.c).
 *
 *	The object will support all block sizes, using SIMD where possible.
 *	The object will also accept float / int arguments, and inputs for one of the two values (according to the connections to the object)
 *
 *	The following *MUST* be defined:
 *
 *	OBJNAME_STR			- The object name as a string (i.e. - with quotes)
 *	OBJNAME_FIRST(a)	- A macro to combine the object name followed by a given string
 *
 *		Should be defined like:
 *		#define OBJNAME_FIRST(a) objname ## a
 *	
 *	OBJNAME_SECOND(a)	- A macro to combine the object name followed by a given string
 *
 *		Should be defined like:
 *		#define OBJNAME_SECOND(a) a ## objname
 * *
 *	F32_VEC_OP				- A binary 32 bit floating point SIMD operator (vFloat, vFloat)
 *	F32_VEC_ARRAY			- A binary 32 bit floating point SIMD array function (float *out, float *in1, float *in2, long length)
 *	F32_SCALAR_OP			- A binary 32 bit floating point scalar operator (float, float)
 *
 *	F64_VEC_OP				- A binary 64 bit floating point SIMD operator (vDouble, vDouble)
 *	F64_VEC_ARRAY			- A binary 64 bit floating point SIMD array function (double *out, double *in1, double *in2, long length)
 *	F64_SCALAR_OP			- A binary 64 bit floating point scalar operator (double, double)
 *
 *
 *  To use the array version over the op define USE_F32_VEC_ARRAY or USE_F64_VEC_ARRAY as appropriate
 *	To avoid using vector processing at all define NO_F32_SIMD or NO_F64_SIMD (according to which are available)
 *	It is best to define these based on the available functions (preferring the operation, then the array version, then no SIMD)
 *
 *	Optionally for objects requiring constants you can define:
 *
 *	SET_CONSTANTS		- Set constants in main routine (if necessary)
 * *  Copyright 2010 Alex Harker. All rights reserved.
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
	
	double double_val;
	
	void *temp_memory;
	
} OBJNAME_SECOND(t_);


void *OBJNAME_FIRST(_new)(double float_val);
void OBJNAME_FIRST(_free)(OBJNAME_SECOND(t_) *x);

void OBJNAME_FIRST(_float) (OBJNAME_SECOND(t_) *x, double value);
void OBJNAME_FIRST(_int) (OBJNAME_SECOND(t_) *x, t_atom_long value);

void OBJNAME_FIRST(_dsp)(OBJNAME_SECOND(t_) *x, t_signal **sp, short *count);

t_int *OBJNAME_FIRST(_perform_single1)(t_int *w);
t_int *OBJNAME_FIRST(_perform_single2)(t_int *w);
t_int *OBJNAME_FIRST(_perform)(t_int *w);
t_int *OBJNAME_FIRST(_perform_single1_misaligned)(t_int *w);
t_int *OBJNAME_FIRST(_perform_single2_misaligned)(t_int *w);
t_int *OBJNAME_FIRST(_perform_misaligned)(t_int *w);
t_int *OBJNAME_FIRST(_perform_single1_scalar)(t_int *w);
t_int *OBJNAME_FIRST(_perform_single2_scalar)(t_int *w);
t_int *OBJNAME_FIRST(_perform_scalar)(t_int *w);

void OBJNAME_FIRST(_dsp64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

#if (defined VECTOR_F64_128BIT) && !defined NO_F64_SIMD 
void OBJNAME_FIRST(_perform_single1_64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void OBJNAME_FIRST(_perform_single2_64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void OBJNAME_FIRST(_perform_64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
#endif
void OBJNAME_FIRST(_perform_single1_scalar_64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void OBJNAME_FIRST(_perform_single2_scalar_64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void OBJNAME_FIRST(_perform_scalar_64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);


void OBJNAME_FIRST(_assist)(OBJNAME_SECOND(t_) *x, void *b, long m, long a, char *s);


// Main routine

int C74_EXPORT main(void)
{
	this_class = class_new (OBJNAME_STR, (method) OBJNAME_FIRST(_new), (method)OBJNAME_FIRST(_free), sizeof(OBJNAME_SECOND(t_)), NULL, A_DEFFLOAT, 0);
	
	class_addmethod(this_class, (method)OBJNAME_FIRST(_float), "float", A_FLOAT, 0);
	class_addmethod(this_class, (method)OBJNAME_FIRST(_int), "int", A_LONG, 0);
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
	
	if (x->temp_memory != NULL)
		ALIGNED_FREE(x->temp_memory);
}


// New routine

void *OBJNAME_FIRST(_new)(double double_val)
{
    OBJNAME_SECOND(t_) *x = (OBJNAME_SECOND(t_)*) object_alloc (this_class);
		
    dsp_setup((t_pxobject *)x, 2);
    outlet_new((t_object *)x,"signal");
    
	x->double_val = double_val;
	x->temp_memory = NULL;
	
    return (x);
}


// Float input routine

void OBJNAME_FIRST(_float) (OBJNAME_SECOND(t_) *x, double value)
{
	x->double_val = value;
}


// Int input routine

void OBJNAME_FIRST(_int) (OBJNAME_SECOND(t_) *x, t_atom_long value)
{
	x->double_val = (double) value;
}


// 32 bit dsp routine

void OBJNAME_FIRST(_dsp)(OBJNAME_SECOND(t_) *x, t_signal **sp, short *count)
{	
	long routine = 0;
	long vec_size_val = sp[0]->s_n;
	#ifdef REVERSE
	float *used_input1 = (float *) sp[1]->s_vec;
	float *used_input2 = (float *) sp[0]->s_vec;
	#else
	float *used_input1 = (float *) sp[0]->s_vec;
	float *used_input2 = (float *) sp[1]->s_vec;
	#endif
	
	// Default to scalar routines
	
	method current_perform_routine = (method) OBJNAME_FIRST(_perform_single1_scalar); 
	
	if (x->temp_memory != NULL)
		ALIGNED_FREE(x->temp_memory);		

	// If nothing is connected then don't do anything here....
	
	if (!count[1] && !count[0])			
		return;  
		
	// Only pass the second input (twice)
	
	if (!count[0])						
	{
		#ifdef REVERSE
		used_input2 = used_input1;
		routine = 1;
		#else
		used_input1 = used_input2;
		routine = 2;
		#endif
	}
		
	// Only pass the first input (twice)
	
	if (!count[1])					
	{
		#ifdef REVERSE
		used_input1 = used_input2;
		routine = 2;
		#else
		used_input2 = used_input1;
		routine = 1;
		#endif
	}
	
#ifndef NO_F32_SIMD
	if (vec_size_val >= 4 && SSE2_check())
	{
		// Use vector routines
		
		routine += 3;
		
#ifdef USE_F32_VEC_ARRAY
		// Make temporary array for array-based vector routines
		
		if (routine != 3)
			x->temp_memory = ALIGNED_MALLOC(vec_size_val * sizeof(float));
#else
		vec_size_val = sp[0]->s_n >> 2;
#endif
			
		// Check memory alignment of all relevant vectors
		
		if ((t_ptr_uint) used_input1 % 16 || (t_ptr_uint) used_input2 % 16 || (t_ptr_uint) sp[2]->s_vec % 16)	
		{
			post ("%s: handed a misaligned signal vector - update to Max 5.1.3 or later", OBJNAME_STR);
			routine += 3;
		}		
	}
#endif
	
	switch (routine)
	{
		case 0:	// Scalar 
			current_perform_routine = (method) OBJNAME_FIRST(_perform_scalar);
			break;
		case 1:	// Scalar single 1 (first operand is at signal rate)
			current_perform_routine = (method) OBJNAME_FIRST(_perform_single1_scalar);
			break;
		case 2:	// Scalar single 2 (second operand is at signal rate)
			current_perform_routine = (method) OBJNAME_FIRST(_perform_single2_scalar);
			break;
#ifndef NO_F32_SIMD
		case 3: // Vector aligned
			current_perform_routine = (method) OBJNAME_FIRST(_perform);
			break;
		case 4: // Vector aligned single 1 (first operand is at signal rate)
			current_perform_routine = (method) OBJNAME_FIRST(_perform_single1);
			break;
		case 5: // Vector aligned single 2 (second operand is at signal rate)
			current_perform_routine = (method) OBJNAME_FIRST(_perform_single2);
			break;
		case 6:	// Vector mis-aligned
			current_perform_routine = (method) OBJNAME_FIRST(_perform_misaligned);
			break;
		case 7:	// Vector mis-aligned single 1 (first operand is at signal rate)
			current_perform_routine = (method) OBJNAME_FIRST(_perform_single1_misaligned);
			break;
		case 8: // Vector mis-aligned single 2 (second operand is at signal rate)
			current_perform_routine = (method) OBJNAME_FIRST(_perform_single2_misaligned);
			break;
#endif
	}
	
	dsp_add(denormals_perform, 6, current_perform_routine, used_input1, used_input2, sp[2]->s_vec, vec_size_val, x);
}


#ifndef NO_F32_SIMD

// 32 bit perform routine with first operand only at signal-rate (SIMD)

t_int *OBJNAME_FIRST(_perform_single1)(t_int *w)
{	
#ifdef USE_F32_VEC_ARRAY
	float *in1 = (float *)(w[2]);
	float *out1 = (float *)(w[4]);
    long vec_size = w[5];
    OBJNAME_SECOND(t_) *x = (OBJNAME_SECOND(t_) *) w[6];
	
	float val = (float) x->double_val;
	float *temp_memory = (float *)x->temp_memory;
	long i;
	
	for (i = 0; i < vec_size; i++)
		temp_memory[i] = val;
		
	F32_VEC_ARRAY(out1, in1, temp_memory, vec_size);
#else
	vFloat *in1 = (vFloat *)(w[2]);
	vFloat *out1 = (vFloat *)(w[4]);
    long vec_size = w[5];
    OBJNAME_SECOND(t_) *x = (OBJNAME_SECOND(t_) *) w[6];
    
	float val = (float) x->double_val;
	vFloat float_val = float2vector(val);
	
	while (vec_size--)
		*out1++ = F32_VEC_OP(*in1++, float_val);
#endif
		
    return w + 7;
}


// 32 bit perform routine with second operand only at signal-rate (SIMD)

t_int *OBJNAME_FIRST(_perform_single2)(t_int *w)
{	
#ifdef USE_F32_VEC_ARRAY
	float *in1 = (float *)(w[2]);
	float *out1 = (float *)(w[4]);
    long vec_size = w[5];
    OBJNAME_SECOND(t_) *x = (OBJNAME_SECOND(t_) *) w[6];
	
	float val = (float) x->double_val;
	float *temp_memory = (float *)x->temp_memory;
	long i;
	
	for (i = 0; i < vec_size; i++)
		temp_memory[i] = val;
		
	F32_VEC_ARRAY(out1, temp_memory, in1, vec_size);
#else
	vFloat *in1 = (vFloat *)(w[2]);
	vFloat *out1 = (vFloat *)(w[4]);
    long vec_size = w[5];
    OBJNAME_SECOND(t_) *x = (OBJNAME_SECOND(t_) *) w[6];
    
	float val = (float) x->double_val;
	vFloat float_val = float2vector(val);

	while (vec_size--)
		*out1++ = F32_VEC_OP(float_val, *in1++);
#endif
		
    return w + 7;
}


// 32 bit perform routine with two signal-rate input (SIMD)

t_int *OBJNAME_FIRST(_perform)(t_int *w)
{	
#ifdef USE_F32_VEC_ARRAY
	float *in1 = (float *)(w[2]);
	float *in2 = (float *)(w[3]);
	float *out1 = (float *)(w[4]);
    long vec_size = w[5];

	F32_VEC_ARRAY(out1, in1, in2, vec_size);
#else
    vFloat *in1 = (vFloat *)(w[2]);
    vFloat *in2 = (vFloat *)(w[3]);
	vFloat *out1 = (vFloat *)(w[4]);
    long vec_size = w[5];
    
	while (vec_size--)
		*out1++ = F32_VEC_OP(*in1++, *in2++);
#endif
		
    return w + 7;
}


// 32 bit perform routine with first operand only at signal-rate (misaligned SIMD - early versions of Max 5 only - slightly slower)

t_int *OBJNAME_FIRST(_perform_single1_misaligned)(t_int *w)
{	    
#ifdef USE_F32_VEC_ARRAY
	return OBJNAME_FIRST(_perform_single1)(w);
#else
	vFloat *in1 = (vFloat *)(w[2]);
	vFloat *out1 = (vFloat *)(w[4]);
    long vec_size = w[5];
    OBJNAME_SECOND(t_) *x = (OBJNAME_SECOND(t_) *) w[6];
    
	vFloat float_val = float2vector((float) x->double_val);
	vFloat Temp;

	while (vec_size--)
	{
		Temp = F32_VEC_OP(F32_VEC_ULOAD((float *)in1++), float_val);
		F32_VEC_USTORE((float *)out1++, Temp);
	}
			
    return w + 7;
#endif
}


// 32 bit perform routine with second operand only at signal-rate (misaligned SIMD - early versions of Max 5 only - slightly slower)

t_int *OBJNAME_FIRST(_perform_single2_misaligned)(t_int *w)
{	
#ifdef USE_F32_VEC_ARRAY
	return OBJNAME_FIRST(_perform_single2)(w);
#else
	vFloat *in1 = (vFloat *)(w[2]);
	vFloat *out1 = (vFloat *)(w[4]);
    long vec_size = w[5];
    OBJNAME_SECOND(t_) *x = (OBJNAME_SECOND(t_) *) w[6];
    
	vFloat float_val = float2vector((float) x->double_val);
	vFloat Temp;
	
	while (vec_size--)
	{
		Temp = F32_VEC_OP(float_val, F32_VEC_ULOAD((float *)in1++));
		F32_VEC_USTORE((float *)out1++, Temp);
	}
			
    return w + 7;
#endif
}


// 32 bit perform routine with two signal-rate inputs (misaligned SIMD - early versions of Max 5 only - slightly slower)

t_int *OBJNAME_FIRST(_perform_misaligned)(t_int *w)
{		
#ifdef USE_F32_VEC_ARRAY
	return OBJNAME_FIRST(_perform)(w);
#else
    vFloat *in1 = (vFloat *)(w[2]);
    vFloat *in2 = (vFloat *)(w[3]);
	vFloat *out1 = (vFloat *)(w[4]);
	vFloat Temp;
    long vec_size = w[5];
	
	while (vec_size--)
	{
		Temp = F32_VEC_OP(F32_VEC_ULOAD((float *)in1++), F32_VEC_ULOAD((float *)in2++));
		F32_VEC_USTORE((float *)out1++, Temp);
	}
	
    return w + 7;
#endif
}

#endif

// 32 bit perform routine with first operand only at signal-rate (scalar calculations for small block sizes)

t_int *OBJNAME_FIRST(_perform_single1_scalar)(t_int *w)
{		
	float *in1 = (float *)(w[2]);
	float *out1 = (float *)(w[4]);
    long vec_size = w[5];
    OBJNAME_SECOND(t_) *x = (OBJNAME_SECOND(t_) *) w[6];
    
	float float_val = (float) x->double_val;
	
	while (vec_size--)
	{
		float out_val = F32_SCALAR_OP(*in1++, float_val);
		*out1++ = AH_FIX_DENORM_FLOAT(out_val);
	}
			
    return w + 7;
}


// 32 bit perform routine with second operand only at signal-rate (scalar calculations for small block sizes)

t_int *OBJNAME_FIRST(_perform_single2_scalar)(t_int *w)
{		
	float *in1 = (float *)(w[2]);
	float *out1 = (float *)(w[4]);
	long vec_size = w[5];
    OBJNAME_SECOND(t_) *x = (OBJNAME_SECOND(t_) *) w[6];
    
	float float_val = (float) x->double_val;
	
	while (vec_size--)
	{
		float out_val = F32_SCALAR_OP(float_val, *in1++);
		*out1++ = AH_FIX_DENORM_FLOAT(out_val);
	}
			
    return w + 7;
}


// 32 bit perform routine with two signal-rate inputs (scalar calculations for small block sizes)

t_int *OBJNAME_FIRST(_perform_scalar)(t_int *w)
{	
    float *in1 = (float *)(w[2]);
    float *in2 = (float *)(w[3]);
	float *out1 = (float *)(w[4]);
    long vec_size = w[5];
    
	while (vec_size--)
	{
		float out_val = F32_SCALAR_OP(*in1++, *in2++);
		*out1++ = AH_FIX_DENORM_FLOAT(out_val);
	}
	
    return w + 7;
}


// 64 bit dsp routine

void OBJNAME_FIRST(_dsp64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	long routine = 0;
	long userparam = 0;
	
	method current_perform_routine = (method) OBJNAME_FIRST(_perform_single1_scalar_64); 
	
	if (x->temp_memory != NULL)
		ALIGNED_FREE(x->temp_memory);
	
	// If nothing is connected then don't do anything here....
	
	if (!count[1] && !count[0])			
		return;  
	
	// Use single routine
	
#ifdef REVERSE
	userparam = 1;
	
	if (!count[0])
	{
		routine = 1;	
	}
	if (!count[1])
	{
		userparam = 0;
		routine = 2;
	}
#else
	if (!count[0])
	{
		userparam = 1;
		routine = 2;
	}
	if (!count[1])
		routine = 1;
#endif
		
#if (defined VECTOR_F64_128BIT) && !defined NO_F64_SIMD 
		// Use SIMD code where possible
		
		if (maxvectorsize > 4)
		{	
			routine += 3;
			
#ifdef USE_F64_VEC_ARRAY
			// Make temporary array for array-based vector routines
			
			if (routine != 3)
				x->temp_memory = ALIGNED_MALLOC(maxvectorsize * sizeof(double));
#endif
		}
#endif
			
		switch (routine)
		{
			case 0:	// Scalar 
				current_perform_routine = (method) OBJNAME_FIRST(_perform_scalar_64);
				break;
			case 1: // Scalar single 1
				current_perform_routine = (method) OBJNAME_FIRST(_perform_single1_scalar_64);
				break;
			case 2: // Scalar single 2
				current_perform_routine = (method) OBJNAME_FIRST(_perform_single2_scalar_64);
				break;
#if (defined VECTOR_F64_128BIT) && !defined NO_F64_SIMD 
			case 3: // Vector aligned
				current_perform_routine = (method) OBJNAME_FIRST(_perform_64);
				break;			
			case 4: // Vector aligned single 1
				current_perform_routine = (method) OBJNAME_FIRST(_perform_single1_64);
				break;
			case 5: // Vector aligned single 1
				current_perform_routine = (method) OBJNAME_FIRST(_perform_single2_64);
				break;
#endif
		}
	
	object_method(dsp64, gensym("dsp_add64"), x, current_perform_routine, 0, userparam);
}


#if (defined VECTOR_F64_128BIT) && !defined NO_F64_SIMD 

// 64 bit perform routine with one LHS signal-rate input (SIMD)

void OBJNAME_FIRST(_perform_single1_64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
#ifdef USE_F64_VEC_ARRAY
	double val = x->double_val;
	double *temp_memory = (double *)x->temp_memory;
	long i;
	
	for (i = 0; i < vec_size; i++)
		temp_memory[i] = val;
		
		F64_VEC_ARRAY(outs[0], ins[userparam ? 1 : 0], temp_memory, vec_size);
#else
	vDouble *in1 = (vDouble *)ins[userparam ? 1 : 0];
	vDouble *out1 = (vDouble *) outs[0];
	
	vDouble double_val = double2vector(x->double_val);
	
	vec_size >>= 1;

	while (vec_size--)
		*out1++ = F64_VEC_OP(*in1++, double_val);
#endif
}


// 64 bit perform routine with one RHS signal-rate input (SIMD)

void OBJNAME_FIRST(_perform_single2_64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
#ifdef USE_F64_VEC_ARRAY
	double val = x->double_val;
	double *temp_memory = (double *)x->temp_memory;
	long i;
	
	for (i = 0; i < vec_size; i++)
		temp_memory[i] = val;
		
	F64_VEC_ARRAY(outs[0], temp_memory, ins[userparam ? 1 : 0], vec_size);
#else
	vDouble *in1 = (vDouble *)ins[userparam ? 1 : 0];
	vDouble *out1 = (vDouble *) outs[0];
	
	vDouble double_val = double2vector(x->double_val);
	
	vec_size >>= 1;

	while (vec_size--)
		*out1++ = F64_VEC_OP(double_val, *in1++);
#endif
}


// 64 bit perform routine with two signal-rate inputs (SIMD)

void OBJNAME_FIRST(_perform_64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{		
#ifdef USE_F64_VEC_ARRAY
	F64_VEC_ARRAY(outs[0], ins[0], ins[1], vec_size);
#else
    vDouble *in1 = (vDouble *)ins[userparam ? 1 : 0];
    vDouble *in2 = (vDouble *)ins[userparam ? 0 : 1];
	vDouble *out1 = (vDouble *)outs[0];
   
	vec_size >>= 1;
	
	while (vec_size--)
		*out1++ = F64_VEC_OP(*in1++, *in2++);
#endif
}

#endif

// 64 bit perform routine with one LHS signal-rate input (scalar calculations for small block sizes)

void OBJNAME_FIRST(_perform_single1_scalar_64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{		
	double *in1 = ins[userparam ? 1 : 0];
	double *out1 = outs[0];
    
	double double_val = x->double_val;
	
	while (vec_size--)
	{
		double out_val = F64_SCALAR_OP(*in1++, double_val);
		*out1++ = AH_FIX_DENORM_DOUBLE(out_val);
	}
}


// 64 bit perform routine with one RHS signal-rate input (scalar calculations for small block sizes)

void OBJNAME_FIRST(_perform_single2_scalar_64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{		
	double *in1 = ins[userparam ? 1 : 0];
	double *out1 = outs[0];
    
	double double_val = x->double_val;
	
	while (vec_size--)
	{
		double out_val = F64_SCALAR_OP(double_val, *in1++);
		*out1++ = AH_FIX_DENORM_DOUBLE(out_val);
	}
}


// 64 bit perform routine with two signal-rate inputs (scalar calculations for small block sizes)

void OBJNAME_FIRST(_perform_scalar_64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
    double *in1 = ins[userparam ? 1 : 0];
    double *in2 = ins[userparam ? 0 : 1];
	double *out1  = outs[0];
    
	while (vec_size--)
	{
		double out_val =  F64_SCALAR_OP(*in1++, *in2++);
		*out1++ = AH_FIX_DENORM_DOUBLE(out_val);
	}
}


// Assist routine

void OBJNAME_FIRST(_assist)(OBJNAME_SECOND(t_) *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) {
        switch (a) {	
            case 0:
                sprintf(s,"(signal, float) In 1");
                break;
            case 1:
                sprintf(s,"(signal, float) In 2");
                break;
        }
    }
    else {
		sprintf(s,"(signal) Out");
	}
}
