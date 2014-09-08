
/*
 *  AH_VectorOpsExtended
 *
 *	This header file provides an interface for extended SIMD functions (transcendentals and other functions) that depend on more than processor intrinsics
 *
 *  Copyright 2013 Alex Harker. All rights reserved.
 *
 */

#ifndef _AH_CROSS_PLATFORM_VECTOR_OPS_EXTENDED_
#define _AH_CROSS_PLATFORM_VECTOR_OPS_EXTENDED_ 

#include "AH_VectorOps.h"


#ifdef __APPLE__

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

#define F64_VEC_SIN_ARRAY(o, i, l) f64_sin_array(o, i, l)
#define F64_VEC_COS_ARRAY(o, i, l) f64_cos_array(o, i, l)
#define F64_VEC_TAN_ARRAY(o, i, l) f64_tan_array(o, i, l)
#define F64_VEC_SINH_ARRAY(o, i, l) f64_sinh_array(o, i, l)
#define F64_VEC_COSH_ARRAY(o, i, l) f64_cosh_array(o, i, l)
#define F64_VEC_TANH_ARRAY(o, i, l) f64_tanh_array(o, i, l)
#define F64_VEC_ASIN_ARRAY(o, i, l) f64_asin_array(o, i, l)
#define F64_VEC_ACOS_ARRAY(o, i, l) f64_acos_array(o, i, l)
#define F64_VEC_ATAN_ARRAY(o, i, l) f64_atan_array(o, i, l)
#define F64_VEC_ASINH_ARRAY(o, i, l) f64_asinh_array(o, i, l)
#define F64_VEC_ACOSH_ARRAY(o, i, l) f64_acosh_array(o, i, l)
#define F64_VEC_ATANH_ARRAY(o, i, l) f64_atanh_array(o, i, l)

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

#define F64_VEC_LOG_ARRAY(o, i, l) f64_log_array(o, i, l)
#define F64_VEC_EXP_ARRAY(o, i, l) f64_exp_array(o, i, l)
#define F64_VEC_POW_ARRAY(o, i1, i2, l) f64_pow_array(o, i1, i2, l)

#define F32_VEC_COS_OP					vcosf
#define F32_VEC_SIN_OP					vsinf
#define F32_VEC_TAN_OP					vtanf
#define F32_VEC_COSH_OP					vcoshf
#define F32_VEC_SINH_OP					vsinhf
#define F32_VEC_TANH_OP					vtanhf
#define F32_VEC_ACOS_OP					vacosf
#define F32_VEC_ASIN_OP					vasinf
#define F32_VEC_ATAN_OP					vatanf
#define F32_VEC_ACOSH_OP				vacoshf
#define F32_VEC_ASINH_OP				vasinhf
#define F32_VEC_ATANH_OP				vatanhf

#define F32_VEC_LOG_OP					vlogf
#define F32_VEC_EXP_OP					vexpf
#define F32_VEC_MOD_OP					vfmodf
#define F32_VEC_POW_OP					vpowf

#ifndef TARGET_INTEL
#define F32_VEC_SQRT_OP					vsqrtf
#endif

#endif

#endif	/* _AH_CROSS_PLATFORM_VECTOR_OPS_EXTENDED_ */
