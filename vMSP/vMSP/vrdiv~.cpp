
/*
 *  vrdiv~
 *
 *  vrdiv~ is a vectorised version of rdiv~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_binary.hpp"
#include "Base/nans.hpp"

struct rdiv_functor
{
    template <class T>
    T operator()(const T a, const T b) { return nan_fixer()(b / a); }
};

typedef v_binary<rdiv_functor, calculation_type::vector_op> vrdiv;

int C74_EXPORT main()
{
    vrdiv::setup<vrdiv>("vrdiv~");
}
