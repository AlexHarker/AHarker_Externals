
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
    // N.B - the behaviour of div~ is different with some NaN inputs

    template <class T>
    T operator()(const T a, const T b) { return nan_fixer()(a / b); }
    
    double m_recip;
};

// Type Alias

using vdiv = v_binary<div_functor, calculation_type::vector_op>;

// Specialise Value In

template<>
void vdiv::value_in(double value, long inlet)
{
    m_value = nan_fixer()(value);
    m_functor.m_recip = nan_fixer()(1.0 / value);
}

// Specialise Perform Routine with LHS Signal Only (use multiply by reciprocal)

template<>
template <class T, int N, inputs Ins>
void vdiv::perform64_single1_op(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{    
    SIMDType<double, N> *in1 = reinterpret_cast<SIMDType<double, N> *>(ins[0]);
    SIMDType<double, N> *out1 = reinterpret_cast<SIMDType<double, N> *>(outs[0]);
    
    SIMDType<double, N> double_val(x->m_functor.m_recip);

    vec_size /= SIMDType<double, N>::size;

    while (vec_size--)
        *out1++ = nan_fixer()(*in1++ * double_val);
}

// Main

int C74_EXPORT main()
{
    vdiv::setup<vdiv>("vdiv~");
}
