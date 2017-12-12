
/*
 *  vmtof~
 *
 *	vmtof~ is a vectorised version of vmtof~.
 *	This object may be marginally less accurate than the standard version, although that disadvantage is porbably outweighed in most cases by the speed benefit.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_unary.hpp>
#include <conversions.hpp>
#include <SIMDExtended.hpp>
#include <AH_Win_Math.h>

struct mtof_functor
{
    const static float mtof_mul_constant_32;
    const static double mtof_mul_constant_64;
    const static float mtof_add_constant_32;
    const static double mtof_add_constant_64;
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return expf(a.mVal * dbtoa_constant_32); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return exp(a.mVal * dbtoa_constant_64); }
    
    void operator()(float *o, float *i, long size)
    {
        f32_mul_add_const_array(o, i, size, mtof_mul_constant_32, mtof_add_constant_32);
        f32_exp_array(o, o, size);
    }
    
    void operator()(double *o, double *i, long size)
    {
        f64_mul_add_const_array(o, i, size, mtof_mul_constant_64, mtof_add_constant_64);
        f64_exp_array(o, o, size);
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

// Initialise constants

const float mtof_functor::mtof_mul_constant_32 = static_cast<float>(log(2.0) / 12.0);
const double mtof_functor::mtof_mul_constant_64 = log(2.0) / 12.0;
const float mtof_functor::mtof_add_constant_32 = static_cast<float>(log(440.0) - (log(2.0) * 69.0 / 12.0));
const double mtof_functor::mtof_add_constant_64 = log(440.0) - (log(2.0) * 69.0 / 12.0);

typedef v_unary<mtof_functor, kVectorArray, kVectorArray> vmtof;

int C74_EXPORT main()
{
    vmtof::setup<vmtof>("vmtof~");
}
