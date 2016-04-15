
/*
 *  gesture_maker_multipart.h
 *
 *	A header file for the multipart section of the gesture_maker object.
 *	This code deals with the multipart nature of the inflection kernel.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>

#include <AH_Types.h>

//  Multipart structure

#define MAX_NUM_KERNEL_PARAMS   16
#define MAX_NUM_KERNELS         64
#define MAX_NUM_SPLITS          63

typedef struct gesture_multipart
{
	double split_points[MAX_NUM_SPLITS];
    long num_splits;
    
    t_atom kernel_params[MAX_NUM_KERNELS * MAX_NUM_KERNEL_PARAMS];
	long kernel_param_count[MAX_NUM_KERNELS];
    long num_kernels;
    long current_kernel;
    
	double lo_val;
	double range_recip;
	double last_phase;
	
	AH_Boolean force_output;
    
} t_gesture_multipart;

// Function protypes

void gesture_maker_multipart_reset(t_gesture_multipart *x);
void gesture_maker_multipart_params(t_gesture_multipart *x, long argc, t_atom *argv);
void gesture_maker_multipart_timings(t_gesture_multipart *x, long argc, t_atom *argv);
double gesture_maker_multipart_phase(t_gesture_multipart *x, t_gesture_kernel *gesture_kernel, double in_val);