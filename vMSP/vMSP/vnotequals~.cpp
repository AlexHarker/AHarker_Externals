
/*
 *  vnotequals~
 *
 *	vnotequals~ is a vectorised version of notequals~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_binary.hpp>
#include <comparisons.hpp>

typedef v_binary<comparison_functor<not_equal>, kVectorOp, kVectorOp> vnotequals;

int C74_EXPORT main()
{
    vnotequals::setup<vnotequals>("vnotequals~");
}
