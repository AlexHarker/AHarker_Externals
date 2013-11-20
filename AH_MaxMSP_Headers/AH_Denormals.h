
/*
 *  AH_DENORMALS
 *
 *	This header file is used to deal with audio-rate denormals. 
 *	The perform routine here turns off denormals (on the processor) in this thread.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#ifndef _AH_DENORMALS_
#define _AH_DENORMALS_ 

#include <ext.h>
#include <z_dsp.h>


// Check for intel compiliation

#ifndef TARGET_INTEL
#if defined( __i386__ ) || defined( __x86_64__ )
#define TARGET_INTEL
#endif
#endif


// Call this perform routine to turn denormals off in this thread - the second item in the array should be the perform routine you want to call subsequently
// This routine is only called once, after which the main perform routine is called directly

t_int *denormals_perform (t_int *w)
{
	method the_method;
	
	// Replace the old MXCSR setting with the same, except set DAZ and FZ bits
	
#ifdef TARGET_INTEL
	_mm_setcsr(_mm_getcsr() | 0x8040);					
#endif
	
	// Swap this routine for the correct one, and then call the correct perform routine
	
	the_method = (method) w[1]; 
	w[0] = w[1];										
	
	return the_method(w);
}


// Under windows we cannot rely on turning denormals off on the vector unit for scalar code, so we must fix denormals manually

#ifndef __APPLE__
#define AH_FIX_DENORM_FLOAT		FIX_DENORM_FLOAT
#define AH_FIX_DENORM_DOUBLE	FIX_DENORM_DOUBLE
#else
#define AH_FIX_DENORM_FLOAT(v)	v
#define AH_FIX_DENORM_DOUBLE(v)	v
#endif

#endif		/* _AH_DENORMALS_ */


