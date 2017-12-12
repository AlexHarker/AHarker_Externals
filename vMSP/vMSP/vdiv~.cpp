
/*
 *  vdiv~
 *
 *	vdiv~ is a vectorised version of div~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_binary.hpp>
#include "Helpers.h"

struct div_functor
{
    template <class T>
    T operator()(const T a, const T b)
    {
        const static T zero(static_cast<typename T::scalar_type>(0));
        
        return sel(b == zero, a / b, zero);
    }
    
    // Empty Implementations

    void operator()(double *o, double *i1, double *i2, long size) {}
    void operator()(float *o, float *i1, float *i2, long size) {}
};

typedef v_binary<div_functor, kVectorOp, kVectorOp> vdiv;

int C74_EXPORT main()
{
    vdiv::setup<vdiv>("vdiv~");
}
