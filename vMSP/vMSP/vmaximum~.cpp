
/*
 *  vmaximum~
 *
 *  vmaximum~ is a vectorised version of maximum~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_binary.hpp"

struct maximum_functor
{
    // N.B. operand ordering gives an exact match to Max's operation with nan inputs

    template <class T>
    T operator()(const T a, const T b) { return max(b, a); }
    
    // Empty Implementations
    
    void operator()(float *o, float *i1, float *i2, long size, double val, inputs type) {}
    void operator()(double *o, double *i1, double *i2, long size, double val, inputs type) {}
};

typedef v_binary<maximum_functor, calculation_type::vector_op, calculation_type::vector_op> vmaximum;

int C74_EXPORT main()
{
    vmaximum::setup<vmaximum>("vmaximum~");
}
