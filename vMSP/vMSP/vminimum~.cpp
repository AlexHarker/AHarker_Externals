
/*
 *  vminimum~
 *
 *  vminimum~ is a vectorised version of minimum~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_binary.hpp"

struct minimum_functor
{
    // N.B. operand ordering gives an exact match to Max's operation with nan inputs
    
    template <class T>
    T operator()(const T a, const T b) { return min(b, a); }
    
    // Empty Implementations
    
    void operator()(float *o, float *i1, float *i2, long size, double val, inputs type) {}
    void operator()(double *o, double *i1, double *i2, long size, double val, inputs type) {}
};

typedef v_binary<minimum_functor, calculation_type::vector_op, calculation_type::vector_op> vminimum;

int C74_EXPORT main()
{
    vminimum::setup<vminimum>("vminimum~");
}
