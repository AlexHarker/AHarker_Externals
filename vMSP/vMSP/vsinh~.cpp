
/*
 *  vsinh~
 *
 *  vsinh~ is a vectorised version of sinh~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "Base/v_unary.hpp"
#include <SIMDExtended.hpp>


// Functor

struct sinh_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return sinhf(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return sinh(a.mVal); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { sinh_array(o, i, size); }
};

// Type Alias

using vsinh = v_unary<sinh_functor, calculation_type::vector_array, calculation_type::vector_array>;

// Main

int C74_EXPORT main()
{
    vsinh::setup<vsinh>("vsinh~");
}
