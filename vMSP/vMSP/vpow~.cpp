
/*
 *  vpow~
 *
 *	vpow~ is a vectorised version of pow~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_binary.hpp"
#include "nans.hpp"
#include <SIMDExtended.hpp>
#include <alloca.h>
#include <algorithm>

struct pow_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a, const SIMDType<float, 1> b)
    {
        return nan_fixer()(powf(b.mVal, a.mVal));
    }
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a, const SIMDType<double, 1> b)
    {
        return nan_fixer()(pow(b.mVal, a.mVal));
    }
 
    template <class T>
    void operator()(T *o, T *i1, T *i2, long size, double val, InputType type)
    {
        switch (type)
        {
            case kScalar1:
            {
                T *t = reinterpret_cast<T *>(alloca(sizeof(T) * size));
                std::fill_n(t, size, val);
                pow_array(o, i1, t, size);
                break;
            }
                
            case kScalar2:
            {
                T *t = reinterpret_cast<T *>(alloca(sizeof(T) * size));
                std::fill_n(t, size, val);
                pow_array(o, t, i2, size);
                break;
            }
                
            case kBinary:
                pow_array(o, i1, i2, size);
                break;
        }
        nan_fixer()(o, size);
    }
    
    // Empty Implementations
   
    template <class T>
    T operator()(const T a, const T b) { return a; }
};

typedef v_binary<pow_functor, kVectorArray, kVectorArray> vpow;

int C74_EXPORT main()
{
    vpow::setup<vpow>("vpow~");
}
