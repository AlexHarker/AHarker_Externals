
/*
 *  vcosh~
 *
 *	vcosh~ is a vectorised version of cosh~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_unary.hpp>
#include <SIMDExtended.hpp>
#include <AH_Win_Math.h>

struct cosh_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return coshf(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return cosh(a.mVal); }
    
    void operator()(double *o, double *i, long size)
    {
        f64_cosh_array(o, i, size);
    }
    
    void operator()(float *o, float *i, long size)
    {
        f32_cosh_array(o, i, size);
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<cosh_functor, kVectorArray, kVectorArray> vcosh;

int C74_EXPORT main()
{
    vcosh::setup<vcosh>("vcosh~");
}
