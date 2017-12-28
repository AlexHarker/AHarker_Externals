
/*
 *  vsinh~
 *
 *	vsinh~ is a vectorised version of sinh~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_unary.hpp>
#include <SIMDExtended.hpp>

struct sinh_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return sinhf(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return sinh(a.mVal); }
    
    void operator()(float *o, float *i, long size)
    {
        f32_sinh_array(o, i, size);
    }
    
    void operator()(double *o, double *i, long size)
    {
        f64_sinh_array(o, i, size);
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<sinh_functor, kVectorArray, kVectorArray> vsinh;

int C74_EXPORT main()
{
    vsinh::setup<vsinh>("vsinh~");
}
