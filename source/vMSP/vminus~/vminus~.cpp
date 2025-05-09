
/*
 *  vminus~
 *
 *  vminus~ is a vectorised version of minus~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "../Base/v_binary.hpp"


// Functor

struct minus_functor
{
    template <class T>
    T operator()(const T a, const T b) { return a - b; }
};

// Type Alias

using vminus = v_binary<minus_functor, calculation_type::vector_op>;

// Main

int C74_EXPORT main()
{
    vminus::setup<vminus>("vminus~");
}
