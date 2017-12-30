
#include <SIMDSupport.hpp>

template <class T>
static void mul_const_array(T *o, T *i, long length, T constant)
{
    const int simd_width = SIMDLimits<T>::max_size;
    
    SIMDType<T, simd_width> *v_o = reinterpret_cast<SIMDType<T, simd_width> *>(o);
    SIMDType<T, simd_width> *v_i = reinterpret_cast<SIMDType<T, simd_width> *>(i);
    SIMDType<T, simd_width> v_constant(constant);
    
    // N.B. we can assume that there are an exact number of vectors and that vectors are aligned

    for (; length > 0; length -= simd_width)
        *v_o++ =  *v_i++ * v_constant;
}

template <class T>
void mul_const_array(T *io, long length, T constant)
{
    mul_const_array(io, io, length, constant);
}

template <class T>
void mul_add_const_array(T *o, T *i, long length, T mul, T add)
{
    const int simd_width = SIMDLimits<T>::max_size;

    SIMDType<T, simd_width> *v_o = reinterpret_cast<SIMDType<T, simd_width> *>(o);
    SIMDType<T, simd_width> *v_i = reinterpret_cast<SIMDType<T, simd_width> *>(i);
    SIMDType<T, simd_width> v_mul(mul);
    SIMDType<T, simd_width> v_add(add);
    
    // N.B. we can assume that there are an exact number of vectors and that vectors are aligned

    for (; length > 0; length -= simd_width)
        *v_o++ =  *v_i++ * v_mul + v_add;
}

template <class T>
void mul_add_const_array(T *io, long length, T mul, T add)
{
    mul_add_const_array(io, io, length, mul, add);
}

