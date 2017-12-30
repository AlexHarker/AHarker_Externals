
#ifndef NANS_H
#define NANS_H

#include <stdint.h>

struct nan_fixer
{
    float operator()(const float a)
    {
        uint32_t a_int = *reinterpret_cast<const uint32_t *>(&a);
        return ((a_int & 0x7F800000UL) == 0x7F800000UL) && ((a_int & 0x007FFFFFUL) != 0) ? 0.f : a;
    }
    
    double operator()(const double a)
    {
        uint64_t a_int = *reinterpret_cast<const uint64_t *>(&a);
        return ((a_int & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL) && ((a_int & 0x000FFFFFFFFFFFFFULL) != 0) ? 0.0 : a;
    }
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a)       { return operator()(a.mVal); }
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a)     { return operator()(a.mVal); }
    
    template <int N>
    SIMDType<float, N> operator()(const SIMDType<float, N> a)
    {
        const static uint32_t nan_mask_32 = 0x7F800000U;
        const static uint32_t inf_mask_32 = 0x7FFFFFFFU;
        
        const SIMDType<float, N> v_nan_mask_32 = *reinterpret_cast<const float *>(&nan_mask_32);
        const SIMDType<float, N> v_inf_mask_32 = *reinterpret_cast<const float *>(&inf_mask_32);
        
        return sel(a, SIMDType<float, N>(0.f), (v_nan_mask_32 == (a & v_nan_mask_32)) & (v_nan_mask_32 != (a & v_inf_mask_32)));
    }
    
    template <int N>
    SIMDType<double, N> operator()(const SIMDType<double, N> a)
    {
        const static uint64_t nan_mask_64 = 0x7FF0000000000000U;
        const static uint64_t inf_mask_64 = 0x7FFFFFFFFFFFFFFFU;

        const SIMDType<double, N> v_nan_mask_64 = *reinterpret_cast<const double *>(&nan_mask_64);
        const SIMDType<double, N> v_inf_mask_64 = *reinterpret_cast<const double *>(&inf_mask_64);
        
        return sel(a, SIMDType<double, N>(0.0), (v_nan_mask_64 == (a & v_nan_mask_64)) & (v_nan_mask_64 != (a & v_inf_mask_64)));
    }
    
    template <class T>
    void operator()(T *io, long size)
    {
        const int simd_width = SIMDLimits<T>::max_size;
        
        SIMDType<T, simd_width> *v_io = reinterpret_cast<SIMDType<T, simd_width> *>(io);
        
        // N.B. we can assume that there are an exact number of vectors and that vectors are aligned
        
        for (; size > 0; size -= simd_width, v_io++)
            *v_io =  operator()(*v_io);
    }
};

#endif /* NANS_H */
