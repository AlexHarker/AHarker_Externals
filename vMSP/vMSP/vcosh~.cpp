
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

struct cosh_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return coshf(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return cosh(a.mVal); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { cosh_array(o, i, size); }
};

typedef v_unary<cosh_functor, calculation_type::vector_array, calculation_type::vector_array> vcosh;

int C74_EXPORT main()
{
    vcosh::setup<vcosh>("vcosh~");
}
