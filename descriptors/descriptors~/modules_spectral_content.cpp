
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