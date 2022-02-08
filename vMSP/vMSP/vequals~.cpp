
/*
 *  vequals~
 *
 *  vequals~ is a vectorised version of equals~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_binary.hpp"
#include "Base/comparisons.hpp"

// Functor

struct equal
{
    template <class T>
    T operator()(const T a, const T b) { return a == b; }
};

// Type Alias

using vequals = v_binary<comparison_functor<equal>, calculation_type::vector_op, calculation_type::vector_op>;

// Main

int C74_EXPORT main()
{
    vequals::setup<vequals>("vequals~");
}
