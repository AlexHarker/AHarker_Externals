
/*
 *  vatodb~
 *
 *	vatodb~ is a vectorised version of vatodb~.
 *	This object may be marginally less accurate than the standard version, although that disadvantage is porbably outweighed in most cases by the speed benefit.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include "conversions.hpp"
#include <SIMDExtended.hpp>

struct atodb_functor
{
    const static float atodb_constant_32;
    const static double atodb_constant_64;
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return 20.f * log10f(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return 20.0 * log10(a.mVal); }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        log_array(o, i, size);
        mul_const_array(o, size, T(atodb_constant_64));
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

// Initialise constants

const float atodb_functor::atodb_constant_32 = static_cast<float>(20.0 / log(10.0));
const double atodb_functor::atodb_constant_64 = 20.0 / log(10.0);

typedef v_unary<atodb_functor, kVectorArray, kVectorArray> vatodb;

int C74_EXPORT main()
{
    vatodb::setup<vatodb>("vatodb~");
}
