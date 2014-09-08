
/*
 *  vminimum~
 *
 *	vminimum~ is a vectorised version of minimum~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <AH_VectorOps.h>


// Object and function naming

#define OBJNAME_STR "vminimum~"
#define OBJNAME_FIRST(a) vminimum ## a
#define OBJNAME_SECOND(a) a ## vminimum

// Core functions (all intrinsic-based)

static __inline float minimum_scalar_32 (float in1, float in2)
{
	return (in1 < in2) ? in1 : in2;
}

static __inline double minimum_scalar_64 (double in1, double in2)
{
	return (in1 < in2) ? in1 : in2;
}

// Define operations (all intrinsic-based)

#define F32_VEC_OP F32_VEC_MIN_OP
#define F32_SCALAR_OP minimum_scalar_32

#define F64_VEC_OP F64_VEC_MIN_OP
#define F64_SCALAR_OP minimum_scalar_64

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Binary.h"

