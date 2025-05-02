
/*
 *  vabs~
 *
 *  vabs~ is a vectorised version of abs~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "../Base/v_unary.hpp"


// Functor

struct abs_functor
{
    template <class T>
    T operator()(const T a) { return abs(a); }
};

// Type Alias

using vabs = v_unary<abs_functor, calculation_type::vector_op>;

// Main

int C74_EXPORT main()
{
    vabs::setup<vabs>("vabs~");
}
