
#ifndef _NANS_HPP_
#define _NANS_HPP_

#include <cstdint>
#include <limits>

#include "vector_loop.hpp"

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
    
    template <class T, int N>
    SIMDType<T, N> operator()(const SIMDType<T, N> a)
    {
        const SIMDType<T, N> v_inf(std::numeric_limits<T>::infinity());
        
        if (FixInfs)
            return and_not(v_inf == (a & v_inf), a);
        
        return and_not(and_not(v_inf == abs(a), v_inf == (a & v_inf)), a);
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

#endif /* _NANS_HPP_ */
