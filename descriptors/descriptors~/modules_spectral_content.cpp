
#include "modules_spectral_content.hpp"
#include "descriptors_graph.hpp"

#include <algorithm>
#include <Statistics.hpp>

static constexpr double infinity() { return std::numeric_limits<double>::infinity(); }

void module_noise_ratio::add_requirements(graph& g)
{
    m_energy_module = g.add_requirement(new module_energy(0.0, 192000.0, false));
    m_median_power_module = g.add_requirement(new module_median_power_spectrum(m_median_span));
}

void module_noise_ratio::calculate(const global_params& params, const double *frame, long size)
{
    double power_sum = m_energy_module->get_output(0);
    
    const double *median_power = m_median_power_module->get_frame();

    // FIX - check median span
    // FIX - requires energy compensation
            
    if (power_sum)
        m_value = std::min(1.0, statSum(median_power, params.num_bins()) / power_sum);
    else
        m_value = infinity();
}

// Harmonic Ratio

void module_harmonic_ratio::add_requirements(graph& g)
{
    m_noise_ratio_module = g.add_requirement(new module_noise_ratio(m_median_span));
}

void module_harmonic_ratio::calculate(const global_params& params, const double *frame, long size)
{
    const double noise_ratio = m_noise_ratio_module->get_output(0);
    
    if (noise_ratio != infinity())
        m_value = 1.0 - noise_ratio;
    else
        m_value = infinity();
}

// Spectral Peaks Module

user_module *module_spectral_peaks::setup(const global_params& params, long argc, t_atom *argv)
{
    long num_peaks = argc > 0 ? atom_getlong(argv + 0) : 15;

    return new module_spectral_peaks(num_peaks);
}
    
bool module_spectral_peaks::module_spectral_peaks::is_the_same(const module *m) const
{
    const module_spectral_peaks *m_typed = dynamic_cast<const module_spectral_peaks *>(m);
    
    return m_typed && m_typed->m_num_peaks == m_num_peaks;
}

void module_spectral_peaks::add_requirements(graph& g)
{
    m_peak_detection_module = g.add_requirement(new module_peak_detection());
}

void module_spectral_peaks::prepare(const global_params& params)
{
    m_values.resize(m_num_peaks * 2);
}

void module_spectral_peaks::calculate(const global_params& params, const double *frame, long size)
{
    auto peaks = m_peak_detection_module->get_peaks();
    
    long num_valid_peaks = std::min(static_cast<long>(peaks.num_peaks()), m_num_peaks);
    long i = 0;
    
    for ( ; i < num_valid_peaks; i++)
    {
        auto peak = peaks.by_value(i);
        
        m_values[i * 2 + 0] = peak.m_position * params.bin_freq();
        m_values[i * 2 + 1] = peak.m_value;
    }
    
    for ( ; i < m_num_peaks; i++)
    {
        m_values[i * 2 + 0] = 0.0;
        m_values[i * 2 + 1] = 0.0;
    }
}

