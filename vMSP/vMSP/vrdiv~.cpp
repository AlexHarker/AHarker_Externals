
/*
 *  vrdiv~
 *
 *  vrdiv~ is a vectorised version of rdiv~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "Base/v_binary.hpp"
#include "Base/nans.hpp"


// Functor

struct rdiv_functor
{
    template <class T>
    T operator()(const T a, const T b) { return nan_fixer()(b / a); }
    
    double m_recip;
};

// Type Alias

using vrdiv = v_binary<rdiv_functor, calculation_type::vector_op, calculation_type::vector_op>;

// Specialise Float In

template<>
template <class T>
void vrdiv::float_in(T *x, double value)
{
    x->m_val = value;
    x->m_functor.m_recip = nan_fixer()(1.0 / value);
}

// Specialise Perform Routines with LHS Signal Only (use multiply by reciprocal)

template<>
template <class T, int N>
void vrdiv::perform_single2_op(t_int *w)
{
    SIMDType<float, N> *in2 = reinterpret_cast< SIMDType<float, N> *>(w[3]);
    SIMDType<float, N> *out1 = reinterpret_cast< SIMDType<float, N> *>(w[4]);
    long vec_size = static_cast<long>(w[5]);
    T *x = reinterpret_cast<T *>(w[6]);
    
    SIMDType<float, N> float_val(static_cast<float>(x->m_functor.m_recip));

    vec_size /= SIMDType<float, N>::size;

    while (vec_size--)
        *out1++ = fix_denorm(float_val * *in2++);
}

template<>
template <class T, int N>
void vrdiv::perform64_single2_op(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    SIMDType<double, N> *in2 = reinterpret_cast<SIMDType<double, N> *>(ins[1]);
    SIMDType<double, N> *out1 = reinterpret_cast<SIMDType<double, N> *>(outs[0]);
    
    SIMDType<double, N> double_val(x->m_functor.m_recip);

    vec_size /= SIMDType<double, N>::size;

    while (vec_size--)
        *out1++ = fix_denorm(double_val * *in2++);
}

// Main

int C74_EXPORT main()
{
    vrdiv::setup<vrdiv>("vrdiv~");
}
