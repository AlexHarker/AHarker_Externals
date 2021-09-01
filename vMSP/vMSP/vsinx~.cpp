
/*
 *  vsinx~
 *
 *  vsinx~ is a vectorised version of sinx~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_unary.hpp"
#include <SIMDExtended.hpp>

struct sinx_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return sinf(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return sin(a.mVal); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { sin_array(o, i, size); }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<sinx_functor, calculation_type::vector_array, calculation_type::vector_array> vsinx;

int C74_EXPORT main()
{
    vsinx::setup<vsinx>("vsinx~");
}
