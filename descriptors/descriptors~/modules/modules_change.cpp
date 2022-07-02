
#include "modules_change.hpp"
#include "descriptors_graph.hpp"
#include "conversion_helpers.hpp"

#include <Statistics.hpp>

#include <cmath>

// Flux Module

user_module *module_flux::setup(const global_params& params, module_arguments& args)
{
    module_flux *m = dynamic_cast<module_flux *>(module_spectral::setup(params, args));
    
    m->m_forward_only = args.get_bool(true);
    m->m_square_flag = args.get_bool(true);
    m->m_normalise_spectrum = args.get_bool(true);
    m->set_lag(args.get_long(1, 1, 33));
    
    return m;
}
    
void module_flux::calculate(const global_params& params, const double *frame, long size)
{
    const double *frame1 = m_ring_buffer_module->get_frame(m_frame_lag);
    const double *frame2 = m_ring_buffer_module->get_frame(0);
    
    double norm_factor1 = 1.0;
    double norm_factor2 = 1.0;
    double sum = 0.0;
    
    auto bin_change = [&](long i)
    {
        return (frame2[i] * norm_factor2) - (frame1[i] * norm_factor1);
    };
    
    if (m_normalise_spectrum)
    {
        norm_factor1 = stat_sum(frame1 + m_min_bin, bin_count());
        norm_factor2 = stat_sum(frame2 + m_min_bin, bin_count());
    }
    
    norm_factor1 = norm_factor1 ? 1.0 / norm_factor1 : 1.0;
    norm_factor2 = norm_factor2 ? 1.0 / norm_factor2 : 1.0;
    
    if (m_square_flag)
    {
        // Squared changes (forward only then both)
        
        if (m_forward_only)
        {
            for (long i = m_min_bin; i < m_max_bin; i++)
            {
                const double value = bin_change(i);
                sum += value > 0.0 ? value * value : 0.0;
            }
        }
        else
        {
            for (long i = m_min_bin; i < m_max_bin; i++)
            {
                const double value = bin_change(i);
                sum += value * value;
            }
        }
    }
    else
    {
        // Non-squared deltas (forward only then both)
        
        if (m_forward_only)
        {
            for (long i = m_min_bin; i < m_max_bin; i++)
            {
                const double value = bin_change(i);
                sum += value > 0.0 ? value : 0.0;
            }
        }
        else
        {
            for (long i = m_min_bin; i < m_max_bin; i++)
                sum += bin_change(i);
        }
    }
    
    m_value = m_square_flag ? sqrt(sum) : sum;
}

// Foote Module

user_module *module_foote::setup(const global_params& params, module_arguments& args)
{
    module_foote *m = dynamic_cast<module_foote *>(module_spectral::setup(params, args));
    
    m->m_forward_only = args.get_bool(true);
    m->set_lag(args.get_long(1, 1, 33));
    
    return m;
}

void module_foote::calculate(const global_params& params, const double *frame, long size)
{
    const double *frame1 = m_ring_buffer_module->get_frame(m_frame_lag);
    const double *frame2 = m_ring_buffer_module->get_frame(0);
    
    double norm_sum1 = 0.0;
    double norm_sum2 = 0.0;
    double sum = 0.0;
    
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
    
    // Finalise
    
    if (norm_sum1 && norm_sum2)
        m_value = 1.0 - (sum / sqrt(norm_sum1 * norm_sum2));
    else
        m_value = norm_sum2 ? 1.0 : 0.0;
}

// MKL Module

user_module *module_mkl::setup(const global_params& params, module_arguments& args)
{
    module_mkl *m = dynamic_cast<module_mkl *>(module_spectral::setup(params, args));
    
    m->m_threshold = args.get_double(-300.0, -500.0, 0.0);
    m->m_forward_only = args.get_bool(true);
    m->m_weight = args.get_bool(false);
    m->m_normalise_spectra = args.get_bool(true);
    m->set_lag(args.get_long(1, 1, 33));

    return m;
}

void module_mkl::add_requirements(graph& g)
{
    module_spectral_change::add_requirements(g);
    m_log_ring_buffer_module = g.add_requirement(new module_log_spectrum_ring_buffer());
    m_log_ring_buffer_module->request_lag(m_frame_lag);
}
    
void module_mkl::calculate(const global_params& params, const double *frame, long size)
{
    const double *log_frame1 = m_log_ring_buffer_module->get_frame(m_frame_lag);
    const double *log_frame2 = m_log_ring_buffer_module->get_frame(0);
    const double *frame1 = m_ring_buffer_module->get_frame(m_frame_lag);
    const double *frame2 = m_ring_buffer_module->get_frame(0);
    
    constexpr double DB_MAX_MKL_EQUAL = -140.0;
    const double MKL_EQUALISE_MAX_LOG = log(pow(10.0, DB_MAX_MKL_EQUAL) * 20.0);
    const double norm_min = sqrt(db_to_pow(db_calc_min()));

    double norm_factor1 = 1.0;
    double norm_factor2 = 1.0;
    double sum = 0.0;
        
    const double log_thresh = log(dbtoa(m_threshold));
   
    if (m_normalise_spectra)
    {
        norm_factor1 = stat_sum(frame1 + m_min_bin, bin_count());
        norm_factor2 = stat_sum(frame2 + m_min_bin, bin_count());
     
        if (!norm_factor1)
            norm_factor1 = norm_min * static_cast<double>(bin_count());
    }
    
    const double log_norm_factor = std::max(log(norm_factor1 / norm_factor2), MKL_EQUALISE_MAX_LOG);
    
    // FIX - note that there was an error in the original code here...
  
    norm_factor2 = norm_factor2 ? 1.0 / norm_factor2 : 1.0;

    auto bin_change = [&](long i)
    {
        return (log_frame2[i] - log_frame1[i]) + log_norm_factor;
    };
    
    if (m_weight)
    {
        // Weighted changes (forward only then both)

        if (m_forward_only)
        {
            for (long i = m_min_bin; i < m_max_bin; i++)
            {
                const double value = bin_change(i);
                if (value > 0.0 && log_frame2[i] >= log_thresh)
                    sum += value * frame2[i] * norm_factor2;
            }
        }
        else
        {
            for (long i = m_min_bin; i < m_max_bin; i++)
            {
                const double value = bin_change(i);
                if (log_frame2[i] >= log_thresh)
                    sum += value * frame2[i] * norm_factor2;
            }
        }
    }
    else
    {
        // Non-weighted changes (forward only then both)
        
        if (m_forward_only)
        {
            for (long i = m_min_bin; i < m_max_bin; i++)
            {
                const double value = bin_change(i);
                if (value > 0.0 && log_frame2[i] >= log_thresh)
                    sum += value;
            }
        }
        else
        {
            for (long i = m_min_bin; i < m_max_bin; i++)
            {
                const double value = bin_change(i);
                if (log_frame2[i] >= log_thresh)
                    sum += value;
            }
        }
    }
    
    m_value = sum / static_cast<double>(2 * (bin_count()));
}

