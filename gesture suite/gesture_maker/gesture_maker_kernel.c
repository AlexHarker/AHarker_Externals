
/*
 *  gesture_maker_kernel.c
 *
 *	This code deals with storing gestures, resolving variable parameters and calculating output values for either the main or inflection gestures.
 *	A kernel has up to 3 parts, each of which is first calculated as curve between the given output parameters.
 *	Kernel parameters may either be specified directly, or with a certain degree of "random" variability.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
    
#include "gesture_maker_kernel.h"
#include <AH_Random.h>

// Symbols

t_symbol *ps_flat;
t_symbol *ps_line;
t_symbol *ps_line_flat;
t_symbol *ps_flat_line;
t_symbol *ps_triangle_return;
t_symbol *ps_triangle;
t_symbol *ps_plateau_return;
t_symbol *ps_plateau;
t_symbol *ps_return;
t_symbol *ps_general;

t_symbol *ps_last;

// Random number generators

t_rand_gen kernel_param_rand_obj;
t_rand_gen *kernel_param_rand;

// Reset the kernel parameters to defaults

void gesture_maker_curve_reset(t_curve_params *x)
{
    x->power_val = 1.0;
    x->scurve_val = 0.0;
    x->curve_type = CURVE_POWER_SIN_FORWARD;
}

void gesture_maker_kernel_reset(t_gesture_kernel *x)
{	
	x->time1 = 0.;
	x->time2 = 1.;
	x->val1 = 0.;
	x->val2 = 0.;
	x->val3 = 1.;
	x->val4 = 1.;
    gesture_maker_curve_reset(x->curve_params + 0);
    gesture_maker_curve_reset(x->curve_params + 1);
    gesture_maker_curve_reset(x->curve_params + 2);
	x->last_val = 0.;
}

// Apply curvature to one of the three parts of the gesture kernel

double gesture_maker_kernel_curve(double in_val, t_curve_params params)
{
	double output = in_val;
	double power_curve, scurve;
    double power_val = params.power_val;
    double scurve_val = params.scurve_val;
	double power_val_recip = 1.0 / power_val;
		
	// Calculate the curved value
	// There are 3 basic mode options, each with two choices 
	//
	// 1 - use power curvature using power val, or reciprocal  
	// 2 - use s curvature based on sin calculation or based asin calculation
	// 3 - use the values as calulated or reverse the values before and after the curvature operations
	//
	// This results in the eight types given below
	
	switch (params.curve_type)
	{
        case CURVE_POWER_SIN_FORWARD:
			power_curve = pow(in_val, power_val);
			scurve =  0.5 + 0.5 * (sin(PI * (power_curve - 0.5)));
			output = power_curve + (scurve_val * (scurve - power_curve));
			break;
			
        case CURVE_POWER_SIN_REVERSE:
			power_curve = pow((1.0 - in_val), power_val);
			scurve =  0.5 + 0.5 * (sin(PI * (power_curve - 0.5)));
			output = 1.0 - (power_curve + (scurve_val * (scurve - power_curve)));
			break;
			
        case CURVE_POWER_RECIP_SIN_REVERSE:
			power_curve = pow((1.0 - in_val), power_val_recip);
			scurve =  0.5 + 0.5 * (sin(PI * (power_curve - 0.5)));
			output = 1.0 - (power_curve + (scurve_val * (scurve - power_curve)));
			break;
			
        case CURVE_POWER_RECIP_SIN_FORWARD:
			power_curve = pow(in_val, power_val_recip);
			scurve =  0.5 + 0.5 * (sin(PI * (power_curve - 0.5)));
			output = power_curve + (scurve_val * (scurve - power_curve));
			break;
			
        case CURVE_POWER_ASIN_FORWARD:
			power_curve = pow(in_val, power_val);
			scurve =  0.5 + ((asin(2 * (power_curve - 0.5))) * PI_RECIP);
			output = power_curve + (scurve_val * (scurve - power_curve));
			break;
			
        case CURVE_POWER_ASIN_REVERSE:
			power_curve = pow((1.0 - in_val), power_val);
			scurve =  0.5 + ((asin(2 * (power_curve - 0.5))) * PI_RECIP);
			output = 1.0 - (power_curve + (scurve_val * (scurve - power_curve)));
			break;
			
        case CURVE_POWER_RECIP_ASIN_REVERSE:
			power_curve = pow((1.0 - in_val), power_val_recip);
			scurve =  0.5 + ((asin(2 * (power_curve - 0.5))) * PI_RECIP);			
			output = 1.0 - (power_curve + (scurve_val * (scurve - power_curve)));
			break;
			
		case CURVE_POWER_RECIP_ASIN_FORWARD:
			power_curve = pow(in_val, power_val_recip);
			scurve =  0.5 + ((asin(2 * (power_curve - 0.5))) * PI_RECIP);
			output = power_curve + (scurve_val * (scurve - power_curve));
			break;
	}
	
	return output;
}

// Calculate the kernel value for a given phase

double gesture_maker_kernel_calc(t_gesture_kernel *x, double in_val)
{
	double time_lo = 0.0;
	double time_hi = 1.0;
	double val1 = 0.; 
	double val2 = 1;
    t_curve_params curve_params;
	
	int curve = 0;
	
	// Calculate which of the 3 curves needs to be calculated
	
	if (in_val > x-> time1)
		curve = 1;
	if (in_val > x-> time2)
		curve = 2;
	
	// Get the values for the correct curve
	
	switch (curve)
	{
		case 0:
			
			time_lo = 0.0;
			time_hi = x->time1;
			val1 = x->val1;
			val2 = x->val2;
            curve_params = x->curve_params[0];
			break;
			
		case 1:
		
			time_lo = x->time1;
			time_hi = x->time2;
			val1 = x->val2;
			val2 = x->val3;
            curve_params = x->curve_params[1];
			break;
			
		case 2:
		
            // N.B. Reversals
            
            time_lo = 1.0;
			time_hi = x->time2;
			val1 = x->val4;
			val2 = x->val3;
            curve_params = x->curve_params[2];
			break;
	}
	
	// Calculate and clip the linear time value (0 to 1) within the current curve
	
	if (time_lo == time_hi)
		in_val = 0.0;
	else 
		in_val = (in_val - time_lo) / (time_hi - time_lo);
	
	in_val = in_val < 0.0 ? 0.0 : in_val;
	in_val = in_val > 1.0 ? 1.0 : in_val;
	
	// The linear time value is now curved
	
	in_val = gesture_maker_kernel_curve(in_val, curve_params);
	
	// Linearly interpolate between the two output values using the time value to generate the normalised output value
    
    in_val = (val2 == val1) ? val2 : val1 + in_val * (val2 - val1);
	x->last_val = in_val;
	
	return in_val;
}

// Set the intial value for the kernel

void gesture_maker_kernel_initial(t_gesture_kernel *x, double in_val)
{
	x->last_val = in_val;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// Parameter Routines //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////


// Parameter storage relating to the resolution of random bands

t_band_params time_params;
t_band_params val_params;
t_band_params pow_curve_params;
t_band_params scurve_params;

// Setup symbols, and the random band parameters for each relevant variable

void gesture_maker_kernel_params_setup()
{	
	ps_line = gensym("line");
	ps_flat = gensym("flat");
	ps_line_flat = gensym("line_flat");
	ps_flat_line = gensym("flat_line");
	ps_triangle_return = gensym("triangle_return");
	ps_triangle = gensym("triangle");
	ps_plateau_return = gensym("plateau_return");
	ps_plateau = gensym("plateau");
	ps_return = gensym("return");
	ps_general = gensym("general");
	
	ps_last = gensym("last");
	
	time_params.num_bands = 15;
	time_params.lo_val = 0.0;
	time_params.hi_val = 1.0;
	time_params.gauss_dev = 0.33;
	time_params.width_val = 0.55;
	time_params.end_ratio = 0.2;
	
	val_params.num_bands = 11;
	val_params.lo_val = 0.0;
	val_params.hi_val = 1.0;
	val_params.gauss_dev = 0.33;
	val_params.width_val = 0.55;
	val_params.end_ratio = 0.2;
	
	pow_curve_params.num_bands = 5;
	pow_curve_params.lo_val = log(1.0);
	pow_curve_params.hi_val = log(15.0);
	pow_curve_params.gauss_dev = 0.33;
	pow_curve_params.width_val = 0.55;
	pow_curve_params.end_ratio = 0.6;
	
	scurve_params.num_bands = 5;
	scurve_params.lo_val = 0.0;
	scurve_params.hi_val = 1.0;
	scurve_params.gauss_dev = 0.33;
	scurve_params.width_val = 0.55;
	scurve_params.end_ratio = 0.1;
	
	kernel_param_rand = &kernel_param_rand_obj;
	rand_seed(kernel_param_rand);
}

// Returns the specified gesture type according to the given symbol

enum t_gesture_type gesture_maker_kernel_params_type(t_gesture_kernel *x, t_symbol *type)
{
	if (type == ps_flat)                return GESTURE_FLAT;
	if (type == ps_line)                return GESTURE_LINE;
	if (type == ps_line_flat)           return GESTURE_LINE_FLAT;
	if (type == ps_flat_line)           return GESTURE_FLAT_LINE;
	if (type == ps_triangle_return)     return GESTURE_TRIANGLE_RETURN;
	if (type == ps_triangle)            return GESTURE_TRIANGLE;
	if (type == ps_plateau_return)      return GESTURE_PLATEAU_RETURN;
	if (type == ps_plateau)             return GESTURE_PLATEAU;
	if (type == ps_return)              return GESTURE_RETURN;
	if (type == ps_general)             return GESTURE_GENERAL;
	
	return GESTURE_FLAT;
}

// Set the kernel parameters to defaults

void gesture_maker_kernel_params_default(t_atom *output_params)
{
	// Time vals
	
	atom_setfloat(output_params++, 1.0);
	atom_setfloat(output_params++, 0.0);
	
	// Vals
	
	atom_setfloat(output_params++, 0.0);
	atom_setfloat(output_params++, 0.0);
	atom_setfloat(output_params++, 0.0);
	atom_setfloat(output_params++, 0.0);
	
	// Curve 1
	
	atom_setfloat(output_params++, 1.0);
	atom_setfloat(output_params++, 0.0);
	atom_setlong(output_params++, 0);
	
	// Curve 2
	
	atom_setfloat(output_params++, 1.0);
	atom_setfloat(output_params++, 0.0);
	atom_setlong(output_params++, 0);
	
	// Curve 3
	
	atom_setfloat(output_params++, 1.);
	atom_setfloat(output_params++, 0.);
	atom_setlong(output_params++, 0);
}

// Return a band number from a specifing atom (choosing randomly when a range is given)

int gesture_maker_kernel_params_random_band(t_rand_gen *gen, t_atom *specifier)
{
	int lo = 0;
	int hi = 0;
	    
	if (atom_gettype(specifier) == A_LONG)
    {
        // N.B. - we avoid values that we know are out of range here - just in case
        
        t_atom_long a = atom_getlong(specifier);
        
        return (a > -1000) ? ((a < 1000) ? a : 1000) : -1000;
    }
	
	if (atom_gettype(specifier) == A_SYM)
	{
		if (sscanf(atom_getsym(specifier)->s_name, "%d-%d", &lo, &hi) == 2)
			return rand_int_range(gen, lo, hi);
	}
	
	return 0;
}

// Calculate an exact value from a given band and a set of band parameters

double gesture_maker_kernel_params_band_to_val(t_rand_gen *gen, int band_in, t_band_params *params)
{	
	double num_bands = params->num_bands;
	double lo_val = params->lo_val;
	double hi_val = params->hi_val;
	double gauss_dev = params->gauss_dev;
	double width_val = params->width_val;
	double end_ratio = params->end_ratio;
	
	double band = band_in - 0.5;
	double rand_val = rand_windgauss(gen, 0.5, gauss_dev);
	
	// Find lo and hi values for this band
	
	double band_space = num_bands - 2.0 + (2.0 * end_ratio);
	double convert_val = (hi_val - lo_val) / band_space;
	
	double lo = band - width_val + end_ratio;
	double hi = band + width_val + end_ratio;
	
	// Clip values to band size
	
	if (lo < 0.0)
		lo = 0.0;
	if (hi < 0.0)
		hi = 0.0;
	if (lo > band_space)
		lo = band_space;
	if (hi > band_space)
		hi = band_space;
	
	// Interpolate random value into bandspace
	
	rand_val = lo + rand_val * (hi - lo);
	
	// Complete conversion to final values
	
	rand_val = ((rand_val) * convert_val) + lo_val;
	
	return rand_val;
}

// Return a time value from a specifier

double gesture_maker_kernel_params_time(t_rand_gen *gen, t_atom *specifier, bool reverse)
{
	double time_val = 0.0;
	
	if (atom_gettype(specifier) == A_FLOAT)
        time_val = atom_getfloat(specifier);
	else
		time_val = gesture_maker_kernel_params_band_to_val(gen, gesture_maker_kernel_params_random_band(gen, specifier), &time_params);
	
	if (reverse)
		time_val = 1.0 - time_val;
	
	return time_val;
}

// Return an output value (0 to 1) from a specifier

double gesture_maker_kernel_params_val(t_rand_gen *gen, t_atom *specifier, double last_val)
{
	if (atom_gettype(specifier) == A_FLOAT)
		return atom_getfloat(specifier);
	
	if (atom_gettype(specifier) == A_SYM && atom_getsym(specifier) == ps_last)
		return last_val;
	
	return gesture_maker_kernel_params_band_to_val(gen, gesture_maker_kernel_params_random_band(gen, specifier), &val_params);
}

// Calculate curve parameters based on given specifiers

void gesture_maker_kernel_params_curve(t_rand_gen *gen, t_atom *curve_params, t_atom *specifiers)
{
	int band_alter = 0;
	
	if (atom_gettype(specifiers + 1) == A_FLOAT)
		curve_params[1] = specifiers[1];
	else
		atom_setfloat(curve_params + 1, gesture_maker_kernel_params_band_to_val(gen, gesture_maker_kernel_params_random_band(gen, specifiers + 1), &scurve_params));
	
	if (atom_gettype(specifiers + 2) == A_FLOAT)
	{
		curve_params[0] = specifiers[2];
		atom_setlong(curve_params + 2, gesture_maker_kernel_params_random_band(gen, specifiers));
	}
	else 
	{
		// Get band
		
		int band = gesture_maker_kernel_params_random_band(gen, specifiers + 2) - (pow_curve_params.num_bands - 1);
		
		if (band > 0)
			band_alter = 1;
		
		band = abs(band) - band_alter;
		
		atom_setfloat(curve_params, exp(gesture_maker_kernel_params_band_to_val(gen, band, &pow_curve_params)));
		atom_setlong(curve_params + 2, gesture_maker_kernel_params_random_band(gen, specifiers) * 2 + band_alter);
	}
}

enum t_curve_type gesture_maker_curve_type(t_atom *specifier)
{
    t_atom_long type = atom_getlong(specifier);
    
    switch (type)
    {
        case 1:     return CURVE_POWER_SIN_REVERSE;
        case 2:     return CURVE_POWER_RECIP_SIN_REVERSE;
        case 3:     return CURVE_POWER_RECIP_SIN_FORWARD;
        case 4:     return CURVE_POWER_ASIN_FORWARD;
        case 5:     return CURVE_POWER_ASIN_REVERSE;
        case 6:     return CURVE_POWER_RECIP_ASIN_REVERSE;
        case 7:     return CURVE_POWER_RECIP_ASIN_FORWARD;
        default:    return CURVE_POWER_SIN_FORWARD;
    }
}

void gesture_maker_get_curve_specification(t_curve_params *params, t_atom *specifiers)
{
    double power_val = atom_getfloat(specifiers + 0);
    double scurve_val = atom_getfloat(specifiers + 1);
    
    power_val = power_val > 1.0 ? 1.0 : power_val;
    scurve_val = scurve_val < 0.0 ? 0.0 : scurve_val;
    scurve_val = scurve_val > 1.0 ? 1.0 : scurve_val;
    scurve_val = pow(scurve_val, 0.35);
    
    params->power_val = power_val;
    params->scurve_val = scurve_val;
    params->curve_type = gesture_maker_curve_type(specifiers + 2);
}

// Set all the kernel parameters based on the given input atoms

void gesture_maker_kernel_params(t_gesture_kernel *x, long argc, t_atom *argv)
{
	t_atom output_params[15];
	enum t_gesture_type gesture_type = GESTURE_FLAT;
	double timetest, swap;
    
	gesture_maker_kernel_params_default(output_params);
	
	// Get the gesture type
	
	if (argc)
	{
		gesture_type = gesture_maker_kernel_params_type(x, atom_getsym(argv++));
		argc--;		 	
	}
	
	// Set the internal parameters based on the given gesture type
	
	switch (gesture_type)
	{
		case GESTURE_FLAT:
						
			if (argc < 1)
				return;
			atom_setfloat(output_params + 2, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			output_params[3] = output_params[2];
			output_params[4] = output_params[3];
			output_params[5] = output_params[4];
			break;
					
		case GESTURE_LINE:
        
			if (argc < 2)
				return;
			atom_setfloat(output_params + 2, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			atom_setfloat(output_params + 3, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			output_params[4] = output_params[3];
			output_params[5] = output_params[4];
			if (argc >= 5)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 6, argv);
			break;

			
		case GESTURE_LINE_FLAT:
						
			if (argc < 3)
				return;
			atom_setfloat(output_params, gesture_maker_kernel_params_time(kernel_param_rand, argv++, false));
			atom_setfloat(output_params + 2, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			atom_setfloat(output_params + 3, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			output_params[4] = output_params[3];
			output_params[5] = output_params[4];
			if (argc >= 6)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 6, argv);
			break;
					
		case GESTURE_FLAT_LINE:
			
			if (argc < 3)
				return;
			atom_setfloat(output_params, gesture_maker_kernel_params_time(kernel_param_rand, argv++, false));
			atom_setfloat(output_params + 1, 1. - atom_getfloat(output_params));
			atom_setfloat(output_params + 2, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			output_params[3] = output_params[2];
			output_params[4] = output_params[3];
			atom_setfloat(output_params + 5, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			if (argc >= 6)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 12, argv);
			break;
			
		case GESTURE_TRIANGLE_RETURN:
			
			if (argc < 3)
				return;
			atom_setfloat(output_params, gesture_maker_kernel_params_time(kernel_param_rand, argv++, false));
			atom_setfloat(output_params + 1, 1. - atom_getfloat(output_params));
			atom_setfloat(output_params + 2, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			atom_setfloat(output_params + 3, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			output_params[4] = output_params[3];
			output_params[5] = output_params[2];
			if (argc >= 6)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 6, argv);
			if (argc >= 9)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 12, argv + 3);
			break;
						
		case GESTURE_TRIANGLE:
			
			if (argc < 4)
				return;
			atom_setfloat(output_params, gesture_maker_kernel_params_time(kernel_param_rand, argv++, false));
			atom_setfloat(output_params + 1, 1. - atom_getfloat(output_params));
			atom_setfloat(output_params + 2, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			atom_setfloat(output_params + 3, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			output_params[4] = output_params[3];
			atom_setfloat(output_params + 5, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			if (argc >= 7)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 6, argv);
			if (argc >= 10)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 12, argv + 3);
			break;
				
		case GESTURE_PLATEAU_RETURN:

			if (argc < 4)
				return;
			atom_setfloat(output_params, gesture_maker_kernel_params_time(kernel_param_rand, argv++, false));
			atom_setfloat(output_params + 1, gesture_maker_kernel_params_time(kernel_param_rand, argv++, true));
			atom_setfloat(output_params + 2, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			atom_setfloat(output_params + 3, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			output_params[4] = output_params[3];
			output_params[5] = output_params[2];
			if (argc >= 7)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 6, argv);
			if (argc >= 10)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 12, argv + 3);
			break;
			
		case GESTURE_PLATEAU:
						
			if (argc < 5)
				return;
			atom_setfloat(output_params, gesture_maker_kernel_params_time(kernel_param_rand, argv++, false));
			atom_setfloat(output_params + 1, gesture_maker_kernel_params_time(kernel_param_rand, argv++, true));
			atom_setfloat(output_params + 2, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			atom_setfloat(output_params + 3, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			output_params[4] = output_params[3];
			atom_setfloat(output_params + 5, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			if (argc >= 8)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 6, argv);
			if (argc >= 11)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 12, argv + 3);
			break;
						
		case GESTURE_RETURN:
			
			if (argc < 5)
				return;
			atom_setfloat(output_params, gesture_maker_kernel_params_time(kernel_param_rand, argv++, false));
			atom_setfloat(output_params + 1, gesture_maker_kernel_params_time(kernel_param_rand, argv++, true));
			atom_setfloat(output_params + 2, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			atom_setfloat(output_params + 3, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			atom_setfloat(output_params + 4, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			output_params[5] = output_params[2];
			if (argc >= 8)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 6, argv);
			if (argc >= 11)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 9, argv + 3);
			if (argc >= 14)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 12, argv + 6);
			break;
					
		case GESTURE_GENERAL:
			
			if (argc < 6)
				return;
			atom_setfloat(output_params, gesture_maker_kernel_params_time(kernel_param_rand, argv++, false));
			atom_setfloat(output_params + 1, gesture_maker_kernel_params_time(kernel_param_rand, argv++, true));
			atom_setfloat(output_params + 2, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			atom_setfloat(output_params + 3, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			atom_setfloat(output_params + 4, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			atom_setfloat(output_params + 5, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			if (argc >= 9)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 6, argv);
			if (argc >= 12)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 9, argv + 3);
			if (argc >= 15)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 12, argv + 6);
			break;
	}
	
	// Correct time ordering if necessary
	
	timetest = atom_getfloat(output_params) + atom_getfloat(output_params + 1);
	swap = 1.0 - atom_getfloat(output_params);
	
	if (timetest > 1.0)
	{
		atom_setfloat(output_params, 1.0 - atom_getfloat(output_params + 1));
		atom_setfloat(output_params + 1, swap);
	}
	
	// Final manipulations and range checking applied to internal parameters to make them ready for direct use
	
	x->time1 = atom_getfloat(output_params);
	x->time2 = 1. - atom_getfloat(output_params + 1);
	x->val1 = atom_getfloat(output_params + 2);
	x->val2 = atom_getfloat(output_params + 3);
	x->val3 = atom_getfloat(output_params + 4);
	x->val4 = atom_getfloat(output_params + 5);
	
    // Store curve parameters
    
    gesture_maker_get_curve_specification(x->curve_params + 0, output_params + 6);
    gesture_maker_get_curve_specification(x->curve_params + 1, output_params + 9);
    gesture_maker_get_curve_specification(x->curve_params + 2, output_params + 12);
}

