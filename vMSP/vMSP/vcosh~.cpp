
/*
 *  vcosh~
 *
 *  vcosh~ is a vectorised version of cosh~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "Base/v_unary.hpp"
#include <SIMDExtended.hpp>


// Functor

struct cosh_functor
{
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return cosh(a.mVal); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { cosh_array(o, i, size); }
};

// Type Alias

using vcosh = v_unary<cosh_functor, calculation_type::vector_array>;

// Main

int C74_EXPORT main()
{
    vcosh::setup<vcosh>("vcosh~");
}
