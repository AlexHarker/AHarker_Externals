
/*
 *  gesture_maker_multipart.h
 *
 *	A header file for the multipart section of the gesture_maker object.
 *	This code deals with the multipart nature of the inflection kernel.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#ifndef _GESTURE_MULTIPART_HPP_
#define _GESTURE_MULTIPART_HPP_

#include <ext.h>

// FIX - defines

#define MAX_NUM_KERNEL_PARAMS   16L
#define MAX_NUM_KERNELS         64
#define MAX_NUM_SPLITS          63

// Multipart Class

class gesture_multipart
{
public:
    
    // Main Methods
    
    void reset();
    
    void initial(double val);
    void initial_specifier(t_atom *specifier);
    
    void params(long argc, t_atom *argv);
    void timings(long argc, t_atom *argv);
    
    double operator()(double phase);
    
private:
    
    // Variables
    
    gesture_kernel kernel;

	double split_points[MAX_NUM_SPLITS];
    long num_splits;
    
    t_atom kernel_params[MAX_NUM_KERNELS * MAX_NUM_KERNEL_PARAMS];
	long kernel_param_count[MAX_NUM_KERNELS];
    long num_kernels;
    long current_kernel;
    
	double lo_val;
    double hi_val;
	double range_recip;
	double last_phase;
	
	bool force_output;
};

#endif /* _GESTURE_MULTIPART_HPP_ */
