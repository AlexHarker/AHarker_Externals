
/*
 *  vatan~
 *
 *  vatan~ is a vectorised version of atan~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "../Base/v_unary.hpp"
#include <SIMDExtended.hpp>


// Functor

struct atan_functor
{
    htl::simd_type<double, 1> operator()(const htl::simd_type<double, 1> a) { return atan(a.m_val); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { atan_array(o, i, size); }
};

// Type Alias

using vatan = v_unary<atan_functor, calculation_type::vector_array>;

// Main

int C74_EXPORT main()
{
    vatan::setup<vatan>("vatan~");
}
