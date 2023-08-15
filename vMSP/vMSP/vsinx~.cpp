
/*
 *  vsinx~
 *
 *  vsinx~ is a vectorised version of sinx~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "Base/v_unary.hpp"
#include <SIMDExtended.hpp>


// Functor

struct sinx_functor
{
    htl::simd_type<double, 1> operator()(const htl::simd_type<double, 1> a) { return sin(a.m_val); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { sin_array(o, i, size); }
};

// Type Alias

using vsinx = v_unary<sinx_functor, calculation_type::vector_array>;

// Main

int C74_EXPORT main()
{
    vsinx::setup<vsinx>("vsinx~");
}
