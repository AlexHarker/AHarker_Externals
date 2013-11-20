

#include <AH_VectorOps.h>


// Nan Handling Array

unsigned long long zero_bits_64 = 0x000000000000000ULL;
unsigned long zero_bits_32 = 0x00000000UL;
unsigned long long nan_mask_64 = 0x7FF0000000000000ULL;
unsigned long nan_mask_32 = 0x7F800000UL;
unsigned long long inf_mask_64 = 0x7FFFFFFFFFFFFFFFULL;
unsigned long inf_mask_32 = 0x7FFFFFFFUL;

vFloat v_zero_32 = {0.f, 0.f, 0.f, 0.f};
vFloat v_nan_mask_32;
vFloat v_inf_mask_32;

#ifdef VECTOR_F64_128BIT
vDouble v_zero_64 = {0.0, 0.0};
vDouble v_nan_mask_64;
vDouble v_inf_mask_64;
#define NAN_CONSTANTS										\
v_inf_mask_64 = double2vector(*(double *)&inf_mask_64);		\
v_inf_mask_32 = float2vector(*(float *)&inf_mask_32);		\
v_nan_mask_64 = double2vector(*(double *)&nan_mask_64);		\
v_nan_mask_32 = float2vector(*(float *)&nan_mask_32);						
#else
#define NAN_CONSTANTS										\
v_inf_mask_32 = float2vector(*(float *)&inf_mask_32);		\
v_nan_mask_32 = float2vector(*(float *)&nan_mask_32);						
#endif


static __inline float nan_fix_scalar_32 (float in)
{
	unsigned long in_ulong = *((unsigned long *) (&in));
	return ((in_ulong & 0x7F800000UL) == 0x7F800000UL) && ((in_ulong & 0x007FFFFFUL) != 0) ? 0.f : in;
}


static __inline double nan_fix_scalar_64 (double in)
{
	unsigned long long in_ulong = *((unsigned long long *) (&in));
	return ((in_ulong & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL) && ((in_ulong & 0x000FFFFFFFFFFFFFULL) != 0) ? 0.0 : in;
}


#define F32_SCALAR_NAN_FIX_OP nan_fix_scalar_32
#define F32_VEC_NAN_FIX_OP(a) (F32_VEC_SEL_OP(a, v_zero_32, F32_VEC_AND_OP(F32_VEC_EQUAL_OP(v_nan_mask_32, F32_VEC_AND_OP(a, v_nan_mask_32)), F32_VEC_NEQUAL_OP(v_nan_mask_32, F32_VEC_AND_OP(a, v_inf_mask_32)))))
#define F64_SCALAR_NAN_FIX_OP nan_fix_scalar_64
#ifdef VECTOR_F64_128BIT
#define F64_VEC_NAN_FIX_OP(a) (F64_VEC_SEL_OP(a, v_zero_64, F64_VEC_AND_OP(F64_VEC_EQUAL_OP(v_nan_mask_64, F64_VEC_AND_OP(a, v_nan_mask_64)), F64_VEC_NEQUAL_OP(v_nan_mask_64, F64_VEC_AND_OP(a, v_inf_mask_64)))))
#endif


static __inline void nan_fix_array_32(float *out, float *in, long length)
{	
	vFloat *v_in = (vFloat *) in;
	vFloat *v_out = (vFloat *) out;
	
	vFloat v_temp;
	
	// N.B. we can assume that there are an exact number of vectors
	
	if (((long) in % 16) && ((long) out % 16) )
	{
		for (; length > 0; length -= 4)
		{
			v_temp = *v_in++;
			*v_out++ = F32_VEC_NAN_FIX_OP(v_temp);
		}
	}
	else 
	{
		for (; length > 0; length -= 4, in += 4, out += 4)
		{
			v_temp = F32_VEC_NAN_FIX_OP(F32_VEC_ULOAD(in));
			F32_VEC_USTORE(out, v_temp);
		}
	}
}


#ifdef VECTOR_F64_128BIT
static __inline void nan_fix_array_64(double *out, double *in, long length)
{
	vDouble *v_in = (vDouble *) in;
	vDouble *v_out = (vDouble *) out;
	
	vDouble v_temp;
	
	// N.B. we can assume that there are an exact number of vectors and that 64 bit vectors are aligned
	
	for (; length > 0; length -= 2)
	{
		v_temp = *v_in++;
		*v_out++ = F64_VEC_NAN_FIX_OP(v_temp);
	}
}
#endif


// Mul Array

static __inline void mul_const_array_32(float *out, float *in, long length, vFloat constant)
{	
	vFloat *v_in = (vFloat *) in;
	vFloat *v_out = (vFloat *) out;
	
	vFloat v_temp;
	
	// N.B. we can assume that there are an exact number of vectors
	
	if (((long) in % 16) && ((long) out % 16) )
	{
		for (; length > 0; length -= 4)
			*v_out++ = F32_VEC_MUL_OP(constant, *v_in++);
	}
	else 
	{
		for (; length > 0; length -= 4, in += 4, out += 4)
		{
			v_temp = F32_VEC_MUL_OP(constant,(F32_VEC_ULOAD(in)));
			F32_VEC_USTORE(out, v_temp);
		}
	}
}


#ifdef VECTOR_F64_128BIT
static __inline void mul_const_array_64(double *out, double *in, long length, vDouble constant)
{
	vDouble *v_in = (vDouble *) in;
	vDouble *v_out = (vDouble *) out;
		
	// N.B. we can assume that there are an exact number of vectors and that 64 bit vectors are aligned

	for (; length > 0; length -= 2)
		*v_out++ = F64_VEC_MUL_OP(constant, *v_in++);
}
#endif


// Mul Add Array

static __inline void mul_add_const_array_32(float *out, float *in, long length, vFloat mul, vFloat add)
{	
	vFloat *v_in = (vFloat *) in;
	vFloat *v_out = (vFloat *) out;
	
	vFloat v_temp;
	
	// N.B. we can assume that there are an exact number of vectors
	
	if (((long) in % 16) && ((long) out % 16) )
	{
		for (; length > 0; length -= 4)
			*v_out++ = F32_VEC_ADD_OP(add, F32_VEC_MUL_OP(mul, *v_in++));
	}
	else 
	{
		for (; length > 0; length -= 4, in += 4, out += 4)
		{
			v_temp = F32_VEC_ADD_OP(add, F32_VEC_MUL_OP(mul,(F32_VEC_ULOAD(in))));
			F32_VEC_USTORE(out, v_temp);
		}
	}
}


#ifdef VECTOR_F64_128BIT
static __inline void mul_add_const_array_64(double *out, double *in, long length, vDouble mul, vDouble add)
{
	vDouble *v_in = (vDouble *) in;
	vDouble *v_out = (vDouble *) out;
	
	// N.B. we can assume that there are an exact number of vectors and that 64 bit vectors are aligned
	
	for (; length > 0; length -= 2)
		*v_out++ = F64_VEC_ADD_OP(add, F64_VEC_MUL_OP(mul, *v_in++));
}
#endif


// Min Clip Array

static __inline void min_clip_array_32(float *out, float *in, long length, vFloat min_val)
{	
	vFloat *v_in = (vFloat *) in;
	vFloat *v_out = (vFloat *) out;
	
	vFloat v_temp;
		
	long v_length = length >> 1;
	
	// N.B. we can assume that there are an exact number of vectors
	
	if (((long) in % 16) && ((long) out % 16) )
	{
		while (v_length--)
			*v_out++ = F32_VEC_MAX_OP(min_val, *v_in++);
	}
	else 
	{
		for (; v_length > 0; v_length--, in += 4, out += 4)
		{
			v_temp = F32_VEC_MAX_OP(min_val,(F32_VEC_ULOAD(in)));
			F32_VEC_USTORE(out, v_temp);
		}
	}
}

#ifdef VECTOR_F64_128BIT
static __inline void min_clip_array_64(double *out, double *in, long length, vDouble min_val)
{
	vDouble *v_in = (vDouble *) in;
	vDouble *v_out = (vDouble *) out;
	
	long v_length = length >> 1;
	
	// N.B. we can assume that there are an exact number of vectors and that 64 bit vectors are aligned
	
	while (v_length--)
		*v_out++ = F64_VEC_MAX_OP(min_val, *v_in++);
}
#endif

