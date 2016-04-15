
/*
 *  gesture_maker_convert.h
 *
 *	A header file for the value conversion section of the gesture_maker object.
 *	This code deals with converting the output of each kernel from values normalised 0 to 1 to the final output range and scaling.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>

// Internal conversion modes

typedef enum {
    
    CONVERT_NONE = 0,
    CONVERT_LINEAR = 1,
    CONVERT_LOG_IN = 2,

} t_conversion_mode;

// Conversion structure

typedef struct gesture_maker_convert
{    
	t_conversion_mode mode;
	
	double mult;
	double subtract;
	double min;
	double max;
		
} t_gesture_maker_convert;

// Function prototypes

void gesture_maker_convert_setup();
void gesture_maker_convert_init(t_gesture_maker_convert *x);
double gesture_maker_convert_scale(t_gesture_maker_convert *x, double input);
void gesture_maker_convert_params(t_gesture_maker_convert *x, long argc, t_atom *argv);