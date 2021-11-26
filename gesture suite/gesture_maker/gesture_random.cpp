
#include "gesture_random.hpp"

#include <RandomGenerator.hpp>

// Random number generator

random_generator<> rand_gen;

// Calculate an exact value from a given band and a set of band parameters

double gesture_random::band_to_val(int band_in) const
{
    constexpr double gauss_dev = 0.33;
    constexpr double width_val = 0.55;
    
    // Find lo and hi values for this band
    
    double band_space = num_bands - 2.0 + (2.0 * end_ratio);
    double convert_val = (hi_val - lo_val) / band_space;
    
    double band = band_in - 0.5;
    double rand_val = rand_gen.rand_windowed_gaussian(0.5, gauss_dev);
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
    
    return rand_val * convert_val + lo_val;
}

// Return a band number from a specifing atom (choosing randomly when a range is given)

int random_band(t_atom *specifier)
{
    if (atom_gettype(specifier) == A_LONG)
    {
        // N.B. - we avoid values that we know are out of range here - just in case
        
        t_atom_long a = atom_getlong(specifier);
        
        return (a > -1000) ? ((a < 1000) ? static_cast<int>(a) : 1000) : -1000;
    }
    
    if (atom_gettype(specifier) == A_SYM)
    {
        int lo = 0;
        int hi = 0;
        
        if (sscanf(atom_getsym(specifier)->s_name, "%d-%d", &lo, &hi) == 2)
            return rand_gen.rand_int(lo, hi);
    }
    
    return 0;
}
