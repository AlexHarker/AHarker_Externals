
/*
 *  gesture_maker_multipart.cpp
 *
 *  This object deals with multipart gestures that are made up of chained kernels for inflections.
 *
 *  Inflection kernel parameters will cycle round if there are not as many sets of parameters as inflections.
 *  Inflection gestures are resolved in realtime.
 *  Therefore, in the case of variable parameters, each realisation will be different.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include "gesture_kernel.hpp"
#include "gesture_multipart.hpp"

#include <algorithm>


// Reset the multipart structure to default values

void gesture_multipart::reset()
{
    *this = gesture_multipart();
    m_kernel.reset();
}

// Calculate the output from an input phase

double gesture_multipart::operator()(t_object *x, double phase)
{
    // If gesture has been re-triggered reset to the beginning, then search for the current split
    
    long search_kernel = (phase < m_last_phase) ? 0 : m_current_kernel;
        
    for (; search_kernel < m_num_splits && phase >= m_split_points[search_kernel]; search_kernel++);
    
    // Get the current values and update the kernel parameters
    // Note that the parameters will cycle round if there are not as many sets of parameters as splits
    
    if ((m_current_kernel != search_kernel || m_force_update))
    {
        m_lo_phase = !search_kernel ? 0.0 : m_split_points[search_kernel - 1];
        m_hi_phase = (search_kernel >= m_num_splits) ? 1.0 : m_split_points[search_kernel];
        m_range_recip = 1.0 / (m_hi_phase - m_lo_phase);
        
        if (m_num_kernels)
        {
            long output_kernel = search_kernel % m_num_kernels;
            t_atom *output_params = m_kernel_params + (max_params * output_kernel);
            m_kernel.params(x, m_kernel_param_count[output_kernel], output_params);
        }
    }
    
    m_last_phase = phase;
    m_current_kernel = search_kernel;
    m_force_update = false;
    
    // Calculate and clip inflection phase and then calculate the output
    
    return m_kernel(std::max(0.0, std::min(1.0, (phase - m_lo_phase) * m_range_recip)));
}

// Set Parameters

void gesture_multipart::params(t_object *x, long argc, t_atom *argv)
{
    if (argc && m_num_kernels < max_kernels)
    {
        t_atom *parameters = m_kernel_params + (max_params * m_num_kernels);
        m_kernel_param_count[m_num_kernels] = std::min(argc, max_params);
        
        for (long i = 0; i < m_kernel_param_count[m_num_kernels]; i++)
            parameters[i] = *argv++;
        
        m_num_kernels++;
        m_force_update = true;
    }
}

// Set the Timings

void gesture_multipart::timings(t_object *x, long argc, t_atom *argv)
{
    m_num_splits = std::min(argc, max_kernels - 1);
    
    // There may be a better way to do this that brings us closer to the desired value
    
    for (long i = 0; i < m_num_splits; i++)
        m_split_points[i] = nextafter(atom_getfloat(argv++), -1.0);
    
    m_force_update = true;
}
