
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

// Curve Types

enum t_gesture_type
{
    GESTURE_FLAT,
    GESTURE_LINE,
    GESTURE_LINE_FLAT,
    GESTURE_FLAT_LINE,
    GESTURE_TRIANGLE_RETURN,
    GESTURE_TRIANGLE,
    GESTURE_PLATEAU_RETURN,
    GESTURE_PLATEAU,
    GESTURE_RETURN,
    GESTURE_GENERAL
};

enum t_curve_type
{
    CURVE_POWER_SIN_FORWARD,
    CURVE_POWER_SIN_REVERSE,
    CURVE_POWER_RECIP_SIN_REVERSE,
    CURVE_POWER_RECIP_SIN_FORWARD,
    CURVE_POWER_ASIN_FORWARD,
    CURVE_POWER_ASIN_REVERSE,
    CURVE_POWER_RECIP_ASIN_REVERSE,
    CURVE_POWER_RECIP_ASIN_FORWARD
};

typedef struct curve_params
{
    // Curve values
    
    double power_val;
    double scurve_val;
    enum t_curve_type curve_type;
    
} t_curve_params;

// Kernel structure

typedef struct gesture_kernel
{
	// Time Points
	
	double time1;
	double time2;
	double val1;
	double val2;
	double val3;
	double val4;
	
    // Curve values
	
    t_curve_params curve_params[3];

    // LAst value
    
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
void gesture_maker_kernel_params(t_gesture_kernel *x, long argc, t_atom *argv);
void gesture_maker_kernel_params_setup();