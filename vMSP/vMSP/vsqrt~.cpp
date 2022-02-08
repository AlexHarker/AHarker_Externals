
/*
 *  vsqrt~
 *
 *  vsqrt~ is a vectorised version of sqrt~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_unary.hpp"
#include "Base/nans.hpp"

// Functor

struct sqrt_functor
{    
    template <class T>
    T operator()(const T a) { return nan_fixer()(sqrt(a)); }
};

// Type Alias

using vsqrt = v_unary<sqrt_functor, calculation_type::vector_op, calculation_type::vector_op>;

// Main

int C74_EXPORT main()
{
    vsqrt::setup<vsqrt>("vsqrt~");
}
