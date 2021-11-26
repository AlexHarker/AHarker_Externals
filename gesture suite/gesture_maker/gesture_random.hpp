
#ifndef _GESTURE_RANDOM_HPP_
#define _GESTURE_RANDOM_HPP_

#include <ext.h>

// Random Band (from specifier)

int random_band(t_atom *specifier);


class gesture_random
{
public:
    
    constexpr gesture_random(int n_bands, double lo, double hi, double end)
    : num_bands(n_bands)
    , lo_val(lo)
    , hi_val(hi)
    , end_ratio(end)
    , band_space(num_bands - 2.0 + (2.0 * end_ratio))
    , convert_val((hi_val - lo_val) / band_space)
    {}
    
    double band_to_val(int band_in) const;
    double specifier_to_val(t_atom *specifier) const;

    int n_bands() const { return num_bands; }
    
private:
    
    const int num_bands;
    
    const double lo_val;
    const double hi_val;
    const double end_ratio;
    const double band_space;
    const double convert_val;
};

#endif /* _GESTURE_RANDOM_HPP_ */
