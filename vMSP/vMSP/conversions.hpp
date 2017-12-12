
#include <SIMDSupport.hpp>

static __inline void f32_mul_const_array(float *o, float *i, long length, float constant)
{
    SIMDType<float, 8> *v_o = reinterpret_cast<SIMDType<float, 8> *>(o);
    SIMDType<float, 8> *v_i = reinterpret_cast<SIMDType<float, 8> *>(i);
    SIMDType<float, 8>  v_constant(constant);
    
    // N.B. we can assume that there are an exact number of vectors and that 64 bit vectors are aligned
    
    for (; length > 0; length -= 8)
        *v_o++ =  *v_i++ * v_constant;
}

static __inline void f64_mul_const_array(double *o, double *i, long length, double constant)
{
    SIMDType<double, 4> *v_o = reinterpret_cast<SIMDType<double, 4> *>(o);
    SIMDType<double, 4> *v_i = reinterpret_cast<SIMDType<double, 4> *>(i);
    SIMDType<double, 4>  v_constant(constant);
    
    // N.B. we can assume that there are an exact number of vectors and that 64 bit vectors are aligned
    
    for (; length > 0; length -= 4)
        *v_o++ =  *v_i++ * v_constant;
}

static __inline void f32_mul_const_array(float *io, long length, float constant)
{
    f32_mul_const_array(io, io, length, constant);
}

static __inline void f64_mul_const_array(double *io, long length, double constant)
{
    f64_mul_const_array(io, io, length, constant);
}
