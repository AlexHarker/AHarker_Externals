
/*
 *  vasinh~
 *
 *  vasinh~ is a vectorised version of asinh~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "Base/v_unary.hpp"
#include <SIMDExtended.hpp>


// Functor

struct asinh_functor
{
    htl::simd_type<double, 1> operator()(const htl::simd_type<double, 1> a) { return asinh(a.m_val); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { asinh_array(o, i, size); }
};

// Type Alias

using vasinh = v_unary<asinh_functor, calculation_type::vector_array>;

// Main

int C74_EXPORT main()
{
    vasinh::setup<vasinh>("vasinh~");
}
