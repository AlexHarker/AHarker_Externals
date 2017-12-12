
/*
 *  vftom~
 *
 *	vftom~ is a vectorised version of v_ftom~.
 *	This object may be marginally less accurate than the standard version, although that disadvantage is porbably outweighed in most cases by the speed benefit.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <AH_VectorOpsExtended.h>
#include "Helpers.h"


// Object and function naming

#define OBJNAME_STR "vftom~"
#define OBJNAME_FIRST(a) vftom ## a
#define OBJNAME_SECOND(a) a ## vftom

// Constants

double ftom_add_constant; 
double ftom_mul_constant; 

vFloat v_ftom_add_constant_32; 
vFloat v_ftom_mul_constant_32;

#ifdef VECTOR_F64_128BIT
vDouble v_ftom_add_constant_64; 
vDouble v_ftom_mul_constant_64;
#define SET_CONSTANTS												\
ftom_mul_constant = 12.0 / log(2.0);								\
ftom_add_constant = 69.0 - (log(440.0) * ftom_mul_constant);		\
v_ftom_add_constant_32 = float2vector((float)ftom_add_constant);		\
v_ftom_mul_constant_32 = float2vector((float)ftom_mul_constant);		\
v_ftom_add_constant_64 = double2vector((double)ftom_add_constant);	\
v_ftom_mul_constant_64 = double2vector((double)ftom_mul_constant);
#else
#define SET_CONSTANTS												\
ftom_mul_constant = 12.0 / log(2.0);								\
ftom_add_constant = 69.0 - (log(440.0) * ftom_mul_constant);		\
v_ftom_add_constant_32 = float2vector((float)ftom_add_constant);		\
v_ftom_mul_constant_32 = float2vector((float)ftom_mul_constant);
#endif

// Array functions

#ifdef F32_VEC_LOG_ARRAY
static __inline void ftom_array_32(float *out, float *in, long length)
{	
	F32_VEC_LOG_ARRAY(out, in, length);
	mul_add_const_array_32(out, out, length, v_ftom_mul_constant_32, v_ftom_add_constant_32);
}
#endif

#if (defined VECTOR_F64_128BIT) && (defined F64_VEC_LOG_ARRAY)
static __inline void ftom_array_64(double *out, double *in, long length)
{
	F64_VEC_LOG_ARRAY(out, in, length);
	mul_add_const_array_64(out, out, length, v_ftom_mul_constant_64, v_ftom_add_constant_64);
}
#endif

// Define the operations 

#define F32_VEC_OP(a) F32_VEC_ADD_OP(F32_VEC_MUL_OP(v_ftom_mul_constant_32, F32_VEC_LOG_OP(a)), v_ftom_add_constant_32)
#define F32_VEC_ARRAY ftom_array_32
#define F32_SCALAR_OP(a) (float) ((ftom_mul_constant * log(a)) + ftom_add_constant)

#define F64_VEC_OP(a) F64_VEC_ADD_OP(F64_VEC_MUL_OP(v_ftom_mul_constant_64, F64_VEC_LOG_OP(a)), v_ftom_add_constant_64)
#define F64_VEC_ARRAY ftom_array_64
#define F64_SCALAR_OP(a) (ftom_mul_constant * log(a)) + ftom_add_constant

// Check for the existence of 32 bit vector operations and set correct approach (by vector / array / scalar fallback)

#ifndef F32_VEC_LOG_OP
#ifdef F32_VEC_LOG_ARRAY
#define USE_F32_VEC_ARRAY
#else
#define NO_F32_SIMD
#endif
#endif

// Check for the existence of 64 bit vector operations and set correct approach (by vector / array / scalar fallback)

#ifndef F64_VEC_LOG_OP
#ifdef F64_VEC_LOG_ARRAY
#define USE_F64_VEC_ARRAY
#else
#define NO_F64_SIMD
#endif
#endif

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Unary.h"



