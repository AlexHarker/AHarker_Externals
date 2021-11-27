
/*
 *  gesture_maker_convert.c
 *
 *  This code deals with converting the output of each kernel from values normalised 0 to 1 to the final output range and scaling.
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

double gesture_convert::operator()(double val)
{
    auto clip = [&](double x) { return std::max(m_min, std::min(m_max, x)); };
    
    switch (m_mode)
    {
        case conversion_mode::none:         return val;
        case conversion_mode::linear:       return clip((val * m_mult) - m_subtract);
        case conversion_mode::log_in:       return clip(exp(val * m_mult - m_subtract));
    }
}

// Routine for setting the parameters of the conversion

void gesture_convert::params(t_object *x, long argc, t_atom *argv)
{
    if (argc < 1)
        return;
    
    t_symbol *mode_sym = atom_getsym(argv++);
    
    if (mode_sym == ps_none || mode_sym == ps_None || argc < 3)
    {
        if (argc < 3)
            object_error(x, "not enough values for conversion parameter change");
        
        *this = gesture_convert();
        return;
    }
    
    // Here we convert captialised symbols, so as to allow capitalisation (which is needed for backward compatibility)
    
    mode_sym = (mode_sym == ps_Scale) ? ps_scale : mode_sym;
    mode_sym = (mode_sym == ps_Log)   ? ps_log   : mode_sym;
    mode_sym = (mode_sym == ps_Amp)   ? ps_amp   : mode_sym;
    mode_sym = (mode_sym == ps_Pitch) ? ps_pitch : mode_sym;
    
    // We can either just specify min and max out, or also specify input range (again for backwards compatibility)
    
    const double min_in = (argc < 5) ? 0.0 : atom_getfloat(argv++);
    const double max_in = (argc < 5) ? 1.0 : atom_getfloat(argv++);
    double min_out = atom_getfloat(argv++);
    double max_out = atom_getfloat(argv++);
    
    if (mode_sym == ps_log || mode_sym == ps_amp || mode_sym == ps_pitch)
        m_mode = conversion_mode::log_in;
    else
        m_mode = conversion_mode::linear;
    
    if (mode_sym == ps_amp)
    {
        min_out = pow(10.0, min_out / 20.0);
        max_out = pow(10.0, max_out / 20.0);
    }
    else if (mode_sym == ps_pitch)
    {
        min_out = pow(2.0, min_out / 12.0);
        max_out = pow(2.0, max_out / 12.0);
    }
    
    m_min = min_out;
    m_max = max_out;
    
    if (m_mode == conversion_mode::log_in)
    {
        min_out = log(min_out);
        max_out = log(max_out);
    }
    
    m_mult = (max_out - min_out) / (max_in - min_in);
    m_subtract = (min_in * m_mult) - min_out;
    
    if (m_mode == conversion_mode::linear && mode_sym != ps_scale)
        object_error(x, "gesture_maker: unknown conversion type - defaulting to scale");
}
