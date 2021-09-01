
/*
 *  vsinh~
 *
 *  vsinh~ is a vectorised version of sinh~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_unary.hpp"
#include <SIMDExtended.hpp>

struct sinh_functor
{
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return sinh(a.mVal); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { sinh_array(o, i, size); }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<sinh_functor, calculation_type::vector_array> vsinh;

int C74_EXPORT main()
{
    vsinh::setup<vsinh>("vsinh~");
}
