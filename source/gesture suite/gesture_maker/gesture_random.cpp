
/*
 *  gesture_random.cpp
 *
 *  This object deals with the randomisation of values based on a specified band or bands.
 *
 *  This class also stores the parameters for a given division of bands.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "gesture_random.hpp"

#include <random_generator.hpp>

#include <algorithm>


// Random number generator

htl::random_generator<> rand_gen;

// Return a band number from a specifing atom (choosing randomly when a range is given)

int random_band(t_object *x, t_atom *specifier)
{
    // N.B. - we avoid values that we know are out of range here - just in case
    
    if (atom_gettype(specifier) == A_LONG)
        return std::max(-1000, std::min(1000, static_cast<int>(atom_getlong(specifier))));
    
    if (atom_gettype(specifier) == A_SYM)
    {
        int lo = 0;
        int hi = 0;
        
        if (sscanf(atom_getsym(specifier)->s_name, "%d-%d", &lo, &hi) == 2)
            return rand_gen.rand_int(lo, hi);
    }
    
    return 0;
}

// Calculate an exact value from a given band

double gesture_random::band_to_val(t_object *x, int band_in) const
{
    constexpr double gauss_dev = 0.33;
    constexpr double width_val = 0.55;
    
    const double band = band_in - 0.5;
    const double lo = std::max(0.0, std::min(m_band_space, band - width_val + m_end_ratio));
    const double hi = std::max(0.0, std::min(m_band_space, band + width_val + m_end_ratio));
    
    // Interpolate a random value into bandspace
    
    const double r = lo + rand_gen.rand_windowed_gaussian(0.5, gauss_dev) * (hi - lo);
    
    // Complete conversion to final value
    
    return r * m_convert_val + m_lo;
}

// Calculate a specific value given a t_atom specifier

double gesture_random::specifier_to_val(t_object *x, t_atom *specifier) const
{
    if (atom_gettype(specifier) == A_FLOAT)
        return atom_getfloat(specifier);
    else
        return band_to_val(x, random_band(x, specifier));
}
