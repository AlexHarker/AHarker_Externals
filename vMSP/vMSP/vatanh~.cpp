
/*
 *  vatanh~
 *
 *	vatanh~ is a vectorised version of atanh~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include "nans.hpp"
#include <SIMDExtended.hpp>

struct atanh_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return nan_fixer()(atanhf(a.mVal)); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return nan_fixer()(atanh(a.mVal)); }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        atanh_array(o, i, size);
        nan_fixer()(o, size);
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<atanh_functor, kVectorArray, kVectorArray> vatanh;

int C74_EXPORT main()
{
    vatanh::setup<vatanh>("vatanh~");
}
