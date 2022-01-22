
/*
 *  vsqrt~
 *
 *  vsqrt~ is a vectorised version of sqrt~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_unary.hpp"
#include "Base/nans.hpp"

struct sqrt_functor
{    
    template <class T>
    T operator()(const T a) { return nan_fixer()(sqrt(a)); }
    
    // Empty Implementations

    void operator()(float *o, float *i, long size) {}
    void operator()(double *o, double *i, long size) {}
};

typedef v_unary<sqrt_functor, calculation_type::vector_op, calculation_type::vector_op> vsqrt;

int C74_EXPORT main()
{
    vsqrt::setup<vsqrt>("vsqrt~");
}
