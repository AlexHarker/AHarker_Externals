
/*
 *  vasin~
 *
 *	vasin~ is a vectorised version of asin~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_unary.hpp>
#include <SIMDExtended.hpp>
#include <AH_Win_Math.h>
#include "Helpers.h"

struct asin_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return F32_SCALAR_NAN_FIX_OP(asinf(a.mVal)); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return F64_SCALAR_NAN_FIX_OP(asin(a.mVal)); }
    
    void operator()(float *o, float *i, long size)
    {
        f32_asin_array(o, i, size);
        //nan_fix_array_64(out, out, length);
    }
    
    void operator()(double *o, double *i, long size)
    {
        f64_asin_array(o, i, size);
        //nan_fix_array_64(out, out, length);
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<asin_functor, kVectorArray, kVectorArray> vasin;

int C74_EXPORT main()
{
    vasin::setup<vasin>("vasin~");
}

/*

#include <AH_VectorOpsExtended.h>
#include <AH_Win_Math.h>
#include "Helpers.h"


// Object and function naming

#define OBJNAME_STR "vasin~"
#define OBJNAME_FIRST(a) vasin ## a
#define OBJNAME_SECOND(a) a ## vasin

// Constants

#define SET_CONSTANTS NAN_CONSTANTS

// Array functions

#ifdef F32_VEC_ASIN_ARRAY
static __inline void asin_array_32(float *out, float *in, long length)
{	
	F32_VEC_ASIN_ARRAY(out, in, length);
	nan_fix_array_32(out, out, length);
}
#endif

#if (defined VECTOR_F64_128BIT) && (defined F64_VEC_ASIN_ARRAY)
static __inline void asin_array_64(double *out, double *in, long length)
{
	F64_VEC_ASIN_ARRAY(out, in, length);
	nan_fix_array_64(out, out, length);
}
#endif

// Op functions

#ifdef F32_VEC_ASIN_OP
static __inline vFloat asin_vec_32 (vFloat a)
{
	vFloat temp = F32_VEC_ASIN_OP(a);
	return F32_VEC_NAN_FIX_OP(temp);
}
#endif

static __inline float asin_scalar_32 (float a)
{
	float temp = asinf(a);
	return F32_SCALAR_NAN_FIX_OP(temp);
}

#ifdef F64_VEC_ASIN_OP
static __inline vDouble asin_vec_64 (vDouble a)
{
	vDouble temp = F64_VEC_ASIN_OP(a);
	return F64_VEC_NAN_FIX_OP(temp);
}
#endif

static __inline double asin_scalar_64 (float a)
{
	double temp = asin(a);
	return F64_SCALAR_NAN_FIX_OP(temp);
}

// Define operations

#define F32_VEC_OP asin_vec_32
#define F32_VEC_ARRAY asin_array_32
#define F32_SCALAR_OP asin_scalar_32

#define F64_VEC_OP asin_vec_64
#define F64_VEC_ARRAY asin_array_64
#define F64_SCALAR_OP asin_scalar_64

// Check for the existence of 32 bit vector operations and set correct approach (by vector / array / scalar fallback)

#ifndef F32_VEC_ASIN_OP
#ifdef F32_VEC_ASIN_ARRAY
#define USE_F32_VEC_ARRAY
#else
#define NO_F32_SIMD
#endif
#endif

// Check for the existence of 64 bit vector operations and set correct approach (by vector / array / scalar fallback)

#ifndef F64_VEC_ASIN_OP
#ifdef F64_VEC_ASIN_ARRAY
#define USE_F64_VEC_ARRAY
#else
#define NO_F64_SIMD
#endif
#endif

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Unary.h"
*/
