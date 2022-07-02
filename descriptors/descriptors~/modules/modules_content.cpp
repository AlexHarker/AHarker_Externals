
#include "modules_content.hpp"
#include "utility_definitions.hpp"
#include "../descriptors_graph.hpp"

#include <algorithm>
#include <cmath>

#include <Statistics.hpp>

void module_noise_ratio::add_requirements(graph& g)
{
    m_power_sum_module = g.add_requirement(new module_power_sum());
    m_median_amplitude_module = g.add_requirement(new module_median_amplitude_spectrum(m_median_span * 2 + 1));
}

void module_noise_ratio::calculate(const global_params& params, const double *frame, long size)
{
    const double *median_amplitudes = m_median_amplitude_module->get_frame();

    double power_sum = m_power_sum_module->get_sum(0, params.num_bins());
            
    if (power_sum)
        m_value = std::min(1.0, stat_sum_squares(median_amplitudes, params.num_bins()) / power_sum);
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

user_module *module_spectral_peaks::setup(const global_params& params, module_arguments& args)
{
    long N = args.get_long(10, 1, std::numeric_limits<long>::max());
    long median_span = args.get_long(15, 1, std::numeric_limits<long>::max());

    return new module_spectral_peaks(N, median_span);
}

void module_spectral_peaks::add_requirements(graph& g)
{
    m_peak_detection_module = g.add_requirement(new module_peak_detection(m_median_span * 2 + 1));
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
        auto& peak = peaks.by_value(i);
        
        m_values[i * 2 + 0] = peak.m_position * params.bin_freq();
        m_values[i * 2 + 1] = peak.m_value;
    }
    
    for ( ; i < m_num_peaks; i++)
    {
        m_values[i * 2 + 0] = 0.0;
        m_values[i * 2 + 1] = 0.0;
    }
}

// Inharmonicity Module

user_module *module_inharmonicity::setup(const global_params& params, module_arguments& args)
{
    long num_peaks = args.get_long(10, 1, std::numeric_limits<long>::max());
    long median_span = args.get_long(15, 1, std::numeric_limits<long>::max());
    double threshold = args.get_double(0.68, 0.0, 1.0);

    return new module_inharmonicity(num_peaks, median_span, threshold);
}

void module_inharmonicity::add_requirements(graph& g)
{
    m_peak_detection_module = g.add_requirement(new module_peak_detection(m_median_span * 2 + 1));
    m_pitch_module = g.add_requirement(new module_pitch(m_threshold));
}

void module_inharmonicity::calculate(const global_params& params, const double *frame, long size)
{
    auto peaks = m_peak_detection_module->get_peaks();
    long num_valid_peaks = std::min(static_cast<long>(peaks.num_peaks()), m_num_peaks);

    double sum1 = 0.0;
    double sum2 = 0.0;
    
    const double pitch = m_pitch_module->get_output(0);

    // If there is a pitch value calculate the inharmonicity
    
    if (pitch != infinity())
    {
        for (long i = 0; i < num_valid_peaks; i++)
        {
            auto& peak = peaks.by_value(i);
            const double peak_freq = peak.m_position * params.bin_freq();
            
            if (peak_freq > 0)
            {
                double divergence = (pitch > peak_freq) ? pitch / peak_freq : peak_freq / pitch;

                divergence -= floor(divergence);
                divergence = (divergence > 0.5) ? 1.0 - divergence : divergence;

                sum1 += peak.m_value * divergence * 2.0;
                sum2 += peak.m_value;
            }
        }
    }
    
    m_value = sum2 ? sum1 / sum2 : infinity();
}

// Roughness Module

user_module *module_roughness::setup(const global_params& params, module_arguments& args)
{
    long N = args.get_long(10, 1, std::numeric_limits<long>::max());
    long median_span = args.get_long(15, 1, std::numeric_limits<long>::max());
    
    return new module_roughness(N, median_span);
}

void module_roughness::add_requirements(graph& g)
{
    m_peak_detection_module = g.add_requirement(new module_peak_detection(m_median_span * 2 + 1));
}

void module_roughness::calculate(const global_params& params, const double *frame, long size)
{
    auto peaks = m_peak_detection_module->get_peaks();
    long num_valid_peaks = std::min(static_cast<long>(peaks.num_peaks()), m_num_peaks);

    // This roughness calulator takes freq and amplitude pairs - the ordering is unimportant

    // Code adapated from Richard Parncutt (Mcgill University / Univeristy of Graz)
    // Richard Parncutt's current webpage is: http://www.uni-graz.at/richard.parncutt/
    // The code can be found at: http://www.uni-graz.at/richard.parncutt/computerprograms.html
    // The original comments from the code are reproduced in an adapted form below.

    // Implementation of Hutchinson & Knopoff (1978) (H&K), which is based on Plomp & Levelt (1965) (P&L)
    // The "standard curve" of P&L is represented by the function
    //
    // g(x) = [e * (x/a) * exp (-x/a)] ^ i, x < 1.2 = 0, x > 1.2
    //
    // where:
    // - x is the interval between two partials expressed in critical bandwidths
    // - a is the interval for maximum roughness (about 0.25 CBs),
    // - i is an index (power) of about 2.
    //
    // (NB: I [ie. Richard Parncutt] invented this function; H&K used a look-up table)
    //
    // Critical bandwidth CBW is given by P&L's function, as cited by H&K.
   
    // Constants for analytic version of standard curve of P&L:
        
    constexpr double cb_int0 = 0.25;                   // interval for max roughness (P&L: ca. 0.25)
    constexpr double cb_int1 = 1.2;                    // interval beyond which roughness is negligible (P&L: 1.2)
    constexpr double index = 2.0;                      // for standard curve of P&L (bigger index => narrower curve)
    constexpr double cb_int0_recip = 1.0 / cb_int0;
    
    // for calculating H&K Eq. (3)
    
    double min_amp = 0.0;
    double numerator = 0.0;
    double denominator = 0.0;
            
    for (long i = 0; i < num_valid_peaks; i++)
    {
        auto& peak1 = peaks.by_value(i);
        const double freq1 = peak1.m_position * params.bin_freq();
        const double amp1 = peak1.m_value;
        
        if (freq1 > 0 && amp1)
        {
            for (long j = i + 1; j < num_valid_peaks; j++)
            {
                auto& peak2 = peaks.by_value(j);
                const double freq2 = peak2.m_position * params.bin_freq();
                const double amp2 = peak2.m_value;
                
                if (freq2 > 0 && amp2)
                {
                    // mean frequency of two cpts
                    
                    const double mean_freq = (freq1 + freq2) / 2.0;
                    
                    // The below is from H&K p. 5
                    // cbw - critical bandwidth according to P&L, H&K
                    // cb_int = interval between two partials in critical bandwidths
                    
                    const double cbw = 1.72 * pow(mean_freq, 0.65);
                    const double cb_int = (fabs(freq1 - freq2)) / cbw;
                    
                    // (Otherwise roughness is negligible) (save computing time)
                    
                    if (cb_int < cb_int1)
                    {
                        const double ratio = cb_int * cb_int0_recip;

                        // Below approximates P&L Fig. 10, H&K Fig. 1
                        
                        const double standard_curve = pow((M_E * ratio) * exp(-ratio), index);
                        numerator += amp1 * amp2 * standard_curve;
                    }
                    denominator += amp1 * amp1;
                    min_amp = std::min(min_amp, amp1);
                }
            }
        }
    }
        
    // This hack allows the algorithm to work without needing the partials in a particular order
        
    denominator -= min_amp * min_amp;
        
    m_value = denominator ? (numerator / denominator) : infinity();
}


