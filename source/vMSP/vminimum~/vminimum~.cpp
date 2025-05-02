
/*
 *  vminimum~
 *
 *  vminimum~ is a vectorised version of minimum~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "../Base/v_binary.hpp"


// Functor

struct minimum_functor
{
    // N.B. operand ordering gives an exact match to Max's operation with nan inputs
    
    template <class T>
    T operator()(const T a, const T b) { return min(b, a); }
};

// Type Alias

using vminimum = v_binary<minimum_functor, calculation_type::vector_op>;

// Main

int C74_EXPORT main()
{
    vminimum::setup<vminimum>("vminimum~");
}
