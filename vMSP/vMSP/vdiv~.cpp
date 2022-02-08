
/*
 *  vdiv~
 *
 *  vdiv~ is a vectorised version of div~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "Base/v_binary.hpp"


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

using vdiv = v_binary<div_functor, calculation_type::vector_op, calculation_type::vector_op>;

// Specialise Float In

template<>
template <class T>
void vdiv::float_in(T *x, double value)
{
    x->m_val = value;
    x->m_functor.m_recip = 1.0 / value;
}

// Specialise Perform Routines with LHS Signal Only (use multiply by reciprocal)

template<>
template <class T, int N>
void vdiv::perform_single1_op(t_int *w)
{
    SIMDType<float, N> *in1 = reinterpret_cast< SIMDType<float, N> *>(w[2]);
    SIMDType<float, N> *out1 = reinterpret_cast< SIMDType<float, N> *>(w[4]);
    long vec_size = static_cast<long>(w[5]);
    T *x = reinterpret_cast<T *>(w[6]);
    
    SIMDType<float, N> float_val(static_cast<float>(x->m_functor.m_recip));

    vec_size /= SIMDType<float, N>::size;

    while (vec_size--)
        *out1++ = fix_denorm(*in1++ * float_val);
}

template<>
template <class T, int N>
void vdiv::perform64_single1_op(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{    
    SIMDType<double, N> *in1 = reinterpret_cast<SIMDType<double, N> *>(ins[0]);
    SIMDType<double, N> *out1 = reinterpret_cast<SIMDType<double, N> *>(outs[0]);
    
    SIMDType<double, N> double_val(x->m_functor.m_recip);

    vec_size /= SIMDType<double, N>::size;

    while (vec_size--)
        *out1++ = fix_denorm(*in1++ * double_val);
}

// Main

int C74_EXPORT main()
{
    vdiv::setup<vdiv>("vdiv~");
}
