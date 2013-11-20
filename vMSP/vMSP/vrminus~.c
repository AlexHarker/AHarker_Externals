
/*
 *  vrminus~
 *
 *	vrminus~ is a vectorised version of rminus~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <AH_VectorOps.h>


// Object and function naming

#define OBJNAME_STR "vrminus~"
#define OBJNAME_FIRST(a) vrminus ## a
#define OBJNAME_SECOND(a) a ## vrminus

// [The reverse of minus]

#define REVERSE

// Define operations (all intrinsic-based)

#define F32_VEC_OP F32_VEC_SUB_OP
#define F32_SCALAR_OP(a,b) (a - b)

#define F64_VEC_OP F64_VEC_SUB_OP
#define F64_SCALAR_OP(a,b) (a - b)

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Binary.h"

