
/*
 *  vacosh~
 *
 *	vacosh~ is a vectorised version of acosh~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include <SIMDExtended.hpp>

struct acosh_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return acoshf(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return acosh(a.mVal); }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        acosh_array(o, i, size);
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<acosh_functor, kVectorArray, kVectorArray> vacosh;

int C74_EXPORT main()
{
    vacosh::setup<vacosh>("vacosh~");
}
