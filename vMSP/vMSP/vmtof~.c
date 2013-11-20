
/*
 *  vmtof~
 *
 *	vmtof~ is a vectorised version of vmtof~.
 *	This object may be marginally less accurate than the standard version, although that disadvantage is porbably outweighed in most cases by the speed benifit.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <AH_VectorOpsExtended.h>
#include "Helpers.h"


// Object and function naming

#define OBJNAME_STR "vmtof~"
#define OBJNAME_FIRST(a) vmtof ## a
#define OBJNAME_SECOND(a) a ## vmtof


// Constants

double mtof_add_constant; 
double mtof_mul_constant; 

vFloat v_mtof_add_constant_32; 
vFloat v_mtof_mul_constant_32;

#ifdef VECTOR_F64_128BIT
vDouble v_mtof_add_constant_64; 
vDouble v_mtof_mul_constant_64;
#define SET_CONSTANTS												\
mtof_mul_constant = log(2.0) / 12.0;								\
mtof_add_constant = log(440.0) - (log(2.0) * 69.0 / 12.0);			\
v_mtof_add_constant_32 = float2vector((float)mtof_add_constant);	\
v_mtof_mul_constant_32 = float2vector((float)mtof_mul_constant);	\
v_mtof_add_constant_64 = double2vector((double)mtof_add_constant);	\
v_mtof_mul_constant_64 = double2vector((double)mtof_mul_constant);
#else
#define SET_CONSTANTS												\
mtof_mul_constant = log(2.0) / 12.0;								\
mtof_add_constant = log(440.0) - (log(2.0) * 69.0 / 12.0);			\
v_mtof_add_constant_32 = float2vector((float)mtof_add_constant);	\
v_mtof_mul_constant_32 = float2vector((float)mtof_mul_constant);
#endif

// Array functions

#ifdef F32_VEC_EXP_ARRAY
static __inline void mtof_array_32(float *out, float *in, long length)
{	
	mul_add_const_array_32(out, in, length, v_mtof_mul_constant_32, v_mtof_add_constant_32);
	F32_VEC_EXP_ARRAY(out, out, length);
}
#endif

#if (defined VECTOR_F64_128BIT) && (defined F64_VEC_EXP_ARRAY)
static __inline void mtof_array_64(double *out, double *in, long length)
{
	mul_add_const_array_64(out, in, length, v_mtof_mul_constant_64, v_mtof_add_constant_64);
	F64_VEC_EXP_ARRAY(out, out, length);
}
#endif

// Define the operations 

#define F32_VEC_OP(a) F32_VEC_EXP_OP(F32_VEC_ADD_OP(F32_VEC_MUL_OP(v_mtof_mul_constant_32, a), v_mtof_add_constant_32))
#define F32_VEC_ARRAY mtof_array_32
#define F32_SCALAR_OP(a) expf((float) (mtof_mul_constant * a + mtof_add_constant))

#define F64_VEC_OP(a) F64_VEC_EXP_OP(F64_VEC_ADD_OP(F64_VEC_MUL_OP(v_mtof_mul_constant_64, a), v_mtof_add_constant_64))
#define F64_VEC_ARRAY mtof_array_64
#define F64_SCALAR_OP(a) exp(mtof_mul_constant * a + mtof_add_constant)

// Check for the existence of 32 bit vector operations and set correct approach (by vector / array / scalar fallback)

#ifndef F32_VEC_EXP_OP
#ifdef F32_VEC_EXP_ARRAY
#define USE_F32_VEC_ARRAY
#else
#define NO_F32_SIMD
#endif
#endif

// Check for the existence of 64 bit vector operations and set correct approach (by vector / array / scalar fallback)

#ifndef F64_VEC_EXP_OP
#ifdef F64_VEC_EXP_ARRAY
#define USE_F64_VEC_ARRAY
#else
#define NO_F64_SIMD
#endif
#endif

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Unary.h"
