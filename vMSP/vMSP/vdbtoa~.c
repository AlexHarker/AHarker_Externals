
/*
 *  vdtbtoa~
 *
 *	vdtbtoa~ is a vectorised version of vdtbtoa~.
 *	This object may be marginally less accurate than the standard version, although that disadvantage is porbably outweighed in most cases by the speed benifit.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <AH_VectorOpsExtended.h>
#include "Helpers.h"

// Object and function naming

#define OBJNAME_STR "vdbtoa~"
#define OBJNAME_FIRST(a) vdtbtoa ## a
#define OBJNAME_SECOND(a) a ## vdtbtoa

// Constants

float dbtoa_constant_32;
double dbtoa_constant_64;
vFloat v_dbtoa_constant_32;

#ifdef VECTOR_F64_128BIT
vDouble v_dbtoa_constant_64;
#define SET_CONSTANTS									\
dbtoa_constant_64 = log(10) / 20.0;						\
v_dbtoa_constant_64 = double2vector(dbtoa_constant_64);	\
dbtoa_constant_32 = (float) (log(10) / 20.0);			\
v_dbtoa_constant_32 = float2vector(dbtoa_constant_32);	
#else
#define SET_CONSTANTS									\
dbtoa_constant_32 = (float) (log(10) / 20.0);			\
v_dbtoa_constant_32 = float2vector(dbtoa_constant_32);	
#endif

// Array functions

#ifdef F32_VEC_EXP_ARRAY
static __inline void dbtoa_array_32(float *out, float *in, long length)
{	
	mul_const_array_32(out, in, length, v_dbtoa_constant_32);
	F32_VEC_EXP_ARRAY(out, out, length);
}
#endif

#if (defined VECTOR_F64_128BIT) && (defined F64_VEC_EXP_ARRAY)
static __inline void dbtoa_array_64(double *out, double *in, long length)
{
	mul_const_array_64(out, in, length, v_dbtoa_constant_64);
	F64_VEC_EXP_ARRAY(out, out, length);
}
#endif

// Define the operations 

#define F32_VEC_OP(a) F32_VEC_EXP_OP(F32_VEC_MUL_OP(a, v_dbtoa_constant_32))
#define F32_VEC_ARRAY dbtoa_array_64
#define F32_SCALAR_OP(a) expf (a * dbtoa_constant_32); 

#define F64_VEC_OP(a) F64_VEC_EXP_OP(F32_VEC_MUL_OP(a, v_dbtoa_constant_64))
#define F64_VEC_ARRAY dbtoa_array_64
#define F64_SCALAR_OP(a) exp (a * dbtoa_constant_64); 

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


