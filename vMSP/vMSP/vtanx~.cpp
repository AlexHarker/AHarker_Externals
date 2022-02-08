
/*
 *  vtanx~
 *
 *  vtanx~ is a vectorised version of tanx~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_unary.hpp"
#include <SIMDExtended.hpp>

struct tanx_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return tanf(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return tan(a.mVal); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { tan_array(o, i, size); }
};

typedef v_unary<tanx_functor, calculation_type::vector_array, calculation_type::vector_array> vtanx;

int C74_EXPORT main()
{
    vtanx::setup<vtanx>("vtanx~");
}
