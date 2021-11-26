
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


class gesture_convert
{
    enum class conversion_mode
    {
        none = 0,
        linear = 1,
        log_in = 2,
    };
    
public:
    
    static void setup();

    gesture_convert(): mode(conversion_mode::none), mult(1.0), subtract(0.0), min(0.0), max(1.0) {}
        
    double operator()(double input);

    void params(t_object *x, long argc, t_atom *argv);
    
private:
    
    conversion_mode mode;
    
    double mult;
    double subtract;
    double min;
    double max;
};

#endif /* _GESTURE_CONVERT_HPP_ */
