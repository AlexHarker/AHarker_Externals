
/*
 *  vcosx~
 *
 *  vcosx~ is a vectorised version of cosx~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "Base/v_unary.hpp"
#include <SIMDExtended.hpp>


// Functor

struct cosx_functor
{
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return cos(a.mVal); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { cos_array(o, i, size); }
};

// Type Alias

using vcosx = v_unary<cosx_functor, calculation_type::vector_array>;

// Main

int C74_EXPORT main()
{
    vcosx::setup<vcosx>("vcosx~");
}
