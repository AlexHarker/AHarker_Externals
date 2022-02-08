
/*
 *  vrminus~
 *
 *  vrminus~ is a vectorised version of rminus~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_binary.hpp"

struct rminus_functor
{
    template <class T>
    T operator()(const T a, const T b) { return b - a; }
};

typedef v_binary<rminus_functor, calculation_type::vector_op, calculation_type::vector_op> vrminus;

int C74_EXPORT main()
{
    vrminus::setup<vrminus>("vrminus~");
}
