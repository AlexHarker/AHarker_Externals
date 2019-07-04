
#ifndef COMPARISONS_H
#define COMPARISONS_H

static SIMDType<float, 1> mask_one(const SIMDType<float, 1>& a) { return a.mVal ? 1.f : 0.f; }
static SIMDType<double, 1> mask_one(const SIMDType<double, 1>& a) { return a.mVal ? 1.0 : 0.0; }

template<class T>
T mask_one(const T& a) { return a & static_cast<typename T::scalar_type>(1); }

template<typename Op>
struct comparison_functor
{
    template <class T>
    T operator()(const T a, const T b) { return mask_one(Op()(a, b)); }
    
    // Empty Implementations
    
    void operator()(float *o, float *i1, float *i2, long size, double val, InputType type) {}
    void operator()(double *o, double *i1, double *i2, long size, double val, InputType type) {}
};

struct equal
{
    template <class T> T operator()(const T a, const T b) { return a == b; }
};

struct not_equal
{
    template <class T> T operator()(const T a, const T b) { return a != b; }
};

struct greater
{
    template <class T> T operator()(const T a, const T b) { return a > b; }
};

struct greater_eq
{
    template <class T> T operator()(const T a, const T b) { return a >= b; }
};

struct less
{
    template <class T> T operator()(const T a, const T b) { return a < b; }
};

struct less_eq
{
    template <class T> T operator()(const T a, const T b) { return a <= b; }
};

#endif
