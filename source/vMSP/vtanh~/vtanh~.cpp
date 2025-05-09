
/*
 *  vtanh~
 *
 *  vtanh~ is a vectorised version of tanh~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "../Base/v_unary.hpp"
#include <SIMDExtended.hpp>


// Functor

struct tanh_functor
{
    htl::simd_type<double, 1> operator()(const htl::simd_type<double, 1> a) { return tanh(a.m_val); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { tanh_array(o, i, size); }
};

// Type Alias

using vtanh = v_unary<tanh_functor, calculation_type::vector_array>;

// Main

int C74_EXPORT main()
{
    vtanh::setup<vtanh>("vtanh~");
}
