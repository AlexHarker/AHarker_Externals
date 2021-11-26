
/*
 *  gesture_maker_kernel.h
 *
 *	A header file for the kernel section of the gesture_maker object.
 *	This code deals with storing gestures, resolving variable parameters and calculating output values for either the main or inflection gestures.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#ifndef _GESTURE_KERNEL_HPP_
#define _GESTURE_KERNEL_HPP_

#include <ext.h>

#include "gesture_curve.hpp"


class gesture_kernel
{
    enum class gesture_type
    {
        flat,
        line,
        line_flat,
        flat_line,
        triangle_return,
        triangle,
        plateau_return,
        plateau,
        general_return,
        general
    };

public:
    
    static void setup();

    void reset();

    void initial(double val);
    void initial_specifier(t_atom *specifier);
    void params(long argc, t_atom *argv);
    
    double operator()(double val);

private:
    
    gesture_type get_type(t_symbol *type);

    double params_val(t_atom *specifier, double last_val);
    double params_time(t_atom *specifier, bool reverse);
    
	// Time Points
	
	double time1;
	double time2;
    
    // Values
    
	double val1;
	double val2;
	double val3;
	double val4;
	
    // Curve values
	
    gesture_curve curves[3];

    // Last value
    
    double last_val;
};

#endif /* _GESTURE_KERNEL_HPP_ */
