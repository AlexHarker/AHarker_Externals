
/*
 *  vftom~
 *
 *	vftom~ is a vectorised version of v_ftom~.
 *	This object may be marginally less accurate than the standard version, although that disadvantage is probably outweighed in most cases by the speed benefit.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include "conversions.hpp"
#include <SIMDExtended.hpp>

struct ftom_functor
{
    const static double ftom_mul_constant;
    const static double ftom_add_constant;
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a)
    {
        const float b = static_cast<float>(ftom_mul_constant) * logf(a.mVal);
        return b + static_cast<float>(ftom_add_constant);
    }
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a)
    {
        return ftom_mul_constant * log(a.mVal) + ftom_add_constant;
    }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        log_array(o, i, size);
        mul_add_const_array(o, size, T(ftom_mul_constant), T(ftom_add_constant));
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

// Initialise constants

const double ftom_functor::ftom_mul_constant = 12.0 / log(2.0);
const double ftom_functor::ftom_add_constant = ((log(2.0) * 69.0 / 12.0) - log(440.0)) * ftom_mul_constant;

typedef v_unary<ftom_functor, kVectorArray, kVectorArray> vftom;

int C74_EXPORT main()
{
    vftom::setup<vftom>("vftom~");
}
