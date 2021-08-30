
/*
 *  vmtof~
 *
 *  vmtof~ is a vectorised version of vmtof~.
 *  This object may be marginally less accurate than the standard version, although that disadvantage is probably outweighed in most cases by the speed benefit.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include "conversions.hpp"
#include "nans.hpp"
#include <SIMDExtended.hpp>

struct mtof_functor
{
    const static double mtof_mul_constant;
    const static double mtof_add_constant;
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a)
    {
        return nan_fixer()(exp(a.mVal * mtof_mul_constant + mtof_add_constant));
    }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        mul_add_const_array(o, i, size, T(mtof_mul_constant), T(mtof_add_constant));
        exp_array(o, o, size);
        nan_fixer()(o, size);
    }

    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

// Initialise constants

const double mtof_functor::mtof_mul_constant = log(2.0) / 12.0;
const double mtof_functor::mtof_add_constant = log(440.0) - (log(2.0) * 69.0 / 12.0);

typedef v_unary<mtof_functor, kVectorArray> vmtof;

int C74_EXPORT main()
{
    vmtof::setup<vmtof>("vmtof~");
}
