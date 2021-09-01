
/*
 *  vequals~
 *
 *  vequals~ is a vectorised version of equals~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_binary.hpp"
#include "comparisons.hpp"

struct equal
{
    template <class T> T operator()(const T a, const T b) { return a == b; }
};

typedef v_binary<comparison_functor<equal>, calculation_type::vector_op, calculation_type::vector_op> vequals;

int C74_EXPORT main()
{
    vequals::setup<vequals>("vequals~");
}
