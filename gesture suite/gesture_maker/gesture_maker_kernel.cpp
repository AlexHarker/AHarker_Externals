
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

// PI defines

#define PI          3.14159265358979323846
#define PI_RECIP    0.31830988618379067154


#include "gesture_maker_kernel.h"

#include <RandomGenerator.hpp>


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

// Random number generator

random_generator<> rand_gen;

// Reset the kernel parameters to defaults

void curve_parameters::reset()
{
    power_val = 1.0;
    scurve_val = 0.0;
    type = curve_type::power_sin_forward;
}

curve_parameters::curve_type curve_parameters::get_type(t_atom *specifier)
{
    t_atom_long type = atom_getlong(specifier);
    
    switch (type)
    {
        case 1:     return curve_type::power_sin_reverse;
        case 2:     return curve_type::power_recip_sin_reverse;
        case 3:     return curve_type::power_recip_sin_forward;
        case 4:     return curve_type::power_asin_forward;
        case 5:     return curve_type::power_asin_reverse;
        case 6:     return curve_type::power_recip_asin_reverse;
        case 7:     return curve_type::power_recip_asin_forward;
        default:    return curve_type::power_sin_forward;
    }
}

void curve_parameters::params(t_atom *specifiers)
{
    power_val = atom_getfloat(specifiers + 0);
    power_val = power_val < 1.0 ? 1.0 : power_val;
    
    scurve_val = atom_getfloat(specifiers + 1);
    scurve_val = scurve_val < 0.0 ? 0.0 : scurve_val;
    scurve_val = scurve_val > 1.0 ? 1.0 : scurve_val;
    scurve_val = pow(scurve_val, 0.35);
    
    type = get_type(specifiers + 2);
}

// Apply curvature to one of the three parts of the gesture kernel

double curve_parameters::operator()(double val)
{
    double power_curve, scurve;
        
    // Calculate the curved value
    // There are 3 basic mode options, each with two choices
    //
    // 1 - use power curvature using power val, or reciprocal
    // 2 - use s curvature based on sin calculation or based asin calculation
    // 3 - use the values as calulated or reverse the values before and after the curvature operations
    //
    // This results in the eight types given below
    
    switch (type)
    {
        case curve_type::power_sin_forward:
            power_curve = pow(val, power_val);
            scurve =  0.5 + 0.5 * (sin(PI * (power_curve - 0.5)));
            return power_curve + (scurve_val * (scurve - power_curve));
            
        case curve_type::power_sin_reverse:
            power_curve = pow((1.0 - val), power_val);
            scurve =  0.5 + 0.5 * (sin(PI * (power_curve - 0.5)));
            return 1.0 - (power_curve + (scurve_val * (scurve - power_curve)));
            
        case curve_type::power_recip_sin_reverse:
            power_curve = pow((1.0 - val), 1.0 / power_val);
            scurve =  0.5 + 0.5 * (sin(PI * (power_curve - 0.5)));
            return 1.0 - (power_curve + (scurve_val * (scurve - power_curve)));
            
        case curve_type::power_recip_sin_forward:
            power_curve = pow(val, 1.0 / power_val);
            scurve =  0.5 + 0.5 * (sin(PI * (power_curve - 0.5)));
            return power_curve + (scurve_val * (scurve - power_curve));
            
        case curve_type::power_asin_forward:
            power_curve = pow(val, power_val);
            scurve =  0.5 + ((asin(2 * (power_curve - 0.5))) * PI_RECIP);
            return power_curve + (scurve_val * (scurve - power_curve));
            
        case curve_type::power_asin_reverse:
            power_curve = pow((1.0 - val), power_val);
            scurve =  0.5 + ((asin(2 * (power_curve - 0.5))) * PI_RECIP);
            return 1.0 - (power_curve + (scurve_val * (scurve - power_curve)));
            
        case curve_type::power_recip_asin_reverse:
            power_curve = pow((1.0 - val), 1.0 / power_val);
            scurve =  0.5 + ((asin(2 * (power_curve - 0.5))) * PI_RECIP);
            return 1.0 - (power_curve + (scurve_val * (scurve - power_curve)));
            
        case curve_type::power_recip_asin_forward:
            power_curve = pow(val, 1.0 / power_val);
            scurve =  0.5 + ((asin(2 * (power_curve - 0.5))) * PI_RECIP);
            return power_curve + (scurve_val * (scurve - power_curve));
    }
}

void gesture_kernel::reset()
{	
	time1 = 0.0;
	time2 = 1.0;

    val1 = 0.0;
	val2 = 0.0;
	val3 = 1.0;
	val4 = 1.0;
    
    curve_params[0].reset();
    curve_params[1].reset();
    curve_params[2].reset();
	
    last_val = 0.0;
}


// Calculate the kernel value for a given phase

double gesture_kernel::operator()(double val)
{
	double time_lo = 0.0;
	double time_hi = 1.0;
	double v1 = 0.0;
	double v2 = 1.0;
	
	int curve_idx = 0;
	
	// Calculate which of the 3 curves needs to be calculated
	
	if (val > time1)
        curve_idx = 1;
	if (val > time2)
        curve_idx = 2;
	
	// Get the values for the correct curve
	
	switch (curve_idx)
	{
		case 0:
			
			time_lo = 0.0;
			time_hi = time1;
			v1 = val1;
			v2 = val2;
			break;
			
		case 1:
		
			time_lo = time1;
			time_hi = time2;
			v1 = val2;
			v2 = val3;
			break;
			
		case 2:
		
            // N.B. Reversals
            
            time_lo = 1.0;
			time_hi = time2;
			v1 = val4;
			v2 = val3;
			break;
	}
	
	// Calculate and clip the linear time value (0 to 1) within the current curve
	
	if (time_lo == time_hi)
		val = 0.0;
	else 
		val = (val - time_lo) / (time_hi - time_lo);
	
	val = val < 0.0 ? 0.0 : val;
	val = val > 1.0 ? 1.0 : val;
	
	// The linear time value is now curved
	
	val = curve_params[curve_idx](val);
	
	// Linearly interpolate between the two output values using the time value to generate the normalised output value
    
    last_val = (v2 == v1) ? v2 : v1 + val * (v2 - v1);
	
	return last_val;
}

// Set the initial value for the kernel

void gesture_kernel::initial(double val)
{
	last_val = val;
}

// Set the initial value for the kernel from a specifier (including a random band)

void gesture_kernel::initial_specifier(t_atom *specifier)
{
    last_val = params_val(specifier, last_val);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// Parameter Routines //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////


// Parameter storage relating to the resolution of random bands

band_parameters time_params;
band_parameters val_params;
band_parameters pow_curve_params;
band_parameters scurve_params;

// Setup symbols, and the random band parameters for each relevant variable

void gesture_kernel::setup()
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
}

// Returns the specified gesture type according to the given symbol

gesture_kernel::gesture_type gesture_kernel::get_type(t_symbol *type)
{
    // FIX errors??
    
	if (type == ps_flat)                return gesture_type::flat;
	if (type == ps_line)                return gesture_type::line;
	if (type == ps_line_flat)           return gesture_type::line_flat;
	if (type == ps_flat_line)           return gesture_type::flat_line;
	if (type == ps_triangle_return)     return gesture_type::triangle_return;
	if (type == ps_triangle)            return gesture_type::triangle;
	if (type == ps_plateau_return)      return gesture_type::plateau_return;
	if (type == ps_plateau)             return gesture_type::plateau;
	if (type == ps_return)              return gesture_type::general_return;
	if (type == ps_general)             return gesture_type::general;
	
	return gesture_type::flat;
}

// Set the kernel parameters to defaults

void default_params(t_atom *output_params)
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

int random_band(t_atom *specifier)
{
	int lo = 0;
	int hi = 0;
	    
	if (atom_gettype(specifier) == A_LONG)
    {
        // N.B. - we avoid values that we know are out of range here - just in case
        
        t_atom_long a = atom_getlong(specifier);
        
        return (a > -1000) ? ((a < 1000) ? static_cast<int>(a) : 1000) : -1000;
    }
	
	if (atom_gettype(specifier) == A_SYM)
	{
		if (sscanf(atom_getsym(specifier)->s_name, "%d-%d", &lo, &hi) == 2)
			return rand_gen.rand_int(lo, hi);
	}
	
	return 0;
}

// Calculate an exact value from a given band and a set of band parameters

double band_parameters::band_to_val(int band_in)
{
	double band = band_in - 0.5;
	double rand_val = rand_gen.rand_windowed_gaussian(0.5, gauss_dev);
	
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

double gesture_kernel::params_time(t_atom *specifier, bool reverse)
{
	double time_val = 0.0;
	
	if (atom_gettype(specifier) == A_FLOAT)
        time_val = atom_getfloat(specifier);
	else
		time_val = time_params.band_to_val(random_band(specifier));
	
	if (reverse)
		time_val = 1.0 - time_val;
	
	return time_val;
}

// Return an output value (0 to 1) from a specifier

double gesture_kernel::params_val(t_atom *specifier, double last_val)
{
	if (atom_gettype(specifier) == A_FLOAT)
		return atom_getfloat(specifier);
	
	if (atom_gettype(specifier) == A_SYM && atom_getsym(specifier) == ps_last)
		return last_val;
	
	return val_params.band_to_val(random_band(specifier));
}

// Calculate curve parameters based on given specifiers

void gesture_maker_kernel_params_curve(t_atom *curve_params, t_atom *specifiers)
{
	int band_alter = 0;
	
	if (atom_gettype(specifiers + 1) == A_FLOAT)
		curve_params[1] = specifiers[1];
	else
		atom_setfloat(curve_params + 1, scurve_params.band_to_val(random_band(specifiers + 1)));
	
	if (atom_gettype(specifiers + 2) == A_FLOAT)
	{
		curve_params[0] = specifiers[2];
		atom_setlong(curve_params + 2, random_band(specifiers));
	}
	else 
	{
		// Get band
		
		int band = random_band(specifiers + 2) - (pow_curve_params.num_bands - 1);
		
		if (band > 0)
			band_alter = 1;
		
		band = abs(band) - band_alter;
		
		atom_setfloat(curve_params, exp(pow_curve_params.band_to_val(band)));
		atom_setlong(curve_params + 2, random_band(specifiers) * 2 + band_alter);
	}
}

// Set all the kernel parameters based on the given input atoms

void gesture_kernel::params(long argc, t_atom *argv)
{
	t_atom output_params[15];
	gesture_type type = gesture_type::flat;
    
    default_params(output_params);
	
	// Get the gesture type
	
	if (argc)
	{
		type = get_type(atom_getsym(argv++));
		argc--;		 	
	}
	
	// Set the internal parameters based on the given gesture type
	
	switch (type)
	{
        case gesture_type::flat:
						
			if (argc < 1)
				return;
			atom_setfloat(output_params + 2, params_val(argv++, last_val));
			output_params[3] = output_params[2];
			output_params[4] = output_params[3];
			output_params[5] = output_params[4];
			break;
					
        case gesture_type::line:
        
			if (argc < 2)
				return;
			atom_setfloat(output_params + 2, params_val(argv++, last_val));
			atom_setfloat(output_params + 3, params_val(argv++, last_val));
			output_params[4] = output_params[3];
			output_params[5] = output_params[4];
			if (argc >= 5)
				gesture_maker_kernel_params_curve(output_params + 6, argv);
			break;

			
        case gesture_type::line_flat:
						
			if (argc < 3)
				return;
			atom_setfloat(output_params + 0, params_time(argv++, false));
			atom_setfloat(output_params + 2, params_val(argv++, last_val));
			atom_setfloat(output_params + 3, params_val(argv++, last_val));
			output_params[4] = output_params[3];
			output_params[5] = output_params[4];
			if (argc >= 6)
				gesture_maker_kernel_params_curve(output_params + 6, argv);
			break;
					
        case gesture_type::flat_line:
			
			if (argc < 3)
				return;
			atom_setfloat(output_params + 0, params_time(argv++, false));
			atom_setfloat(output_params + 1, 1.0 - atom_getfloat(output_params));
			atom_setfloat(output_params + 2, params_val(argv++, last_val));
			output_params[3] = output_params[2];
			output_params[4] = output_params[3];
			atom_setfloat(output_params + 5, params_val(argv++, last_val));
			if (argc >= 6)
				gesture_maker_kernel_params_curve(output_params + 12, argv);
			break;
			
        case gesture_type::triangle_return:
			
			if (argc < 3)
				return;
			atom_setfloat(output_params + 0, params_time(argv++, false));
			atom_setfloat(output_params + 1, 1.0 - atom_getfloat(output_params));
			atom_setfloat(output_params + 2, params_val(argv++, last_val));
			atom_setfloat(output_params + 3, params_val(argv++, last_val));
			output_params[4] = output_params[3];
			output_params[5] = output_params[2];
			if (argc >= 6)
				gesture_maker_kernel_params_curve(output_params + 6, argv);
			if (argc >= 9)
				gesture_maker_kernel_params_curve(output_params + 12, argv + 3);
			break;
						
        case gesture_type::triangle:
			
			if (argc < 4)
				return;
			atom_setfloat(output_params + 0, params_time(argv++, false));
			atom_setfloat(output_params + 1, 1.0 - atom_getfloat(output_params));
			atom_setfloat(output_params + 2, params_val(argv++, last_val));
			atom_setfloat(output_params + 3, params_val(argv++, last_val));
			output_params[4] = output_params[3];
			atom_setfloat(output_params + 5, params_val(argv++, last_val));
			if (argc >= 7)
				gesture_maker_kernel_params_curve(output_params + 6, argv);
			if (argc >= 10)
				gesture_maker_kernel_params_curve(output_params + 12, argv + 3);
			break;
				
        case gesture_type::plateau_return:

			if (argc < 4)
				return;
			atom_setfloat(output_params + 0, params_time(argv++, false));
			atom_setfloat(output_params + 1, params_time(argv++, true));
			atom_setfloat(output_params + 2, params_val(argv++, last_val));
			atom_setfloat(output_params + 3, params_val(argv++, last_val));
			output_params[4] = output_params[3];
			output_params[5] = output_params[2];
			if (argc >= 7)
				gesture_maker_kernel_params_curve(output_params + 6, argv);
			if (argc >= 10)
				gesture_maker_kernel_params_curve(output_params + 12, argv + 3);
			break;
			
        case gesture_type::plateau:
						
			if (argc < 5)
				return;
			atom_setfloat(output_params + 0, params_time(argv++, false));
			atom_setfloat(output_params + 1, params_time(argv++, true));
			atom_setfloat(output_params + 2, params_val(argv++, last_val));
			atom_setfloat(output_params + 3, params_val(argv++, last_val));
			output_params[4] = output_params[3];
			atom_setfloat(output_params + 5, params_val(argv++, last_val));
			if (argc >= 8)
				gesture_maker_kernel_params_curve(output_params + 6, argv);
			if (argc >= 11)
				gesture_maker_kernel_params_curve(output_params + 12, argv + 3);
			break;
						
        case gesture_type::general_return:
			
			if (argc < 5)
				return;
			atom_setfloat(output_params + 0, params_time(argv++, false));
			atom_setfloat(output_params + 1, params_time(argv++, true));
			atom_setfloat(output_params + 2, params_val(argv++, last_val));
			atom_setfloat(output_params + 3, params_val(argv++, last_val));
			atom_setfloat(output_params + 4, params_val(argv++, last_val));
			output_params[5] = output_params[2];
			if (argc >= 8)
				gesture_maker_kernel_params_curve(output_params + 6, argv);
			if (argc >= 11)
				gesture_maker_kernel_params_curve(output_params + 9, argv + 3);
			if (argc >= 14)
				gesture_maker_kernel_params_curve(output_params + 12, argv + 6);
			break;
					
        case gesture_type::general:
			
			if (argc < 6)
				return;
			atom_setfloat(output_params + 0, params_time(argv++, false));
			atom_setfloat(output_params + 1, params_time(argv++, true));
			atom_setfloat(output_params + 2, params_val(argv++, last_val));
			atom_setfloat(output_params + 3, params_val(argv++, last_val));
			atom_setfloat(output_params + 4, params_val(argv++, last_val));
			atom_setfloat(output_params + 5, params_val(argv++, last_val));
			if (argc >= 9)
				gesture_maker_kernel_params_curve(output_params + 6, argv);
			if (argc >= 12)
				gesture_maker_kernel_params_curve(output_params + 9, argv + 3);
			if (argc >= 15)
				gesture_maker_kernel_params_curve(output_params + 12, argv + 6);
			break;
	}
	
	// Correct time ordering if necessary
	
    const double timetest = atom_getfloat(output_params) + atom_getfloat(output_params + 1);
	
	if (timetest > 1.0)
	{
        double swap = 1.0 - atom_getfloat(output_params);
		atom_setfloat(output_params, 1.0 - atom_getfloat(output_params + 1));
		atom_setfloat(output_params + 1, swap);
	}
	
	// Final manipulations and range checking applied to internal parameters to make them ready for direct use
	
	time1 = atom_getfloat(output_params + 0);
	time2 = 1.0 - atom_getfloat(output_params + 1);
	
    val1 = atom_getfloat(output_params + 2);
	val2 = atom_getfloat(output_params + 3);
	val3 = atom_getfloat(output_params + 4);
	val4 = atom_getfloat(output_params + 5);
	
    // Store curve parameters
    
    curve_params[0].params(output_params + 6);
    curve_params[1].params(output_params + 9);
    curve_params[2].params(output_params + 12);
}
