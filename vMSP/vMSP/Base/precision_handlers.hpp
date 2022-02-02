
#ifndef _PRECISION_HANDLERS_HPP_
#define _PRECISION_HANDLERS_HPP_

// These allow double precision ouputs to be created from single precision inputs

template <typename Op>
struct unary_precision_handler
{
    SIMDType<float, 1> operator()(const SIMDType<float, 1> a) { return op(a); }
    
    SIMDType<double, 1> operator()(const SIMDType<double, 1> a)
    {
        const SIMDType<float, 1> b = op(static_cast<float>(a.mVal));
        return SIMDType<double, 1>(b.mVal);
    }
    
    void operator()(float *o, float *i, long size)
    {
        op(o, i, size);
    }
    
    void operator()(double *o, double *i, long size)
    {
        float *of = reinterpret_cast<float *>(o + size / 2);
        
        for (long it = size - 1; it >= 0; it--)
            of[it] = i[it];
        
        op(of, of, size);
        
        for (long it = 0; it < size; it++)
            o[it] = of[it];
    }
    
    // Empty Implementations
    
    template <class T>
    T operator()(const T a) { return a; }
    
    Op op;
};

#endif /* _PRECISION_HANDLERS_HPP_ */
