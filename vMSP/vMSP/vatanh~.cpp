
/*
 *  vatanh~
 *
 *  vatanh~ is a vectorised version of atanh~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include "vector_loop.hpp"
#include <SIMDExtended.hpp>

struct atanh_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a)
    {
        return abs(a).mVal >= 1.f ? 0.f : atanhf(a.mVal);
    }
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a)
    {
        return abs(a).mVal >= 1.0 ? 0.0 : atanh(a.mVal);
    }
    
    struct zero_functor
    {
        template <class T>
        T operator()(const T& a) { return and_not(abs(a) >= T(1.0), a); }
    };
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        vector_loop<zero_functor>(o, i, size);
        atanh_array(o, o, size);
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
