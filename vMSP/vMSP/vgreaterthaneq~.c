
/*
 *  vgreaterthaneq~
 *
 *	vgreaterthaneq~ is a vectorised version of greaterthaneq~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <AH_VectorOps.h>


// Object and function naming

#define OBJNAME_STR "vgreaterthaneq~"
#define OBJNAME_FIRST(a) vgreaterthaneq ## a
#define OBJNAME_SECOND(a) a ## vgreaterthaneq

// Constants

vFloat one_32 = {1.f, 1.f, 1.f, 1.f};
#ifdef VECTOR_F64_128BIT
vDouble one_64 = {1.0, 1.0};
#endif

// Define operations (all intrinsic-based)

#define F32_VEC_OP(a,b) F32_VEC_SUB_OP(one_32, F32_VEC_LT_MSP_OP(a,b)) 
#define F32_SCALAR_OP(a,b) (a >= b)

#define F64_VEC_OP(a,b) F64_VEC_SUB_OP(one_64, F64_VEC_LT_MSP_OP(a,b)) 
#define F64_SCALAR_OP(a,b) (a >= b)

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Binary.h"

