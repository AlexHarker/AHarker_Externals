
#ifndef _GESTURE_CURVE_HPP_
#define _GESTURE_CURVE_HPP_

#include <ext.h>

// Curvature Class

class gesture_curve
{
    // Curve Type
    
    enum class curve_type
    {
        power_sin_forward,
        power_sin_reverse,
        recip_sin_reverse,
        recip_sin_forward,
        power_asin_forward,
        power_asin_reverse,
        recip_asin_reverse,
        recip_asin_forward
    };
    
public:
    
    // Main Methods
    
    gesture_curve() : m_power(1.0), m_scurve(0.0), m_type(curve_type::power_sin_forward) {}
    
    void reset() { *this = gesture_curve(); }
    
    double operator()(double val) const;

    void params(t_atom *specifiers);
    
private:
    
    // Helpers
    
    curve_type get_type(int band) const;
    
    double scurve_sin(double val) const;
    double scurve_asin(double val) const;

    // Curvature Values
    
    double m_power;
    double m_scurve;
    curve_type m_type;
};

#endif /* _GESTURE_CURVE_HPP_ */
