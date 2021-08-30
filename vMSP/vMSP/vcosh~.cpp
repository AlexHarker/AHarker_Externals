
/*
 *  vcosh~
 *
 *  vcosh~ is a vectorised version of cosh~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include <SIMDExtended.hpp>

struct cosh_functor
{
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return cosh(a.mVal); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { cosh_array(o, i, size); }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<cosh_functor, kVectorArray> vcosh;

int C74_EXPORT main()
{
    vcosh::setup<vcosh>("vcosh~");
}
