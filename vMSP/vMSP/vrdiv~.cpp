
/*
 *  vrdiv~
 *
 *	vrdiv~ is a vectorised version of rdiv~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_binary.hpp>

struct div_functor
{
    template <class T>
    T operator()(const T a, const T b) { return a / b; }
    
    // Empty Implementations
    
    void operator()(double *o, double *i1, double *i2, long size) {}
    void operator()(float *o, float *i1, float *i2, long size) {}
};

typedef v_binary<div_functor, kVectorOp, kVectorOp, true> vrdiv;

int C74_EXPORT main()
{
    vrdiv::setup<vrdiv>("vrdiv~");
}

/*
#include <AH_VectorOps.h>


// Object and function naming

#define OBJNAME_STR "vrdiv~"
#define OBJNAME_FIRST(a) vrdiv ## a
#define OBJNAME_SECOND(a) a ## vrdiv

// [The reverse of divide]

#define REVERSE

// Constants

vFloat zero_32 = {0.f, 0.f, 0.f, 0.f};

#ifdef VECTOR_F64_128BIT
vDouble zero_64 = {0.0, 0.0};
#endif

// Core functions

static __inline vFloat divide_vec_32 (vFloat a, vFloat b)
{
	return F32_VEC_SEL_OP(F32_VEC_DIV_OP(a,b), zero_32, F32_VEC_EQ_OP(b, zero_32));
}

static __inline float divide_scalar_32 (float a, float b)
{
	return !b ? 0.f : (a / b);
}

#ifdef VECTOR_F64_128BIT
static __inline vDouble divide_vec_64 (vDouble a, vDouble b)
{
	return F64_VEC_SEL_OP(F64_VEC_DIV_OP(a,b), zero_64, F64_VEC_EQ_OP(b, zero_64));
}
#endif

static __inline float divide_scalar_64 (float a, float b)
{
	return !b ? 0.f : (a / b);
}

// Define operations (all intrinsic-based)

#define F32_VEC_OP(a,b) divide_vec_32(a,b)
#define F32_SCALAR_OP(a,b) divide_scalar_32(a,b)

#define F64_VEC_OP(a,b) divide_vec_64(a,b)
#define F64_SCALAR_OP(a,b) divide_scalar_64(a,b)

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Binary.h"
*/
