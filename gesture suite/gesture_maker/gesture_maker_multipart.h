
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


//  Multipart structure

typedef struct gesture_multipart
{
	long num_params;
	long num_splits;
	long current_point;
	
	double split_points[63];
	t_atom params[64 * 16];
	long param_count[64];
	
	double lo_val;
	double range_recip;
	double last_phase;
	
	long force_output;
    
} t_gesture_multipart;


// Function protypes

void gesture_maker_multipart_reset(t_gesture_multipart *x);
void gesture_maker_multipart_params(t_gesture_multipart *x, short argc, t_atom *argv);
void gesture_maker_multipart_timings(t_gesture_multipart *x, short argc, t_atom *argv);
double gesture_maker_multipart_phase(t_gesture_multipart *x, t_gesture_kernel *gesture_kernel, double in_val);