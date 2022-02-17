
#ifndef _MODULES_CORE_HPP_
#define _MODULES_CORE_HPP_

#include "descriptors_modules.hpp"
#include "processing_containers.hpp"

#include "median_filter.hpp"
#include "peak_finder.hpp"

#include <algorithm>
#include <vector>

// A Generic Core Module

template <class T>
struct module_core : module
{
    bool is_the_same(const module *m) const override
    {
        return dynamic_cast<const T *>(m);
    }
};

// Windowing Module

struct module_window : module_core<module_window>
{
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
    const double *get_frame() const { return m_windowed_frame.data(); }

private:
    
    aligned_vector m_window;
    aligned_vector m_windowed_frame;
};

// FFT Module

struct module_fft : module_core<module_fft>
{
    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
    const FFT_SPLIT_COMPLEX_D& get_frame() const { return m_fft_frame.data(); }
    
private:
    
    module_window *m_window_module;
    fft_setup m_fft_setup;
    fft_split m_fft_frame;
};

// Power Spectrum Module

struct module_power_spectrum : module_core<module_power_spectrum>
{
    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
    const double *get_frame() const { return m_spectrum.data(); }

private:
    
    module_fft *m_fft_module;
    aligned_vector m_spectrum;
};

// Amplitude Spectrum Module

struct module_amplitude_spectrum : module_core<module_amplitude_spectrum>
{
    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
    const double *get_frame() const { return m_spectrum.data(); }
    
private:
    
    module_power_spectrum *m_power_module;
    aligned_vector m_spectrum;
};

// Median Power Spectrum Module

struct module_median_power_spectrum : module
{
    module_median_power_spectrum(long median_span)
    : m_filter(median_span), m_median_span(median_span) {}
    
    bool is_the_same(const module *m) const override;
    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
    const double *get_frame() const { return m_spectrum.data(); }
    
private:
    
    module_power_spectrum *m_power_module;
    median_filter<double> m_filter;
    aligned_vector m_spectrum;
    const long m_median_span;
};

// Peak Detection Module

struct module_peak_detection : module_core<module_peak_detection>
{
    using peak_list = peak_set<double>;
    using peak_detector = peak_finder<double>;

    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
    const peak_list& get_peaks() const { return m_peaks; }

private:
    
    module_amplitude_spectrum *m_amplitude_module;
    peak_list m_peaks;
    peak_detector m_detector;
};

// Ring Buffer Modules

struct module_spectrum_ring_buffer : module_core<module_spectrum_ring_buffer>
{
    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
    const double *get_frame(long frame_lag) const { return m_spectra[get_idx(frame_lag)].data(); }
    void request_lag(long lag) { m_max_lag = std::max(m_max_lag, lag); }
    
private:
    
    long get_idx(long lag) const;
    
    module_amplitude_spectrum *m_amplitude_module;
    std::vector<aligned_vector> m_spectra;
    long m_counter = 0;
    long m_max_lag = 0;
};

struct module_log_spectrum_ring_buffer : module_core<module_log_spectrum_ring_buffer>
{
    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
    const double *get_frame(long frame_lag) const { return m_spectra[get_idx(frame_lag)].data(); }
    void request_lag(long lag) { m_max_lag = std::max(m_max_lag, lag); }
    
private:
    
    long get_idx(long lag) const;
    
    module_amplitude_spectrum *m_amplitude_module;
    std::vector<aligned_vector> m_spectra;
    long m_counter = 0;
    long m_max_lag = 0;
};

// Autocorrelation Module

struct module_autocorrelation : module_core<module_autocorrelation>
{
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
    const double *get_coefficients() const { return m_coefficients.data(); }
    long get_length() const { return static_cast<long>(m_coefficients.size()); }

private:
    
    fft_setup m_fft_setup;
    fft_split m_full_frame;
    fft_split m_half_frame;
    aligned_vector m_coefficients;
};

#endif /* _MODULES_CORE_HPP_ */
