
#ifndef NANS_H
#define NANS_H

#include <stdint.h>

struct nan_fixer
{
    float operator()(const float a)
    {
        uint32_t a_int = *reinterpret_cast<const uint32_t *>(&a);
        return ((a_int & 0x7F800000UL) == 0x7F800000UL) && (a_int & 0x007FFFFFUL) ? 0.f : a;
    }
    
    double operator()(const double a)
    {
        uint64_t a_int = *reinterpret_cast<const uint64_t *>(&a);
        return ((a_int & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL) && (a_int & 0x000FFFFFFFFFFFFFULL) ? 0.0 : a;
    }
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a)       { return operator()(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a)     { return operator()(a.mVal); }
    
    template <int N>
    SIMDType<float, N> operator()(const SIMDType<float, N> a)
    {
        const static uint32_t inf_32 = 0x7F800000U;
        
        const SIMDType<float, N> v_inf_32 = *reinterpret_cast<const float *>(&inf_32);
        
        return and_not((v_inf_32 == (a & v_inf_32)) & (v_inf_32 != abs(a)), a);
    }
    
    template <int N>
    SIMDType<double, N> operator()(const SIMDType<double, N> a)
    {
        const static uint64_t inf_64 = 0x7FF0000000000000U;

        const SIMDType<double, N> v_inf_64 = *reinterpret_cast<const double *>(&inf_64);
        
        return and_not((v_inf_64 == (a & v_inf_64)) & (v_inf_64 != abs(a)), a);
    }
    
    template <class T>
    void operator()(T *io, long size)
    {
        const int simd_width = SIMDLimits<T>::max_size;
        
        SIMDType<T, simd_width> *v_io = reinterpret_cast<SIMDType<T, simd_width> *>(io);
        
        // N.B. we can assume that there are an exact number of vectors and that vectors are aligned
        
        for (; size > 0; size -= simd_width, v_io++)
            *v_io = operator()(*v_io);
    }
};

#endif /* NANS_H */
