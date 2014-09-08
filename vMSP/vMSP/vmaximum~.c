
/*
 *  vmaximum~
 *
 *	vmaximum~ is a vectorised version of maximum~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <AH_VectorOps.h>


// Object and function naming

#define OBJNAME_STR "vmaximum~"
#define OBJNAME_FIRST(a) vmaximum ## a
#define OBJNAME_SECOND(a) a ## vmaximum

// Core functions (all intrinsic-based)

static __inline float maximum_scalar_32 (float in1, float in2)
{
	return (in1 > in2) ? in1 : in2;
}

static __inline double maximum_scalar_64 (double in1, double in2)
{
	return (in1 > in2) ? in1 : in2;
}

// Define operations (all intrinsic-based)

#define F32_VEC_OP F32_VEC_MAX_OP
#define F32_SCALAR_OP maximum_scalar_32

#define F64_VEC_OP F64_VEC_MAX_OP
#define F64_SCALAR_OP maximum_scalar_64

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Binary.h"

