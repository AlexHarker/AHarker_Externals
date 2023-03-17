
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


// Functor

struct atodb_functor
{
    static const double atodb_constant;
    
    // IO Limiting Functors
    
    template <int N>
    static SIMDType<double, N> is_negative_inf(const SIMDType<double, N>& a)
    {
        constexpr uint64_t neg_inf_64 = 0xFFF0000000000000U;
        const SIMDType<double, N> v_neg_inf_64 = *reinterpret_cast<const double *>(&neg_inf_64);
        
        return a == v_neg_inf_64;
    }
    
    struct input_fixer
    {
        template <class T>
        T operator()(const T& a) { return and_not(a < T(0.0), a); }
    };
    
    struct output_fixer
    {
        template <class T>
        T operator()(const T& a) { return sel(nan_fixer()(a), T(-999), is_negative_inf(a)); }
    };
    
    // Ops + Array Operators

    SIMDType<double, 1> operator()(const SIMDType<double, 1> a)
    {
        return a.mVal <= 0.0 ? -999.0 : nan_fixer()(20.0 * log10(a.mVal));
    }
    
    template <class T>
    void operator()(T *o, T *i, long size)
    {
        vector_loop<input_fixer>(o, i, size);
        log_array(o, o, size);
        mul_const_array(o, size, T(atodb_constant));
        vector_loop<output_fixer>(o, o, size);
    }
};

// Initialise Constants

const double atodb_functor::atodb_constant = 20.0 / log(10.0);

// Type Alias

using vatodb = v_unary<atodb_functor, calculation_type::vector_array>;

// Main

int C74_EXPORT main()
{
    vatodb::setup<vatodb>("vatodb~");
}
