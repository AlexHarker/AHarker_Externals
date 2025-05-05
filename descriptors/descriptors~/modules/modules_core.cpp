
#include "modules_core.hpp"
#include "utility_definitions.hpp"

#include "../descriptors_graph.hpp"

#include <SIMDExtended.hpp>
#include <statistics.hpp>
#include <window.hpp>

#include <algorithm>
#include <numeric>

// Power Spectrum Helper

void calculate_power_spectrum(double *spectrum, const htl::split_type<double> &fft_frame, long num_bins)
{
    VecType *power = reinterpret_cast<VecType *>(spectrum);
    const VecType *real = reinterpret_cast<const VecType *>(fft_frame.realp);
    const VecType *imag = reinterpret_cast<const VecType *>(fft_frame.imagp);
    long nyquist = num_bins - 1;
    long loop_size = nyquist / VecType::size;
    
    // Calculate power spectrum
    
    for (long i = 0; i < loop_size; i++)
        power[i] = (real[i] * real[i]) + (imag[i] * imag[i]);
    
    // Calculate DC and Nyquist Values (replace any previous values)
    
    spectrum[0] = fft_frame.realp[0] * fft_frame.realp[0];
    spectrum[nyquist] = fft_frame.imagp[0] * fft_frame.imagp[0];
}

// Windowing Module

void module_window::prepare(const global_params& params)
{
    auto gain_compensation = [](double *data, long size)
    {
        return 1.0 / std::accumulate(data, data + size, 0.0);
    };
    
    if (params.frame_size() == m_frame_size && params.window_type() == m_window_type)
        return;
    
    m_frame_size = params.frame_size();
    m_window_type = params.window_type();
    
    m_windowed_frame.resize(params.frame_size());
    m_window.resize(params.frame_size());
    
    double *window = m_window.data();
    uint32_t window_size = static_cast<uint32_t>(m_window.size());

    t_symbol *window_type = params.window_type();
    
    htl::window::params win_params(6.8);
    
    if (window_type == gensym("rect"))
        htl::window::rect(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("hamming"))
        htl::window::hamming(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("kaiser"))
        htl::window::kaiser(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("triangle"))
        htl::window::triangle(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("blackman"))
        htl::window::exact_blackman(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("blackman62"))
        htl::window::blackman_harris_62dB(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("blackman70"))
        htl::window::blackman_harris_71dB(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("blackman74"))
        htl::window::blackman_harris_74dB(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("blackman92"))
        htl::window::blackman_harris_92dB(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("blackman-harris"))
        htl::window::blackman_harris_92dB(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("flattop"))
        htl::window::ni_flat_top(window, window_size, 0, window_size, win_params);
    else
        htl::window::hann(window, window_size, 0, window_size, win_params);
    
    // N.B. Default to hann
    
    // Calculate the gain of the window and apply compensation
    
    double gain = gain_compensation(window, window_size);
    std::transform(window, window + window_size, window, [&](double a) { return a * gain; });
    
    // Calculate the energy gain of the window

    auto frame_size = params.frame_size();
    auto fft_size_log2 = params.fft_size_log2();
    auto num_bins = params.num_bins();
    auto fft_size = params.fft_size();
    
    fft_setup setup(fft_size_log2);
    aligned_vector<> raw_frame(frame_size);
    aligned_vector<> power_spectrum(num_bins);
    fft_split spectrum(params.fft_size());

    // Create window

    for (long i = 0; i < frame_size; i++)
        raw_frame.data()[i] = window[i] * sin((16.0 * M_PI * i) / fft_size);
        
    // Do fft / calculate power spectrum / calculate energy compensation
        
    htl::rfft(setup.get(), raw_frame.data(), &spectrum.data(), frame_size, fft_size_log2);
    calculate_power_spectrum(power_spectrum.data(), spectrum.data(), num_bins);
    m_energy_compensation = gain_compensation(power_spectrum.data(), num_bins);
    m_rms_compensation = 1.0 / htl::stat_rms(raw_frame.data(), frame_size);
}

template <class T>
void multiply_loop(T *out, const T *a, const T *b, uintptr_t size)
{
    for (uintptr_t i = 0; i < size; i++)
        *out++ = *a++ * *b++;
}

void module_window::calculate(const global_params& params, const double *frame, long size)
{
    constexpr long v_size = htl::simd_limits<double>::max_size;
    using V = htl::simd_type<double, v_size>;

    double* windowed_frame = m_windowed_frame.data();
    const double* window = m_window.data();
     
    V *v_windowed_frame = reinterpret_cast<V *>(windowed_frame);
    const V *v_frame = reinterpret_cast<const V *>(frame);
    const V *v_window = reinterpret_cast<const V *>(window);
    
    long v_loop = size / v_size;
    long vs_loop = v_size * v_size;
        
    multiply_loop(v_windowed_frame, v_frame, v_window, v_loop);
    multiply_loop(windowed_frame + vs_loop, frame + vs_loop, window + vs_loop, size - vs_loop);
}

// FFT Module

void module_fft::add_requirements(graph& g)
{
    m_window_module = g.add_requirement(new module_window());
}

void module_fft::prepare(const global_params& params)
{
    m_fft_frame.resize(params.fft_size());
    m_fft_setup.resize(params.fft_size_log2());
    m_energy_compensation = m_window_module->get_energy_compensation();
}

void module_fft::calculate(const global_params& params, const double *frame, long size)
{
    // Get windowed frame
    
    const double *windowed_frame = m_window_module->get_frame();

    // Do FFT (as well as any zero padding)
    
    htl::rfft(m_fft_setup.get(), windowed_frame, &m_fft_frame.data(), size, m_fft_setup.size());
}

// Power Spectrum Module

void module_power_spectrum::add_requirements(graph& g)
{
    m_fft_module = g.add_requirement(new module_fft());
}

void module_power_spectrum::prepare(const global_params& params)
{
    m_spectrum.resize(params.num_bins());
    m_energy_compensation = m_fft_module->get_energy_compensation();
}

void module_power_spectrum::calculate(const global_params& params, const double *frame, long size)
{
    calculate_power_spectrum(m_spectrum.data(), m_fft_module->get_frame(), params.num_bins());
}

// Amplitude Spectrum Module

void module_amplitude_spectrum::add_requirements(graph& g)
{
    m_power_module = g.add_requirement(new module_power_spectrum());
}

void module_amplitude_spectrum::prepare(const global_params& params)
{
    m_spectrum.resize(params.num_bins());
}

void module_amplitude_spectrum::calculate(const global_params& params, const double *frame, long size)
{
    const double *power_frame = m_power_module->get_frame();
    
    VecType *amps = reinterpret_cast<VecType *>(m_spectrum.data());
    const VecType *power = reinterpret_cast<const VecType *>(power_frame);
    long nyquist = params.num_bins() - 1;
    long loop_size = nyquist / VecType::size;
    
    // Calculate amplitude spectrum
    
    for (long i = 0; i < loop_size; i++)
        amps[i] = sqrt(power[i]);
    
    // Do Nyquist Value
    
    m_spectrum.data()[nyquist] = sqrt(power_frame[nyquist]);
}

// Frame Summming Modules (N.B. pass through methods to ensure linking)

template <class T, class U>
void module_frame_sum<T, U>::add_requirements(graph& g)
{
    m_core_module = g.add_requirement(new T());
}

void module_power_sum::add_requirements(graph& g)
{
    module_frame_sum::add_requirements(g);
}

void module_amplitude_sum::add_requirements(graph& g)
{
    module_frame_sum::add_requirements(g);
}

// Median Amplitude Spectrum Module

void module_median_amplitude_spectrum::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
}

void module_median_amplitude_spectrum::prepare(const global_params& params)
{
    m_spectrum.resize(params.num_bins());
}

void module_median_amplitude_spectrum::calculate(const global_params& params, const double *frame, long size)
{
    const double *amplitudes = m_amplitude_module->get_frame();

    m_filter(m_spectrum.data(), amplitudes, params.num_bins(), m_median_width, median_filter<double>::Edges::Fold, 50.0);
}

// Log Bins Module

void module_log_bins::prepare(const global_params& params)
{
    m_log_bins.resize(params.num_bins());

    double *log_bins = m_log_bins.data();
        
    for (long i = 0; i < params.num_bins(); i++)
        log_bins[i] = i ? log2(i): 0;
}
    
// Peak Detection Module

void module_peak_detection::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
    m_median_amplitude_module = g.add_requirement(new module_median_amplitude_spectrum(m_median_width));
}

void module_peak_detection::prepare(const global_params& params)
{
    m_peaks.resize(params.num_bins() / 2);
    m_detector.resize(params.num_bins());
}

void module_peak_detection::calculate(const global_params& params, const double *frame, long size)
{
    const double *spectrum = m_amplitude_module->get_frame();
    const double *median_spectrum = m_median_amplitude_module->get_frame();

    peak_detector::options options;
    
    options.mask_gain = m_median_gain;
    
    m_detector(m_peaks, spectrum, median_spectrum, params.num_bins(), options);
}

// Spectrum Ring Buffer Module

void module_spectrum_ring_buffer::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
}

void module_spectrum_ring_buffer::prepare(const global_params& params)
{
    for (long i = 0; i < m_max_lag + 1; i++)
    {
        if (m_spectra.size() <= i)
            m_spectra.emplace_back(params.num_bins());
        m_spectra[i].resize(params.num_bins());
        std::fill_n(m_spectra[i].data(), params.num_bins(), 0.0);
    }
}

void module_spectrum_ring_buffer::calculate(const global_params& params, const double *frame, long size)
{
    const double *spectrum = m_amplitude_module->get_frame();
        
    std::copy_n(spectrum, params.num_bins(), m_spectra[m_counter].data());
    
    m_counter = (m_counter + 1) % (m_max_lag + 1);
}

void module_spectrum_ring_buffer::update_empty(const global_params& params)
{
    double *spectrum_out = m_spectra[m_counter].data();
        
    std::fill_n(spectrum_out, params.num_bins(), 0.0);
    
    m_counter = (m_counter + 1) % (m_max_lag + 1);
}

long module_spectrum_ring_buffer::get_idx(long lag) const
{
    return (m_max_lag + (m_counter - std::min(m_max_lag, lag))) % (m_max_lag + 1);
}

// Log Spectrum Ring Buffer Module

void module_log_spectrum_ring_buffer::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
}

void module_log_spectrum_ring_buffer::prepare(const global_params& params)
{
    for (long i = 0; i < m_max_lag + 1; i++)
    {
        if (m_spectra.size() <= i)
            m_spectra.emplace_back(params.num_bins());
        m_spectra[i].resize(params.num_bins());
        std::fill_n(m_spectra[i].data(), params.num_bins(), m_log_min);
    }
}

void module_log_spectrum_ring_buffer::calculate(const global_params& params, const double *frame, long size)
{
    const double *spectrum_in = m_amplitude_module->get_frame();
    double *spectrum_out = m_spectra[m_counter].data();
    log_array(spectrum_out, spectrum_in, params.num_bins());
    
    for (long i = 0; i < params.num_bins(); i++)
        spectrum_out[i] = std::max(spectrum_out[i], m_log_min);
    
    m_counter = (m_counter + 1) % (m_max_lag + 1);
}

void module_log_spectrum_ring_buffer::update_empty(const global_params& params)
{
    double *spectrum_out = m_spectra[m_counter].data();
        
    for (long i = 0; i < params.num_bins(); i++)
        spectrum_out[i] = m_log_min;
    
    m_counter = (m_counter + 1) % (m_max_lag + 1);
}

long module_log_spectrum_ring_buffer::get_idx(long lag) const
{
    return (m_max_lag + (m_counter - std::min(m_max_lag, lag))) % (m_max_lag + 1);
}

// Autocorrelation Module

void module_autocorrelation::prepare(const global_params& params)
{
    long out_size = params.frame_size() + (params.frame_size() >> 1);
    long fft_size_log2 = int_log2(out_size);
    long fft_size = 1 << fft_size_log2;
    
    m_fft_setup.resize(fft_size_log2);
    m_full_frame.resize(fft_size);
    m_half_frame.resize(fft_size);
    m_coefficients.resize(fft_size);
    
    m_coefficients_size = params.frame_size();
}

void module_autocorrelation::calculate(const global_params& params, const double *frame, long size)
{
    auto full_frame = m_full_frame.data();
    auto half_frame = m_half_frame.data();
    
    VecType *real1 = reinterpret_cast<VecType *>(full_frame.realp);
    VecType *imag1 = reinterpret_cast<VecType *>(full_frame.imagp);
    VecType *real2 = reinterpret_cast<VecType *>(half_frame.realp);
    VecType *imag2 = reinterpret_cast<VecType *>(half_frame.imagp);
        
    long fft_size = 1 << m_fft_setup.size();
    double scale = 0.0;
    
    // Calculate normalisation factor
    
    for (long i = 0; i < (size >> 1); i++)
        scale += frame[i] * frame[i];
        
    scale = 0.25 / (fft_size * (scale ? scale : 1.0));
        
    // Do ffts straight into position with zero padding (one half the size of the other)
    
    htl::rfft(m_fft_setup.get(), frame, &full_frame, size, m_fft_setup.size());
    htl::rfft(m_fft_setup.get(), frame, &half_frame, (size >> 1), m_fft_setup.size());
        
    // Calculate ac coefficients
    
    VecType vscale(scale);
    
    const double nyquist1 = full_frame.imagp[0];
    const double nyquist2 = half_frame.imagp[0];
        
    full_frame.imagp[0] = 0.0;
    half_frame.imagp[0] = 0.0;
        
    for (long i = 0; i < fft_size / (2 * VecType::size); i++)
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
        
    htl::rifft(m_fft_setup.get(), &full_frame, m_coefficients.data(), m_fft_setup.size());
}
