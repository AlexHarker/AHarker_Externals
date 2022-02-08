
/*
 *  vtrunc~
 *
 *  vtrunc~ is a vectorised version of trunc~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "Base/v_unary.hpp"


// Functor

struct trunc_functor
{
    template <class T>
    T operator()(const T a) { return trunc(a); }
};

// Type Alias

using vtrunc = v_unary<trunc_functor, calculation_type::vector_op, calculation_type::vector_op>;

// Main

int C74_EXPORT main()
{
    vtrunc::setup<vtrunc>("vtrunc~");
}
