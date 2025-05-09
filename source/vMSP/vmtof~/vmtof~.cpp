
/*
 *  vmtof~
 *
 *  vmtof~ is a vectorised version of vmtof~.
 *  The exact results from this object may vary slightly different to the native Max version in terms of accuracy.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "../Base/v_unary.hpp"
#include "../Base/conversions.hpp"
#include "../Base/nans.hpp"
#include <SIMDExtended.hpp>


// Functor

struct mtof_functor
{
    static const double mtof_mul_constant;
    static const double mtof_add_constant;
    
    htl::simd_type<double, 1> operator()(const htl::simd_type<double, 1> a)
    {
        return nan_fixer()(exp(a.m_val * mtof_mul_constant + mtof_add_constant));
    }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        mul_add_const_array(o, i, size, T(mtof_mul_constant), T(mtof_add_constant));
        exp_array(o, o, size);
        nan_fixer()(o, size);
    }
};

// Initialise Constants

const double mtof_functor::mtof_mul_constant = log(2.0) / 12.0;
const double mtof_functor::mtof_add_constant = log(440.0) - (log(2.0) * 69.0 / 12.0);

// Type Alias

using vmtof = v_unary<mtof_functor, calculation_type::vector_array>;

// Main

int C74_EXPORT main()
{
    vmtof::setup<vmtof>("vmtof~");
}
