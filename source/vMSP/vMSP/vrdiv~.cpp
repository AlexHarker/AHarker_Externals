
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
    // N.B - the behaviour of rdiv~ is different with some NaN inputs

    template <class T>
    T operator()(const T a, const T b) { return nan_fixer()(b / a); }
    
    double m_recip;
};

// Type Alias

using vrdiv = v_binary<rdiv_functor, calculation_type::vector_op>;

// Specialise Value In

template<>
void vrdiv::value_in(double value, long inlet)
{
    m_value = nan_fixer()(value);
    m_functor.m_recip = nan_fixer()(1.0 / value);
}

// Specialise Perform Routine with LHS Signal Only (use multiply by reciprocal)

template<>
template <class T, int N>
void vrdiv::perform64_single2_op(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    htl::simd_type<double, N> *in2 = reinterpret_cast<htl::simd_type<double, N> *>(ins[1]);
    htl::simd_type<double, N> *out1 = reinterpret_cast<htl::simd_type<double, N> *>(outs[0]);
    
    htl::simd_type<double, N> double_val(x->m_functor.m_recip);

    vec_size /= htl::simd_type<double, N>::size;

    while (vec_size--)
        *out1++ = nan_fixer()(double_val * *in2++);
}

// Main

int C74_EXPORT main()
{
    vrdiv::setup<vrdiv>("vrdiv~");
}
