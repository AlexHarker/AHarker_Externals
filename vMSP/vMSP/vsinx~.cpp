
/*
 *  vsinx~
 *
 *	vsinx~ is a vectorised version of sinx~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_unary.hpp>
#include <SIMDExtended.hpp>
#include <AH_Win_Math.h>

struct sinx_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return sinf(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return sin(a.mVal); }
    
    void operator()(double *o, double *i, long size)
    {
        f64_sin_array(o, i, size);
    }
    
    void operator()(float *o, float *i, long size)
    {
        f32_sin_array(o, i, size);
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<sinx_functor, kVectorArray, kVectorArray> vsinx;

int C74_EXPORT main()
{
    vsinx::setup<vsinx>("vsinx~");
}
