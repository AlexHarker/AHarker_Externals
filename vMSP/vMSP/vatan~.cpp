
/*
 *  vatan~
 *
 *  vatan~ is a vectorised version of atan~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_unary.hpp"
#include <SIMDExtended.hpp>

struct atan_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return atanf(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return atan(a.mVal); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { atan_array(o, i, size); }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<atan_functor, calculation_type::vector_array, calculation_type::vector_array> vatan;

int C74_EXPORT main()
{
    vatan::setup<vatan>("vatan~");
}
