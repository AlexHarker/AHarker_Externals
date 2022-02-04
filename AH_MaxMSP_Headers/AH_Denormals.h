
/*
 *  AH_DENORMALS
 *
 *	This header file is used to deal with audio-rate denormals. 
 *	The perform routine here turns off denormals (on the processor) in this thread.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#ifndef _AH_DENORMALS_H_
#define _AH_DENORMALS_H_

#include <ext.h>
#include <z_dsp.h>

#include <SIMDSupport.hpp>


// Call this perform routine to turn denormals off in this thread
// The second item in the array should be the perform routine you want to call subsequently
// This routine is only called once, after which the main perform routine is called directly

t_int *denormals_perform(t_int *w)
{	
	// Replace the old MXCSR setting with the same, except set DAZ and FZ bits
	
    SIMDDenormals::off();
    
	// Swap this routine for the correct one, and then call the correct perform routine
	
	w[0] = w[1];										
	
 	return w + 1;
}

#endif /* _AH_DENORMALS_H_ */


