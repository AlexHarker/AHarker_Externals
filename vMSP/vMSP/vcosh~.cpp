
/*
 *  vcosh~
 *
 *  vcosh~ is a vectorised version of cosh~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "Base/v_unary.hpp"
#include <SIMDExtended.hpp>


// Functor

struct cosh_functor
{
    htl::simd_type<double, 1> operator()(const htl::simd_type<double, 1> a) { return cosh(a.m_val); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { cosh_array(o, i, size); }
};

// Type Alias

using vcosh = v_unary<cosh_functor, calculation_type::vector_array>;

// Main

int C74_EXPORT main()
{
    vcosh::setup<vcosh>("vcosh~");
}
