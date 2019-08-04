
/*
 *  vtanh~
 *
 *  vtanh~ is a vectorised version of tanh~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include <SIMDExtended.hpp>

struct tanh_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return tanhf(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return tanh(a.mVal); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { tanh_array(o, i, size); }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<tanh_functor, kVectorArray, kVectorArray> vtanh;

int C74_EXPORT main()
{
    vtanh::setup<vtanh>("vtanh~");
}
