
/*
 *  vtanx~
 *
 *	vtanx~ is a vectorised version of tanx~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <AH_VectorOpsExtended.h>
#include <AH_Win_Math.h>
#include "Helpers.h"

// Object and function naming

#define OBJNAME_STR "vtanx~"
#define OBJNAME_FIRST(a) vtanx ## a
#define OBJNAME_SECOND(a) a ## vtanx

// Constants

#define SET_CONSTANTS NAN_CONSTANTS

// Array functions

#ifdef F32_VEC_TAN_ARRAY
static __inline void tan_array_32(float *out, float *in, long length)
{	
	F32_VEC_TAN_ARRAY(out, in, length);
	nan_fix_array_32(out, out, length);
}
#endif

#if (defined VECTOR_F64_128BIT) && (defined F64_VEC_TAN_ARRAY)
static __inline void tan_array_64(double *out, double *in, long length)
{
	F64_VEC_TAN_ARRAY(out, in, length);
	nan_fix_array_64(out, out, length);
}
#endif

// Op functions

#ifdef F32_VEC_TAN_OP
static __inline vFloat tan_vec_32 (vFloat a)
{
	vFloat temp = F32_VEC_TAN_OP(a);
	return F32_VEC_NAN_FIX_OP(temp);
}
#endif

static __inline float tan_scalar_32 (float a)
{
	float temp = tanf(a);
	return F32_SCALAR_NAN_FIX_OP(temp);
}

#ifdef F64_VEC_TAN_OP
static __inline vDouble tan_vec_64 (vDouble a)
{
	vDouble temp = F64_VEC_TAN_OP(a);
	return F64_VEC_NAN_FIX_OP(temp);
}
#endif

static __inline double tan_scalar_64 (float a)
{
	double temp = tan(a);
	return F64_SCALAR_NAN_FIX_OP(temp);
}

// Define operations

#define F32_VEC_OP tan_vec_32
#define F32_VEC_ARRAY tan_array_32
#define F32_SCALAR_OP tan_scalar_32

#define F64_VEC_OP tan_vec_64
#define F64_VEC_ARRAY tan_array_64
#define F64_SCALAR_OP tan_scalar_64

// Check for the existence of 32 bit vector operations and set correct approach (by vector / array / scalar fallback)

#ifndef F32_VEC_TAN_OP
#ifdef F32_VEC_TAN_ARRAY
#define USE_F32_VEC_ARRAY
#else
#define NO_F32_SIMD
#endif
#endif

// Check for the existence of 64 bit vector operations and set correct approach (by vector / array / scalar fallback)

#ifndef F64_VEC_TAN_OP
#ifdef F64_VEC_TAN_ARRAY
#define USE_F64_VEC_ARRAY
#else
#define NO_F64_SIMD
#endif
#endif

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Unary.h"

