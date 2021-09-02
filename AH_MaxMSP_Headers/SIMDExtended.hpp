
#ifndef SIMDEXTENDED
#define SIMDEXTENDED

#ifdef __APPLE__

#include <Accelerate/Accelerate.h>

static inline void sin_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvsin(out, in, &pass_length);
}

static inline void cos_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvcos(out, in, &pass_length);
}

static inline void tan_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvtan(out, in, &pass_length);
}

static inline void sinh_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvsinh(out, in, &pass_length);
}

static inline void cosh_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvcosh(out, in, &pass_length);
}

static inline void tanh_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvtanh(out, in, &pass_length);
}

static inline void asin_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvasin(out, in, &pass_length);
}

static inline void acos_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvacos(out, in, &pass_length);
}

static inline void atan_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvatan(out, in, &pass_length);
}

static inline void asinh_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvasinh(out, in, &pass_length);
}

static inline void acosh_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvacosh(out, in, &pass_length);
}

static inline void atanh_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvatanh(out, in, &pass_length);
}

static inline void log_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvlog(out, in, &pass_length);
}

static inline void exp_array(double *out, const double *in, long length)
{
    int pass_length = length;
    vvexp(out, in, &pass_length);
}

static inline void pow_array(double *out, const double *in1, const double *in2, long length)
{
    int pass_length = length;
    vvpow(out, in1, in2, &pass_length);
}

static inline void sin_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvsinf(out, in, &pass_length);
}

static inline void cos_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvcosf(out, in, &pass_length);
}

static inline void tan_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvtanf(out, in, &pass_length);
}

static inline void sinh_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvsinhf(out, in, &pass_length);
}

static inline void cosh_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvcoshf(out, in, &pass_length);
}

static inline void tanh_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvtanhf(out, in, &pass_length);
}

static inline void asin_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvasinf(out, in, &pass_length);
}

static inline void acos_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvacosf(out, in, &pass_length);
}

static inline void atan_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvatanf(out, in, &pass_length);
}

static inline void asinh_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvasinhf(out, in, &pass_length);
}

static inline void acosh_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvacoshf(out, in, &pass_length);
}

static inline void atanh_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvatanhf(out, in, &pass_length);
}

static inline void log_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvlogf(out, in, &pass_length);
}

static inline void exp_array(float *out, const float *in, long length)
{
    int pass_length = length;
    vvexpf(out, in, &pass_length);
}

static inline void pow_array(float *out, const float *in1, const float *in2, long length)
{
    int pass_length = length;
    vvpowf(out, in1, in2, &pass_length);
}

#else

#include <mkl_vml_functions.h>

static inline void sin_array(double *out, const double *in, long length)
{
    vdSin(length, in, out);
}

static inline void cos_array(double *out, const double *in, long length)
{
    vdCos(length, in, out);
}

static inline void tan_array(double *out, const double *in, long length)
{
    vdTan(length, in out);
}

static inline void sinh_array(double *out, const double *in, long length)
{
    vdSinh(length, in, out);
}

static inline void cosh_array(double *out, const double *in, long length)
{
    vdCosh(length, in, out);
}

static inline void tanh_array(double *out, const double *in, long length)
{
    vdTanh(length, in, out);
}

static inline void asin_array(double *out, const double *in, long length)
{
    vdAsin(length, in, out);
}

static inline void acos_array(double *out, const double *in, long length)
{
    vdAcos(length, in, out);
}

static inline void atan_array(double *out, const double *in, long length)
{
    vdAtan(length, in, out);
}

static inline void asinh_array(double *out, const double *in, long length)
{
    vdAsinh(length, in, out);
}

static inline void acosh_array(double *out, const double *in, long length)
{
    vdAcosh(length, in, out);
}

static inline void atanh_array(double *out, const double *in, long length)
{
    vdAtanh(length, in, out);
}

static inline void log_array(double *out, const double *in, long length)
{
    vdLn(length, in, out);
}

static inline void exp_array(double *out, const double *in, long length)
{
    vdExp(length, in, out);
}

static inline void pow_array(double *out, const double *in1, const double *in2, long length)
{
    vdPow(length, in1, in2, out);
}

static inline void sin_array(float *out, const float *in, long length)
{
    vsSin(length, in, out);
}

static inline void cos_array(float *out, const float *in, long length)
{
    vsCos(length, in, out);;
}

static inline void tan_array(float *out, const float *in, long length)
{
    vsTan(length, in, out);
}

static inline void sinh_array(float *out, const float *in, long length)
{
    vsSinh(length, in, out);
}

static inline void cosh_array(float *out, const float *in, long length)
{
    vsCosh(length, in, out);
}

static inline void tanh_array(float *out, const float *in, long length)
{
    vsTanh(length, in, out);
}

static inline void asin_array(float *out, const float *in, long length)
{
    vsAsin(length, in, out);
}

static inline void acos_array(float *out, const float *in, long length)
{
    vsAcos(length, in, out);
}

static inline void atan_array(float *out, const float *in, long length)
{
    vsAtan(length, in, out);
}

static inline void asinh_array(float *out, const float *in, long length)
{
    vsAsinh(length, in, out);
}

static inline void acosh_array(float *out, const float *in, long length)
{
    vsAcosh(length, in, out);
}

static inline void atanh_array(float *out, const float *in, long length)
{
    vsAtanh(length, in, out);
}

static inline void log_array(float *out, const float *in, long length)
{
    vsLog(length, in, out);
}

static inline void exp_array(float *out, const float *in, long length)
{
    vsExp(length, in, out);
}

static inline void pow_array(float *out, const float *in1, const float *in2, long length)
{
    vsPow(length, in1, in2, out);
}

#endif

#endif  

