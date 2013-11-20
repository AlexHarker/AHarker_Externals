
/*
 *  vacosh~
 *
 *	vacosh~ is a vectorised version of acosh~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <AH_VectorOpsExtended.h>
#include <AH_Win_Math.h>
#include "Helpers.h"


// Object and function naming

#define OBJNAME_STR "vacosh~"
#define OBJNAME_FIRST(a) vacosh ## a
#define OBJNAME_SECOND(a) a ## vacosh

// Constants

vFloat one_32 = {1.f, 1.f, 1.f, 1.f}; 

#ifdef VECTOR_F64_128BIT
vDouble one_64 = {1.0, 1.0};
#endif

// Array functions

#ifdef F32_VEC_ACOSH_ARRAY
static __inline void acosh_array_32(float *out, float *in, long length)
{	
	min_clip_array_32(out, in, length, one_32);	
	F32_VEC_ACOSH_ARRAY(out, out, length);
}
#endif

#if (defined VECTOR_F64_128BIT) && (defined F64_VEC_ACOSH_ARRAY)
static __inline void acosh_array_64(double *out, double *in, long length)
{
	min_clip_array_64(out, in, length, one_64);	
	F64_VEC_ACOSH_ARRAY(out, out, length);
}
#endif

// Op functions

__inline float acosh_scalar_32 (float a)
{
	a = a < 1.f ? 1.f : a;
	
	return acoshf(a);
}

__inline double acosh_scalar_64 (double a)
{
	a = a < 1.0 ? 1.0 : a;
	
	return acosh(a);
}

// Define operations

#define F32_VEC_OP(a) F32_VEC_ACOSH_OP(F32_VEC_MAX_OP(a, one_32))
#define F32_VEC_ARRAY acosh_array_32
#define F32_SCALAR_OP acosh_scalar_32

#define F64_VEC_OP(a) F64_VEC_ACOSH_OP(F64_VEC_MAX_OP(a, one_64))
#define F64_VEC_ARRAY acosh_array_64
#define F64_SCALAR_OP acosh_scalar_64

// Check for the existence of 32 bit vector operations and set correct approach (by vector / array / scalar fallback)

#ifndef F32_VEC_ACOSH_OP
#ifdef F32_VEC_ACOSH_ARRAY
#define USE_F32_VEC_ARRAY
#else
#define NO_F32_SIMD
#endif
#endif

// Check for the existence of 64 bit vector operations and set correct approach (by vector / array / scalar fallback)

#ifndef F64_VEC_ACOSH_OP
#ifdef F64_VEC_ACOSH_ARRAY
#define USE_F64_VEC_ARRAY
#else
#define NO_F64_SIMD
#endif
#endif

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Unary.h"







