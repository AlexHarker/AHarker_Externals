
/*
 *  vrminus~
 *
 *	vrminus~ is a vectorised version of rminus~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_binary.hpp"

struct rminus_functor
{
    template <class T>
    T operator()(const T a, const T b) { return b - a; }
    
    // Empty Implementations
    
    void operator()(float *o, float *i1, float *i2, long size) {}
    void operator()(double *o, double *i1, double *i2, long size) {}
};

typedef v_binary<rminus_functor, kVectorOp, kVectorOp> vrminus;

int C74_EXPORT main()
{
    vrminus::setup<vrminus>("vrminus~");
}
