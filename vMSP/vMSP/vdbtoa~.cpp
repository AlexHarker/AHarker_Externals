
/*
 *  vdtbtoa~
 *
 *  vdtbtoa~ is a vectorised version of vdtbtoa~.
 *  This object may be marginally less accurate than the standard version, although that disadvantage is probably outweighed in most cases by the speed benefit.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include "conversions.hpp"
#include "nans.hpp"
#include <SIMDExtended.hpp>

struct dbtoa_functor
{
    const static double dbtoa_constant;
    
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
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

// Initialise constants

const double dbtoa_functor::dbtoa_constant = log(10.0) / 20.0;

typedef v_unary<dbtoa_functor, calculation_type::vector_array, calculation_type::vector_array> vdbtoa;

int C74_EXPORT main()
{
    vdbtoa::setup<vdbtoa>("vdbtoa~");
}
