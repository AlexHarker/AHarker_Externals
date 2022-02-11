
#include "modules_pitch.hpp"

#include <algorithm>
#include <limits>

static constexpr double infinity() { return std::numeric_limits<double>::infinity(); }

// Autocorrelation Module

bool module_autocorrelation::is_the_same(const module *m) const
{
    return dynamic_cast<const module_autocorrelation *>(m);
}

void module_autocorrelation::prepare(const global_params& params)
{
    m_fft_setup.resize(params.m_fft_size_log2);
    m_full_frame.resize(params.fft_size());;
    m_half_frame.resize(params.fft_size());;
    m_coefficients.resize(params.fft_size());
}

void module_autocorrelation::calculate(const global_params& params, const double *frame, long size)
{
    using VecType = SIMDType<double, SIMDLimits<double>::max_size>;

    auto full_frame = m_full_frame.data();
    auto half_frame = m_half_frame.data();
    
    VecType *real1 = reinterpret_cast<VecType *>(full_frame.realp);
    VecType *imag1 = reinterpret_cast<VecType *>(full_frame.imagp);
    VecType *real2 = reinterpret_cast<VecType *>(half_frame.realp);
    VecType *imag2 = reinterpret_cast<VecType *>(half_frame.imagp);
        
    double scale = 0.0;
    
    // Calculate normalisation factor
    
    for (long i = 0; i < (size >> 1); i++)
        scale += frame[i] * frame[i];
        
    scale = 0.25 / (get_length() * scale);
        
    // Do ffts straight into position with zero padding (one half the size of the other)
    
    hisstools_rfft(m_fft_setup.get(), frame, &full_frame, size, m_fft_setup.size());
    hisstools_rfft(m_fft_setup.get(), frame, &half_frame, (size >> 1), m_fft_setup.size());
        
    // Calculate ac coefficients
    
    VecType vscale(scale);
    
    const double nyquist1 = full_frame.imagp[0];
    const double nyquist2 = half_frame.imagp[0];
        
    full_frame.imagp[0] = 0.0;
    half_frame.imagp[0] = 0.0;
    
    for (long i = 0; i < size / (2 * VecType::size); i++)
    {
        const VecType r1 = real1[i];
        const VecType i1 = imag1[i];
        const VecType r2 = real2[i];
        const VecType i2 = imag2[i];
        
        real1[i] = ((r1 * r2) + (i1 * i2)) * vscale;
        imag1[i] = ((i1 * r2) - (r1 * i2)) * vscale;
    }
    
    full_frame.imagp[0] = (nyquist1 * nyquist2) * scale;
        
    // Inverse fft
        
    hisstools_rifft(m_fft_setup.get(), &full_frame, m_coefficients.data(), m_fft_setup.size());
}

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
    
    if (pitch > 1.0 && pitch != infinity() && centroid != infinity())
        m_value = centroid / pitch;
    else
        m_value = infinity();
}
