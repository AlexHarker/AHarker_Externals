
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

void gesture_maker_kernel_reset(t_gesture_kernel *x)
{	
	x->time1 = 0.;
	x->time2 = 1.;
	x->val1 = 0.;
	x->val2 = 0.;
	x->val3 = 1.;
	x->val4 = 1.;
	x->power_val1 = 1.;
	x->power_val2 = 1.;
	x->power_val3 = 1.;
	x->scurve_val1 = 0.;
	x->scurve_val2 = 0.;
	x->scurve_val3 = 0.;
	x->curve_type1 = 0;	
	x->curve_type2 = 0;
	x->curve_type3 = 0;
	x->last_val = 0.;
}


// Apply curvature to one of the three parts of the gesture kernel

double gesture_maker_kernel_curve(double in_val, double power_val, double scurve_val, long curve_type)
{
	double output = in_val;
	double power_curve, scurve;
	double power_val_recip = 1. / power_val;
		
	// Calculate the curved value
	// There are 3 basic mode options, each with two choices 
	//
	// 1 - use power curvature using power val, or reciprocal  
	// 2 - use s curvature based on sin calculation or based asin calculation
	// 3 - use the values as calulated or reverse the values before and after the curvature operations
	//
	// This results in the eight types given below
	
	switch (curve_type)
	{
		case 0:
			
			power_curve = pow(in_val, power_val);
			scurve =  0.5 + 0.5 * (sin (PI * (power_curve - 0.5)));
			output = power_curve + (scurve_val * (scurve - power_curve));
			
			break;
			
		case 1:
			
			power_curve = pow((1.0 - in_val), power_val);
			scurve =  0.5 + 0.5 * (sin (PI * (power_curve - 0.5)));
			output = 1.0 - (power_curve + (scurve_val * (scurve - power_curve)));
			
			break;
			
		case 2:
			
			power_curve = pow((1.0 - in_val), power_val_recip);
			scurve =  0.5 + 0.5 * (sin (PI * (power_curve - 0.5)));
			output = 1.0 - (power_curve + (scurve_val * (scurve - power_curve)));
			
			break;
			
		case 3:
		
			power_curve = pow(in_val, power_val_recip);
			scurve =  0.5 + 0.5 * (sin (PI * (power_curve - 0.5)));
			output = power_curve + (scurve_val * (scurve - power_curve));
			
			break;
			
		case 4:
		
			power_curve = pow(in_val, power_val);
			scurve =  0.5 + ((asin(2 * (power_curve - 0.5))) * PI_RECIP);
			output = power_curve + (scurve_val * (scurve - power_curve));
			
			break;
			
		case 5:
		
			power_curve = pow((1.0 - in_val), power_val);
			scurve =  0.5 + ((asin(2 * (power_curve - 0.5))) * PI_RECIP);
			output = 1.0 - (power_curve + (scurve_val * (scurve - power_curve)));
			
			break;
			
		case 6:
			
			power_curve = pow((1.0 - in_val), power_val_recip);
			scurve =  0.5 + ((asin(2 * (power_curve - 0.5))) * PI_RECIP);			
			output = 1.0 - (power_curve + (scurve_val * (scurve - power_curve)));
			
			break;
			
		case 7:
		
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
	double time_lo = 0.; 
	double time_hi = 1;
	double val1 = 0.; 
	double val2 = 1;
	double power_val = 1; 
	double scurve_val = 0; 
	long curve_type = 0;
	
	long curve = 0;
	
	// Calculate which of the 3 curves needs to be calculated
	
	if (in_val > x-> time1)
		curve = 1;
	if (in_val > x-> time2)
		curve = 2;
	

	// Get the values for the correct curve
	
	switch (curve)
	{
		case 0:
			
			time_lo = 0.;
			time_hi = x->time1;
			val1 = x->val1;
			val2 = x->val2;
			power_val = x->power_val1;
			scurve_val = x->scurve_val1;
			curve_type = x->curve_type1;
			break;
			
		case 1:
		
			time_lo = x->time1;
			time_hi = x->time2;
			val1 = x->val2;
			val2 = x->val3;
			power_val = x->power_val2;
			scurve_val = x->scurve_val2;
			curve_type = x->curve_type2;
			break;
			
		case 2:
		
			time_lo = 1.;
			time_hi = x->time2;
			val1 = x->val4;
			val2 = x->val3;
			power_val = x->power_val3;
			scurve_val = x->scurve_val3;
			curve_type = x->curve_type3;
			break;
	}
	
	// Calculate and clip the linear time value (0 to 1) within the current curve
	
	if (time_lo == time_hi)
		in_val = 0.;
	else 
		in_val = (in_val - time_lo) / (time_hi - time_lo);
	
	in_val = in_val < 0. ? 0. : in_val;
	in_val = in_val > 1. ? 1. : in_val;
	
	// The linear time value is now curved
	
	in_val = gesture_maker_kernel_curve(in_val, power_val, scurve_val, curve_type);
	
	// Linearly interpolate between the two output values using the time value to generate the normalised output value
	if (val2 == val1)
		in_val = val2;
	else
		in_val = val1 + in_val * (val2 - val1);
	
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


// Parmater storage relating to the resolution of random bands

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
	time_params.lo_val = 0.;
	time_params.hi_val = 1.;
	time_params.gauss_dev = 0.33;
	time_params.width_val = 0.55;
	time_params.end_ratio = 0.2;
	
	val_params.num_bands = 11;
	val_params.lo_val = 0.;
	val_params.hi_val = 1.;
	val_params.gauss_dev = 0.33;
	val_params.width_val = 0.55;
	val_params.end_ratio = 0.2;
	
	pow_curve_params.num_bands = 5;
	pow_curve_params.lo_val = log(1.);
	pow_curve_params.hi_val = log(15.);
	pow_curve_params.gauss_dev = 0.33;
	pow_curve_params.width_val = 0.55;
	pow_curve_params.end_ratio = 0.6;
	
	scurve_params.num_bands = 5;
	scurve_params.lo_val = 0.;
	scurve_params.hi_val = 1.;
	scurve_params.gauss_dev = 0.33;
	scurve_params.width_val = 0.55;
	scurve_params.end_ratio = 0.1;
	
	kernel_param_rand = &kernel_param_rand_obj;
	rand_seed (kernel_param_rand);
}


// Returns the specified gesture type according to the given symbol

long gesture_maker_kernel_params_type (t_gesture_kernel *x, t_symbol *type)
{
	if (type == ps_flat) return 0;
	if (type == ps_line) return 1;
	if (type == ps_line_flat) return 2;
	if (type == ps_flat_line) return 3;
	if (type == ps_triangle_return) return 4;
	if (type == ps_triangle) return 5;
	if (type == ps_plateau_return) return 6;
	if (type == ps_plateau) return 7;
	if (type == ps_return) return 8;
	if (type == ps_general) return 9;
	
	return 0;
}


// Set the kernel parameters to defaults

void gesture_maker_kernel_params_default(t_atom *output_params)
{
	// Time vals
	
	atom_setfloat(output_params++, 1.);
	atom_setfloat(output_params++, 0.);
	
	// Vals
	
	atom_setfloat(output_params++, 0.);
	atom_setfloat(output_params++, 0.);
	atom_setfloat(output_params++, 0.);
	atom_setfloat(output_params++, 0.);
	
	// Curve 1
	
	atom_setfloat(output_params++, 1.);
	atom_setfloat(output_params++, 0.);
	atom_setlong(output_params++, 0);
	
	// Curve 2
	
	atom_setfloat(output_params++, 1.);
	atom_setfloat(output_params++, 0.);
	atom_setlong(output_params++, 0);
	
	// Curve 3
	
	atom_setfloat(output_params++, 1.);
	atom_setfloat(output_params++, 0.);
	atom_setlong(output_params++, 0);
}


// Return a band number from a specifing atom (choosing randomly when a range is given)

long gesture_maker_kernel_params_random_band(t_rand_gen *gen, t_atom *specifier)
{
	long lo = 0;
	long hi = 0;
	
	if (specifier->a_type == A_LONG)
		return specifier->a_w.w_long;
	
	if (specifier->a_type == A_SYM)
	{
		if (sscanf(specifier->a_w.w_sym->s_name, "%ld-%ld", &lo, &hi) == 2)
			return rand_int_range (gen, lo, hi);
	}
	
	return 0;
}


// Calculate an exact value from a given band and a set of band parameters

double gesture_maker_kernel_params_band_to_val(t_rand_gen *gen, long band_in, t_band_params *params)
{	
	double num_bands = params->num_bands;
	double lo_val = params->lo_val;
	double hi_val = params->hi_val;
	double gauss_dev = params->gauss_dev;
	double width_val = params->width_val;
	double end_ratio = params->end_ratio;
	
	double band = band_in - 0.5;
	double rand_val = rand_windgauss (gen, 0.5, gauss_dev);
	
	// Find lo and hi values for this band
	
	double band_space = num_bands - 2. + (2. * end_ratio);
	double convert_val = (hi_val - lo_val) / band_space;
	
	double lo = band - width_val + end_ratio;
	double hi = band + width_val + end_ratio;
	
	// Clip values to band size
	
	if (lo < 0.)
		lo = 0.;
	if (hi < 0.)
		hi = 0.;
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

double gesture_maker_kernel_params_time(t_rand_gen *gen, t_atom *specifier, long mode)
{
	double time_val = 0.;
	
	if (specifier->a_type == A_FLOAT)
		time_val = specifier->a_w.w_float;
	else
		time_val = gesture_maker_kernel_params_band_to_val(gen, gesture_maker_kernel_params_random_band(gen, specifier), &time_params);
	
	if (mode)
		time_val = 1. - time_val;
	
	return time_val;
}


// Return an output value (0 to 1) from a specifier

double gesture_maker_kernel_params_val(t_rand_gen *gen, t_atom *specifier, double last_val)
{
	if (specifier->a_type == A_FLOAT)
		return specifier->a_w.w_float;
	
	if (specifier->a_type == A_SYM && atom_getsym(specifier) == ps_last)
		return last_val;
	
	return gesture_maker_kernel_params_band_to_val(gen, gesture_maker_kernel_params_random_band(gen, specifier), &val_params);
}


// Calculate curve parameters based on given specifiers

void gesture_maker_kernel_params_curve(t_rand_gen *gen, t_atom *curve_params, t_atom *specifiers)
{
	long band_alter = 0;
	
	if (specifiers[1].a_type == A_FLOAT)
		curve_params[1] = specifiers[1];
	else
		atom_setfloat(curve_params + 1, gesture_maker_kernel_params_band_to_val(gen, gesture_maker_kernel_params_random_band(gen, specifiers + 1), &scurve_params));
	
	if (specifiers[2].a_type == A_FLOAT)
	{
		curve_params[0] = specifiers[2];
		atom_setlong(curve_params + 2, gesture_maker_kernel_params_random_band(gen, specifiers));
	}
	else 
	{
		// Get band
		
		long band = gesture_maker_kernel_params_random_band(gen, specifiers + 2) - (pow_curve_params.num_bands - 1);
		
		if (band > 0)
			band_alter = 1;
		
		band = abs(band) - band_alter;
		
		atom_setfloat(curve_params, exp(gesture_maker_kernel_params_band_to_val(gen, band, &pow_curve_params)));
		atom_setlong(curve_params + 2, gesture_maker_kernel_params_random_band(gen, specifiers) * 2 + band_alter);
	}
}


// Set all the kernel parameters based on the given input atoms

void gesture_maker_kernel_params(t_gesture_kernel *x, short argc, t_atom *argv)
{
	t_atom output_params[15];
	
	double timetest, swap,  power_val, scurve_val;
	
	long gesture_type = 0;
	long curve_type;

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
		case 0:
			
			// Flat
			
			if (argc < 1)
				return;
			atom_setfloat(output_params + 2, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			output_params[3] = output_params[2];
			output_params[4] = output_params[3];
			output_params[5] = output_params[4];
			break;
					
		case 1:
			
			// Line

			if (argc < 2)
				return;
			atom_setfloat(output_params + 2, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			atom_setfloat(output_params + 3, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			output_params[4] = output_params[3];
			output_params[5] = output_params[4];
			if (argc >= 5)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 6, argv);
			break;

			
		case 2:
			
			// Line - Flat
			
			if (argc < 3)
				return;
			atom_setfloat(output_params, gesture_maker_kernel_params_time(kernel_param_rand, argv++, 0));
			atom_setfloat(output_params + 2, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			atom_setfloat(output_params + 3, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			output_params[4] = output_params[3];
			output_params[5] = output_params[4];
			if (argc >= 6)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 6, argv);
			break;
					
		case 3:
			
			// Flat - Line

			if (argc < 3)
				return;
			atom_setfloat(output_params, gesture_maker_kernel_params_time(kernel_param_rand, argv++, 0));
			atom_setfloat(output_params + 1, 1. - atom_getfloat(output_params));
			atom_setfloat(output_params + 2, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			output_params[3] = output_params[2];
			output_params[4] = output_params[3];
			atom_setfloat(output_params + 5, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			if (argc >= 6)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 12, argv);
			break;
			
		case 4:
			
			// Triangle Return

			if (argc < 3)
				return;
			atom_setfloat(output_params, gesture_maker_kernel_params_time(kernel_param_rand, argv++, 0));
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
						
		case 5:
			
			// Triangle

			if (argc < 4)
				return;
			atom_setfloat(output_params, gesture_maker_kernel_params_time(kernel_param_rand, argv++, 0));
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
				
		case 6:
			
			// Plateau Return

			if (argc < 4)
				return;
			atom_setfloat(output_params, gesture_maker_kernel_params_time(kernel_param_rand, argv++, 0));
			atom_setfloat(output_params + 1, gesture_maker_kernel_params_time(kernel_param_rand, argv++, 1));
			atom_setfloat(output_params + 2, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			atom_setfloat(output_params + 3, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			output_params[4] = output_params[3];
			output_params[5] = output_params[2];
			if (argc >= 7)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 6, argv);
			if (argc >= 10)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 12, argv + 3);
			break;
			
		case 7:
			
			// Plateau
			
			if (argc < 5)
				return;
			atom_setfloat(output_params, gesture_maker_kernel_params_time(kernel_param_rand, argv++, 0));
			atom_setfloat(output_params + 1, gesture_maker_kernel_params_time(kernel_param_rand, argv++, 1));
			atom_setfloat(output_params + 2, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			atom_setfloat(output_params + 3, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			output_params[4] = output_params[3];
			atom_setfloat(output_params + 5, gesture_maker_kernel_params_val(kernel_param_rand, argv++, x->last_val));
			if (argc >= 8)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 6, argv);
			if (argc >= 11)
				gesture_maker_kernel_params_curve(kernel_param_rand, output_params + 12, argv + 3);
			break;
						
		case 8:
			
			// Return

			if (argc < 5)
				return;
			atom_setfloat(output_params, gesture_maker_kernel_params_time(kernel_param_rand, argv++, 0));
			atom_setfloat(output_params + 1, gesture_maker_kernel_params_time(kernel_param_rand, argv++, 1));
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
					
		case 9:
			
			// General

			if (argc < 6)
				return;
			atom_setfloat(output_params, gesture_maker_kernel_params_time(kernel_param_rand, argv++, 0));
			atom_setfloat(output_params + 1, gesture_maker_kernel_params_time(kernel_param_rand, argv++, 1));
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
	swap = 1. - atom_getfloat(output_params);
	
	if (timetest > 1.)
	{
		atom_setfloat(output_params, 1. - atom_getfloat(output_params + 1));
		atom_setfloat(output_params + 1, swap);
	}
	
	// Final manipulations and range checking applied to internal parameters to make them ready for direct use
	
	x->time1 = atom_getfloat(output_params);
	x->time2 = 1. - atom_getfloat(output_params + 1);
	x->val1 = atom_getfloat(output_params + 2);
	x->val2 = atom_getfloat(output_params + 3);
	x->val3 = atom_getfloat(output_params + 4);
	x->val4 = atom_getfloat(output_params + 5);
	
	power_val = atom_getfloat(output_params + 6);
	scurve_val = atom_getfloat(output_params + 7);
	curve_type = atom_getlong(output_params + 8);
	
	if (power_val < 1.) 
		power_val = 1.;
	
	if (scurve_val > 1.0) 
		scurve_val = 1.0;
	if (scurve_val < 0.0) 
		scurve_val = 0.0;
	scurve_val = pow(scurve_val, 0.35);
	
	if (curve_type < 0) 
		curve_type = -curve_type;
	curve_type = curve_type % 8;
	
	x->power_val1 = power_val;
	x->scurve_val1 = scurve_val;
	x->curve_type1 = curve_type;
	
	power_val = atom_getfloat(output_params + 9);
	scurve_val = atom_getfloat(output_params + 10);
	curve_type = atom_getlong(output_params + 11);
	
	if (power_val < 1.) 
		power_val = 1.;
	
	if (scurve_val > 1.0) 
		scurve_val = 1.0;
	if (scurve_val < 0.0) 
		scurve_val = 0.0;
	scurve_val = pow(scurve_val, 0.35);
	
	if (curve_type < 0) 
		curve_type = -curve_type;
	curve_type = curve_type % 8;
	
	x->power_val2 = power_val;
	x->scurve_val2 = scurve_val;
	x->curve_type2 = curve_type;
	
	power_val = atom_getfloat(output_params + 12);
	scurve_val = atom_getfloat(output_params + 13);
	curve_type = atom_getlong(output_params + 14);
	
	if (power_val < 1.) 
		power_val = 1.;
	
	if (scurve_val > 1.0) 
		scurve_val = 1.0;
	if (scurve_val < 0.0) 
		scurve_val = 0.0;
	scurve_val = pow(scurve_val, 0.35);
	
	if (curve_type < 0) 
		curve_type = -curve_type;
	curve_type = curve_type % 8;
	
	x->power_val3 = power_val;
	x->scurve_val3 = scurve_val;
	x->curve_type3 = curve_type;
}

