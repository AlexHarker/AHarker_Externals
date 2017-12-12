
/*
 *  vacos~
 *
 *	vacos~ is a vectorised version of acos~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_unary.hpp>
#include <SIMDExtended.hpp>
#include <AH_Win_Math.h>
#include "Helpers.h"

struct acos_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return F32_SCALAR_NAN_FIX_OP(acosf(a.mVal)); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return F64_SCALAR_NAN_FIX_OP(acos(a.mVal)); }
    
    void operator()(double *o, double *i, long size)
    {
        f64_acos_array(o, i, size);
        //nan_fix_array_64(out, out, length);
    }
    
    void operator()(float *o, float *i, long size)
    {
        f32_acos_array(o, i, size);
        //nan_fix_array_64(out, out, length);
    }
    
    // Empty Implementations

    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<acos_functor, kVectorArray, kVectorArray> vacos;

int C74_EXPORT main()
{
    vacos::setup<vacos>("vacos~");
}

/*
// Constants

#define SET_CONSTANTS NAN_CONSTANTS

// Array functions

#ifdef F32_VEC_ACOS_ARRAY
static __inline void acos_array_32(float *out, float *in, long length)
{	
	F32_VEC_ACOS_ARRAY(out, in, length);
	nan_fix_array_32(out, out, length);
}
#endif

#if (defined VECTOR_F64_128BIT) && (defined F64_VEC_ACOS_ARRAY)
static __inline void acos_array_64(double *out, double *in, long length)
{
	F64_VEC_ACOS_ARRAY(out, in, length);
	nan_fix_array_64(out, out, length);
}
#endif

// Op functions

#ifdef F32_VEC_ACOS_OP
static __inline vFloat acos_vec_32 (vFloat a)
{
	vFloat temp = F32_VEC_ACOS_OP(a);
	return F32_VEC_NAN_FIX_OP(temp);
}
#endif

static __inline float acos_scalar_32 (float a)
{
	float temp = acosf(a);
	return F32_SCALAR_NAN_FIX_OP(temp);
}

#ifdef F64_VEC_ACOS_OP
static __inline vDouble acos_vec_64 (vDouble a)
{
	vDouble temp = F64_VEC_ACOS_OP(a);
	return F64_VEC_NAN_FIX_OP(temp);
}
#endif

static __inline double acos_scalar_64 (float a)
{
	double temp = acos(a);
	return F64_SCALAR_NAN_FIX_OP(temp);
}

// Define operations

#define F32_VEC_OP acos_vec_32
#define F32_VEC_ARRAY acos_array_32
#define F32_SCALAR_OP acos_scalar_32

#define F64_VEC_OP acos_vec_64
#define F64_VEC_ARRAY acos_array_64
#define F64_SCALAR_OP acos_scalar_64

// Check for the existence of 32 bit vector operations and set correct approach (by vector / array / scalar fallback)

#ifndef F32_VEC_ACOS_OP
#ifdef F32_VEC_ACOS_ARRAY
#define USE_F32_VEC_ARRAY
#else
#define NO_F32_SIMD
#endif
#endif

// Check for the existence of 64 bit vector operations and set correct approach (by vector / array / scalar fallback)

#ifndef F64_VEC_ACOS_OP
#ifdef F64_VEC_ACOS_ARRAY
#define USE_F64_VEC_ARRAY
#else
#define NO_F64_SIMD
#endif
#endif

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Unary.h"
*/
