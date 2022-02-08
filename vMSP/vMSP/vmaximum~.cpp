
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
};

typedef v_binary<maximum_functor, calculation_type::vector_op> vmaximum;

int C74_EXPORT main()
{
    vmaximum::setup<vmaximum>("vmaximum~");
}
