
/*
 *  vlessthaneq~
 *
 *	vlessthaneq~ is a vectorised version of lessthaneq~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_binary.hpp"
#include "comparisons.hpp"

typedef v_binary<comparison_functor<less_eq>, kVectorOp, kVectorOp> vlessthaneq;

int C74_EXPORT main()
{
    vlessthaneq::setup<vlessthaneq>("vlessthaneq~");
}
