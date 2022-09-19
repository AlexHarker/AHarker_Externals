
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


// Functor

struct tanx_functor
{
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return tan(a.mVal); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { tan_array(o, i, size); }
};

// Type Alias

using vtanx = v_unary<tanx_functor, calculation_type::vector_array>;

// Main

int C74_EXPORT main()
{
    vtanx::setup<vtanx>("vtanx~");
}
