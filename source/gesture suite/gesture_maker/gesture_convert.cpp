
/*
 *  gesture_maker_convert.cpp
 *
 *  This object deals with converts the output of a kernel from normalised range to the final output range and scaling.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "gesture_convert.hpp"

#include <algorithm>


// Symbols

t_symbol *ps_scale = gensym("scale");
t_symbol *ps_log = gensym("log");
t_symbol *ps_amp = gensym("amp");
t_symbol *ps_pitch = gensym("pitch");
t_symbol *ps_none = gensym("none");

t_symbol *ps_Scale = gensym("Scale");
t_symbol *ps_Log = gensym("Log");
t_symbol *ps_Amp = gensym("Amp");
t_symbol *ps_Pitch = gensym("Pitch");
t_symbol *ps_None = gensym("None");

// Scale a value accoring to the range and scaling specified

double gesture_convert::operator()(double val)
{
    auto clip = [&](double x) { return std::max(m_min, std::min(m_max, x)); };
    
    switch (m_mode)
    {
        case conversion_mode::none:         return val;
        case conversion_mode::linear:       return clip((val * m_mult) - m_subtract);
        case conversion_mode::log_in:       return clip(exp(val * m_mult - m_subtract));
        
        default:                            return val;
    }
}

// Routine for setting the parameters of the conversion

void gesture_convert::params(t_object *x, long argc, t_atom *argv)
{
    if (argc < 1)
        return;
    
    t_symbol *mode_sym = atom_getsym(argv++);
    
    // Here we convert captialised symbols, so as to allow capitalisation (which is needed for backward compatibility)
    
    mode_sym = (mode_sym == ps_Scale) ? ps_scale : mode_sym;
    mode_sym = (mode_sym == ps_Log)   ? ps_log   : mode_sym;
    mode_sym = (mode_sym == ps_Amp)   ? ps_amp   : mode_sym;
    mode_sym = (mode_sym == ps_Pitch) ? ps_pitch : mode_sym;
    mode_sym = (mode_sym == ps_None)  ? ps_none : mode_sym;

    if (mode_sym == ps_none || argc < 3)
    {
        if (mode_sym != ps_none)
            object_error(x, "not enough values for conversion parameter change");
        
        *this = gesture_convert();
        return;
    }
    
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
