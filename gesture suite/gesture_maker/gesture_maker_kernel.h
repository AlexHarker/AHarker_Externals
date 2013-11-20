
/*
 *  gesture_maker_kernel.h
 *
 *	A header file for the kernel section of the gesture_maker object.
 *	This code deals with storing gestures, resolving variable parameters and calculating output values for either the main or inflection gestures.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>


// PI defines

#define PI			3.14159265358979323846
#define PI_RECIP	0.31830988618379067154


// Kernel structure

typedef struct gesture_kernel
{
	
    t_object a_obj;
	
	// Points
	
	double time1;
	double time2;
	double val1;
	double val2;
	double val3;
	double val4;
	
    // Curve values
	
    double power_val1;
	double power_val2;
	double power_val3;
	double scurve_val1;
	double scurve_val2;
	double scurve_val3;
	long curve_type1;
	long curve_type2;
	long curve_type3;
	
	double last_val;
    
} t_gesture_kernel;


// Struture to hold the parameters relating to random bands

typedef struct rand_banding_params
{
	double num_bands;
	double lo_val;
	double hi_val;
	double gauss_dev;
	double width_val;
	double end_ratio;
	
} t_band_params;


// Function protypes

void gesture_maker_kernel_reset(t_gesture_kernel *x);
double gesture_maker_kernel_calc(t_gesture_kernel *x, double in_val);
void gesture_maker_kernel_initial(t_gesture_kernel *x, double in_val);
void gesture_maker_kernel_params(t_gesture_kernel *x, short argc, t_atom *argv);
void gesture_maker_kernel_params_setup();