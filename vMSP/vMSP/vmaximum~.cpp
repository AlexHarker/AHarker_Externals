
/*
 *  vmaximum~
 *
 *  vmaximum~ is a vectorised version of maximum~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_binary.hpp"

struct maximum_functor
{
    template <class T>
    T operator()(const T a, const T b) { return max(a, b); }
    
    // Empty Implementations
    
    void operator()(double *o, double *i1, double *i2, long size, double val, InputType type) {}
};

typedef v_binary<maximum_functor, kVectorOp, kVectorOp> vmaximum;

int C74_EXPORT main()
{
    vmaximum::setup<vmaximum>("vmaximum~");
}
