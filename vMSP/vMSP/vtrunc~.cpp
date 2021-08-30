
/*
 *  vtrunc~
 *
 *  vtrunc~ is a vectorised version of trunc~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_unary.hpp"

struct trunc_functor
{
    template <class T>
    T operator()(const T a) { return trunc(a); }
    
    // Empty Implementations
    
    void operator()(float *o, float *i, long size) {}
    void operator()(double *o, double *i, long size) {}
};

typedef v_unary<trunc_functor, kVectorOp> vtrunc;

int C74_EXPORT main()
{
    vtrunc::setup<vtrunc>("vtrunc~");
}
