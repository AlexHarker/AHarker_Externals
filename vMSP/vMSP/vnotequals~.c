
/*
 *  vnotequals~
 *
 *	vnotequals~ is a vectorised version of notequals~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <AH_VectorOps.h>


// Object and function naming

#define OBJNAME_STR "vnotequals~"
#define OBJNAME_FIRST(a) vnotequals ## a
#define OBJNAME_SECOND(a) a ## vnotequals

// Define operations (all intrinsic-based)

#define F32_VEC_OP F32_VEC_NEQ_MSP_OP
#define F32_SCALAR_OP(a,b) (a != b)

#define F64_VEC_OP F64_VEC_NEQ_MSP_OP
#define F64_SCALAR_OP(a,b) (a != b)

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Binary.h"

