
#ifndef SIMDEXTENDED
#define SIMDEXTENDED

#ifdef __APPLE__

#include <Accelerate/Accelerate.h>

static __inline void sin_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvsin(out, in, &pass_length);
}

static __inline void cos_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvcos(out, in, &pass_length);
}

static __inline void tan_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvtan(out, in, &pass_length);
}

static __inline void sinh_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvsinh(out, in, &pass_length);
}

static __inline void cosh_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvcosh(out, in, &pass_length);
}

static __inline void tanh_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvtanh(out, in, &pass_length);
}

static __inline void asin_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvasin(out, in, &pass_length);
}

static __inline void acos_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvacos(out, in, &pass_length);
}

static __inline void atan_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvatan(out, in, &pass_length);
}

static __inline void asinh_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvasinh(out, in, &pass_length);
}

static __inline void acosh_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvacosh(out, in, &pass_length);
}

static __inline void atanh_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvatanh(out, in, &pass_length);
}

static __inline void log_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvlog(out, in, &pass_length);
}

static __inline void exp_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvexp(out, in, &pass_length);
}

static __inline void pow_array(double *out, const double *in1, const double *in2, long length)
{
    int pass_length = length;
    vvpow(out, in1, in2, &pass_length);
}

static __inline void sin_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvsinf(out, in, &pass_length);
}

static __inline void cos_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvcosf(out, in, &pass_length);
}

static __inline void tan_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvtanf(out, in, &pass_length);
}

static __inline void sinh_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvsinhf(out, in, &pass_length);
}

static __inline void cosh_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvcoshf(out, in, &pass_length);
}

static __inline void tanh_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvtanhf(out, in, &pass_length);
}

static __inline void asin_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvasinf(out, in, &pass_length);
}

static __inline void acos_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvacosf(out, in, &pass_length);
}

static __inline void atan_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvatanf(out, in, &pass_length);
}

static __inline void asinh_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvasinhf(out, in, &pass_length);
}

static __inline void acosh_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvacoshf(out, in, &pass_length);
}

static __inline void atanh_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvatanhf(out, in, &pass_length);
}

static __inline void log_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvlogf(out, in, &pass_length);
}

static __inline void exp_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvexpf(out, in, &pass_length);
}

static __inline void pow_array(float *out, const float *in1, const float *in2, long length)
{
    int pass_length = length;
    vvpowf(out, in1, in2, &pass_length);
}

#endif

#endif  

