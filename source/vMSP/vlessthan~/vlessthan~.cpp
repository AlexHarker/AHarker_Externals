
/*
 *  vlessthan~
 *
 *  vlessthan~ is a vectorised version of lessthan~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "../Base/v_binary.hpp"
#include "../Base/comparisons.hpp"


// Functor

struct less
{
    template <class T>
    T operator()(const T a, const T b) { return a < b; }
};

// Type Alias

using vlessthan = v_binary_comparison<less>;

// Main

int C74_EXPORT main()
{
    vlessthan::setup<vlessthan>("vlessthan~");
}
