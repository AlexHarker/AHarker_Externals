
/*
 *  conversion.hpp
 *
 *  A header file with helpers for linear conversions.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#ifndef _CONVERSIONS_HPP_
#define _CONVERSIONS_HPP_

#include <simd_support.hpp>

#include "vector_loop.hpp"


// Functors

struct mul_functor
{
    template <class T>
    T operator()(const T& a, const T& mul) { return a * mul; }
};

struct mul_add_functor
{
    template <class T>
    T operator()(const T& a, const T& mul, T& add) { return a * mul + add; }
};

// Array Functions

template <class T>
void mul_const_array(T *o, const T *i, long size, T mul)
{
    htl::simd_type<T, htl::simd_limits<T>::max_size> v_mul(mul);
    
    vector_loop<mul_functor>(o, i, size, v_mul);
}

template <class T>
void mul_const_array(T *io, long size, T mul)
{
    mul_const_array(io, io, size, mul);
}

template <class T>
void mul_add_const_array(T *o, const T *i, long size, T mul, T add)
{
    htl::simd_type<T, htl::simd_limits<T>::max_size> v_mul(mul);
    htl::simd_type<T, htl::simd_limits<T>::max_size> v_add(add);

    vector_loop<mul_add_functor>(o, i, size, v_mul, v_add);
}

template <class T>
void mul_add_const_array(T *io, long size, T mul, T add)
{
    mul_add_const_array(io, io, size, mul, add);
}

#endif /* _CONVERSIONS_HPP_ */
