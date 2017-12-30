
/*
 *  vatan~
 *
 *	vatan~ is a vectorised version of atan~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include <SIMDExtended.hpp>

struct atan_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return atanf(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return atan(a.mVal); }
    
    void operator()(float *o, float *i, long size)
    {
        f32_atan_array(o, i, size);
    }
    
    void operator()(double *o, double *i, long size)
    {
        f64_atan_array(o, i, size);
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<atan_functor, kVectorArray, kVectorArray> vatan;

int C74_EXPORT main()
{
    vatan::setup<vatan>("vatan~");
}
