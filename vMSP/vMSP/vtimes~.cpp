
/*
 *  vtimes~
 *
 *	vtimes~ is a vectorised version of times~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_binary.hpp"
#include "nans.hpp"

struct times_functor
{
    template <class T>
    T operator()(const T a, const T b) { return nan_fixer()(a * b); }
    
    // Empty Implementations

    void operator()(float *o, float *i1, float *i2, long size, InputType type) {}
    void operator()(double *o, double *i1, double *i2, long size, InputType type) {}
};

typedef v_binary<times_functor, kVectorOp, kVectorOp> vtimes;

int C74_EXPORT main()
{
    vtimes::setup<vtimes>("vtimes~");
}
