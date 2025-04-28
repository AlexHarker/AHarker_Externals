
/*
 *  vplus~
 *
 *  vplus~ is a vectorised version of plus~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "Base/v_binary.hpp"


// Functor

struct plus_functor
{
    template <class T>
    T operator()(const T a, const T b) { return a + b; }
};

// Type Alias

using vplus = v_binary<plus_functor, calculation_type::vector_op>;

// Main

int C74_EXPORT main()
{
    vplus::setup<vplus>("vplus~");
}
