
#include "modules_spectral_change.hpp"
#include "descriptors_graph.hpp"

#include <Statistics.hpp>

#include <cmath>

static constexpr double infinity() { return std::numeric_limits<double>::infinity(); }

// Foote Module

user_module *module_foote::setup(const global_params& params, long argc, t_atom *argv)
{
    module_foote *m = dynamic_cast<module_foote *>(module_spectral::setup(params, argc, argv));
    
    m->m_forward_only = argc > 2 ? atom_getfloat(argv + 3) : true;
    m->set_lag(argc > 3 ? atom_getlong(argv + 3) : 1);

    return m;
}

bool module_foote::is_the_same(const module *m) const
{
    const module_foote *m_typed = dynamic_cast<const module_foote *>(m);
    
    return module_spectral::is_the_same(m) && m_typed->m_forward_only == m_forward_only && m_typed->m_frame_lag == m_frame_lag;
}

void module_foote::calculate(const global_params& params, const double *frame, long size)
{
    double norm_sum1 = 0.0;
    double norm_sum2 = 0.0;
    double sum = 0.0;
 
    const double *frame1 = m_ring_buffer_module->get_frame(m_frame_lag);
    const double *frame2 = m_ring_buffer_module->get_frame(0);
    
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

// Flux Module

user_module *module_flux::setup(const global_params& params, long argc, t_atom *argv)
{
    module_flux *m = dynamic_cast<module_flux *>(module_spectral::setup(params, argc, argv));
    
    m->m_forward_only = argc > 2 ? atom_getfloat(argv + 3) : true;
    m->m_square_flag = argc > 3 ? atom_getfloat(argv + 4) : true;
    m->m_normalise_spectrum = argc > 4 ? atom_getlong(argv + 5) : false;
    m->set_lag(argc > 5 ? atom_getlong(argv + 5) : 1);
    
    return m;
}

bool module_flux::is_the_same(const module *m) const
{
    const module_flux *m_typed = dynamic_cast<const module_flux *>(m);
    
    return module_spectral::is_the_same(m) && m_typed->m_forward_only == m_forward_only && m_typed->m_normalise_spectrum == m_normalise_spectrum && m_typed->m_frame_lag == m_frame_lag;
}
    
void module_flux::calculate(const global_params& params, const double *frame, long size)
{
    double norm_factor1 = 1.0;
    double norm_factor2 = 1.0;
    double sum = 0.0;
    
    const double *frame1 = m_ring_buffer_module->get_frame(m_frame_lag);
    const double *frame2 = m_ring_buffer_module->get_frame(0);
   
    // FIX - check!!!

    if (m_normalise_spectrum)
    {
        norm_factor1 = statSum(frame1 + m_min_bin, m_max_bin - m_min_bin);
        norm_factor2 = statSum(frame2 + m_min_bin, m_max_bin - m_min_bin);
    }
    
    norm_factor1 = norm_factor1 ? norm_factor1 = 1.0 / norm_factor1 : 1.0;
    norm_factor2 = norm_factor1 ? norm_factor2 = 1.0 / norm_factor2 : 1.0;
    
    if (m_square_flag)
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
    
    m_value = m_square_flag ? sqrt(sum) : sum;
}

// MKL Module

user_module *module_mkl::setup(const global_params& params, long argc, t_atom *argv)
{
    module_mkl *m = dynamic_cast<module_mkl *>(module_spectral::setup(params, argc, argv));
    
    m->m_threshold = argc > 2 ? atom_getfloat(argv + 2) : -300.0;
    m->m_forward_only = argc > 3 ? atom_getfloat(argv + 3) : true;
    m->m_weight_second_frame = argc > 4 ? atom_getlong(argv + 4) : false;
    m->m_normalise_spectrum = argc > 5 ? atom_getlong(argv + 5) : true;
    m->set_lag(argc > 6 ? atom_getlong(argv + 6) : 1);

    return m;
}

bool module_mkl::is_the_same(const module *m) const
{
    const module_mkl *m_typed = dynamic_cast<const module_mkl *>(m);
    
    return module_spectral::is_the_same(m) && m_typed->m_forward_only == m_forward_only && m_typed->m_normalise_spectrum == m_normalise_spectrum && m_typed->m_weight_second_frame == m_weight_second_frame && m_typed->m_frame_lag == m_frame_lag && m_typed->m_threshold == m_threshold;
}
    
void module_mkl::calculate(const global_params& params, const double *frame, long size)
{
    double norm_factor1 = 1.0;
    double norm_factor2 = 1.0;
    double sum = 0.0;
    
    // FIX
    
    const double log_thresh = m_threshold;
    double *log_frame1;
    double *log_frame2;
    const double *frame2 = m_ring_buffer_module->get_frame(m_frame_lag);
    
    if (m_normalise_spectrum)
    {
        // FIX
        
        /*
        norm_factor1 = cumulate_ptr1[max_bin - 1];
        if (min_bin)
            norm_factor1 -= cumulate_ptr1[min_bin - 1];
        norm_factor2 = cumulate_ptr2[max_bin - 1];
        if (min_bin)
            norm_factor2 -= cumulate_ptr2[min_bin - 1];
        if (!norm_factor1) norm_factor1 = sqrt(POW_MIN) * (double) (max_bin - min_bin);
         */
    }
    
#define DB_MAX_MKL_EQUAL -140.
    double MKL_EQUALISE_MAX_LOG = log(pow(10, DB_MAX_MKL_EQUAL) * 20.);
    
    const double log_norm_factor = std::max(log(norm_factor1 / norm_factor2), MKL_EQUALISE_MAX_LOG);
    
    if (norm_factor2)
        norm_factor1 = 1. / norm_factor2;
    else
        norm_factor2 = 1.;
    
    if (m_weight_second_frame)
    {
        if (m_forward_only)
        {
            // Forward changes only weighting by the second frame
            
            for (long i = m_min_bin; i < m_max_bin; i++)
            {
                double current_val = log_frame2[i] - log_frame1[i];
                current_val += log_norm_factor;
                if (current_val > 0 && log_frame2[i] >= log_thresh)
                    sum += current_val * frame2[i] * norm_factor2;
            }
        }
        else
        {
            // Both changes weighting by the second frame
            
            for (long i = m_min_bin; i < m_max_bin; i++)
            {
                double current_val = log_frame2[i] - log_frame1[i];
                current_val += log_norm_factor;
                if (log_frame2[i] >= log_thresh)
                    sum += current_val * frame2[i] * norm_factor2;
            }
        }
    }
    else
    {
        if (m_forward_only)
        {
            // Forward changes only
            
            for (long i = m_min_bin; i < m_max_bin; i++)
            {
                double current_val = log_frame2[i] - log_frame1[i];
                current_val += log_norm_factor;
                if (current_val > 0 && log_frame2[i] >= log_thresh)
                    sum += current_val;
            }
        }
        else
        {
            // Both changes
            
            for (long i = m_min_bin; i < m_max_bin; i++)
            {
                double current_val = log_frame2[i] - log_frame1[i];
                current_val += log_norm_factor;
                if (log_frame2[i] >= log_thresh)
                    sum += current_val;
            }
        }
    }
    
    m_value = sum / static_cast<double>(2 * (m_max_bin - m_min_bin));
}

