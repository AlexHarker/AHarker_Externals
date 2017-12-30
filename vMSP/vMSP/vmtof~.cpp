
/*
 *  vmtof~
 *
 *	vmtof~ is a vectorised version of vmtof~.
 *	This object may be marginally less accurate than the standard version, although that disadvantage is porbably outweighed in most cases by the speed benefit.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include "conversions.hpp"
#include <SIMDExtended.hpp>

struct mtof_functor
{
    const static float mtof_mul_constant_32;
    const static double mtof_mul_constant_64;
    const static float mtof_add_constant_32;
    const static double mtof_add_constant_64;
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return expf(a.mVal * mtof_mul_constant_32 + mtof_add_constant_32); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return exp(a.mVal * mtof_mul_constant_64 + mtof_add_constant_32); }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        mul_add_const_array(o, i, size, T(mtof_mul_constant_64), T(mtof_add_constant_64));
        exp_array(o, o, size);
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
