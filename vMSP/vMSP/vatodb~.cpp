
/*
 *  vatodb~
 *
 *  vatodb~ is a vectorised version of vatodb~.
 *  The exact results from this object may vary slightly different to the native Max version in terms of accuracy.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_unary.hpp"
#include "Base/conversions.hpp"
#include "Base/nans.hpp"
#include <SIMDExtended.hpp>

struct atodb_functor
{
    static const double atodb_constant;
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a)
    {
        return a.mVal > 0.f ? nan_fixer()(20.f * log10f(a.mVal)) : -999.f;
    }
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a)
    {
        return a.mVal > 0.0 ? nan_fixer()(20.0 * log10(a.mVal)) : -999.0;
    }
    
    struct input_fixer
    {
        template <class T>
        T operator()(const T& a) { return and_not(a < T(0.0), a); }
    };
    
    template <int N>
    static SIMDType<float, N> is_negative_inf(const SIMDType<float, N>& a)
    {
        constexpr uint32_t neg_inf_32 = 0xFF800000U;
        const SIMDType<float, N> v_neg_inf_32 = *reinterpret_cast<const float *>(&neg_inf_32);
        
        return a == v_neg_inf_32;
    }
    
    template <int N>
    static SIMDType<double, N> is_negative_inf(const SIMDType<double, N>& a)
    {
        constexpr uint64_t neg_inf_64 = 0xFFF0000000000000U;
        const SIMDType<double, N> v_neg_inf_64 = *reinterpret_cast<const double *>(&neg_inf_64);
        
        return a == v_neg_inf_64;
    }
    
    struct output_fixer
    {
        template <class T>
        T operator()(const T& a) { return sel(nan_fixer()(a), T(-999), is_negative_inf(a)); }
    };
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        vector_loop<input_fixer>(o, i, size);
        log_array(o, o, size);
        mul_const_array(o, size, T(atodb_constant));
        vector_loop<output_fixer>(o, o, size);
    }
};

// Initialise constants

const double atodb_functor::atodb_constant = 20.0 / log(10.0);

typedef v_unary<atodb_functor, calculation_type::vector_array, calculation_type::vector_array> vatodb;

int C74_EXPORT main()
{
    vatodb::setup<vatodb>("vatodb~");
}
