
/*
 *  vdtbtoa~
 *
 *	vdtbtoa~ is a vectorised version of vdtbtoa~.
 *	This object may be marginally less accurate than the standard version, although that disadvantage is porbably outweighed in most cases by the speed benefit.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_unary.hpp>
#include <conversions.hpp>
#include <SIMDExtended.hpp>
#include <AH_Win_Math.h>

struct dbtoa_functor
{
    const static float dbtoa_constant_32;
    const static double dbtoa_constant_64;
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return expf(a.mVal * dbtoa_constant_32); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return exp(a.mVal * dbtoa_constant_64); }
    
    void operator()(float *o, float *i, long size)
    {
        f32_mul_const_array(o, i, size, dbtoa_constant_32);
        f32_exp_array(o, o, size);
    }
    
    void operator()(double *o, double *i, long size)
    {
        f64_mul_const_array(o, i, size, dbtoa_constant_64);
        f64_exp_array(o, o, size);
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

// Initialise constants

const float dbtoa_functor::dbtoa_constant_32 = static_cast<float>(20.0 / log(10.0));
const double dbtoa_functor::dbtoa_constant_64 = 20.0 / log(10.0);

typedef v_unary<dbtoa_functor, kVectorArray, kVectorArray> vdbtoa;

int C74_EXPORT main()
{
    vdbtoa::setup<vdbtoa>("vdbtoa~");
}
