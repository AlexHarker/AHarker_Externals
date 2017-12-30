
/*
 *  vacos~
 *
 *	vacos~ is a vectorised version of acos~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include "nans.hpp"
#include <SIMDExtended.hpp>

struct acos_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return nan_fixer()(acosf(a.mVal)); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return nan_fixer()(acos(a.mVal)); }
    
    void operator()(float *o, float *i, long size)
    {
        f32_acos_array(o, i, size);
        nan_fixer()(o, size);
    }
    
    void operator()(double *o, double *i, long size)
    {
        f64_acos_array(o, i, size);
        nan_fixer()(o, size);
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
