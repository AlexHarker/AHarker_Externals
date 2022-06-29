
#include "modules_summary.hpp"

#include <Statistics.hpp>

#include <algorithm>
#include <limits>

static constexpr double infinity() { return std::numeric_limits<double>::infinity(); }

// Duration

user_module *summary_module_duration::setup(const global_params& params, module_arguments& args)
{
    return new summary_module_duration();
}

void summary_module_duration::calculate(const global_params& params, const double *data, long size)
{
    m_value = 1000.0 * params.m_signal_length / params.m_sr;
}

// Helper functions

double ms_to_frame(const global_params& params, double ms)
{
    return ms * params.m_sr / ( params.m_hop_size * 1000.0);
}

double frame_to_ms(const global_params& params, double frame)
{
    return frame < 0 ? infinity() : (frame * params.m_hop_size * 1000.0) / params.m_sr;
}

double calculate_mean(const double *data, long size)
{
    double sum = 0.0;
    long num_valid = 0;
    
    for (long i = 0; i < size; i++)
    {
        if (data[i] != infinity())
        {
            sum += data[i];
            num_valid++;
        }
    }
    
    return num_valid ? sum / num_valid : infinity();
}

double calculate_max(const double *data, long size)
{
    double maximum = infinity();
    long i;
    
    for (i = 0; i < size; i++)
    {
        if (data[i] != infinity())
        {
            maximum = data[i];
            break;
        }
    }
    
    for (; i < size; i++)
        if (data[i] != infinity())
            maximum = std::max(maximum, data[i]);
    
    return maximum;
}

// Mean

void stat_module_mean::calculate(const global_params& params, const double *data, long size)
{
    m_value = calculate_mean(data, size);
}

// Median

void stat_module_median::prepare(const global_params& params)
{
    m_indices.resize(params.num_frames());
}

void stat_module_median::calculate(const global_params& params, const double *data, long size)
{
    long *indices = m_indices.data();
    
    sort_ascending(indices, data, size);
    
    // Ignore spurious values
    
    for (long i = size - 1; i >= 0; i--)
    {
        if (data[i] != infinity())
        {
            size = i;
            break;
        }
    }
    
    m_value = data[indices[size >> 1]];
}

// Centroid
    
void stat_module_centroid::calculate(const global_params& params, const double *data, long size)
{
    double sum = 0.0;
    double weight_sum = 0.0;
    
    for (long i = 0; i < size; i++)
    {
        if (data[i] != infinity())
        {
            sum += static_cast<double>(i) * data[i];;
            weight_sum += data[i];
        }
    }

    m_value = weight_sum ? frame_to_ms(params, sum / weight_sum) : infinity();
}

// Standard Deviation

void stat_module_stddev::calculate(const global_params& params, const double *data, long size)
{
    double mean = m_mean->get_output(0);
    double sum = 0.0;
    long num_valid = 0;
    
    for (long i = 0; i < size; i++)
    {
        if (data[i] != infinity())
        {
            const double delta = data[i] - mean;
            sum += delta * delta;
            num_valid++;
        }
    }
    
    m_value = num_valid ? sqrt(sum / static_cast<double>(num_valid)) : infinity();
}

// Range

void stat_module_range::calculate(const global_params& params, const double *data, long size)
{
    m_value = calculate_max(data, size) - stat_min(data, size);
}

// Specifiers

// Mask Time

user_module *specifier_mask_time::setup(const global_params& params, module_arguments& args)
{
    double time = args.get_double(0.0, 0.0, infinity());
    return new specifier_mask_time(time);
}

void specifier_mask_time::update_to_final(const module *m)
{
    auto time = dynamic_cast<const specifier_mask_time *>(m)->m_mask_time;
    if (time > 0.0)
        m_mask_time = time;
}

void specifier_mask_time::prepare(const global_params& params)
{
    m_mask_span = m_mask_time < 0.0 ? 0 : ms_to_frame(params, m_mask_time);
}
    
// Threshold

user_module *specifier_threshold::setup(const global_params& params, module_arguments& args)
{
    double threshold = args.get_double(0.0, -infinity(), infinity());
    t_symbol *type_specifier = args.get_symbol(gensym("abs"));
    mode type = mode::abs;
    
    if (type_specifier == gensym("mean_mul"))
        type = mode::mean_mul;
    else if (type_specifier == gensym("mean_add"))
        type = mode::mean_add;
    else if (type_specifier == gensym("mean_db"))
        type = mode::mean_db;
    else if (type_specifier == gensym("peak_mul"))
        type = mode::peak_mul;
    else if (type_specifier == gensym("peak_add"))
        type = mode::peak_add;
    else if (type_specifier == gensym("peak_db"))
        type = mode::peak_db;
    
    return new specifier_threshold(threshold, type);
}

void specifier_threshold::update_to_final(const module *m)
{
    auto b = dynamic_cast<const specifier_threshold *>(m);
    if (b->m_threshold != infinity())
    {
        m_threshold = b->m_threshold;
        m_type = b->m_type;
    }
}

void specifier_threshold::calculate(const global_params& params, const double *data, long size)
{
    const bool use_mean = m_type == mode::mean_mul || m_type == mode::mean_add || m_type == mode::mean_db;
    const bool use_peak = m_type == mode::peak_mul || m_type == mode::peak_add || m_type == mode::peak_db;
    const bool use_db = m_type == mode::mean_db || m_type == mode::peak_db;
    
    const double specified = use_db ? dbtoa(m_threshold) : m_threshold;
    
    // Calculate stat if needed
    
    double stat = -infinity();
    
    if (use_mean)
        stat = calculate_mean(data, size);
    else if (use_peak)
        stat = calculate_max(data, size);
    
    // Return the final threshold
    
    switch (m_type)
    {
        case mode::abs:
            m_calculated_threshold = specified;
            break;
        case mode::mean_mul:
        case mode::mean_db:
        case mode::peak_mul:
        case mode::peak_db:
            m_calculated_threshold = specified * stat;
            break;
        case mode::mean_add:
        case mode::peak_add:
            m_calculated_threshold = specified + stat;
            break;
    }
}
