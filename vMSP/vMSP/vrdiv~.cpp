
/*
 *  vrdiv~
 *
 *	vrdiv~ is a vectorised version of rdiv~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_binary.hpp"
#include "nans.hpp"

struct rdiv_functor
{
    template <class T>
    T operator()(const T a, const T b)
    {
        const static T zero(static_cast<typename T::scalar_type>(0));
        
        return nan_fixer()(b / a);
    }
    
    // Empty Implementations
    
    void operator()(float *o, float *i1, float *i2, long size, double val, InputType type) {}
    void operator()(double *o, double *i1, double *i2, long size, double val, InputType type) {}
};

typedef v_binary<rdiv_functor, kVectorOp, kVectorOp> vrdiv;

int C74_EXPORT main()
{
    vrdiv::setup<vrdiv>("vrdiv~");
}
