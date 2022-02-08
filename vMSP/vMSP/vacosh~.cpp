
/*
 *  vacosh~
 *
 *  vacosh~ is a vectorised version of acosh~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_unary.hpp"
#include "Base/vector_loop.hpp"
#include <SIMDExtended.hpp>

// Functor

struct acosh_functor
{
    // Input Limiting Functor
    
    struct replace_functor
    {
        template <class T>
        T operator()(const T& a) { return sel(a, T(1.0), a < T(1.0)); }
    };
    
    // Ops + Array Operators
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a)
    {
        return a.mVal < 1.f ? 0.f : acoshf(a.mVal);
    }
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a)
    {
        return a.mVal < 1.0 ? 0.0 : acosh(a.mVal);
    }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        vector_loop<replace_functor>(o, i, size);
        acosh_array(o, o, size);
    }
};

// Type Alias

using vacosh = v_unary<acosh_functor, calculation_type::vector_array, calculation_type::vector_array>;

// Main

int C74_EXPORT main()
{
    vacosh::setup<vacosh>("vacosh~");
}
