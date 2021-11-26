
#ifndef _GESTURE_RANDOM_HPP_
#define _GESTURE_RANDOM_HPP_

#include <ext.h>

// Random Band (from specifier)

int random_band(t_atom *specifier);

// Random Class (deals with banding)

class gesture_random
{
public:
    
    constexpr gesture_random(int num_bands, double lo, double hi, double end_ratio)
    : m_num_bands(num_bands)
    , m_lo(lo)
    , m_hi(hi)
    , m_end_ratio(end_ratio)
    , m_band_space(num_bands - 2.0 + (2.0 * end_ratio))
    , m_convert_val((m_hi - m_lo) / m_band_space)
    {}
    
    double band_to_val(int band_in) const;
    double specifier_to_val(t_atom *specifier) const;

    int num_bands() const { return m_num_bands; }
    
private:
    
    const int m_num_bands;
    
    const double m_lo;
    const double m_hi;
    const double m_end_ratio;
    const double m_band_space;
    const double m_convert_val;
};

#endif /* _GESTURE_RANDOM_HPP_ */
