
/*
 *  vdiv~
 *
 *  vdiv~ is a vectorised version of div~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_binary.hpp"

struct div_functor
{
    template <class T>
    T operator()(const T a, const T b)
    {
        const static T zero(static_cast<typename T::scalar_type>(0));
        
        // N.B - the exact behaviour of div~ is different when a is NaN and b is zero, but it uses a * (1/b)
        
        return sel(zero, a / b, b != zero);
    }
    
    // Empty Implementations

    void operator()(float *o, float *i1, float *i2, long size, double val, inputs type) {}
    void operator()(double *o, double *i1, double *i2, long size, double val, inputs type) {}
};

typedef v_binary<div_functor, calculation_type::vector_op, calculation_type::vector_op> vdiv;

int C74_EXPORT main()
{
    vdiv::setup<vdiv>("vdiv~");
}
