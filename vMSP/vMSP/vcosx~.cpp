
/*
 *  vcosx~
 *
 *  vcosx~ is a vectorised version of cosx~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_unary.hpp"
#include <SIMDExtended.hpp>

struct cosx_functor
{
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a) { return cos(a.mVal); }
    
    template <class T>
    void operator()(T *o, T *i, long size) { cos_array(o, i, size); }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
};

typedef v_unary<cosx_functor, calculation_type::vector_array> vcosx;

int C74_EXPORT main()
{
    vcosx::setup<vcosx>("vcosx~");
}
