
/*
 *  gesture_maker_multipart.c
 *
 *	This code deals with multipart gestures that are made up of chained kernels.
 *	Note that the inflection kernel parameters will cycle round if there are not as many sets of parameters as inflections.
 *	Note also that each inflection gesture is resolved in realtime, so if values are specified as variable, each realisation will be different, even in the case of parameter cycling.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include "gesture_maker_kernel.h"
#include "gesture_maker_multipart.h"


// Reset the multipart structure to default values

void gesture_multipart::reset()
{
	num_kernels = 0;
	num_splits = 0;
	current_kernel = 0;
	
	lo_val = 0.0;
    hi_val = 1.0;
	range_recip = 1.0;
	last_phase = 1.0;
	
    force_output = true;
    
    kernel.reset();
}

void gesture_multipart::initial(double val)
{
    kernel.initial(val);
}

void gesture_multipart::initial_specifier(t_atom *specifier)
{
    kernel.initial_specifier(specifier);
}

// Set the parameters

void gesture_multipart::params(long argc, t_atom *argv)
{
	t_atom *parameters = kernel_params + (MAX_NUM_KERNEL_PARAMS * num_kernels);
		
	if (argc && num_kernels < MAX_NUM_KERNELS)
	{
		if (argc > MAX_NUM_KERNEL_PARAMS)
			argc = MAX_NUM_KERNEL_PARAMS;
		
		kernel_param_count[num_kernels] = argc;
		
		while (argc--)
			*parameters++ = *argv++;
		
		num_kernels++;
		force_output = true;
	}
}

// Set the timings at which each new inflection gesture starts

void gesture_multipart::timings(long argc, t_atom *argv)
{
	num_splits = argc > MAX_NUM_SPLITS ? MAX_NUM_SPLITS : argc;
	
    for (long i = 0; i < num_splits; i++)
		split_points[i] = nextafterf(atom_getfloat(argv++), -1.0);		// There may be a better way to do this that brings us closer to the desired value

	force_output = true;
}

// This function takes in the overall gesture phase, updates the inflection gesture (if needed) and returns the inflection gesture phase

double gesture_multipart::operator()(double phase)
{
    // If gesture has been re-triggered, so reset to the beginning

    long search_kernel = (phase < last_phase) ? 0 : current_kernel;
	
	// Search forward to find the current inflection gesture
	
	for (; search_kernel < num_splits && phase >= split_points[search_kernel]; search_kernel++);
	
	// Get the current values and update the inflection gesture parameters
	// Note (as stated above) that the parameters will cycle round if there are not as many sets of parameters as inflections
	
	if ((current_kernel != search_kernel || force_output))
	{
        lo_val = !search_kernel ? 0.0 : split_points[search_kernel - 1];
        hi_val = (search_kernel >= num_splits) ? 1.0 : split_points[search_kernel];
		range_recip = 1.0 / (hi_val - lo_val);

		if (num_kernels)
		{
            long output_kernel = search_kernel % num_kernels;
			t_atom *output_params = kernel_params + (MAX_NUM_KERNEL_PARAMS * output_kernel);
            kernel.params(kernel_param_count[output_kernel], output_params);
		}
	}
	
	last_phase = phase;
	current_kernel = search_kernel;
	force_output = false;

	// Calculate and clip inflection phase
	
	double kernel_phase = (phase - lo_val) * range_recip;
	
	if (kernel_phase > 1.0)
        kernel_phase = 1.0;
	if (kernel_phase < 0.0)
        kernel_phase = 0.0;
	
	return kernel(kernel_phase);
}
