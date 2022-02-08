
/*
 *  vnotequals~
 *
 *  vnotequals~ is a vectorised version of notequals~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "Base/v_binary.hpp"
#include "Base/comparisons.hpp"


// Functor

struct not_equal
{
    template <class T>
    T operator()(const T a, const T b) { return a != b; }
};

// Type Alias

using vnotequals = v_binary<comparison_functor<not_equal>, calculation_type::vector_op>;

// Main

int C74_EXPORT main()
{
    vnotequals::setup<vnotequals>("vnotequals~");
}
