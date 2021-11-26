
/*
 *  gesture_maker_kernel.h
 *
 *	A header file for the kernel section of the gesture_maker object.
 *	This code deals with storing gestures, resolving variable parameters and calculating output values for either the main or inflection gestures.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>

// Curve Types

class curve_parameters
{
    enum class curve_type
    {
        power_sin_forward,
        power_sin_reverse,
        power_recip_sin_reverse,
        power_recip_sin_forward,
        power_asin_forward,
        power_asin_reverse,
        power_recip_asin_reverse,
        power_recip_asin_forward
    };
    
public:
    
    void reset();
    
    double operator()(double val);

    void params(t_atom *specifiers);

private:
    
    curve_type get_type(t_atom *specifier);
    
    // Curve values
    
    double power_val;
    double scurve_val;
    curve_type type;
};

// Kernel structure

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
    
    double params_val(t_atom *specifier, double last_val);
    double params_time(t_atom *specifier, bool reverse);
    
    gesture_type get_type(t_symbol *type);
    
	// Time Points
	
	double time1;
	double time2;
    
    // Values
    
	double val1;
	double val2;
	double val3;
	double val4;
	
    // Curve values
	
    curve_parameters curve_params[3];

    // Last value
    
    double last_val;
};

// Struture to hold the parameters relating to random bands

struct band_parameters
{
	double num_bands;
	double lo_val;
	double hi_val;
	double gauss_dev;
	double width_val;
	double end_ratio;
    
    double band_to_val(int band_in);
};
