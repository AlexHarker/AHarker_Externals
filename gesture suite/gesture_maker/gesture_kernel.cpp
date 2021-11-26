
/*
 *  gesture_kernel.c
 *
 *  This code deals with storing gestures, resolving variable parameters and calculating output values for either the main or inflection gestures.
 *  A kernel has up to 3 parts, each of which is first calculated as curve between the given output parameters.
 *  Kernel parameters may either be specified directly, or with a certain degree of "random" variability.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include "gesture_kernel.hpp"
#include "gesture_random.hpp"

#include <algorithm>


// Random Band Parameters for Times / Values

constexpr gesture_random time_params(15, 0.0, 1.0, 0.2);
constexpr gesture_random val_params(11, 0.0, 1.0, 0.2);

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

// Calculate one part of a kernel

double gesture_kernel::calc(double phase, double t1, double t2, double v1, double v2, gesture_curve& curve)
{
    // Calculate and clip the linear time value (0 to 1) within the current curve
    
    double val = (t1 == t2) ? 0.0 : std::max(0.0, std::min(1.0, (phase - t1) / (t2 - t1)));
    
    // The linear time value is now curved and inearly interpolate between the two output values
        
    return (m_last_val = v1 + curve(val) * (v2 - v1));
};

// Calculate the kernel value for a given phase

double gesture_kernel::operator()(double val)
{
    // Test which curve to use (reverse order) - note reversals for the final curve
    
    if (val > m_time2)
        return calc(val, 1.0, m_time2, m_val4, m_val3, m_curves[2]);
    
    if (val > m_time1)
        return calc(val, m_time1, m_time2, m_val2, m_val3, m_curves[1]);
    
    return calc(val, 0.0, m_time1, m_val1, m_val2, m_curves[0]);
}

// Parameter Methods

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

double gesture_kernel::params_time(t_atom *specifier)
{
    return time_params.specifier_to_val(specifier);
}

// Return an output value (0 to 1) from a specifier

double gesture_kernel::params_val(t_atom *specifier)
{
    if (atom_gettype(specifier) == A_SYM && atom_getsym(specifier) == ps_last)
        return m_last_val;
    
    return val_params.specifier_to_val(specifier);
}

// Set all the kernel parameters based on the given input atoms

void gesture_kernel::params(long argc, t_atom *argv)
{
    gesture_type type = gesture_type::flat;
    
    // Time vals
    
    m_time1 = 0.0;
    m_time2 = 1.0;
        
    // Curves
    
    m_curves[0].reset();
    m_curves[1].reset();
    m_curves[2].reset();
    
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
            
            m_val1 = m_val2 = m_val3 = m_val4 = params_val(argv++);
            break;
                    
        case gesture_type::line:
        
            if (argc < 2)
                return;
            
            m_val1 = params_val(argv++);
            m_val2 = m_val3 = m_val4 = params_val(argv++);

            m_curves[0].params(argv, argc - 2);
            break;

            
        case gesture_type::line_flat:
                        
            if (argc < 3)
                return;
            
            m_time1 = params_time(argv++);
            
            m_val1 = params_val(argv++);
            m_val2 = m_val3 = m_val4 = params_val(argv++);
            
            m_curves[0].params(argv, argc - 3);
            break;
                    
        case gesture_type::flat_line:
            
            if (argc < 3)
                return;
            
            m_time1 = params_time(argv++);
            m_time2 = m_time1;
            
            m_val1 = m_val2 = m_val3 = params_val(argv++);
            m_val4 = params_val(argv++);
            
            m_curves[2].params(argv, argc - 3);
            break;
            
        case gesture_type::triangle_return:
            
            if (argc < 3)
                return;
            
            m_time1 = params_time(argv++);
            m_time2 = m_time1;
            
            m_val1 = m_val4 = params_val(argv++);
            m_val2 = m_val3 = params_val(argv++);
            
            m_curves[0].params(argv, argc - 3);
            m_curves[2].params(argv, argc - 6);
            break;
                        
        case gesture_type::triangle:
            
            if (argc < 4)
                return;
            
            m_time1 = params_time(argv++);
            m_time2 = m_time1;
            
            m_val1 = m_val3 = params_val(argv++);
            m_val2 = params_val(argv++);
            m_val4 = params_val(argv++);
            
            m_curves[0].params(argv, argc - 4);
            m_curves[2].params(argv, argc - 7);
            break;
                
        case gesture_type::plateau_return:

            if (argc < 4)
                return;
            
            m_time1 = params_time(argv++);
            m_time2 = params_time(argv++);
            
            m_val1 = m_val4 = params_val(argv++);
            m_val2 = m_val3 = params_val(argv++);
            
            m_curves[0].params(argv, argc - 4);
            m_curves[2].params(argv, argc - 7);
            break;
            
        case gesture_type::plateau:
                        
            if (argc < 5)
                return;
            
            m_time1 = params_time(argv++);
            m_time2 = params_time(argv++);
            
            m_val1 = params_val(argv++);
            m_val2 = m_val3 = params_val(argv++);
            m_val4 = params_val(argv++);
            
            m_curves[0].params(argv, argc - 5);
            m_curves[2].params(argv, argc - 8);
            break;
                        
        case gesture_type::general_return:
            
            if (argc < 5)
                return;
            
            m_time1 = params_time(argv++);
            m_time2 = params_time(argv++);
            
            m_val1 = m_val4 = params_val(argv++);
            m_val2 = params_val(argv++);
            m_val3 = params_val(argv++);
            
            m_curves[0].params(argv, argc - 5);
            m_curves[1].params(argv, argc - 8);
            m_curves[2].params(argv, argc - 11);
            break;
                    
        case gesture_type::general:
            
            if (argc < 6)
                return;
            
            m_time1 = params_time(argv++);
            m_time2 = params_time(argv++);
        
            m_val1 = params_val(argv++);
            m_val2 = params_val(argv++);
            m_val3 = params_val(argv++);
            m_val4 = params_val(argv++);

            m_curves[0].params(argv, argc - 6);
            m_curves[1].params(argv, argc - 9);
            m_curves[2].params(argv, argc - 12);
            break;
    }
    
    // Correct time ordering if necessary
        
    if (m_time1 >  m_time2)
        std::swap(m_time1, m_time2);
}
