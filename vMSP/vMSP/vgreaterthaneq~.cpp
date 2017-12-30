
/*
 *  vgreaterthaneq~
 *
 *	vgreaterthaneq~ is a vectorised version of greaterthaneq~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_binary.hpp"
#include "comparisons.hpp"

typedef v_binary<comparison_functor<greater_eq>, kVectorOp, kVectorOp> vgreaterthaneq;

int C74_EXPORT main()
{
    vgreaterthaneq::setup<vgreaterthaneq>("vgreaterthaneq~");
}
