
#ifndef _COMPARISONS_HPP_
#define _COMPARISONS_HPP_

// Functions for converting alls bits on to 1 for output

static SIMDType<float, 1> mask_one(const SIMDType<float, 1>& a) { return a.mVal ? 1.f : 0.f; }
static SIMDType<double, 1> mask_one(const SIMDType<double, 1>& a) { return a.mVal ? 1.0 : 0.0; }

template<class T>
T mask_one(const T& a) { return a & static_cast<typename T::scalar_type>(1); }

// Generic Comparison Functor

template<typename Op>
struct comparison_functor
{
    template <class T>
    T operator()(const T a, const T b) { return mask_one(Op()(a, b)); }
};

#endif /* _COMPARISONS_HPP_ */
