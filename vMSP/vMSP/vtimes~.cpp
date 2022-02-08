
/*
 *  vtimes~
 *
 *  vtimes~ is a vectorised version of times~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_binary.hpp"
#include "Base/nans.hpp"

struct times_functor
{
    template <class T>
    T operator()(const T a, const T b) { return nan_fixer()(a * b); }
};

typedef v_binary<times_functor, calculation_type::vector_op, calculation_type::vector_op> vtimes;

int C74_EXPORT main()
{
    vtimes::setup<vtimes>("vtimes~");
}
