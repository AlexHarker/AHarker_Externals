
#include <SIMDSupport.hpp>

#include "vector_loop.hpp"

struct mul_functor
{
    template <class T>
    T operator()(const T& a, const T& mul) { return a * mul; }
};

struct mul_add_functor
{
    template <class T>
    T operator()(const T& a, const T& mul, T& add) { return a * mul + add; }
};

template <class T>
static void mul_const_array(T *o, const T *i, long size, T mul)
{
    SIMDType<T, SIMDLimits<T>::max_size> v_mul(mul);
    
    vector_loop<mul_functor>(o, i, size, v_mul);
}

template <class T>
void mul_const_array(T *io, long size, T mul)
{
    mul_const_array(io, io, size, mul);
}

template <class T>
void mul_add_const_array(T *o, const T *i, long size, T mul, T add)
{
    SIMDType<T, SIMDLimits<T>::max_size> v_mul(mul);
    SIMDType<T, SIMDLimits<T>::max_size> v_add(add);

    vector_loop<mul_add_functor>(o, i, size, v_mul, v_add);
}

template <class T>
void mul_add_const_array(T *io, long size, T mul, T add)
{
    mul_add_const_array(io, io, size, mul, add);
}
