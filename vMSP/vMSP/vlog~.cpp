
/*
 *  vlog~
 *
 *	vlog~ is a vectorised version of log~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_binary.hpp"
#include "conversions.hpp"
#include <SIMDExtended.hpp>
#include <alloca.h>

// FIX - clipping
//#define MIN_INPUT_VAL 0.0000000001

struct log_functor
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a, const SIMDType<float, 1> b)
    {
        return static_cast<float>(logf(a.mVal) / logf(b.mVal));
    }
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a, const SIMDType<double, 1> b)
    {
        return log(a.mVal) / log(b.mVal);
    }
    
    double update_base(double base)
    {
        if (!base)
            base = M_E;
        
        if (base != m_base)
        {
            m_base = base;
            m_log_base = log(base);
            m_base_mul = 1.0 / m_log_base;
        }
        
        return m_base_mul;
    }
    
    double get_mul(double base)
    {
        update_base(base);
        return m_base_mul;
    }
    
    double get_log(double base)
    {
        update_base(base);
        return m_log_base;
    }

    template <class T>
    void operator()(T *o, T *i1, T *i2, long size, InputType type)
    {
        const int simd_width = SIMDLimits<T>::max_size;
        
        switch (type)
        {
            case kScalar1:
            {
                const T mul = static_cast<T>(get_mul(*i2));
                
                log_array(o, i1, size);
                mul_const_array(o, size, mul);
                break;
            }
                
            case kScalar2:
            {
                SIMDType<T, simd_width> v(get_log(*i1));
                SIMDType<T, simd_width> *v_io = reinterpret_cast<SIMDType<T, simd_width> *>(o);

                log_array(o, i2, size);

                // N.B. we can assume that there are an exact number of vectors and that vectors are aligned
                
                for (; size > 0; size -= simd_width, v_io++)
                    *v_io = v / *v_io;
                
                break;
            }
                
            case kBinary:
            {
                T *t = reinterpret_cast<T *>(alloca(sizeof(T) * size));
                
                log_array(t, i2, size);
                log_array(o, i1, size);
                
                SIMDType<T, simd_width> *v_io = reinterpret_cast<SIMDType<T, simd_width> *>(o);
                SIMDType<T, simd_width> *v_i2 = reinterpret_cast<SIMDType<T, simd_width> *>(t);

                // N.B. we can assume that there are an exact number of vectors and that vectors are aligned
                
                for (; size > 0; size -= simd_width, v_io++)
                    *v_io = *v_io / *v_i2++;

                break;
            }
        }
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a, const T b) { return a; }
    
    double m_base = 1.0;
    double m_log_base = 0.0;
    double m_base_mul = 0.0;
};

typedef v_binary<log_functor, kVectorArray, kVectorArray> vlog;

int C74_EXPORT main()
{
    vlog::setup<vlog>("vlog~");
}

