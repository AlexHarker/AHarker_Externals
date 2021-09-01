
/*
 *  vplus~
 *
 *  vplus~ is a vectorised version of plus~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_binary.hpp"

struct plus_functor
{
    template <class T>
    T operator()(const T a, const T b) { return a + b; }
    
    // Empty Implementations
    
    void operator()(float *o, float *i1, float *i2, long size, double val, inputs type) {}
    void operator()(double *o, double *i1, double *i2, long size, double val, inputs type) {}
};

typedef v_binary<plus_functor, calculation_type::vector_op, calculation_type::vector_op> vplus;

int C74_EXPORT main()
{
    vplus::setup<vplus>("vplus~");
}
