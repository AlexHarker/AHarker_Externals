
/*
 *  vasinh~
 *
 *	vasinh~ is a vectorised version of asinh~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_unary.hpp>
#include <SIMDExtended.hpp>
#include <AH_Win_Math.h>

struct asinh_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return asinhf(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return asinh(a.mVal); }
    
    void operator()(float *o, float *i, long size)
    {
        f32_asinh_array(o, i, size);
    }
    
    void operator()(double *o, double *i, long size)
    {
        f64_asinh_array(o, i, size);
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<asinh_functor, kVectorArray, kVectorArray> vasinh;

int C74_EXPORT main()
{
    vasinh::setup<vasinh>("vasinh~");
}
