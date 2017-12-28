
/*
 *  vcosx~
 *
 *	vcosx~ is a vectorised version of cosx~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_unary.hpp>
#include <SIMDExtended.hpp>

struct cosx_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return cosf(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return cos(a.mVal); }
    
    void operator()(float *o, float *i, long size)
    {
        f32_cos_array(o, i, size);
    }
    
    void operator()(double *o, double *i, long size)
    {
        f64_cos_array(o, i, size);
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<cosx_functor, kVectorArray, kVectorArray> vcosx;

int C74_EXPORT main()
{
    vcosx::setup<vcosx>("vcosx~");
}
