
/* 
 *  vgreaterthan~
 *
 *  vgreaterthan~ is a vectorised version of greaterthan~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_binary.hpp"
#include "comparisons.hpp"

struct greater
{
    template <class T> T operator()(const T a, const T b) { return a > b; }
};

typedef v_binary<comparison_functor<greater>, kVectorOp, kVectorOp> vgreaterthan;

int C74_EXPORT main()
{
    vgreaterthan::setup<vgreaterthan>("vgreaterthan~");
}
