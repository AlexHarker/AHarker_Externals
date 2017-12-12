
/*
 *  vtimes~
 *
 *	vtimes~ is a vectorised version of times~.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <v_binary.hpp>

struct times_functor
{
    template <class T>
    T operator()(const T a, const T b) { return a * b; }
    
    // Empty Implementations

    void operator()(float *o, float *i1, float *i2, long size) {}
    void operator()(double *o, double *i1, double *i2, long size) {}
};

typedef v_binary<times_functor, kVectorOp, kVectorOp> vtimes;

int C74_EXPORT main()
{
    vtimes::setup<vtimes>("vtimes~");
}


/*
// Object and function naming

#define OBJNAME_STR "vtimes~"
#define OBJNAME_FIRST(a) vtimes ## a
#define OBJNAME_SECOND(a) a ## vtimes

// Constants

#define SET_CONSTANTS NAN_CONSTANTS										

// Core functions (all intrinsic-based)

static __inline vFloat vec_times_32 (vFloat a, vFloat b)
{
	vFloat temp = F32_VEC_MUL_OP(a, b);
	return F32_VEC_NAN_FIX_OP(temp);
}

static __inline float scalar_times_32 (float a, float b)
{
	float temp = a * b;
	return F32_SCALAR_NAN_FIX_OP(temp);
}

#ifdef VECTOR_F64_128BIT
static __inline vDouble vec_times_64 (vDouble a, vDouble b)
{
	vDouble temp = F64_VEC_MUL_OP(a, b);
	return temp = F64_VEC_NAN_FIX_OP(temp);
}
#endif

static __inline double scalar_times_64 (double a, double b)
{
	double temp = a * b;
	return F64_SCALAR_NAN_FIX_OP(temp);
}


// Define operations (all intrinsic-based)

#define F32_VEC_OP vec_times_32
#define F32_SCALAR_OP scalar_times_32

// N.B. Nan fix off for max 6 for times~ (uncomment functions above and switch below to reinstate)

//#define F64_VEC_OP vec_times_64
//#define F64_SCALAR_OP scalar_times_64

#define F64_VEC_OP F64_VEC_MUL_OP
#define F64_SCALAR_OP(a, b) (a * b)

// Having defined the necessary constants and macro the bulk of the code can now be included

#include "Template_Binary.h"
*/
