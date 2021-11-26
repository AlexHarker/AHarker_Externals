
/*
 *  gesture_maker_convert.c
 *
 *	This code deals with converting the output of each kernel from values normalised 0 to 1 to the final output range and scaling.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include "gesture_convert.hpp"

#include <algorithm>


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

void gesture_convert::setup()
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

// Scale a value accoring to the range and scaling specified

double gesture_convert::operator()(double input)
{
    auto clip = [&](double x)
    {
        return std::max(min, std::min(max, x));
    };
    
	switch (mode)
	{
        case conversion_mode::none:
			return input;
			
        case conversion_mode::linear:
			return clip((input * mult) - subtract);
    
        case conversion_mode::log_in:
            return clip(exp(input * mult - subtract));
	}
}

// Routine for setting the parameters of the conversion

void gesture_convert::params(t_object *x, long argc, t_atom *argv)
{
	double min_in;
	double max_in;
	double min_out;
	double max_out;
		
	t_symbol *mode_sym;

	if (argc < 1)
		return;
	
	mode_sym = atom_getsym(argv++);
	
	if (mode_sym == ps_none || mode_sym == ps_None)
	{
		mode = conversion_mode::none;
		return;
	}
	
	if (argc < 3)
	{
		object_error(x, "not enough values for conversion parameter change");
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
		mode = conversion_mode::log_in;
    else
        mode = conversion_mode::linear;
	
	if (mode_sym == ps_amp)
	{
		min_out = pow(10.0, min_out / 20.0);
		max_out = pow(10.0, max_out / 20.0);
	}

	if (mode_sym == ps_pitch)
	{
		min_out = pow(2.0, min_out / 12.0);
		max_out = pow(2.0, max_out / 12.0);
	}	
	
	min = min_out;
	max = max_out;
	
	if (mode == conversion_mode::log_in)
	{
		min_out = log(min_out);
		max_out = log(max_out);
	}
	
	mult = (max_out - min_out) / (max_in - min_in);
	subtract = (min_in * mult) - min_out;
	
	if (mode == conversion_mode::linear && mode_sym != ps_scale)
		object_error(x, "gesture_maker: unknown conversion type - defaulting to scale");
}
