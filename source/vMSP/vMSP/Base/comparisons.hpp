
/*
 *  comparisons.hpp
 *
 *  A header file with helper a functor for comparison functions.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#ifndef _COMPARISONS_HPP_
#define _COMPARISONS_HPP_


// Functions for converting alls bits on to 1 for output

static htl::simd_type<double, 1> mask_one(const htl::simd_type<double, 1>& a) { return a.m_val ? 1.0 : 0.0; }

template<class T>
T mask_one(const T& a) { return a & static_cast<typename T::scalar_type>(1); }

// Generic Comparison Functor

template<typename Op>
struct comparison
{
    template <class T>
    T operator()(const T a, const T b) { return mask_one(Op()(a, b)); }
};

// Type Aliasing Helper

template <class T>
using v_binary_comparison = v_binary<comparison<T>, calculation_type::vector_op, false>;

#endif /* _COMPARISONS_HPP_ */
