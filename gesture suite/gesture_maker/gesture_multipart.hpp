
/*
 *  gesture_maker_multipart.h
 *
 *  A header file for the multipart section of the gesture_maker object.
 *  This code deals with the multipart nature of the inflection kernel.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#ifndef _GESTURE_MULTIPART_HPP_
#define _GESTURE_MULTIPART_HPP_

#include <ext.h>


static constexpr long max_params = 16;
static constexpr long max_kernels = 64;

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
    
    void initial(double val)                                    { m_kernel.initial(val); }
    void initial_specifier(t_object *x, t_atom *specifier)      { m_kernel.initial_specifier(x, specifier); }
    
    double operator()(t_object *x, double phase);
    
    void params(t_object *x, long argc, t_atom *argv);
    void timings(t_object *x, long argc, t_atom *argv);
        
private:
    
    // Kernel
    
    gesture_kernel m_kernel;
    
    // Split Values
    
    double m_split_points[max_kernels - 1];
    long m_num_splits;
    
    // Kernel Parameters
    
    t_atom m_kernel_params[max_kernels * max_params];
    long m_kernel_param_count[max_kernels];
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
