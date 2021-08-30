
/*
 *  vlog~
 *
 *  vlog~ is a vectorised version of log~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "v_binary.hpp"
#include "conversions.hpp"
#include "vector_loop.hpp"
#include <SIMDExtended.hpp>
#include <alloca.h>
#include <limits>

struct log_functor
{
    static const double min_constant;
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a, const SIMDType<double, 1> b)
    {
        return log(a.mVal > 0.0 ? a.mVal : min_constant) * update_base(b.mVal);
    }
    
    double update_base(double base)
    {
        if (!base)
            base = M_E;
        else if (base == 1.0)
            base = std::numeric_limits<double>::infinity();
        
        if (base != m_base)
        {
            m_base = base;
            m_base_mul = 1.0 / log(base);
        }
        
        return m_base_mul;
    }

    struct replace_input_functor
    {
        template <class T>
        T operator()(const T& a) { return sel(T(min_constant), a, a > T(0.0)); }
    };
    
    struct replace_base_functor
    {
        template <class T>
        T operator()(const T& a)
        {
            const T b = sel(a, T(M_E), a == T(0.0));
            return sel(b, T(std::numeric_limits<typename T::scalar_type>::infinity()), b == T(1.0));
        }
    };
    
    template <class T>
    void operator()(T *o, T *i1, T *i2, long size, double val, InputType type)
    {
        const int simd_width = SIMDLimits<T>::max_size;
        
        switch (type)
        {
            // N.B. - there is no signal/float input for log~ so if there's only one connection we take this route
                
            case kScalar1:
            case kScalar2:
            {
                const T mul = static_cast<T>(update_base(val));
                
                vector_loop<replace_input_functor>(o, i1, size);
                log_array(o, o, size);
                mul_const_array(o, size, mul);
                break;
            }
                
            case kBinary:
            {
                T *t = reinterpret_cast<T *>(alloca(sizeof(T) * size));

                // N.B. must copy i2 first before writing to the output
                
                vector_loop<replace_base_functor>(t, i2, size);
                log_array(t, t, size);
                vector_loop<replace_input_functor>(o, i1, size);
                log_array(o, o, size);

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
    double m_base_mul = 0.0;
};

// Initialise constants

const double log_functor::min_constant = 0.0000000001;

typedef v_binary<log_functor, kVectorArray, true> vlog;

int C74_EXPORT main()
{
    vlog::setup<vlog>("vlog~");
}
