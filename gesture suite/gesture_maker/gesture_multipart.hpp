
/*
 *  gesture_maker_multipart.h
 *
 *	A header file for the multipart section of the gesture_maker object.
 *	This code deals with the multipart nature of the inflection kernel.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#ifndef _GESTURE_MULTIPART_HPP_
#define _GESTURE_MULTIPART_HPP_

#include <ext.h>

// FIX - defines

#define MAX_NUM_KERNEL_PARAMS   16L
#define MAX_NUM_KERNELS         64
#define MAX_NUM_SPLITS          63

// Multipart Class

class gesture_multipart
{
public:
    
    // Main Methods
    
    gesture_multipart()
    : m_num_splits(0)
    , m_num_kernels(0)
    , m_current_kernel(0)
    , m_lo_phase(0.0)
    , m_hi_phase(1.0)
    , m_range_recip(1.0)
    , m_last_phase(1.0)
    , m_force_update(true)
    {}
    
    void reset();
    
    void initial(double val)                    { m_kernel.initial(val); }
    void initial_specifier(t_atom *specifier)   { m_kernel.initial_specifier(specifier); }
    
    double operator()(double phase);

    void params(long argc, t_atom *argv);
    void timings(long argc, t_atom *argv);
        
private:
    
    // Kernel
    
    gesture_kernel m_kernel;

    // Split Values
    
	double m_split_points[MAX_NUM_SPLITS];
    long m_num_splits;
    
    // Kernel Parameters
    
    t_atom m_kernel_params[MAX_NUM_KERNELS * MAX_NUM_KERNEL_PARAMS];
	long m_kernel_param_count[MAX_NUM_KERNELS];
    long m_num_kernels;
    long m_current_kernel;
    
    // Current Phase
    
	double m_lo_phase;
    double m_hi_phase;
	double m_range_recip;
	double m_last_phase;
	
    // Force Reset
    
	bool m_force_update;
};

#endif /* _GESTURE_MULTIPART_HPP_ */
