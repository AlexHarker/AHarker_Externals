
/*
 *  vabs~
 *
 *	vabs~ is a vectorised version of abs~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_unary.hpp>

struct abs_functor
{
    const static t_uint32 bit_mask_32 = 0x7FFFFFFFU;
    const static t_uint64 bit_mask_64 = 0x7FFFFFFFFFFFFFFFU;
    
    const float bit_mask_64_f = *(reinterpret_cast<const float *>(&bit_mask_32));
    const double bit_mask_64_d = *(reinterpret_cast<const double *>(&bit_mask_64));
    
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a)
    {
        t_uint32 temp = *(reinterpret_cast<const t_uint32 *>(&a)) & bit_mask_32;
        return *(reinterpret_cast<float *>(&temp));
    }
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a)
    {
        t_uint64 temp = *(reinterpret_cast<const t_uint64 *>(&a)) & bit_mask_64;
        return *(reinterpret_cast<double *>(&temp));
    }
                 
    template <int N>
    SIMDType<float, N> operator()(const SIMDType<float, N> a) { return a & SIMDType<float, N>(bit_mask_64_f); }
   
    template <int N>
    SIMDType<double, N> operator()(const SIMDType<double, N> a) { return a & SIMDType<double, N>(bit_mask_64_d); }
    
    // Empty Implementations
    
    void operator()(float *o, float *i, long size) {}
    void operator()(double *o, double *i, long size) {}
};

typedef v_unary<abs_functor, kVectorOp, kVectorOp> vabs;

int C74_EXPORT main()
{
    vabs::setup<vabs>("vabs~");
}
