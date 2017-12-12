
/*
 *  vatodb~
 *
 *	vatodb~ is a vectorised version of vatodb~.
 *	This object may be marginally less accurate than the standard version, although that disadvantage is porbably outweighed in most cases by the speed benefit.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_unary.hpp>
#include <conversions.hpp>
#include <SIMDExtended.hpp>
#include <AH_Win_Math.h>

struct atodb_functor
{
    const static float atodb_constant_32;
    const static double atodb_constant_64;
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return 20.f * log10f(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return 20.0 * log10(a.mVal); }
    
    void operator()(float *o, float *i, long size)
    {
        f32_log_array(o, i, size);
        f32_mul_const_array(o, size, atodb_constant_32);
    }
    
    void operator()(double *o, double *i, long size)
    {
        f64_log_array(o, i, size);
        f64_mul_const_array(o, size, atodb_constant_64);
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

// Initialise constants

const float atodb_functor::atodb_constant_32 = static_cast<float>(20.0 / log(10.0));
const double atodb_functor::atodb_constant_64 = 20.0 / log(10.0);

typedef v_unary<atodb_functor, kVectorArray, kVectorArray> vatodb;

int C74_EXPORT main()
{
    vatodb::setup<vatodb>("vatodb~");
}

/*
#include <AH_VectorOpsExtended.h>
#include "Helpers.h"


// Object and function naming

#define OBJNAME_STR "vatodb~"
#define OBJNAME_FIRST(a) vatodb ## a
#define OBJNAME_SECOND(a) a ## vatodb

// Constants

vFloat v_atodb_constant_32;

#ifdef VECTOR_F64_128BIT
vDouble v_atodb_constant_64;
#define SET_CONSTANTS											\
v_atodb_constant_64 = double2vector((20.0 / log(10)));			\
v_atodb_constant_32 = float2vector((float) (20.0 / log(10)));
#else
#define SET_CONSTANTS											\
v_atodb_constant_32 = float2vector((float) (20.0 / log(10)));
#endif

// Array functions

#ifdef F32_VEC_LOG_ARRAY
static __inline void atodb_array_32(float *out, float *in, long length)
{	
	F32_VEC_LOG_ARRAY(out, in, length);
	mul_const_array_32(out, out, length, v_atodb_constant_32);
}
#endif

#if (defined VECTOR_F64_128BIT) && (defined F64_VEC_LOG_ARRAY)
static __inline void atodb_array_64(double *out, double *in, long length)
{
	F64_VEC_LOG_ARRAY(out, in, length);
	mul_const_array_64(out, out, length, v_atodb_constant_64);
}
#endif

// Define the operations 

#define F32_VEC_OP(a) F32_VEC_MUL_OP(v_atodb_constant_32, F32_VEC_LOG_OP(a))
#define F32_VEC_ARRAY atodb_array_32
#define F32_SCALAR_OP(a) (20.f * log10f(a));

#define F64_VEC_OP(a) F64_VEC_MUL_OP(v_atodb_constant_64, F64_VEC_LOG_OP(a))
#define F64_VEC_ARRAY atodb_array_64
#define F64_SCALAR_OP(a) (20.0 * log10(a));

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
*/

