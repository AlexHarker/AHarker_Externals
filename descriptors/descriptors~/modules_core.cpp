
#include "modules_core.hpp"

#include <WindowFunctions.hpp>

#include <algorithm>
#include <numeric>

using VecType = SIMDType<double, SIMDLimits<double>::max_size>;

// Windowing Module

bool module_window::is_the_same(const module *m) const
{
    return dynamic_cast<const module_window *>(m);
}

void module_window::prepare(const global_params& params)
{
    m_windowed_frame.resize(params.m_frame_size);
    m_window.resize(params.m_frame_size);
    
    double *window = m_window.data();
    uint32_t window_size = static_cast<uint32_t>(m_window.size());

    t_symbol *window_type = params.m_window_type;

    // FIX - parameters where needed.
    window_functions::params win_params;
    
    if (window_type == gensym("rect"))
        window_functions::rect(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("hamming"))
        window_functions::hamming(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("kaiser"))
        window_functions::kaiser(window, window_size, 0, window_size, win_params);
            //(x_sq = 46.24;)
    else if (window_type == gensym("blackman"))
        window_functions::triangle(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("triangle"))
        window_functions::exact_blackman(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("blackman62"))
        window_functions::blackman_harris_62dB(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("blackman70"))
        window_functions::blackman_harris_71dB(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("blackman74"))
        window_functions::blackman_harris_74dB(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("blackman92"))
        window_functions::blackman_harris_92dB(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("blackman-harris"))
        window_functions::blackman_harris_92dB(window, window_size, 0, window_size, win_params);
    else if (window_type == gensym("flattop"))
        window_functions::ni_flat_top(window, window_size, 0, window_size, win_params);
    else
        window_functions::hann(window, window_size, 0, window_size, win_params);
    
    // N.B. Default to hann
    
    // Calculate the gain of the window and apply compensation
    
    double gain = 1.0 / std::accumulate(window, window + window_size, 0.0);
    std::transform(window, window + window_size, window, [&](double a) { return a * gain; });
    
    /*
         using VecType = SIMDType<double, SIMDLimits<double>::max_size>;

    // Calculate the energy gain of the window

    // Copy window

    for (i = 0; i < window_size; i++)
        raw_frame[i] = window[i] * sin((8 * FFTW_TWOPI * (double) i) / (double) fft_size);
        
    // Do fft with zero padding
        
    hisstools_rfft(fft_setup_real, raw_frame, &raw_fft_frame, window_size, fft_size_log2);
    
    // Zero nyquist bin (have to match what is done elsewhere)
    
    raw_fft_frame.imagp[0] = 0.f;
    
    // Calulate square amplitudes
        
    for (i = 0; i < fft_size_halved >> 2; i++)
        v_amplitudes[i] = (real_data[i] * real_data[i]) + (imag_data[i] * imag_data[i]);
        
    // Now sum

    for (i = 0, gain = 0.0; i < fft_size_halved; i++)
        gain += amplitudes[i];
        
    // N.B. - the window has been scaled for amplitude gain
    
    x->energy_compensation = 1.0 / gain;*/
}

void module_window::calculate(const global_params& params, const double *frame, long size)
{
    double* windowed_frame = m_windowed_frame.data();
    double* window = m_window.data();
            
    // FIX - vectorise
    
    for (long i = 0; i < size; i++)
        windowed_frame[i] = frame[i] * window[i];
}

// FFT Module

bool module_fft::is_the_same(const module *m) const
{
    return dynamic_cast<const module_fft *>(m);
}

void module_fft::add_requirements(graph& g)
{
    m_window_module = g.add_requirement(new module_window());
}

void module_fft::prepare(const global_params& params)
{
    m_fft_frame.resize(params.fft_size());
    m_fft_setup.resize(params.m_fft_size_log2);
}

void module_fft::calculate(const global_params& params, const double *frame, long size)
{
    // Get windowed frame
    
    const double *windowed_frame = m_window_module->get_frame();

    // Do FFT (as well as any zero padding)
    
    hisstools_rfft(m_fft_setup.get(), windowed_frame, &m_fft_frame.data(), size, m_fft_setup.size());
}

// Power Spectrum Module

bool module_power_spectrum::is_the_same(const module *m) const
{
    return dynamic_cast<const module_power_spectrum *>(m);
}

void module_power_spectrum::add_requirements(graph& g)
{
    m_fft_module = g.add_requirement(new module_fft());
}

void module_power_spectrum::prepare(const global_params& params)
{
    m_spectrum.resize((params.fft_size() >> 1) + 1);
}

void module_power_spectrum::calculate(const global_params& params, const double *frame, long size)
{
    auto fft_frame = m_fft_module->get_frame();
    
    VecType *power = reinterpret_cast<VecType *>(m_spectrum.data());
    const VecType *real = reinterpret_cast<const VecType *>(fft_frame.realp);
    const VecType *imag = reinterpret_cast<const VecType *>(fft_frame.imagp);
    long nyquist = num_bins() - 1;
    long loop_size = nyquist / VecType::size;
    
    // Calculate power spectrum
    
    for (long i = 0; i < loop_size; i++)
        power[i] = (real[i] * real[i]) + (imag[i] * imag[i]);
    
    // Fix DC and Nyquist Values
    
    m_spectrum.data()[0] = fft_frame.realp[0] * fft_frame.realp[0];
    m_spectrum.data()[nyquist] = fft_frame.imagp[0] * fft_frame.imagp[0];
}

// Amplitude Spectrum Module

bool module_amplitude_spectrum::is_the_same(const module *m) const
{
    return dynamic_cast<const module_amplitude_spectrum *>(m);
}

void module_amplitude_spectrum::add_requirements(graph& g)
{
    m_power_module = g.add_requirement(new module_power_spectrum());
}

void module_amplitude_spectrum::prepare(const global_params& params)
{
    m_spectrum.resize((params.fft_size() >> 1) + 1);
}

void module_amplitude_spectrum::calculate(const global_params& params, const double *frame, long size)
{
    const double *power_frame = m_power_module->get_frame();
    
    VecType *amps = reinterpret_cast<VecType *>(m_spectrum.data());
    const VecType *power = reinterpret_cast<const VecType *>(power_frame);
    long nyquist = num_bins() - 1;
    long loop_size = nyquist / VecType::size;
    
    // Calculate amplitude spectrum
    
    for (long i = 0; i < loop_size; i++)
        amps[i] = sqrt(power[i]);
    
    // Do Nyquist Value
    
    m_spectrum.data()[nyquist] = sqrt(power_frame[nyquist]);
}
