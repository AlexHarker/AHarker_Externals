
/*
 *  vtrunc~
 *
 *	vtrunc~ is a vectorised version of trunc~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <AH_VectorOps.h>
#include "ext.h"


// Object and function naming

#define OBJNAME_STR "vtrunc~"
#define OBJNAME_FIRST(a) vtrunc ## a
#define OBJNAME_SECOND(a) a ## vtrunc

// Constants

vFloat v_largest_int_32; 
vFloat v_bit_mask_32;

t_uint64 large_mask_64 = 0xFFFFFFFFE0000000U;
t_uint64 bit_mask_64 = 0x7FFFFFFFFFFFFFFFU;
t_uint32 bit_mask_32 = 0x7FFFFFFFU;

float largest_int_32 = 8388608.f;
double largest_int_64 = 4503599627370496.0;

#ifdef VECTOR_F64_128BIT
vDouble v_large_mask_64;
vDouble v_bit_mask_64;
vDouble v_intermediate_int_64;
vDouble v_largest_int_64;
#define SET_CONSTANTS											\
v_large_mask_64 = double2vector(*(double *)&large_mask_64);     \
v_bit_mask_64 = double2vector(*(double *)&bit_mask_64);			\
v_bit_mask_32 = float2vector(*(float *)&bit_mask_32);			\
v_largest_int_64 = double2vector(largest_int_64);				\
v_intermediate_int_64 = double2vector((double) largest_int_32);	\
v_largest_int_32 = float2vector(largest_int_32);
#else
#define SET_CONSTANTS											\
v_bit_mask_32 = float2vector(*(float *)&bit_mask_32);			\
v_largest_int_32 = float2vector(largest_int_32);
#endif 

// Core functions

static __inline float trunc_scalar_32(float in)
{
	t_uint32 abs_temp = (*(t_uint32 *) &in) & 0x7FFFFFFFUL;
	float abs_float = *(float *) &abs_temp;
	t_int32 trunc_temp = (t_int32) in;
	return (abs_float < largest_int_32) ? (float) trunc_temp : in;
}

static __inline double trunc_scalar_64(double in)
{
	t_uint64 abs_temp = (*(t_uint64 *) &in) & 0x7FFFFFFF;
	double abs_double = *(double *) &abs_temp;
	t_int64 trunc_temp = (t_int64) in;
	return (abs_double < largest_int_64) ? (double) trunc_temp : in;
}

static __inline vFloat trunc_vec_32(vFloat in)
{
	vFloat trunc_vec= F32_VEC_FROM_I32(I32_VEC_FROM_F32_TRUNC(in));
	return F32_VEC_SEL_OP(trunc_vec, in, F32_VEC_GT_OP(F32_VEC_AND_OP(in, v_bit_mask_32), v_largest_int_32));
}

#ifdef VECTOR_F64_128BIT

// N.B. large_vec has 0.5 subtracted pre conversion to avoid incorrect rounding - always integer saturated when used so results are correct

static __inline vDouble trunc_vec_64(vDouble in)
{
	vDouble abs_vec = F64_VEC_AND_OP(in, v_bit_mask_64);
	vDouble large_vec = F64_VEC_AND_OP(in, v_large_mask_64);
	vDouble trunc_vec1 = F64_VEC_FROM_I32(I32_VEC_FROM_F64_TRUNC(in));
	vDouble trunc_vec2 = F64_VEC_ADD_OP(F64_VEC_FROM_I32(I32_VEC_FROM_F64_TRUNC(F64_VEC_SUB_OP(in, large_vec))), large_vec);
	vDouble trunc_vec = F64_VEC_SEL_OP(trunc_vec1, trunc_vec2, F64_VEC_GT_OP(abs_vec, v_intermediate_int_64));
	return F64_VEC_SEL_OP(trunc_vec, in, F64_VEC_GT_OP(abs_vec, v_largest_int_64));
}
#endif

// Define operations (all intrinsic-based)

#define F32_VEC_OP trunc_vec_32
#define F32_SCALAR_OP(a) trunc_scalar_32(a)

#define F64_VEC_OP trunc_vec_64
#define F64_SCALAR_OP(a) trunc_scalar_64(a)

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Unary.h"
