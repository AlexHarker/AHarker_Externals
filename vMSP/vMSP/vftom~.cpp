
/*
 *  vftom~
 *
 *  vftom~ is a vectorised version of v_ftom~.
 *  This object may be marginally less accurate than the standard version, although that disadvantage is probably outweighed in most cases by the speed benefit.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_unary.hpp"
#include "Base/conversions.hpp"
#include "Base/nans.hpp"
#include <SIMDExtended.hpp>

struct ftom_functor
{
    static const double ftom_mul_constant;
    static const double ftom_add_constant;
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a)
    {
        return nan_fixer()(ftom_mul_constant * log(a.mVal) + ftom_add_constant);
    }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        log_array(o, i, size);
        mul_add_const_array(o, size, T(ftom_mul_constant), T(ftom_add_constant));
        nan_fixer()(o, size);
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

// Initialise constants

const double ftom_functor::ftom_mul_constant = 12.0 / log(2.0);
const double ftom_functor::ftom_add_constant = ((log(2.0) * 69.0 / 12.0) - log(440.0)) * ftom_mul_constant;

typedef v_unary<ftom_functor, calculation_type::vector_array> vftom;

int C74_EXPORT main()
{
    vftom::setup<vftom>("vftom~");
}
