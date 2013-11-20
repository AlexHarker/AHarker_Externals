
/*
 *  vpow~
 *
 *	vpow~ is a vectorised version of pow~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <AH_VectorOpsExtended.h>


// Object and function naming

#define OBJNAME_STR "vpow~"
#define OBJNAME_FIRST(a) vpow ## a
#define OBJNAME_SECOND(a) a ## vpow

// Define operations

#define F32_VEC_OP(a,b) F32_VEC_POW_OP(b,a)
#define F32_VEC_ARRAY(a,b,c,d) F32_VEC_POW_ARRAY(a,b,c,d)  
#define F32_SCALAR_OP(a,b) powf(b,a)

#define F64_VEC_OP(a,b) F64_VEC_POW_OP(b,a)
#define F64_VEC_ARRAY(a,b,c,d) F64_VEC_POW_ARRAY(a,b,c,d) 
#define F64_SCALAR_OP(a,b) pow(b,a)

// Check for the existence of 32 bit vector operations and set correct approach (by vector / array / scalar fallback)

#ifndef F32_VEC_POW_OP
#ifdef F32_VEC_POW_ARRAY
#define USE_F32_VEC_ARRAY
#else
#define NO_F32_SIMD
#endif
#endif

// Check for the existence of 64 bit vector operations and set correct approach (by vector / array / scalar fallback)

#ifndef F64_VEC_POW_OP
#ifdef F64_VEC_POW_ARRAY
#define USE_F64_VEC_ARRAY
#else
#define NO_F64_SIMD
#endif
#endif

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Binary.h"

