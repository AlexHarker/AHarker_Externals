
/*
 *  vabs~
 *
 *	vabs~ is a vectorised version of abs~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"

struct abs_functor
{
    template <class T>
    T operator()(const T a) { return abs(a); }

    // Empty Implementations
    
    void operator()(float *o, float *i, long size) {}
    void operator()(double *o, double *i, long size) {}
};

typedef v_unary<abs_functor, kVectorOp, kVectorOp> vabs;

int C74_EXPORT main()
{
    vabs::setup<vabs>("vabs~");
}
