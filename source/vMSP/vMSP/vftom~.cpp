
/*
 *  vftom~
 *
 *  vftom~ is a vectorised version of v_ftom~.
 *  The exact results from this object may vary slightly different to the native Max version in terms of accuracy.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "Base/v_unary.hpp"
#include "Base/conversions.hpp"
#include "Base/nans.hpp"
#include <SIMDExtended.hpp>


// Functor

struct ftom_functor
{
    static const double ftom_mul_constant;
    static const double ftom_add_constant;
    
    htl::simd_type<double, 1> operator()(const htl::simd_type<double, 1> a)
    {
        return nan_fixer()(ftom_mul_constant * log(a.m_val) + ftom_add_constant);
    }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        log_array(o, i, size);
        mul_add_const_array(o, size, T(ftom_mul_constant), T(ftom_add_constant));
        nan_fixer()(o, size);
    }
};

// Initialise Constants

const double ftom_functor::ftom_mul_constant = 12.0 / log(2.0);
const double ftom_functor::ftom_add_constant = ((log(2.0) * 69.0 / 12.0) - log(440.0)) * ftom_mul_constant;

// Type Alias

using vftom = v_unary<ftom_functor, calculation_type::vector_array>;

// Main

int C74_EXPORT main()
{
    vftom::setup<vftom>("vftom~");
}
