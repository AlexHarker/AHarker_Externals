
#include "gesture_curve.hpp"
#include "gesture_random.hpp"

#include <algorithm>
#include <cmath>


// Random Band Parameters for Curves

const gesture_random pow_curve_params(5, std::log(1.0), std::log(15.0), 0.6);
constexpr gesture_random scurve_params(5, 0.0, 1.0, 0.1);

// Curve Helpers

double gesture_curve::scurve_sin(double val) const
{
    const double s = 0.5 + 0.5 * (sin(M_PI * (val - 0.5)));
    
    return val + (m_scurve * (s - val));
}

double gesture_curve::scurve_asin(double val) const
{
    constexpr double PI_RECIP = 0.31830988618379067154;
    
    const double s = 0.5 + ((asin(2.0 * (val - 0.5))) * PI_RECIP);
    
    return val + (m_scurve * (s - val));
}

// Apply Curvature

double gesture_curve::operator()(double val) const
{
    // Calculate the curved value - There are 3 basic type options, each with two choices
    
    // 1 - use power curvature using power val, or reciprocal
    // 2 - use s curvature based on sin calculation or based asin calculation
    // 3 - use the values as calulated or reverse the values before and after the curvature operations
    
    switch (m_type)
    {
        case curve_type::power_sin_forward:     return scurve_sin(pow(val, m_power));
        case curve_type::power_sin_reverse:     return 1.0 - scurve_sin(pow((1.0 - val), m_power));
        case curve_type::recip_sin_reverse:     return 1.0 - scurve_sin(pow((1.0 - val), 1.0 / m_power));
        case curve_type::recip_sin_forward:     return scurve_sin(pow(val, 1.0 / m_power));
        case curve_type::power_asin_forward:    return scurve_asin(pow(val, m_power));
        case curve_type::power_asin_reverse:    return 1.0 - scurve_asin(pow((1.0 - val), m_power));
        case curve_type::recip_asin_reverse:    return 1.0 - scurve_asin(pow((1.0 - val), 1.0 / m_power));
        case curve_type::recip_asin_forward:    return scurve_asin(pow(val, 1.0 / m_power));
    }
}

// Parameter Setting

gesture_curve::curve_type gesture_curve::get_type(int type) const
{
    switch (type)
    {
        case 1:     return curve_type::power_sin_reverse;
        case 2:     return curve_type::recip_sin_reverse;
        case 3:     return curve_type::recip_sin_forward;
        case 4:     return curve_type::power_asin_forward;
        case 5:     return curve_type::power_asin_reverse;
        case 6:     return curve_type::recip_asin_reverse;
        case 7:     return curve_type::recip_asin_forward;
        default:    return curve_type::power_sin_forward;
    }
}

void gesture_curve::params(t_atom *specifiers, long argc)
{
    if (argc < 3)
        return;
    
    const double s = scurve_params.specifier_to_val(specifiers + 1);
    
    m_scurve = pow(std::max(0.0, std::min(1.0, s)), 0.35);
    
    if (atom_gettype(specifiers + 2) == A_FLOAT)
    {
        m_power = std::max(1.0, atom_getfloat(specifiers + 2));
        m_type = get_type(random_band(specifiers));
    }
    else
    {
        // Get band
        
        int band = random_band(specifiers + 2) - (pow_curve_params.num_bands() - 1);
        int band_alter = band > 0 ? 1 : 0;
        
        band = abs(band) - band_alter;
        
        m_power = std::max(1.0, exp(pow_curve_params.band_to_val(band)));
        m_type = get_type(random_band(specifiers) * 2 + band_alter);
    }
}
