
/*
 *  vlessthan~
 *
 *  vlessthan~ is a vectorised version of lessthan~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_binary.hpp"
#include "comparisons.hpp"

struct less
{
    template <class T> T operator()(const T a, const T b) { return a < b; }
};

typedef v_binary<comparison_functor<less>, kVectorOp, kVectorOp> vlessthan;

int C74_EXPORT main()
{
    vlessthan::setup<vlessthan>("vlessthan~");
}
