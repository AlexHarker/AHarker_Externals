
/*
 *  vasin~
 *
 *	vasin~ is a vectorised version of asin~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include "nans.hpp"
#include <SIMDExtended.hpp>

struct asin_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return nan_fixer()(asinf(a.mVal)); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return nan_fixer()(asin(a.mVal)); }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        asin_array(o, i, size);
        nan_fixer()(o, size);
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<asin_functor, kVectorArray, kVectorArray> vasin;

int C74_EXPORT main()
{
    vasin::setup<vasin>("vasin~");
}

