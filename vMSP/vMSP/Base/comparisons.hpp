
#ifndef _COMPARISONS_HPP_
#define _COMPARISONS_HPP_

static SIMDType<double, 1> mask_one(const SIMDType<double, 1>& a) { return a.mVal ? 1.0 : 0.0; }

template<class T>
T mask_one(const T& a) { return a & static_cast<typename T::scalar_type>(1); }

template<typename Op>
struct comparison_functor
{
    template <class T>
    T operator()(const T a, const T b) { return mask_one(Op()(a, b)); }
    
    // Empty Implementations
    
    void operator()(double *o, double *i1, double *i2, long size, double val, inputs type) {}
};

#endif /* _COMPARISONS_HPP_ */
