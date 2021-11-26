
#include "gesture_curve.hpp"
#include "gesture_random.hpp"

#include <algorithm>
#include <cmath>

// N.B the limits for the pow_curve params are log(1.0) and log(15.0)

const gesture_random pow_curve_params(5, std::log(1.0), std::log(15.0), 0.6);
//constexpr band_parameters pow_curve_params(5, 0.0, std::log(15.0), 0.6);
constexpr gesture_random scurve_params(5, 0.0, 1.0, 0.1);

void gesture_curve::reset()
{
    power_val = 1.0;
    scurve_val = 0.0;
    type = curve_type::power_sin_forward;
}

gesture_curve::curve_type gesture_curve::get_type(int type)
{
    switch (type)
    {
        case 1:     return curve_type::power_sin_reverse;
        case 2:     return curve_type::power_recip_sin_reverse;
        case 3:     return curve_type::power_recip_sin_forward;
        case 4:     return curve_type::power_asin_forward;
        case 5:     return curve_type::power_asin_reverse;
        case 6:     return curve_type::power_recip_asin_reverse;
        case 7:     return curve_type::power_recip_asin_forward;
        default:    return curve_type::power_sin_forward;
    }
}

void gesture_curve::params(t_atom *specifiers)
{
    double s = 0.0;
    
    if (atom_gettype(specifiers + 1) == A_FLOAT)
        s = atom_getfloat(specifiers + 1);
    else
        s = scurve_params.band_to_val(random_band(specifiers + 1));
    
    scurve_val = pow(std::max(0.0, std::min(1.0, s)), 0.35);

    if (atom_gettype(specifiers + 2) == A_FLOAT)
    {
        power_val = std::max(1.0, atom_getfloat(specifiers + 2));
        type = get_type(random_band(specifiers));
    }
    else
    {
        // Get band
        
        int band = random_band(specifiers + 2) - (pow_curve_params.n_bands() - 1);
        int band_alter = 0;

        if (band > 0)
            band_alter = 1;
        
        band = abs(band) - band_alter;
        
        power_val = std::max(1.0, exp(pow_curve_params.band_to_val(band)));
        type = get_type(random_band(specifiers) * 2 + band_alter);
    }
}

double gesture_curve::scurve_sin(double val) const
{
    const double s = 0.5 + 0.5 * (sin(M_PI * (val - 0.5)));
    
    return val + (scurve_val * (s - val));
}

double gesture_curve::scurve_asin(double val) const
{
    constexpr double PI_RECIP = 0.31830988618379067154;

    const double s = 0.5 + ((asin(2.0 * (val - 0.5))) * PI_RECIP);
    
    return val + (scurve_val * (s - val));
}

// Apply curvature to one of the three parts of the gesture kernel

double gesture_curve::operator()(double val)
{
    // Calculate the curved value
    // There are 3 basic mode options, each with two choices
    //
    // 1 - use power curvature using power val, or reciprocal
    // 2 - use s curvature based on sin calculation or based asin calculation
    // 3 - use the values as calulated or reverse the values before and after the curvature operations
    //
    // This results in the eight types given below
    
    switch (type)
    {
        case curve_type::power_sin_forward:
            return scurve_sin(pow(val, power_val));
            
        case curve_type::power_sin_reverse:
            return 1.0 - scurve_sin(pow((1.0 - val), power_val));

        case curve_type::power_recip_sin_reverse:
            return 1.0 - scurve_sin(pow((1.0 - val), 1.0 / power_val));

        case curve_type::power_recip_sin_forward:
            return scurve_sin(pow(val, 1.0 / power_val));

        case curve_type::power_asin_forward:
            return scurve_asin(pow(val, power_val));

        case curve_type::power_asin_reverse:
            return 1.0 - scurve_asin(pow((1.0 - val), power_val));

        case curve_type::power_recip_asin_reverse:
            return 1.0 - scurve_asin(pow((1.0 - val), 1.0 / power_val));

        case curve_type::power_recip_asin_forward:
            return scurve_asin(pow(val, 1.0 / power_val));
    }
}
