
/*
 *  vpow~
 *
 *	vpow~ is a vectorised version of pow~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_binary.hpp>
#include <SIMDExtended.hpp>

struct pow_functor
{
    float operator()(const float a, const float b) { return powf(a, b); }
    double operator()(const double a, const double b) { return pow(a, b); }
 
    void operator()(double *o, double *i1, double *i2, long size)
    {
        f64_pow_array(o, i1, i2, size);
    }
    
    void operator()(float *o, float *i1, float *i2, long size)
    {
        f32_pow_array(o, i1, i2, size);
    }
    
    // Empty Implementations
   
    template <class T>
    T operator()(const T a, const T b) { return a; }
};

typedef v_binary<pow_functor, kVectorArray, kVectorArray> vpow;

int C74_EXPORT main()
{
    vpow::setup<vpow>("vpow~");
}
