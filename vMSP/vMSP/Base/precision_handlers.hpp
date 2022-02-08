
/*
 *  precision_handlers.hpp
 *
 *  A header file that allows double precision objects to calculate in single precision in vMSP objects.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#ifndef _PRECISION_HANDLERS_HPP_
#define _PRECISION_HANDLERS_HPP_


// Class For Using Single Precision Array Handlers for Double Precision IO

template <typename Op>
struct unary_precision_handler
{
    // Scalar Operators
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a)
    {
        return op(a);
    }
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a)
    {
        const SIMDType<float, 1> b = op(SIMDType<float, 1>(static_cast<float>(a.mVal)));
        return SIMDType<double, 1>(b.mVal);
    }
    
    // Array Operators
    
    void operator()(float *o, float *i, long size)
    {
        op(o, i, size);
    }
    
    void operator()(double *o, double *i, long size)
    {
        float *of = reinterpret_cast<float *>(o + size / 2);
        
        for (long it = size - 1; it >= 0; it--)
            of[it] = i[it];
        
        op(of, of, size);
        
        for (long it = 0; it < size; it++)
            o[it] = of[it];
    }
  
    Op op;
};

#endif /* _PRECISION_HANDLERS_HPP_ */
