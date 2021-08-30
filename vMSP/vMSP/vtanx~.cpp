
/*
 *  vtanx~
 *
 *  vtanx~ is a vectorised version of tanx~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include <SIMDExtended.hpp>

struct tanx_functor
{
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return tan(a.mVal); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { tan_array(o, i, size); }

    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<tanx_functor, kVectorArray> vtanx;

int C74_EXPORT main()
{
    vtanx::setup<vtanx>("vtanx~");
}
