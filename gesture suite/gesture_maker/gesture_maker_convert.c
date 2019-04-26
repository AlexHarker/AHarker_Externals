
/*
 *  gesture_maker_convert.c
 *
 *	This code deals with converting the output of each kernel from values normalised 0 to 1 to the final output range and scaling.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include "gesture_maker_convert.h"

// Symbols

t_symbol *ps_scale;
t_symbol *ps_log;
t_symbol *ps_amp;
t_symbol *ps_pitch;
t_symbol *ps_none;

t_symbol *ps_Scale;
t_symbol *ps_Log;
t_symbol *ps_Amp;
t_symbol *ps_Pitch;
t_symbol *ps_None;

// Setup the symbol variables

void gesture_maker_convert_setup()
{
	ps_scale = gensym("scale");
	ps_log = gensym("log");
	ps_amp = gensym("amp");
	ps_pitch = gensym("pitch");
	ps_none = gensym("none");
	
	ps_Scale = gensym("Scale");
	ps_Log = gensym("Log");
	ps_Amp = gensym("Amp");
	ps_Pitch = gensym("Pitch");
	ps_None = gensym("None");
}

// Initialise the convertor to default values

void gesture_maker_convert_init(t_gesture_maker_convert *x)
{
	x->mode = 0;
	x->min = 0;
	x->max = 1;
	x->mult = 1;
	x->subtract = 0;
}

// Scale a value accoring to the range and scaling specified

double gesture_maker_convert_scale(t_gesture_maker_convert *x, double input)
{
	double scaled = 0.;
	double max = x->max;
	double min = x->min;
	
	switch (x->mode)
	{
		case CONVERT_NONE:
			
			// No scaling
			
			return input;
			
		case CONVERT_LINEAR:
			
			// Linear scaling
			
			scaled = (input * x->mult) - x->subtract;
			break;
			
		case CONVERT_LOG_IN:
			
			// Exponential scaling (for a logarithmic input scaling)
			
			scaled = exp(input * x->mult - x->subtract);
			break;
	}
	
	// Clip output
	
	if (scaled > max)
		scaled = max;
	if (scaled < min)
		scaled = min;
	
	return scaled;
}

// Routine for setting the parameters of the conversion

void gesture_maker_convert_params(t_gesture_maker_convert *x, long argc, t_atom *argv)
{
	double min_in;
	double max_in;
	double min_out;
	double max_out;
	double subtract;
	double mult;
	double min;
	double max;
	
	t_conversion_mode mode = CONVERT_LINEAR;
	
	t_symbol *mode_sym;

	if (argc < 1)
		return;
	
	mode_sym = atom_getsym(argv++);
	
	if (mode_sym == ps_none || mode_sym == ps_None)
	{
		x->mode = CONVERT_NONE;
		return;
	}
	
	if (argc < 3)
	{
		object_error((t_object *)x, "not enough values for conversion parameter change");
		return;
	}
	
	// Here we convert captialised symbols, so as to allow capitalisation (which is needed for backward compatibility)
	
	if (mode_sym == ps_Scale) 
		mode_sym = ps_scale;
	if (mode_sym == ps_Log) 
		mode_sym = ps_log;
	if (mode_sym == ps_Amp) 
		mode_sym = ps_amp;
	if (mode_sym == ps_Pitch) 
		mode_sym = ps_pitch;

	// We can either just specify min and max out, or also specify input range (again for backwards compatibility)
	
	if (argc < 5)
	{
		min_in = 0;
		max_in = 1;
		min_out = atom_getfloat(argv++);
		max_out = atom_getfloat(argv++);
	}
	else
	{
		min_in = atom_getfloat(argv++);
		max_in = atom_getfloat(argv++);
		min_out = atom_getfloat(argv++);
		max_out = atom_getfloat(argv++);
	}

	if (mode_sym == ps_log || mode_sym == ps_amp || mode_sym == ps_pitch)
		mode = CONVERT_LOG_IN;
	
	if (mode_sym == ps_amp)
	{
		min_out = pow (10, min_out / 20.);
		max_out = pow (10, max_out / 20.);
	}

	if (mode_sym == ps_pitch)
	{
		min_out = pow(2, min_out / 12.);
		max_out = pow(2, max_out / 12.);
	}	
	
	min = min_out;
	max = max_out;
	
	if (mode == CONVERT_LOG_IN)
	{
		min_out = log(min_out);
		max_out = log(max_out);
	}
	
	mult = (max_out - min_out) / (max_in - min_in);
	subtract = (min_in * mult) - min_out;
	
	if (mode == CONVERT_LINEAR && mode_sym != ps_scale)
		object_error((t_object *)x, "gesture_maker: unknown conversion type - defaulting to scale");

	x->mode = mode;
	x->mult = mult;
	x->min = min;
	x->max = max;
	x->subtract = subtract;
}

