
/*
 *  vminimum~
 *
 *	vminimum~ is a vectorised version of minimum~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_binary.hpp"

struct minimum_functor
{
    template <class T>
    T operator()(const T a, const T b) { return min(a, b); }
    
    // Empty Implementations
    
    void operator()(float *o, float *i1, float *i2, long size) {}
    void operator()(double *o, double *i1, double *i2, long size) {}
};

typedef v_binary<minimum_functor, kVectorOp, kVectorOp> vminimum;

int C74_EXPORT main()
{
    vminimum::setup<vminimum>("vminimum~");
}
