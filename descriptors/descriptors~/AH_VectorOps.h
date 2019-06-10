
/*
 *  AH_VectorOps
 *
 *	This header file provides a platform independent interface for SIMD operations (or vector operations).
 *	Code using this interface should compile for SSE or Altivec processors (and is also open to future changes in processor architecture).
 *	The list of operations here is by no means exhasutive, and is updated as needed using a consistent naming scheme.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#ifndef _AH_CROSS_PLATFORM_VECTOR_OPS_
#define _AH_CROSS_PLATFORM_VECTOR_OPS_ 

#ifdef __APPLE__

#include <Accelerate/Accelerate.h>

#define FORCE_INLINE				__attribute__ ((always_inline))
#define FORCE_INLINE_DEFINITION

#define ALIGNED_MALLOC malloc
#define ALIGNED_FREE free

#else

// Windows

#include <emmintrin.h>
#include <malloc.h>

#define FORCE_INLINE				__forceinline
#define FORCE_INLINE_DEFINITION		__forceinline;

#define ALIGNED_MALLOC(x)  _aligned_malloc(x, 16)
#define ALIGNED_FREE  _aligned_free

typedef	__m128i	vUInt8;
typedef __m128i vSInt8;
typedef	__m128i vUInt16;
typedef __m128i vSInt16;
typedef __m128i vUInt32;
typedef	__m128i vSInt32;
typedef	__m128i vBool32;
typedef __m128i vUInt64;
typedef __m128i vSInt64;
typedef	__m128  vFloat;
typedef	__m128d vDouble;

#endif

// Test for intel compilation

#ifndef TARGET_INTEL
#if defined( __i386__ ) || defined( __x86_64__ ) || defined(WIN_VERSION)
#define TARGET_INTEL
#endif
#endif

// Define for 64 bit float vector in 128bits (2 doubles)

#ifdef TARGET_INTEL
#ifndef VECTOR_F64_128BIT
#define VECTOR_F64_128BIT
#endif
#endif

// Runtime test for SSE2

static __inline int SSE2_check()
{
#ifdef __APPLE__
	return 1;
#else
	int SSE2_flag = 0;
	int CPUInfo[4] = {-1, 0, 0, 0};
	int nIds;

	__cpuid(CPUInfo, 0);
	 nIds = CPUInfo[0];

	if (nIds > 0)
	{
		__cpuid(CPUInfo, 1);
		SSE2_flag = (CPUInfo[3] >> 26) & 0x1;
	}
	
	return SSE2_flag;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////// Utility macros (non platform-specific)  //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// The standard compare operations return all bits on, but for use in MSP we probably want values of one
// These routines can be used in this case

static const vFloat Vec_Ops_F32_One = {1.,1.,1.,1.};

#define F32_VEC_EQ_MSP_OP(a,b)          F32_VEC_AND_OP(Vec_Ops_F32_One, F32_VEC_EQ_OP(a,b))
#define F32_VEC_NEQ_MSP_OP(a,b)         F32_VEC_SUB_OP(Vec_Ops_F32_One, F32_VEC_AND_OP(F32_VEC_EQ_OP(a,b), Vec_Ops_F32_One))
#define F32_VEC_GT_MSP_OP(a,b)			F32_VEC_AND_OP(Vec_Ops_F32_One, F32_VEC_GT_OP(a,b))
#define F32_VEC_LT_MSP_OP(a,b)			F32_VEC_AND_OP(Vec_Ops_F32_One, F32_VEC_LT_OP(a,b)) 


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////// Macros for platform-specific vector ops ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TARGET_INTEL

// Intel

// Comparisons that return one or zero

static const vDouble Vec_Ops_F64_One = {1.,1.};

#define F64_VEC_EQ_MSP_OP(a,b)          F64_VEC_AND_OP(Vec_Ops_F64_One, F64_VEC_EQ_OP(a,b))
#define F64_VEC_NEQ_MSP_OP(a,b)         F64_VEC_SUB_OP(Vec_Ops_F64_One, F64_VEC_AND_OP(F64_VEC_EQ_OP(a,b), Vec_Ops_F64_One))
#define F64_VEC_GT_MSP_OP(a,b)			F64_VEC_AND_OP(Vec_Ops_F64_One, F64_VEC_GT_OP(a,b))
#define F64_VEC_LT_MSP_OP(a,b)			F64_VEC_AND_OP(Vec_Ops_F64_One, F64_VEC_LT_OP(a,b)) 

// Floating point single precision (32 bit) intrinsics or local functions

#define float2vector					_mm_set1_ps

#define F32_VEC_MUL_OP					_mm_mul_ps
#define F32_VEC_DIV_OP					_mm_div_ps
#define F32_VEC_ADD_OP					_mm_add_ps
#define F32_VEC_SUB_OP					_mm_sub_ps

#define F32_VEC_AND_OP					_mm_and_ps
#define F32_VEC_ANDNOT_OP				_mm_andnot_ps
#define F32_VEC_OR_OP					_mm_or_ps
#define F32_VEC_XOR_OP					_mm_xor_ps

#define F32_VEC_SEL_OP					_mm_sel_ps

#define F32_VEC_MIN_OP					_mm_min_ps
#define F32_VEC_MAX_OP					_mm_max_ps

#define F32_VEC_EQ_OP                   _mm_cmpeq_ps
#define F32_VEC_NEQ_OP                  _mm_cmpneq_ps
#define F32_VEC_GT_OP                   _mm_cmpgt_ps
#define F32_VEC_LT_OP                   _mm_cmplt_ps

#define F32_VEC_SQRT_OP					_mm_sqrt_ps

#define F32_VEC_ULOAD					_mm_loadu_ps
#define F32_VEC_USTORE					_mm_storeu_ps
#define F32_VEC_MOVE_LO					_mm_movelh_ps
#define F32_VEC_MOVE_HI					_mm_movehl_ps
#define F32_VEC_SHUFFLE					_mm_shuffle_ps

// Conversions from and to 32 bit floating point vectors

#define F32_VEC_FROM_I32				_mm_cvtepi32_ps
#define I32_VEC_FROM_F32_ROUND			_mm_cvtps_epi32
#define I32_VEC_FROM_F32_TRUNC			_mm_cvttps_epi32

// Floating-point double precision (64 bit) intrinsics or local functions (only available under intel)

#define double2vector					_mm_set1_pd

#define F64_VEC_MUL_OP					_mm_mul_pd
#define F64_VEC_DIV_OP					_mm_div_pd
#define F64_VEC_ADD_OP					_mm_add_pd
#define F64_VEC_SUB_OP					_mm_sub_pd

#define F64_VEC_AND_OP					_mm_and_pd
#define F64_VEC_ANDNOT_OP				_mm_andnot_pd
#define F64_VEC_OR_OP					_mm_or_pd
#define F64_VEC_XOR_OP					_mm_xor_pd

#define F64_VEC_SEL_OP					_mm_sel_pd

#define F64_VEC_MIN_OP					_mm_min_pd
#define F64_VEC_MAX_OP					_mm_max_pd

#define F64_VEC_EQ_OP                   _mm_cmpeq_pd
#define F64_VEC_NEQ_OP                  _mm_cmpneq_pd
#define F64_VEC_GT_OP					_mm_cmpgt_pd
#define F64_VEC_LT_OP					_mm_cmplt_pd

#define F64_VEC_SQRT_OP					_mm_sqrt_pd

#define F64_VEC_ULOAD					_mm_loadu_pd
#define F64_VEC_USTORE					_mm_storeu_pd
#define F64_VEC_UNPACK_LO				_mm_unpacklo_pd
#define F64_VEC_UNPACK_HI				_mm_unpackhi_pd
#define F64_VEC_STORE_HI				_mm_storeh_pd
#define F64_VEC_STORE_LO				_mm_storel_pd
#define F64_VEC_SET_BOTH				_mm_set1_pd
#define F64_VEC_SHUFFLE					_mm_shuffle_pd

// Conversions from and to 64 bit floating point vectors

#define F64_VEC_FROM_F32				_mm_cvtps_pd
#define F32_VEC_FROM_F64				_mm_cvtpd_ps

#define F64_VEC_FROM_I32				_mm_cvtepi32_pd 
#define I32_VEC_FROM_F64_ROUND			_mm_cvtpd_epi32
#define I32_VEC_FROM_F64_TRUNC			_mm_cvttpd_epi32

// Integer 32 bit intrinsics

#define s32int2vector					_mm_set1_epi32

#define I32_VEC_ADD_OP					_mm_add_epi32
#define I32_VEC_SUB_OP					_mm_sub_epi32
#define I32_VEC_ADD_OP					_mm_add_epi32

#define I32_VEC_MIN_OP					_mm_min_epi32
#define I32_VEC_MAX_OP					_mm_max_epi32

#define I32_VEC_OR_OP					_mm_or_si128
#define I32_VEC_AND_OP					_mm_and_si128
#define I32_VEC_ANDNOT_OP				_mm_andnot_si128

#define I32_VEC_EQ_OP                   _mm_cmpeq_epi32
#define I32_VEC_GT_OP                   _mm_cmpgt_epi32
#define I32_VEC_LT_OP                   _mm_cmplt_epi32

#define I32_VEC_SHUFFLE                 _mm_shuffle_epi32

//#define I32_VEC_SL_OP                   _mm_sll_epi32
//#define I32_VEC_SR_OP                   _mm_srl_epi32
//#define I32_VEC_SRA_OP                  _mm_sra_epi32
#define I32_VEC_SLI_OP                  _mm_slli_epi32
#define I32_VEC_SRI_OP                  _mm_srli_epi32
#define I32_VEC_SRAI_OP                 _mm_srai_epi32

// Integer 64 intrinsics

#define I64_VEC_SUB_OP                  _mm_sub_epi64
#define I64_VEC_ADD_OP                  _mm_add_epi64

#define I64_VEC_SL_OP                   _mm_shift_left_variable_epi64
#define I64_VEC_SR_OP                   _mm_shift_right_variable_epi64
//#define I64_VEC_SL_OP                   _mm_sll_epi64
//#define I64_VEC_SR_OP                   _mm_srl_epi64
#define I64_VEC_SLI_OP                  _mm_slli_epi64
#define I64_VEC_SRI_OP                  _mm_srli_epi64

#define I64_VEC_XOR_OP                  _mm_xor_si128
#define I64_VEC_OR_OP                   _mm_or_si128
#define I64_VEC_AND_OP                  _mm_and_si128
#define I64_VEC_ANDNOT_OP               _mm_andnot_si128



// Altivec has min / max intrinics for 32 bit signed integers, but on intel this must be done in software (although it is provided under windows)
// These routines are taken directly from the apple SSE migration guide
// The guide can be found at http://developer.apple.com/legacy/mac/library/documentation/Performance/Conceptual/Accelerate_sse_migration/Accelerate_sse_migration.pdf

#if defined(__APPLE__) && (!defined(__SSE4_1__))
static __inline vSInt32 _mm_min_epi32(vSInt32 a, vSInt32 b) FORCE_INLINE;
static __inline vSInt32 _mm_min_epi32(vSInt32 a, vSInt32 b) 
{ 
	vSInt32 t = _mm_cmpgt_epi32(a,b);
	return _mm_or_si128( _mm_and_si128(t,b),_mm_andnot_si128(t,a));
}

static __inline vSInt32 _mm_max_epi32(vSInt32 a, vSInt32 b) FORCE_INLINE;
static __inline vSInt32 _mm_max_epi32(vSInt32 a, vSInt32 b) 
{ 
	vSInt32 t = _mm_cmpgt_epi32(a,b);
	return _mm_or_si128( _mm_andnot_si128(t,b),_mm_and_si128(t,a));
}
#endif 

// Altivec has selection intrinics for 32 bit floating point vectors, but on intel this must be done in software
// These routines are taken directly from the apple SSE migration guide
// The guide can be found at http://developer.apple.com/legacy/mac/library/documentation/Performance/Conceptual/Accelerate_sse_migration/Accelerate_sse_migration.pdf

#ifdef __APPLE__
static __inline vFloat _mm_sel_ps(vFloat a, vFloat b, vFloat mask) FORCE_INLINE;
static __inline vUInt32 _mm_sel_epi32(vUInt32 a, vUInt32 b, vUInt32 mask) FORCE_INLINE;
static __inline vDouble _mm_sel_pd(vDouble a, vDouble b, vDouble mask) FORCE_INLINE;
static __inline vSInt64 _mm_convert_trunc_pd_epi64(vDouble a) FORCE_INLINE;
#endif

static __inline vFloat _mm_sel_ps(vFloat a, vFloat b, vFloat mask) FORCE_INLINE_DEFINITION
{ 
    b = _mm_and_ps(b, mask); 
    a = _mm_andnot_ps(mask, a); 
    return _mm_or_ps(a, b); 
} 

static __inline vUInt32 _mm_sel_epi32(vUInt32 a, vUInt32 b, vUInt32 mask) FORCE_INLINE_DEFINITION
{ 
    b = _mm_and_si128(b, mask); 
    a = _mm_andnot_si128(mask, a); 
    return _mm_or_si128(a, b); 
} 

static __inline vDouble _mm_sel_pd(vDouble a, vDouble b, vDouble mask) FORCE_INLINE_DEFINITION
{ 
    b = _mm_and_pd(b, mask); 
    a = _mm_andnot_pd(mask, a); 
    return _mm_or_pd(a, b); 
} 


static __inline vSInt64 _mm_shift_left_variable_epi64(vSInt64 a, vSInt64 shifts)
{
    vSInt64 lo = _mm_sll_epi64(a, _mm_move_epi64(shifts));
    vSInt64 hi = _mm_sll_epi64(a, _mm_srli_si128(shifts, 0x8));
    
    return _mm_shuffle_pd(lo, hi, 0x2);
}

static __inline vSInt64 _mm_shift_right_variable_epi64(vSInt64 a, vSInt64 shifts)
{
    vSInt64 lo = _mm_srl_epi64(a, _mm_move_epi64(shifts));
    vSInt64 hi = _mm_srl_epi64(a, _mm_srli_si128(shifts, 0x8));
    
    return _mm_shuffle_pd(lo, hi, 0x2);
}


static __inline void _mm_split_pd_epi64(vDouble a, vSInt64 *whole, vDouble *fract)
{
    static const vSInt64 mant_mask = {0xFFFFFFFFFFFFF, 0xFFFFFFFFFFFFF};
    static const vSInt64 leading_bit = {0x10000000000000, 0x10000000000000};
    static const vSInt64 exp_mask = {0x7FF0000000000000, 0x7FF0000000000000};
    //static const vSInt64 max_negative = {0x8000000000000000, 0x8000000000000000};
    static const vSInt64 shift_bias = {0x43D, 0x43D};
    //static const vSInt32 shift_bias_32 = {0x43D, 0x43D, 0x43D, 0x43D};
    //static const vSInt32 min_exp = {0x3FF, 0x3FF, 0x3FF, 0x3FF};
    
    //static const vSInt64 neg_one = {-1, -1};
    //static const vSInt64 shift_bias2 = {0x433, 0x433};
    static const vSInt32 min_exp = {0x3FF, 0x3FF, 0x3FF, 0x3FF};
    //static const vSInt32 max_exp = {0x433, 0x433, 0x433, 0x433};
    
    vSInt64 expbits = I64_VEC_AND_OP(exp_mask, a);
    vSInt64 exp = I64_VEC_SRI_OP(expbits, 0x34);
    
    // Detect edge cases
    
    vSInt32 exp32 = I32_VEC_SHUFFLE(exp, 0xA0);
    //vSInt32 overflow2 = I32_VEC_GT_OP(exp32, max_exp);
    
    // Handle underflow
    
    a = I64_VEC_ANDNOT_OP(I32_VEC_LT_OP(exp32, min_exp), a);
    
    // Construct mask and apply
        
    //vSInt64 mask = I64_VEC_SL_OP(neg_one, I64_VEC_ANDNOT_OP(overflow2, I64_VEC_SUB_OP(shift_bias2, exp)));
    //a = I64_VEC_AND_OP(mask, a);
    
    // Mantissa
    
    vSInt64 mantissa = I64_VEC_OR_OP(I64_VEC_AND_OP(mant_mask, a), leading_bit);
    
    // Shift

    vSInt64 shift = I64_VEC_SUB_OP(shift_bias, exp);
    vSInt64 result = I64_VEC_SR_OP(I64_VEC_SLI_OP(mantissa, 0xA), shift);
    
    *whole = result;
    *fract = F64_VEC_SUB_OP(a, F64_VEC_OR_OP(expbits, F64_VEC_ANDNOT_OP(exp_mask, I64_VEC_SRI_OP(I64_VEC_SL_OP(result, shift), 0xA))));
    /*
     // Sign
     
     vSInt64 sign = I32_VEC_SRAI_OP(I32_VEC_SHUFFLE(a, 0xF5), 0x1F);
     result = I64_VEC_SUB_OP(I64_VEC_XOR_OP(sign, result), sign);
     
     // Handle overflow
     
     vSInt32 overflow = I32_VEC_GT_OP(exp32, shift_bias_32);
     result = I64_VEC_OR_OP(I64_VEC_AND_OP(overflow, max_negative), I64_VEC_ANDNOT_OP(overflow, result));
     */
}

/*
static __inline vSInt64 _mm_convert_trunc_pd_epi64(vDouble a)
{
    static const vSInt64 mant_mask = {0xFFFFFFFFFFFFF, 0xFFFFFFFFFFFFF};
    static const vSInt64 leading_bit = {0x10000000000000, 0x10000000000000};
    static const vSInt64 exp_mask = {0x7FF0000000000000, 0x7FF0000000000000};
    static const vSInt64 max_negative = {0x8000000000000000, 0x8000000000000000};
    static const vSInt64 shift_bias = {0x43D, 0x43D};
    static const vSInt32 shift_bias_32 = {0x43D, 0x43D, 0x43D, 0x43D};
    static const vSInt32 min_exp = {0x3FF, 0x3FF, 0x3FF, 0x3FF};
    
    // Mantissa
    
    vSInt64 mantissa = I64_VEC_OR_OP(I64_VEC_AND_OP(mant_mask, a), leading_bit);
    
    // Shift
    
    vSInt64 exp = I64_VEC_SRI_OP(I64_VEC_AND_OP(exp_mask, a), 0x34);
    vSInt64 result = I64_VEC_SR_OP(I64_VEC_SLI_OP(mantissa, 0xA), I64_VEC_SUB_OP(shift_bias, exp));
    
    // Handle underflow
    
    vSInt32 exp32 = I32_VEC_SHUFFLE(exp, 0xA0);
    result = I64_VEC_ANDNOT_OP(I32_VEC_LT_OP(exp32, min_exp), result);
    
    // Sign
    
    vSInt64 sign = I32_VEC_SRAI_OP(I32_VEC_SHUFFLE(a, 0xF5), 0x1F);
    result = I64_VEC_SUB_OP(I64_VEC_XOR_OP(sign, result), sign);
    
    // Handle overflow
    
    vSInt32 overflow = I32_VEC_GT_OP(exp32, shift_bias_32);
    result = I64_VEC_OR_OP(I64_VEC_AND_OP(overflow, max_negative), I64_VEC_ANDNOT_OP(overflow, result));
    
    return result;
}
*/
static __inline vDouble trunc_vec_64(vDouble a)
{
    static const vSInt64 neg_one = {-1, -1};
    static const vSInt64 exp_mask = {0x7FF0000000000000, 0x7FF0000000000000};
    static const vSInt64 shift_bias = {0x433, 0x433};
    static const vSInt32 min_exp = {0x3FF, 0x3FF, 0x3FF, 0x3FF};
    static const vSInt32 max_exp = {0x433, 0x433, 0x433, 0x433};
    
    vSInt64 exp = I64_VEC_SRI_OP(I64_VEC_AND_OP(exp_mask, a), 0x34);
    
    // Detect edge cases
    
    vSInt32 exp32 = I32_VEC_SHUFFLE(exp, 0xA0);
    vSInt32 underflow = I32_VEC_LT_OP(exp32, min_exp);
    vSInt32 overflow = I32_VEC_GT_OP(exp32, max_exp);
    
    // Construct mask
        
    vSInt64 mask = I64_VEC_ANDNOT_OP(underflow, I64_VEC_SL_OP(neg_one, I64_VEC_ANDNOT_OP(overflow, I64_VEC_SUB_OP(shift_bias, exp))));

    // Apply
    
    return I64_VEC_AND_OP(mask, a);
}


// FIX - temp as fuck

#define F64_VEC_TRUNC_OP            trunc_vec_64
#define I64_VEC_FROM_F64_TRUNC      _mm_convert_trunc_pd_epi64
#define F64_VEC_SPLIT_I64_F64       _mm_split_pd_epi64

/*
 // Constants
 
 vFloat v_largest_int_32;
 vFloat v_bit_mask_32;
 
 t_uint64 large_mask_64 = 0xFFFFFFFFE0000000U;
 t_uint64 bit_mask_64 = 0x7FFFFFFFFFFFFFFFU;
 t_uint32 bit_mask_32 = 0x7FFFFFFFU;
 
 float largest_int_32 = 8388608.f;
 double largest_int_64 = 4503599627370496.0;
 
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
 // FIX - check this....
 
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
 
 // N.B. large_vec has 0.5 subtracted pre conversion to avoid incorrect rounding - always integer saturated when used so results are correct
 // FIX - explain the above properly
 
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
 */



#else

#endif	/* TARGET_INTEL */

#endif	/* _AH_CROSS_PLATFORM_VECTOR_OPS_ */
