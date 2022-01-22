
/* 
 *  vgreaterthan~
 *
 *  vgreaterthan~ is a vectorised version of greaterthan~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_binary.hpp"
#include "Base/comparisons.hpp"

struct greater
{
    template <class T> T operator()(const T a, const T b) { return a > b; }
};

typedef v_binary<comparison_functor<greater>, calculation_type::vector_op, calculation_type::vector_op> vgreaterthan;

int C74_EXPORT main()
{
    vgreaterthan::setup<vgreaterthan>("vgreaterthan~");
}
