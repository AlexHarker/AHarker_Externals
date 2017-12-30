
#ifndef NANS_H
#define NANS_H

#include <stdint.h>

struct nan_fixer
{
    // Empty implementation for now...
    
    template <class T>
    T operator()(const T a) { return a; }
    
    float operator()(const float a)
    {
        uint32_t a_int = *reinterpret_cast<const uint32_t *>(&a);
        return ((a_int & 0x7F800000UL) == 0x7F800000UL) && ((a_int & 0x007FFFFFUL) != 0) ? 0.f : a;
    }
    
    double operator()(const double a)
    {
        uint64_t a_int = *reinterpret_cast<const uint64_t *>(&a);
        return ((a_int & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL) && ((a_int & 0x000FFFFFFFFFFFFFULL) != 0) ? 0.0 : a;
    }
    
    void operator()(float *io, long size) {}
    void operator()(double *io, long size) {}
};
#endif /* NANS_H */
