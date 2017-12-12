
/*
 *  vequals~
 *
 *	vequals~ is a vectorised version of equals~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_binary.hpp>
#include <comparisons.hpp>

typedef v_binary<comparison_functor<equal>, kVectorOp, kVectorOp> vequals;

int C74_EXPORT main()
{
    vequals::setup<vequals>("vequals~");
}
