
/*
 *  vacosh~
 *
 *	vacosh~ is a vectorised version of acosh~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include "nans.hpp"
#include <SIMDExtended.hpp>

struct acosh_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return nan_fixer()(acoshf(a.mVal)); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return nan_fixer()(acosh(a.mVal)); }
    
    void operator()(float *o, float *i, long size)
    {
        f32_acosh_array(o, i, size);
        nan_fixer()(o, size);
    }
    
    void operator()(double *o, double *i, long size)
    {
        f64_acosh_array(o, i, size);
        nan_fixer()(o, size);
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<acosh_functor, kVectorArray, kVectorArray> vacosh;

int C74_EXPORT main()
{
    vacosh::setup<vacosh>("vacosh~");
}
