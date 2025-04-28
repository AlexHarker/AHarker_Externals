
/*
 *  gesture_maker_kernel.hpp
 *
 *  A header file for the kernel class of the gesture_maker object.
 *
 *  This object deals with storing gestures, resolving parameters and calculating output values for a kernel.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#ifndef _GESTURE_KERNEL_HPP_
#define _GESTURE_KERNEL_HPP_

#include <ext.h>

#include "gesture_curve.hpp"


// Kernel Class

class gesture_kernel
{
    // Gesture Type
    
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
    
    // Main Methods
        
    gesture_kernel()
    : m_time1(0.0)
    , m_time2(1.0)
    , m_val1(0.0)
    , m_val2(0.0)
    , m_val3(1.0)
    , m_val4(1.0)
    , m_last_val(0.0)
    {}
    
    void reset() { *this = gesture_kernel(); }
    
    void initial(double val)                                 { m_last_val = val; };
    void initial_specifier(t_object *x, t_atom *specifier)   { m_last_val = params_val(x, specifier); }
    
    void params(t_object *x, long argc, t_atom *argv);
    
    double operator()(double val);
    
private:
    
    // Helpers
    
    gesture_type get_type(t_object *x, t_symbol *type);
    
    double params_time(t_object *x, t_atom *specifier);
    double params_val(t_object *x, t_atom *specifier);
    
    double calc(double val, double t1, double t2, double v1, double v2, gesture_curve& curve);
    
    // Time Points
    
    double m_time1;
    double m_time2;
    
    // Values
    
    double m_val1;
    double m_val2;
    double m_val3;
    double m_val4;
    
    // Curve Values
    
    gesture_curve m_curves[3];
    
    // Last Value
    
    double m_last_val;
};

#endif /* _GESTURE_KERNEL_HPP_ */
