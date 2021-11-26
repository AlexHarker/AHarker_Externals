
#ifndef _GESTURE_RANDOM_HPP_
#define _GESTURE_RANDOM_HPP_

#include <ext.h>

class gesture_random
{
public:
    
    constexpr gesture_random(int n_bands, double lo, double hi, double end)
    : num_bands(n_bands), lo_val(lo), hi_val(hi), end_ratio(end)
    {}
    
    double band_to_val(int band_in) const;
    
    int n_bands() const { return num_bands; }
    
private:
    
    const int num_bands;
    
    const double lo_val;
    const double hi_val;
    const double end_ratio;
};

int random_band(t_atom *specifier);

#endif /* _GESTURE_RANDOM_HPP_ */
