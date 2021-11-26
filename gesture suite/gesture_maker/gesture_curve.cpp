
#include "gesture_curve.hpp"
#include "gesture_random.hpp"

#include <cmath>

// PI defines

#define PI          3.14159265358979323846
#define PI_RECIP    0.31830988618379067154

// N.B the limits for the pow_curve params are og(1.0) and log(15.0)

const band_parameters pow_curve_params(5, std::log(1.0), std::log(15.0), 0.6);
//constexpr band_parameters pow_curve_params(5, 0.0, std::log(15.0), 0.6);
constexpr band_parameters scurve_params(5, 0.0, 1.0, 0.1);

void curve_parameters::reset()
{
    power_val = 1.0;
    scurve_val = 0.0;
    type = curve_type::power_sin_forward;
}

curve_parameters::curve_type curve_parameters::get_type(t_atom *specifier)
{
    t_atom_long type = atom_getlong(specifier);
    
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

void curve_parameters::params(t_atom *specifiers)
{
    power_val = atom_getfloat(specifiers + 0);
    power_val = power_val < 1.0 ? 1.0 : power_val;
    
    scurve_val = atom_getfloat(specifiers + 1);
    scurve_val = scurve_val < 0.0 ? 0.0 : scurve_val;
    scurve_val = scurve_val > 1.0 ? 1.0 : scurve_val;
    scurve_val = pow(scurve_val, 0.35);
    
    type = get_type(specifiers + 2);
}

// Calculate curve parameters based on given specifiers

void curve_parameters::params_curve(t_atom *curve_params, t_atom *specifiers)
{
    int band_alter = 0;
    
    if (atom_gettype(specifiers + 1) == A_FLOAT)
        curve_params[1] = specifiers[1];
    else
        atom_setfloat(curve_params + 1, scurve_params.band_to_val(random_band(specifiers + 1)));
    
    if (atom_gettype(specifiers + 2) == A_FLOAT)
    {
        curve_params[0] = specifiers[2];
        atom_setlong(curve_params + 2, random_band(specifiers));
    }
    else
    {
        // Get band
        
        int band = random_band(specifiers + 2) - (pow_curve_params.n_bands() - 1);
        
        if (band > 0)
            band_alter = 1;
        
        band = abs(band) - band_alter;
        
        atom_setfloat(curve_params, exp(pow_curve_params.band_to_val(band)));
        atom_setlong(curve_params + 2, random_band(specifiers) * 2 + band_alter);
    }
}

// Apply curvature to one of the three parts of the gesture kernel

double curve_parameters::operator()(double val)
{
    double power_curve, scurve;
        
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
            power_curve = pow(val, power_val);
            scurve =  0.5 + 0.5 * (sin(PI * (power_curve - 0.5)));
            return power_curve + (scurve_val * (scurve - power_curve));
            
        case curve_type::power_sin_reverse:
            power_curve = pow((1.0 - val), power_val);
            scurve =  0.5 + 0.5 * (sin(PI * (power_curve - 0.5)));
            return 1.0 - (power_curve + (scurve_val * (scurve - power_curve)));
            
        case curve_type::power_recip_sin_reverse:
            power_curve = pow((1.0 - val), 1.0 / power_val);
            scurve =  0.5 + 0.5 * (sin(PI * (power_curve - 0.5)));
            return 1.0 - (power_curve + (scurve_val * (scurve - power_curve)));
            
        case curve_type::power_recip_sin_forward:
            power_curve = pow(val, 1.0 / power_val);
            scurve =  0.5 + 0.5 * (sin(PI * (power_curve - 0.5)));
            return power_curve + (scurve_val * (scurve - power_curve));
            
        case curve_type::power_asin_forward:
            power_curve = pow(val, power_val);
            scurve =  0.5 + ((asin(2 * (power_curve - 0.5))) * PI_RECIP);
            return power_curve + (scurve_val * (scurve - power_curve));
            
        case curve_type::power_asin_reverse:
            power_curve = pow((1.0 - val), power_val);
            scurve =  0.5 + ((asin(2 * (power_curve - 0.5))) * PI_RECIP);
            return 1.0 - (power_curve + (scurve_val * (scurve - power_curve)));
            
        case curve_type::power_recip_asin_reverse:
            power_curve = pow((1.0 - val), 1.0 / power_val);
            scurve =  0.5 + ((asin(2 * (power_curve - 0.5))) * PI_RECIP);
            return 1.0 - (power_curve + (scurve_val * (scurve - power_curve)));
            
        case curve_type::power_recip_asin_forward:
            power_curve = pow(val, 1.0 / power_val);
            scurve =  0.5 + ((asin(2 * (power_curve - 0.5))) * PI_RECIP);
            return power_curve + (scurve_val * (scurve - power_curve));
    }
}
