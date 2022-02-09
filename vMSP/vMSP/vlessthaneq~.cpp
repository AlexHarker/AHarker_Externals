
/*
 *  vlessthaneq~
 *
 *  vlessthaneq~ is a vectorised version of lessthaneq~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "Base/v_binary.hpp"
#include "Base/comparisons.hpp"


// Functor

struct less_eq
{
    template <class T>
    T operator()(const T a, const T b) { return a <= b; }
};

// Type Alias

using vlessthaneq = v_binary_comparison<less_eq>;

// Main

int C74_EXPORT main()
{
    vlessthaneq::setup<vlessthaneq>("vlessthaneq~");
}
