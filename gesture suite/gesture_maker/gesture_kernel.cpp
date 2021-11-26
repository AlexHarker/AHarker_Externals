
/*
 *  gesture_kernel.c
 *
 *	This code deals with storing gestures, resolving variable parameters and calculating output values for either the main or inflection gestures.
 *	A kernel has up to 3 parts, each of which is first calculated as curve between the given output parameters.
 *	Kernel parameters may either be specified directly, or with a certain degree of "random" variability.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include "gesture_kernel.hpp"
#include "gesture_random.hpp"

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

// Reset the kernel parameters to defaults

void gesture_kernel::reset()
{	
	time1 = 0.0;
	time2 = 1.0;

    val1 = 0.0;
	val2 = 0.0;
	val3 = 1.0;
	val4 = 1.0;
    
    curves[0].reset();
    curves[1].reset();
    curves[2].reset();
	
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
	
	val = curves[curve_idx](val);
	
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

constexpr gesture_random time_params(15, 0.0, 1.0, 0.2);
constexpr gesture_random val_params(11, 0.0, 1.0, 0.2);

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

// Set all the kernel parameters based on the given input atoms

void gesture_kernel::params(long argc, t_atom *argv)
{
	t_atom output_params[2];
	gesture_type type = gesture_type::flat;
    
    // Time vals
    
    atom_setfloat(output_params + 0, 1.0);
    atom_setfloat(output_params + 1, 0.0);
        
    // Curve 1
    
    curves[0].reset();
    curves[1].reset();
    curves[2].reset();
	
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
			
            val1 = params_val(argv++, last_val);
            val2 = val1;
            val3 = val1;
            val4 = val1;
			break;
					
        case gesture_type::line:
        
			if (argc < 2)
				return;
            
            val1 = params_val(argv++, last_val);
            val2 = params_val(argv++, last_val);
            val3 = val2;
            val4 = val2;

			if (argc >= 5)
                curves[0].params(argv);
			break;

			
        case gesture_type::line_flat:
						
			if (argc < 3)
				return;
            
			atom_setfloat(output_params + 0, params_time(argv++, false));
			
            val1 = params_val(argv++, last_val);
            val2 = params_val(argv++, last_val);
            val3 = val2;
            val4 = val2;
            
			if (argc >= 6)
                curves[0].params(argv);
			break;
					
        case gesture_type::flat_line:
			
			if (argc < 3)
                return;
            
			atom_setfloat(output_params + 0, params_time(argv++, false));
			atom_setfloat(output_params + 1, 1.0 - atom_getfloat(output_params));
            
            val1 = params_val(argv++, last_val);
            val2 = val1;
            val3 = val1;
            val4 = params_val(argv++, last_val);
            
            if (argc >= 6)
                curves[2].params(argv);
			break;
			
        case gesture_type::triangle_return:
			
			if (argc < 3)
				return;
			
            atom_setfloat(output_params + 0, params_time(argv++, false));
            atom_setfloat(output_params + 1, 1.0 - atom_getfloat(output_params));
            
            val1 = params_val(argv++, last_val);
            val2 = params_val(argv++, last_val);
            val3 = val2;
            val4 = val1;
            
			if (argc >= 6)
                curves[0].params(argv);
			if (argc >= 9)
                curves[2].params(argv + 3);
			break;
						
        case gesture_type::triangle:
			
			if (argc < 4)
				return;
            
			atom_setfloat(output_params + 0, params_time(argv++, false));
			atom_setfloat(output_params + 1, 1.0 - atom_getfloat(output_params));
			
            val1 = params_val(argv++, last_val);
            val2 = params_val(argv++, last_val);
            val3 = val1;
            val4 = params_val(argv++, last_val);
            
            if (argc >= 7)
                curves[0].params(argv);
            if (argc >= 10)
                curves[2].params(argv + 3);
			break;
				
        case gesture_type::plateau_return:

			if (argc < 4)
				return;
            
			atom_setfloat(output_params + 0, params_time(argv++, false));
			atom_setfloat(output_params + 1, params_time(argv++, true));
            
            val1 = params_val(argv++, last_val);
            val2 = params_val(argv++, last_val);
            val3 = val2;
            val4 = val1;
			
            if (argc >= 7)
                curves[0].params(argv);
			if (argc >= 10)
                curves[2].params(argv + 3);
			break;
			
        case gesture_type::plateau:
						
			if (argc < 5)
				return;
            
			atom_setfloat(output_params + 0, params_time(argv++, false));
			atom_setfloat(output_params + 1, params_time(argv++, true));
			
            val1 = params_val(argv++, last_val);
            val2 = params_val(argv++, last_val);
            val3 = val2;
            val4 = params_val(argv++, last_val);
			
            if (argc >= 8)
                curves[0].params(argv);
			if (argc >= 11)
                curves[2].params(argv + 3);
			break;
						
        case gesture_type::general_return:
			
			if (argc < 5)
				return;
            
			atom_setfloat(output_params + 0, params_time(argv++, false));
			atom_setfloat(output_params + 1, params_time(argv++, true));
            
            val1 = params_val(argv++, last_val);
			val2 = params_val(argv++, last_val);
            val3 = params_val(argv++, last_val);
            val4 = val1;
            
            if (argc >= 8)
                curves[0].params(argv);
            if (argc >= 11)
                curves[1].params(argv + 3);
            if (argc >= 14)
                curves[2].params(argv + 6);
			break;
					
        case gesture_type::general:
			
			if (argc < 6)
				return;
            
			atom_setfloat(output_params + 0, params_time(argv++, false));
			atom_setfloat(output_params + 1, params_time(argv++, true));
		
            val1 = params_val(argv++, last_val);
            val2 = params_val(argv++, last_val);
            val3 = params_val(argv++, last_val);
            val4 = params_val(argv++, last_val);
            
			if (argc >= 9)
                curves[0].params(argv);
			if (argc >= 12)
                curves[1].params(argv + 3);
			if (argc >= 15)
                curves[2].params(argv + 6);
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
}
