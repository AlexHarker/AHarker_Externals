
#include "modules_pitch.hpp"
#include "descriptors_graph.hpp"
#include "utility_definitions.hpp"

#include <algorithm>

// Pitch Module

void module_pitch::add_requirements(graph& g)
{
    m_autocorrelation_module = g.add_requirement(new module_autocorrelation());
}

template <class Op>
struct threshold_test
{
    threshold_test(double threshold) : m_threshold(threshold) {}
    
    bool operator()(const double& a) { return Op()(a, m_threshold); }
    
    double m_threshold;
};

void module_pitch::calculate(const global_params& params, const double *frame, long size)
{
    auto ac_coeff = m_autocorrelation_module->get_coefficients();
    long length = m_autocorrelation_module->get_length();
            
    const double threshold = ac_coeff[0] * m_threshold;
    
    // Ignore first peak, find start and end of second peak
    
    auto first = std::find_if(ac_coeff + 1, ac_coeff + length, threshold_test<std::less<double>>(threshold));
    auto begin = std::find_if(first, ac_coeff + length, threshold_test<std::greater<double>>(threshold));
    auto end = std::find_if(begin, ac_coeff + length, threshold_test<std::less<double>>(threshold));

    if (end < ac_coeff + length - 1)
    {
        auto max_ptr = std::max_element(begin, end);
        
        long bin_freq = max_ptr - ac_coeff;
        double value = *max_ptr;
        
        // Do the correction if we have found a pitch (using parabolic interpolation)

        const double prev = ac_coeff[bin_freq - 1];
        const double next = ac_coeff[bin_freq + 1];
        const double beta = 0.5 * (next - prev);
        const double gamma = (2.0  * value) - next - prev;
     
        m_value = params.m_sr / (bin_freq + (beta / gamma));
        m_confidence = std::min(value, 1.0);
    }
    else
    {
        m_value = infinity();
        m_confidence = *std::max_element(first, end);
    }
}

// Confidence Module

void module_confidence::add_requirements(graph& g)
{
    m_pitch_module = g.add_requirement(new module_pitch(m_threshold));
}

void module_confidence::calculate(const global_params& params, const double *frame, long size)
{
    m_value = m_pitch_module->get_confidence();
}
    
// Brightness Linear

void module_lin_brightness::add_requirements(graph& g)
{
    m_pitch_module = g.add_requirement(new module_pitch(m_threshold));
    m_centroid_module = g.add_requirement(new module_lin_centroid(m_lo_freq, m_hi_freq));
}

void module_lin_brightness::calculate(const global_params& params, const double *frame, long size)
{
    const double pitch = m_pitch_module->get_output(0);
    const double centroid = m_centroid_module->get_output(0);
    
    // FIX - weird threshold
    
    if (pitch > 1.0 && pitch != infinity() && centroid != infinity())
        m_value = centroid / pitch;
    else
        m_value = infinity();
}

// Brightness Log

void module_log_brightness::add_requirements(graph& g)
{
    m_pitch_module = g.add_requirement(new module_pitch(m_threshold));
    m_centroid_module = g.add_requirement(new module_log_centroid(m_lo_freq, m_hi_freq));
}

void module_log_brightness::calculate(const global_params& params, const double *frame, long size)
{
    const double pitch = m_pitch_module->get_output(0);
    const double centroid = m_centroid_module->get_output(0);
    
    // FIX - weird threshold

    if (pitch > 1.0 && pitch != infinity() && centroid != infinity())
        m_value = centroid / pitch;
    else
        m_value = infinity();
}
