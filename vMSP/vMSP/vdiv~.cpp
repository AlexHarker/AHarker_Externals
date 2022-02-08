
/*
 *  vdiv~
 *
 *  vdiv~ is a vectorised version of div~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "Base/v_binary.hpp"
#include "Base/nans.hpp"


// Functor

struct div_functor
{
    template <class T>
    T operator()(const T a, const T b)
    {
        static const T zero(static_cast<typename T::scalar_type>(0));
        
        // N.B - the exact behaviour of div~ is different when a is NaN and b is zero, but it uses a * (1/b)
        
        return sel(zero, a / b, b != zero);
    }
    
    double m_recip;
};

// Type Alias

using vdiv = v_binary<div_functor, calculation_type::vector_op>;

// Specialise Float In

template<>
template <class T>
void vdiv::float_in(T *x, double value)
{
    x->m_val = value;
    x->m_functor.m_recip = nan_fixer()(1.0 / value);
}

// Specialise Perform Routine with LHS Signal Only (use multiply by reciprocal)

template<>
template <class T, int N>
void vdiv::perform64_single1_op(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{    
    SIMDType<double, N> *in1 = reinterpret_cast<SIMDType<double, N> *>(ins[0]);
    SIMDType<double, N> *out1 = reinterpret_cast<SIMDType<double, N> *>(outs[0]);
    
    SIMDType<double, N> double_val(x->m_functor.m_recip);

    vec_size /= SIMDType<double, N>::size;

    while (vec_size--)
        *out1++ = *in1++ * double_val;
}

// Main

int C74_EXPORT main()
{
    vdiv::setup<vdiv>("vdiv~");
}
