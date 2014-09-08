
/*
 *  vsqrt~
 *
 *	vsqrt~ is a vectorised version of sqrt~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <AH_VectorOpsExtended.h>
#include <AH_Win_Math.h>
#include "Helpers.h"


// Object and function naming

#define OBJNAME_STR "vsqrt~"
#define OBJNAME_FIRST(a) vsqrt ## a
#define OBJNAME_SECOND(a) a ## vsqrt

// Constants

#define SET_CONSTANTS NAN_CONSTANTS

// Core functions (all intrinsic-based)

static __inline vFloat sqrt_vec_32 (vFloat a)
{
	vFloat temp = F32_VEC_SQRT_OP(a);
	return F32_VEC_NAN_FIX_OP(temp);
}

static __inline float sqrt_scalar_32 (float a)
{
	float temp = sqrtf(a);
	return F32_SCALAR_NAN_FIX_OP(temp);
}

#ifdef VECTOR_F64_128BIT
static __inline vDouble sqrt_vec_64 (vDouble a)
{
	vDouble temp = F64_VEC_SQRT_OP(a);
	return F64_VEC_NAN_FIX_OP(temp);
}
#endif

static __inline double sqrt_scalar_64 (float a)
{
	double temp = sqrt(a);
	return F64_SCALAR_NAN_FIX_OP(temp);
}


// Define operations (all intrinsic-based)

#define F32_VEC_OP sqrt_vec_32
#define F32_SCALAR_OP sqrt_scalar_32

#define F64_VEC_OP sqrt_vec_64
#define F64_SCALAR_OP sqrt_scalar_64

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Unary.h"

