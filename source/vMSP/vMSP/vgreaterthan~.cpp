
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


// Functor

struct greater
{
    template <class T>
    T operator()(const T a, const T b) { return a > b; }
};

// Type Alias

using vgreaterthan = v_binary_comparison<greater>;

// Main

int C74_EXPORT main()
{
    vgreaterthan::setup<vgreaterthan>("vgreaterthan~");
}
