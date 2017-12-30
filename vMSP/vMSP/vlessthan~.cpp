
/*
 *  vlessthan~
 *
 *	vlessthan~ is a vectorised version of lessthan~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_binary.hpp"
#include "comparisons.hpp"

typedef v_binary<comparison_functor<less>, kVectorOp, kVectorOp> vlessthan;

int C74_EXPORT main()
{
    vlessthan::setup<vlessthan>("vlessthan~");
}
