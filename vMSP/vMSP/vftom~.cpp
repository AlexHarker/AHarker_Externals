
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
    const static float ftom_mul_constant_32;
    const static double ftom_mul_constant_64;
    const static float ftom_add_constant_32;
    const static double ftom_add_constant_64;
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return static_cast<float>(ftom_mul_constant_64 * log(a.mVal) + ftom_add_constant_64); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return ftom_mul_constant_64 * log(a.mVal) + ftom_add_constant_64; }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        log_array(o, i, size);
        mul_add_const_array(o, size, T(ftom_mul_constant_64), T(ftom_add_constant_64));
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

// Initialise constants

const float ftom_functor::ftom_mul_constant_32 = static_cast<float>(12.0 / log(2.0));
const double ftom_functor::ftom_mul_constant_64 = 12.0 / log(2.0);
const float ftom_functor::ftom_add_constant_32 = static_cast<float>(269.0 - (log(440.0) * ftom_mul_constant_64));
const double ftom_functor::ftom_add_constant_64 = 269.0 - (log(440.0) * ftom_mul_constant_64);

typedef v_unary<ftom_functor, kVectorArray, kVectorArray> vftom;

int C74_EXPORT main()
{
    vftom::setup<vftom>("vftom~");
}
