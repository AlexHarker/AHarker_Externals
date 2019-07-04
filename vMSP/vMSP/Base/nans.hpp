
#ifndef NANS_H
#define NANS_H

#include <stdint.h>

#include "vector_loops.hpp"

// For fixing nans

template <bool FixInfs>
struct nan_and_inf_fixer
{
    float operator()(const float a)
    {
        uint32_t a_int = *reinterpret_cast<const uint32_t *>(&a);
        
        if (FixInfs)
            return ((a_int & 0x7F800000UL) == 0x7F800000UL) ? 0.f : a;
        
        return ((a_int & 0x7F800000UL) == 0x7F800000UL) && (a_int & 0x007FFFFFUL) ? 0.f : a;
    }
    
    double operator()(const double a)
    {
        uint64_t a_int = *reinterpret_cast<const uint64_t *>(&a);
        
        if (FixInfs)
            return ((a_int & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL) ? 0.0 : a;
        
        return ((a_int & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL) && (a_int & 0x000FFFFFFFFFFFFFULL) ? 0.0 : a;
    }
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a)       { return operator()(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a)     { return operator()(a.mVal); }
    
    template <int N>
    SIMDType<float, N> operator()(const SIMDType<float, N> a)
    {
        const static uint32_t inf_32 = 0x7F800000U;
        
        const SIMDType<float, N> v_inf_32 = *reinterpret_cast<const float *>(&inf_32);
        
        if (FixInfs)
            return and_not(v_inf_32 == (a & v_inf_32), a);
        
        return and_not(and_not(v_inf_32 == abs(a), v_inf_32 == (a & v_inf_32)), a);
    }
    
    template <int N>
    SIMDType<double, N> operator()(const SIMDType<double, N> a)
    {
        const static uint64_t inf_64 = 0x7FF0000000000000U;

        const SIMDType<double, N> v_inf_64 = *reinterpret_cast<const double *>(&inf_64);
        
        if (FixInfs)
            return and_not(v_inf_64 == (a & v_inf_64), a);
        
        return and_not(and_not(v_inf_64 == abs(a), v_inf_64 == (a & v_inf_64)), a);
    }
    
    struct nan_loop_fixer
    {
        template <class T>
        T operator()(const T& a, nan_and_inf_fixer& op) { return op(a); }
    };
    
    template <class T>
    void operator()(T *io, long size)
    {
        vector_loop<nan_loop_fixer>(io, io, size, *this);
    }
};

using nan_fixer = nan_and_inf_fixer<true>;

#endif /* NANS_H */
