
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
	x->num_params = 0;
	x->num_splits = 0;
	x->current_point = 0;
	
	x->lo_val = 0.;
	x->range_recip = 1.;
	x->last_phase = 1.;	
	x->force_output = 1;
}


// Set the parameters

void gesture_maker_multipart_params(t_gesture_multipart *x, short argc, t_atom *argv)
{
	long num_params = x->num_params;
	
	t_atom *params = x->params + (16 * num_params);
	long *param_count = x->param_count;
		
	if (argc && num_params < 64)
	{
		if (argc > 16)
			argc = 16;
		
		param_count[num_params] = argc;
		
		while (argc--)
			*params++ = *argv++;
		
		x->num_params = num_params + 1;
		x->force_output = 1;	
	}
}


// Set the timings at which each new inflection gesture starts

void gesture_maker_multipart_timings(t_gesture_multipart *x, short argc, t_atom *argv)
{
	long num_splits;
	double *split_points = x->split_points;
	
	if (argc > 63)
		argc = 63;
	
	num_splits = argc;
	
	while (argc--)
		*split_points++ = nextafterf(atom_getfloat(argv++), -1.);		// There may be a better way to do this that brings us closer to the desired value

	x->num_splits = num_splits;
	x->force_output = 1;
}


// This function takes in the overall gesture phase, updates the inflection gesture (if needed) and returns the inflection gesture phase

double gesture_maker_multipart_phase(t_gesture_multipart *x, t_gesture_kernel *gesture_kernel, double in_val)
{
	long num_params = x->num_params;
	long num_splits = x->num_splits;
	long current_point = x->current_point;
	long search_point = x->current_point;
	long output_point;
	
	double last_phase = x->last_phase;
	
	double *split_points = x->split_points;
	t_atom *params = x->params;
	long *param_count = x->param_count;

	// Gesture has been re-triggered, so reset to the beginning
	
	if (in_val < last_phase)
		search_point = 0;
	
	// Search forward to find the current inflection gesture
	
	for (; search_point < num_splits && in_val >= split_points[search_point]; search_point++);
	
	// Get the current values and update the inflection gesture parameters
	// Note (as stated above) that the parameters will cycle round if there are not as many sets of parameters as inflections
	
	if ((current_point != search_point || x->force_output))
	{
		if (!search_point)
			x->lo_val = 0.;
		else 
			x->lo_val = split_points[search_point - 1];
		
		if (search_point >= num_splits)
			x->range_recip = 1. / (1. - x->lo_val);
		else 
			x->range_recip = 1. / (split_points[search_point] - x->lo_val);

		if (num_params)
		{
			t_atom *output_ptr;
			output_point = search_point % num_params;
			output_ptr = params + (16 * output_point);
			gesture_maker_kernel_params(gesture_kernel, param_count[output_point], output_ptr);
		}
	}
	
	x->last_phase = in_val;
	x->current_point = search_point;
	x->force_output = 0;

	// Calculate and clip inflection phase
	
	in_val = (in_val - x->lo_val) * x->range_recip;
	
	if (in_val > 1.)
		in_val = 1.;
	if (in_val < 0.)
		in_val = 0.;
	
	return in_val;
}

