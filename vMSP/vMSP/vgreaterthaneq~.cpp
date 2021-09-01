
/*
 *  vgreaterthaneq~
 *
 *  vgreaterthaneq~ is a vectorised version of greaterthaneq~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_binary.hpp"
#include "comparisons.hpp"

struct greater_eq
{
    template <class T> T operator()(const T a, const T b) { return a >= b; }
};

typedef v_binary<comparison_functor<greater_eq>, calculation_type::vector_op, calculation_type::vector_op> vgreaterthaneq;

int C74_EXPORT main()
{
    vgreaterthaneq::setup<vgreaterthaneq>("vgreaterthaneq~");
}
