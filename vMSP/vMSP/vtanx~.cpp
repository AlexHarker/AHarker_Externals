
/*
 *  vtanx~
 *
 *	vtanx~ is a vectorised version of tanx~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include "nans.hpp"
#include <SIMDExtended.hpp>

struct tanx_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return nan_fixer()(tanf(a.mVal)); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return nan_fixer()(tan(a.mVal)); }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        tan_array(o, i, size);
        nan_fixer()(o, size);
    }

    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<tanx_functor, kVectorArray, kVectorArray> vtanx;

int C74_EXPORT main()
{
    vtanx::setup<vtanx>("vtanx~");
}
