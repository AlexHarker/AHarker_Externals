
/*
 *  vftom~
 *
 *	vftom~ is a vectorised version of v_ftom~.
 *	This object may be marginally less accurate than the standard version, although that disadvantage is porbably outweighed in most cases by the speed benefit.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_unary.hpp>
#include <conversions.hpp>
#include <SIMDExtended.hpp>
#include <AH_Win_Math.h>

struct ftom_functor
{
    const static float ftom_mul_constant_32;
    const static double ftom_mul_constant_64;
    const static float ftom_add_constant_32;
    const static double ftom_add_constant_64;
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return static_cast<float>(ftom_mul_constant_64 * log(a.mVal) + ftom_add_constant_64); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return ftom_mul_constant_64 * log(a.mVal) + ftom_add_constant_64; }
    
    void operator()(float *o, float *i, long size)
    {
        f32_log_array(o, i, size);
        f32_mul_add_const_array(o, size, ftom_mul_constant_32, ftom_add_constant_32);
    }
    
    void operator()(double *o, double *i, long size)
    {
        f64_log_array(o, i, size);
        f64_mul_add_const_array(o, size, ftom_mul_constant_64, ftom_add_constant_64);
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

// Initialise constants

const float ftom_functor::ftom_mul_constant_32 = static_cast<float>(12.0 / log(2.0));
const double ftom_functor::ftom_mul_constant_64 = 12.0 / log(2.0);
const float ftom_functor::ftom_add_constant_32 = static_cast<float>(269.0 - (log(440.0) * ftom_mul_constant_64));
const double ftom_functor::ftom_add_constant_64 = 269.0 - (log(440.0) * ftom_mul_constant_64);

typedef v_unary<ftom_functor, kVectorArray, kVectorArray> vftom;

int C74_EXPORT main()
{
    vftom::setup<vftom>("vftom~");
}
