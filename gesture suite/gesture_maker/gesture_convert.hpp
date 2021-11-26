
/*
 *  gesture_convert.h
 *
 *	A header file for the value conversion section of the gesture_maker object.
 *	This code deals with converting the output of each kernel from values normalised 0 to 1 to the final output range and scaling.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#ifndef _GESTURE_CONVERT_HPP_
#define _GESTURE_CONVERT_HPP_


#include <ext.h>


// Conversion Class

class gesture_convert
{
    // Conversion Mode
    
    enum class conversion_mode
    {
        none = 0,
        linear = 1,
        log_in = 2,
    };
    
public:
    
    // Main Methods
    
    static void setup();

    gesture_convert()
    : m_mode(conversion_mode::none), m_mult(1.0), m_subtract(0.0), m_min(0.0), m_max(1.0) {}
        
    double operator()(double val);

    void params(t_object *x, long argc, t_atom *argv);
    
private:
    
    // Variables
    
    conversion_mode m_mode;
    
    double m_mult;
    double m_subtract;
    double m_min;
    double m_max;
};

#endif /* _GESTURE_CONVERT_HPP_ */
