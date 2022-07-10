
/*
 *  nans.hpp
 *
 *  A header file for dealing with nans in vMSP objects.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#ifndef _NANS_HPP_
#define _NANS_HPP_

#include <cstdint>
#include <limits>

#include "vector_loop.hpp"


// Struct For Fixing Nans

template <bool FixInfs>
struct nan_and_inf_fixer
{
    // Raw Scalar Operators for Fixing Nans (+ Infs)
    
    double operator()(const double a)
    {
        uint64_t a_int = *reinterpret_cast<const uint64_t *>(&a);
        
        if (FixInfs)
            return ((a_int & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL) ? 0.0 : a;
        
        return ((a_int & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL) && (a_int & 0x000FFFFFFFFFFFFFULL) ? 0.0 : a;
    }
    
    // Scalar Fixing Operators
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a)     { return operator()(a.mVal); }
    
    // Vector Fixing Operators
    
    template <class T, int N>
    SIMDType<T, N> operator()(const SIMDType<T, N> a)
    {
        const SIMDType<T, N> v_inf(std::numeric_limits<T>::infinity());
        
        if (FixInfs)
            return and_not(v_inf == (a & v_inf), a);
        
        return and_not(and_not(v_inf == abs(a), v_inf == (a & v_inf)), a);
    }
    
    // Array Fixer
    
    template <class T>
    void operator()(T *io, long size)
    {
        vector_loop<nan_and_inf_fixer>(io, io, size);
    }
};

// Type Alias

using nan_fixer = nan_and_inf_fixer<true>;

#endif /* _NANS_HPP_ */
