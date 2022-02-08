
/*
 *  vdtbtoa~
 *
 *  vdtbtoa~ is a vectorised version of vdtbtoa~.
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

struct dbtoa_functor
{
    static const double dbtoa_constant;
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a)
    {
        return nan_fixer()(expf(a.mVal * static_cast<float>(dbtoa_constant)));
    }
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a)
    {
        return nan_fixer()(exp(a.mVal * dbtoa_constant));
    }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        mul_const_array(o, i, size, T(dbtoa_constant));
        exp_array(o, o, size);
        nan_fixer()(o, size);
    }
};

// Initialise Constants

const double dbtoa_functor::dbtoa_constant = log(10.0) / 20.0;

// Type Alias

using vdbtoa = v_unary<dbtoa_functor, calculation_type::vector_array, calculation_type::vector_array>;

// Main

int C74_EXPORT main()
{
    vdbtoa::setup<vdbtoa>("vdbtoa~");
}
