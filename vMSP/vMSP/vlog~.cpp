
/*
 *  vlog~
 *
 *  vlog~ is a vectorised version of log~.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include "Base/v_binary.hpp"
#include "Base/conversions.hpp"
#include "Base/vector_loop.hpp"
#include <SIMDExtended.hpp>
#include <limits>

#ifdef __APPLE__
#include <alloca.h>
#else
#include <malloc.h>
#endif 


struct log_functor
{
    static constexpr double min_constant = 0.0000000001;
    
    // IO Limiting Functors

    struct replace_input_functor
    {
        template <class T>
        T operator()(const T& a)
        {
            using scalar = typename T::scalar_type;
            const scalar min = static_cast<scalar>(min_constant);
            return sel(T(min), a, a > T(scalar(0)));
        }
    };
    
    struct replace_base_functor
    {
        template <class T>
        T operator()(const T& a)
        {
            using scalar = typename T::scalar_type;
            const scalar e = static_cast<scalar>(M_E);
            const T b = sel(a, T(e), a == T(scalar(0)));
            return sel(b, T(std::numeric_limits<scalar>::infinity()), b == T(1.0));
        }
    };
    
    // Base Update
    
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
    
    // Ops + Array Operators

    SIMDType<float, 1> operator()(const SIMDType<float, 1> a, const SIMDType<float, 1> b)
    {
        float min_constant_f = static_cast<float>(min_constant);
        return logf(a.mVal > 0.0 ? a.mVal : min_constant_f) * static_cast<float>(update_base(b.mVal));
    }
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a, const SIMDType<double, 1> b)
    {
        return log(a.mVal > 0.0 ? a.mVal : min_constant) * update_base(b.mVal);
    }
    
    template <class T>
    void operator()(T *o, T *i1, T *i2, long size, double val, inputs type)
    {
        constexpr int simd_width = SIMDLimits<T>::max_size;
        
        switch (type)
        {
            // N.B. - the lhs input is always a signal, so if there's less than twp connections we take this route
                
            case inputs::none:
            case inputs::lhs:
            case inputs::rhs:
            {
                const T mul = static_cast<T>(update_base(val));
                
                vector_loop<replace_input_functor>(o, i1, size);
                log_array(o, o, size);
                mul_const_array(o, size, mul);
                break;
            }
                
            case inputs::both:
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
        
    double m_base = 1.0;
    double m_base_mul = 0.0;
};

// Type Alias

using vlog = v_binary<log_functor, calculation_type::vector_array, calculation_type::vector_array, true, true>;

// Main

int C74_EXPORT main()
{
    vlog::setup<vlog>("vlog~");
}
