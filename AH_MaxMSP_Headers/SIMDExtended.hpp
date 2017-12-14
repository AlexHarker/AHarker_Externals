
#ifndef SIMDEXTENDED
#define SIMDEXTENDED

#ifdef __APPLE__

#include <Accelerate/Accelerate.h>

static __inline void f64_sin_array(double *out, double *in, long length)
{
    int pass_length = length;
    vvsin(out, in, &pass_length);
}

static __inline void f64_cos_array(double *out, double *in, long length)
{
    int pass_length = length;
    vvcos(out, in, &pass_length);
}

static __inline void f64_tan_array(double *out, double *in, long length)
{
    int pass_length = length;
    vvtan(out, in, &pass_length);
}

static __inline void f64_sinh_array(double *out, double *in, long length)
{
    int pass_length = length;
    vvsinh(out, in, &pass_length);
}

static __inline void f64_cosh_array(double *out, double *in, long length)
{
    int pass_length = length;
    vvcosh(out, in, &pass_length);
}

static __inline void f64_tanh_array(double *out, double *in, long length)
{
    int pass_length = length;
    vvtanh(out, in, &pass_length);
}

static __inline void f64_asin_array(double *out, double *in, long length)
{
    int pass_length = length;
    vvasin(out, in, &pass_length);
}

static __inline void f64_acos_array(double *out, double *in, long length)
{
    int pass_length = length;
    vvacos(out, in, &pass_length);
}

static __inline void f64_atan_array(double *out, double *in, long length)
{
    int pass_length = length;
    vvatan(out, in, &pass_length);
}

static __inline void f64_asinh_array(double *out, double *in, long length)
{
    int pass_length = length;
    vvasinh(out, in, &pass_length);
}

static __inline void f64_acosh_array(double *out, double *in, long length)
{
    int pass_length = length;
    vvacosh(out, in, &pass_length);
}

static __inline void f64_atanh_array(double *out, double *in, long length)
{
    int pass_length = length;
    vvatanh(out, in, &pass_length);
}

static __inline void f64_log_array(double *out, double *in, long length)
{
    int pass_length = length;
    vvlog(out, in, &pass_length);
}

static __inline void f64_exp_array(double *out, double *in, long length)
{
    int pass_length = length;
    vvexp(out, in, &pass_length);
}

static __inline void f64_pow_array(double *out, double *in1, double *in2, long length)
{
    int pass_length = length;
    vvpow(out, in1, in2, &pass_length);
}

static __inline void f32_sin_array(float *out, float *in, long length)
{
    int pass_length = length;
    vvsinf(out, in, &pass_length);
}

static __inline void f32_cos_array(float *out, float *in, long length)
{
    int pass_length = length;
    vvcosf(out, in, &pass_length);
}

static __inline void f32_tan_array(float *out, float *in, long length)
{
    int pass_length = length;
    vvtanf(out, in, &pass_length);
}

static __inline void f32_sinh_array(float *out, float *in, long length)
{
    int pass_length = length;
    vvsinhf(out, in, &pass_length);
}

static __inline void f32_cosh_array(float *out, float *in, long length)
{
    int pass_length = length;
    vvcoshf(out, in, &pass_length);
}

static __inline void f32_tanh_array(float *out, float *in, long length)
{
    int pass_length = length;
    vvtanhf(out, in, &pass_length);
}

static __inline void f32_asin_array(float *out, float *in, long length)
{
    int pass_length = length;
    vvasinf(out, in, &pass_length);
}

static __inline void f32_acos_array(float *out, float *in, long length)
{
    int pass_length = length;
    vvacosf(out, in, &pass_length);
}

static __inline void f32_atan_array(float *out, float *in, long length)
{
    int pass_length = length;
    vvatanf(out, in, &pass_length);
}

static __inline void f32_asinh_array(float *out, float *in, long length)
{
    int pass_length = length;
    vvasinhf(out, in, &pass_length);
}

static __inline void f32_acosh_array(float *out, float *in, long length)
{
    int pass_length = length;
    vvacoshf(out, in, &pass_length);
}

static __inline void f32_atanh_array(float *out, float *in, long length)
{
    int pass_length = length;
    vvatanhf(out, in, &pass_length);
}

static __inline void f32_log_array(float *out, float *in, long length)
{
    int pass_length = length;
    vvlogf(out, in, &pass_length);
}

static __inline void f32_exp_array(float *out, float *in, long length)
{
    int pass_length = length;
    vvexpf(out, in, &pass_length);
}

static __inline void f32_pow_array(float *out, float *in1, float *in2, long length)
{
    int pass_length = length;
    vvpowf(out, in1, in2, &pass_length);
}

#endif

#endif  

