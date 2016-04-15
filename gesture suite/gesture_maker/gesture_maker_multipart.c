
/*
 *  gesture_maker_multipart.c
 *
 *	This code deals with the multipart nature of the inflection kernel.
 *	Note that the inflection kernel parameters will cycle round if there are not as many sets of parameters as inflections.
 *	Note also that each inflection gesture is resolved in realtime, so if values are specified as variable, each realisation will be different, even in the case of parameter cycling.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#ifndef __APPLE__
#include <nextafterf.h>
#endif

#include "gesture_maker_kernel.h"
#include "gesture_maker_multipart.h"


// Reset the multipart structure to default values

void gesture_maker_multipart_reset(t_gesture_multipart *x)
{
	x->num_kernels = 0;
	x->num_splits = 0;
	x->current_kernel = 0;
	
	x->lo_val = 0.;
	x->range_recip = 1.;
	x->last_phase = 1.;	
	x->force_output = true;
}

// Set the parameters

void gesture_maker_multipart_params(t_gesture_multipart *x, long argc, t_atom *argv)
{
	long num_kernels = x->num_kernels;
	
	t_atom *kernel_params = x->kernel_params + (MAX_NUM_KERNEL_PARAMS * num_kernels);
	long *kernel_param_count = x->kernel_param_count;
		
	if (argc && num_kernels < MAX_NUM_KERNELS)
	{
		if (argc > MAX_NUM_KERNEL_PARAMS)
			argc = MAX_NUM_KERNEL_PARAMS;
		
		kernel_param_count[num_kernels] = argc;
		
		while (argc--)
			*kernel_params++ = *argv++;
		
		x->num_kernels++;
		x->force_output = true;
	}
}

// Set the timings at which each new inflection gesture starts

void gesture_maker_multipart_timings(t_gesture_multipart *x, long argc, t_atom *argv)
{
	long num_splits;
	double *split_points = x->split_points;
	
	if (argc > MAX_NUM_SPLITS)
		argc = MAX_NUM_SPLITS;
	
	num_splits = argc;
	
	while (argc--)
		*split_points++ = nextafterf(atom_getfloat(argv++), -1.);		// There may be a better way to do this that brings us closer to the desired value

	x->num_splits = num_splits;
	x->force_output = true;
}

// This function takes in the overall gesture phase, updates the inflection gesture (if needed) and returns the inflection gesture phase

double gesture_maker_multipart_phase(t_gesture_multipart *x, t_gesture_kernel *gesture_kernel, double in_val)
{
	long num_kernels = x->num_kernels;
	long num_splits = x->num_splits;
	long current_kernel = x->current_kernel;
	long search_kernel = x->current_kernel;
	
	double last_phase = x->last_phase;
	
	double *split_points = x->split_points;
	t_atom *kernel_params = x->kernel_params;
	long *kernel_param_count = x->kernel_param_count;

	// Gesture has been re-triggered, so reset to the beginning
	
	if (in_val < last_phase)
		search_kernel = 0;
	
	// Search forward to find the current inflection gesture
	
	for (; search_kernel < num_splits && in_val >= split_points[search_kernel]; search_kernel++);
	
	// Get the current values and update the inflection gesture parameters
	// Note (as stated above) that the parameters will cycle round if there are not as many sets of parameters as inflections
	
	if ((current_kernel != search_kernel || x->force_output))
	{
		if (!search_kernel)
			x->lo_val = 0.;
		else 
			x->lo_val = split_points[search_kernel - 1];
		
		if (search_kernel >= num_splits)
			x->range_recip = 1. / (1. - x->lo_val);
		else 
			x->range_recip = 1. / (split_points[search_kernel] - x->lo_val);

		if (num_kernels)
		{
            long output_kernel = search_kernel % num_kernels;
			t_atom *output_params = kernel_params + (MAX_NUM_KERNEL_PARAMS * output_kernel);
			gesture_maker_kernel_params(gesture_kernel, kernel_param_count[output_kernel], output_params);
		}
	}
	
	x->last_phase = in_val;
	x->current_kernel = search_kernel;
	x->force_output = false;

	// Calculate and clip inflection phase
	
	in_val = (in_val - x->lo_val) * x->range_recip;
	
	if (in_val > 1.0)
		in_val = 1.0;
	if (in_val < 0.0)
		in_val = 0.0;
	
	return in_val;
}

