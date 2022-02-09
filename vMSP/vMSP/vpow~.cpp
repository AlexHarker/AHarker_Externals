
/*
 *  vpow~
 *
 *  vpow~ is a vectorised version of pow~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_binary.hpp"
#include "Base/nans.hpp"
#include <SIMDExtended.hpp>
#include <algorithm>

#ifdef __APPLE__
#include <alloca.h>
#else
#include <malloc.h>
#endif 

// Functor

struct pow_functor
{
    double m_exp = 1.0;
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a, const SIMDType<float, 1> b)
    {
        return nan_fixer()(powf(b.mVal, a.mVal));
    }
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a, const SIMDType<double, 1> b)
    {
        return nan_fixer()(pow(b.mVal, a.mVal));
    }
 
    template <class T>
    void operator()(T *o, T *i1, T *i2, long size, double val, inputs type)
    {
        // N.B. ordering of bases / exponents
        
        switch (type)
        {
            case inputs::none:
                std::fill_n(o, size, static_cast<T>(0));
                return;
                
            case inputs::lhs:
            {
                T *t = reinterpret_cast<T *>(alloca(sizeof(T) * size));
                std::fill_n(t, size, static_cast<T>(val));
                pow_array(o, t, i1, size);
                break;
            }
                
            case inputs::rhs:
            {
                T *t = reinterpret_cast<T *>(alloca(sizeof(T) * size));
                std::fill_n(t, size, static_cast<T>(val));
                pow_array(o, i2, t, size);
                break;
            }
                
            case inputs::both:
                pow_array(o, i2, i1, size);
                break;
        }

        nan_fixer()(o, size);
    }
};

// Type Alias

using vpow = v_binary<pow_functor, calculation_type::vector_array, calculation_type::vector_array>;

// Specialise Value In

template<>
void vpow::value_in(double value, long inlet)
{
    if (inlet)
        m_value = value;
    else
        m_functor.m_exp = value;
}

// Specialise Value Retrieval

template<>
double vpow::get_value(long inlet) const
{
    if (inlet)
        return m_value;
    else
        return m_functor.m_exp;
}

// Main

int C74_EXPORT main()
{
    vpow::setup<vpow>("vpow~");
}
