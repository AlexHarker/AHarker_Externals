
/*
 *  vsinx~
 *
 *	vsinx~ is a vectorised version of sinx~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <AH_VectorOpsExtended.h>
#include <AH_Win_Math.h>


// Object and function naming

#define OBJNAME_STR "vsinx~"
#define OBJNAME_FIRST(a) vsinx ## a
#define OBJNAME_SECOND(a) a ## vsinx

// Define 32 bit operations (N.B. the vector operations may in fact be undefined -dealt with below)

#define F32_VEC_OP F32_VEC_SIN_OP
#define F32_VEC_ARRAY F32_VEC_SIN_ARRAY
#define F32_SCALAR_OP sinf

// Define 64 bit operations (N.B. the vector operations may in fact be undefined -dealt with below)

#define F64_VEC_OP F64_VEC_SIN_OP
#define F64_VEC_ARRAY F64_VEC_SIN_ARRAY
#define F64_SCALAR_OP sin

// Check for the existence of 32 bit vector operations and set correct approach (by vector / array / scalar fallback)

#ifndef F32_VEC_SIN_OP
#ifdef F32_VEC_SIN_ARRAY
#define USE_F32_VEC_ARRAY
#else
#define NO_F32_SIMD
#endif
#endif

// Check for the existence of 64 bit vector operations and set correct approach (by vector / array / scalar fallback)

#ifndef F64_VEC_SIN_OP
#ifdef F64_VEC_SIN_ARRAY
#define USE_F64_VEC_ARRAY
#else
#define NO_F64_SIMD
#endif
#endif

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Unary.h"

