
/*
 *  vacos~
 *
 *  vacos~ is a vectorised version of acos~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_unary.hpp"
#include "Base/vector_loop.hpp"
#include <SIMDExtended.hpp>

// Functor

struct acos_functor
{
    struct replace_functor
    {
        template <class T>
        T operator()(const T& a) { return sel(a, T(1.0), abs(a) > T(1.0)); }
    };
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a)
    {
        return abs(a).mVal > 1.f ? 0.f : acosf(a.mVal);
    }
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a)
    {
        return abs(a).mVal > 1.0 ? 0.0 : acos(a.mVal);
    }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        vector_loop<replace_functor>(o, i, size);
        acos_array(o, o, size);
    }
};

// Type Alias

using vacos = v_unary<acos_functor, calculation_type::vector_array, calculation_type::vector_array>;

// Main

int C74_EXPORT main()
{
    vacos::setup<vacos>("vacos~");
}
