
#include "modules_summary.hpp"

#include <Statistics.hpp>

#include <algorithm>
#include <limits>

static constexpr double infinity() { return std::numeric_limits<double>::infinity(); }

using VecType = SIMDType<double, SIMDLimits<double>::max_size>;

// Duration

user_module *summary_module_duration::setup(const global_params& params, module_arguments& args)
{
    return new summary_module_duration();
}

void summary_module_duration::calculate(const global_params& params, const double *data, long size)
{
    m_value = 1000.0 * params.m_signal_length / params.m_sr;
}

// Spectral Peaks

// Spectrum Average

void summary_module_spectral_peaks::spectrum_average::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
}

void summary_module_spectral_peaks::spectrum_average::prepare(const global_params& params)
{
    m_spectrum.resize(params.num_bins());
    
    std::fill_n(m_spectrum.data(), params.num_bins(), 0.0);
}

void summary_module_spectral_peaks::spectrum_average::calculate(const global_params& params, const double *frame, long size)
{
    const double *amp_frame = m_amplitude_module->get_frame();
    
    VecType *average = reinterpret_cast<VecType *>(m_spectrum.data());
    const VecType *amps = reinterpret_cast<const VecType *>(amp_frame);
    long nyquist = params.num_bins() - 1;
    long loop_size = nyquist / VecType::size;
    
    const double recip = 1.0 / params.num_frames();
    
    // Calculate amplitude spectrum
    
    for (long i = 0; i < loop_size; i++)
        average[i] += amps[i] * recip;
    
    // Do Nyquist Value
    
    m_spectrum.data()[nyquist] += amp_frame[nyquist] * recip;
}

user_module *summary_module_spectral_peaks::setup(const global_params& params, module_arguments& args)
{
    return new summary_module_spectral_peaks(args.get_long(10, 1, std::numeric_limits<long>::max()));
}

void summary_module_spectral_peaks::add_requirements(graph& g)
{
    m_spectrum = g.add_requirement(new spectrum_average());
}

void summary_module_spectral_peaks::prepare(const global_params& params)
{
    m_peaks.resize(params.num_bins() / 2);
    m_detector.resize(params.num_bins());
}

void summary_module_spectral_peaks::calculate(const global_params& params, const double *frame, long size)
{
    const double *spectrum = m_spectrum->get_average();
    
    m_detector(m_peaks, spectrum, params.num_bins());
        
    long num_valid_peaks = std::min(static_cast<long>(m_peaks.num_peaks()), m_num_peaks);
    long i = 0;
    
    for ( ; i < num_valid_peaks; i++)
    {
        auto& peak = m_peaks.by_value(i);
        
        m_values[i * 2 + 0] = peak.m_position * params.bin_freq();
        m_values[i * 2 + 1] = peak.m_value;
    }
    
    for ( ; i < m_num_peaks; i++)
    {
        m_values[i * 2 + 0] = 0.0;
        m_values[i * 2 + 1] = 0.0;
    }
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
            sum += static_cast<double>(i) * data[i];
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
    // N.B. - this should only ever be called after a dynamic cast check
    
    auto time = static_cast<const specifier_mask_time *>(m)->m_mask_time;
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
    // N.B. - this should only ever be called after a dynamic cast check

    auto b = static_cast<const specifier_threshold *>(m);
    
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
