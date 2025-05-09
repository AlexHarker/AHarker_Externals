
/*
 *  vasin~
 *
 *  vasin~ is a vectorised version of asin~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "../Base/v_unary.hpp"
#include "../Base/vector_loop.hpp"
#include <SIMDExtended.hpp>


struct asin_functor
{
    // Input Limiting Functor

    struct zero_functor
    {
        template <class T>
        T operator()(const T& a) { return and_not(abs(a) > T(1.0), a); }
    };

    // Ops + Array Operators

    htl::simd_type<double, 1> operator()(const htl::simd_type<double, 1> a)
    {
        return abs(a).m_val > 1.0 ? 0.0 : asin(a.m_val);
    }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        vector_loop<zero_functor>(o, i, size);
        asin_array(o, o, size);
    }
};

typedef v_unary<asin_functor, calculation_type::vector_array> vasin;

int C74_EXPORT main()
{
    vasin::setup<vasin>("vasin~");
}
