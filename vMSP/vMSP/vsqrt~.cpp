
/*
 *  vsqrt~
 *
 *  vsqrt~ is a vectorised version of sqrt~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"
#include "nans.hpp"

struct sqrt_functor
{    
    template <class T>
    T operator()(const T a) { return nan_fixer()(sqrt(a)); }
    
    // Empty Implementations

    void operator()(double *o, double *i, long size) {}
};

typedef v_unary<sqrt_functor, kVectorOp> vsqrt;

int C74_EXPORT main()
{
    vsqrt::setup<vsqrt>("vsqrt~");
}
