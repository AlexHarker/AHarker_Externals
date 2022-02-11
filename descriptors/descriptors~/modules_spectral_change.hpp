
#ifndef _MODULES_SPECTRAL_CHANGE_HPP_
#define _MODULES_SPECTRAL_CHANGE_HPP_

#include "modules_spectral.hpp"

#include <cmath>

static constexpr double infinity() { return std::numeric_limits<double>::infinity(); }

// RIng Buffer

struct module_spectrum_ring_buffer : module
{
    void calculate(const global_params& params, const double *frame, long size) override {}
    
    bool is_the_same(const module *m) const override
    {
        return dynamic_cast<const module_spectrum_ring_buffer *>(m);
    }
};

// Foote Module

struct module_foote : module_spectral<module_foote>
{
    static module_foote *setup(const global_params& params, long argc, t_atom *argv)
    {
        module_foote *m = dynamic_cast<module_foote *>(module_spectral::setup(params, argc, argv));
        
        m->m_forward_only = argc > 2 ? atom_getfloat(argv + 3) : true;
        m->m_frames_back = argc > 3 ? atom_getlong(argv + 4) : 1;
        
        return m;
    }
    
    bool is_the_same(const module *m) const override
    {
        const module_foote *m_typed = dynamic_cast<const module_foote *>(m);
        
        return module_spectral::is_the_same(m) && m_typed->m_forward_only == m_forward_only && m_typed->m_frames_back == m_frames_back;
    }
    
    void add_requirements(graph& g) override
    {
        m_ring_buffer_module = g.add_requirement(new module_spectrum_ring_buffer());
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        double norm_sum1 = 0.0;
        double norm_sum2 = 0.0;
        double sum = 0.0;
     
        double *frame1;
        double *frame2;
        
        auto bin_calculate = [&](double value1, double value2)
        {
            sum += value1 * value2;
            norm_sum1 += value1 * value1;
            norm_sum2 += value2 * value2;
        };
        
        // Calculate sums (looking at forward only changes if requested)
        
        if (m_forward_only)
        {
            for (long i = m_min_bin; i < m_max_bin; i++)
                if (frame2[i] > frame1[i])
                    bin_calculate(frame1[i], frame2[i]);
        }
        else
        {
            for (long i = m_min_bin; i < m_max_bin; i++)
                bin_calculate(frame1[i], frame2[i]);
        }
        
        if (norm_sum1 && norm_sum2)
            m_value =  1.0 - (sum / sqrt(norm_sum1 * norm_sum2));
        else if (norm_sum2)
            m_value = 1.0;
        else
            m_value = infinity();
    }
    
private:
    
    module_spectrum_ring_buffer *m_ring_buffer_module;
    
    bool m_forward_only;
    long m_frames_back;
};

// Flux Module

struct module_flux : module_spectral<module_flux>
{
    static module_flux *setup(const global_params& params, long argc, t_atom *argv)
    {
        module_flux *m = dynamic_cast<module_flux *>(module_spectral::setup(params, argc, argv));
        
        m->m_forward_only = argc > 2 ? atom_getfloat(argv + 3) : true;
        m->m_normalise_spectrum = argc > 3 ? atom_getlong(argv + 4) : false;
        m->m_frames_back = argc > 4 ? atom_getlong(argv + 5) : 1;
        
        return m;
    }
    
    bool is_the_same(const module *m) const override
    {
        const module_flux *m_typed = dynamic_cast<const module_flux *>(m);
        
        return module_spectral::is_the_same(m) && m_typed->m_forward_only == m_forward_only && m_typed->m_normalise_spectrum == m_normalise_spectrum && m_typed->m_frames_back == m_frames_back;
    }
    
    void add_requirements(graph& g) override
    {
        m_ring_buffer_module = g.add_requirement(new module_spectrum_ring_buffer());
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        double norm_factor1 = 1.0;
        double norm_factor2 = 1.0;
        double sum = 0.0;
        
        double *frame1;
        double *frame2;
        bool square_flag; // FIX
        
        if (m_normalise_spectrum)
        {
            // FIX
            /*
            norm_factor1 = cumulate_ptr1[max_bin - 1];
            if (min_bin)
                norm_factor1 -= cumulate_ptr1[min_bin - 1];
            norm_factor2 = cumulate_ptr2[max_bin - 1];
            if (min_bin)
                norm_factor2 -= cumulate_ptr2[min_bin - 1];*/
        }
        
        norm_factor1 = norm_factor1 ? norm_factor1 = 1.0 / norm_factor1 : 1.0;
        norm_factor2 = norm_factor1 ? norm_factor2 = 1.0 / norm_factor2 : 1.0;
        
        if (square_flag)
        {
            if (m_forward_only)
            {
                // Forward changes only using square amplitudes
                
                for (long i = m_min_bin; i < m_max_bin; i++)
                {
                    double current_val = (frame2[i] * norm_factor2) - (frame1[i] * norm_factor1);
                    if (current_val < 0.) current_val = 0.;
                    current_val *= current_val;
                    sum += current_val;
                }
            }
            else
            {
                // Both changes using square amplitudes
                
                for (long i = m_min_bin; i < m_max_bin; i++)
                {
                    double current_val = (frame2[i] * norm_factor2) - (frame1[i] * norm_factor1);
                    current_val *= current_val;
                    sum += current_val;
                }
            }
        }
        else
        {
            if (m_forward_only)
            {
                // Forward changes only using amplitudes
                
                for (long i = m_min_bin; i < m_max_bin; i++)
                {
                    double current_val = (frame2[i] * norm_factor2) - (frame1[i] * norm_factor1);
                    if (current_val < 0.) current_val = 0.;
                    sum += current_val;
                }
            }
            else
            {
                // Both changes using amplitudes
                
                for (long i = m_min_bin; i < m_max_bin; i++)
                {
                    double current_val = (frame2[i] * norm_factor2) - (frame1[i] * norm_factor1);
                    sum += current_val;
                }
            }
        }
        
        m_value = square_flag ? sqrt(sum) : sum;
    }
    
private:
    
    module_spectrum_ring_buffer *m_ring_buffer_module;
    
    bool m_forward_only;
    bool m_normalise_spectrum;
    long m_frames_back;
};
#endif /* _MODULES_SPECTRAL_CHANGE_HPP_ */
