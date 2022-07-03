
#ifndef _MODULES_CORE_HPP_
#define _MODULES_CORE_HPP_

#include "../descriptors_modules.hpp"
#include "processing_containers.hpp"

#include "../library/median_filter.hpp"
#include "../library/peak_finder.hpp"

#include <algorithm>
#include <vector>

// A Generic Core Module

template <class T>
struct module_core : comparable_module<T>
{
    auto get_params() const { return std::make_tuple(); }
};

// Windowing Module

struct module_window : module_core<module_window>
{
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
    const double *get_frame() const { return m_windowed_frame.data(); }
    const double get_energy_compensation() const { return m_energy_compensation; }
    const double get_rms_compensation() const { return m_rms_compensation; }

private:
    
    aligned_vector<> m_window;
    aligned_vector<> m_windowed_frame;
    
    double m_rms_compensation;
    double m_energy_compensation;
    long m_frame_size = 0;
    t_symbol *m_window_type = nullptr;
};

// FFT Module

struct module_fft : module_core<module_fft>
{
    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
    const FFT_SPLIT_COMPLEX_D& get_frame() const { return m_fft_frame.data(); }
    const double get_energy_compensation() const { return m_energy_compensation; }

private:
    
    module_window *m_window_module;
    
    fft_setup m_fft_setup;
    fft_split m_fft_frame;
    
    double m_energy_compensation;
};

// Power Spectrum Module

struct module_power_spectrum : module_core<module_power_spectrum>
{
    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
    const double *get_frame() const { return m_spectrum.data(); }
    const double get_energy_compensation() const { return m_energy_compensation; }

private:
    
    module_fft *m_fft_module;
    
    aligned_vector<> m_spectrum;
    
    double m_energy_compensation;
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
    
    aligned_vector<> m_spectrum;
};

// Frame Summing Modules

template <class T, class U>
struct module_frame_sum : module_core<U>
{
    void add_requirements(graph& g) override;
    
    void prepare(const global_params& params) override
    {
        m_cumulative_spectrum.resize(params.num_bins());
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        const double *spectrum = m_core_module->get_frame();
        double *cumulative = m_cumulative_spectrum.data();
        
        double sum = 0.0;
        
        for (long i = 0; i < params.num_bins(); i++)
            cumulative[i] = (sum += spectrum[i]);
    }
    
    const double get_sum(long min_bin, long max_bin)
    {
        double *cumulative = m_cumulative_spectrum.data();
        
        return min_bin ? cumulative[max_bin - 1] - cumulative[min_bin - 1] : cumulative[max_bin - 1];
    }
    
protected:
    
    T *m_core_module;
    
    aligned_vector<> m_cumulative_spectrum;
};

// Power / Amplitude Sums

struct module_power_sum : module_frame_sum<module_power_spectrum, module_power_sum>
{
    void add_requirements(graph& g) override;
    const double get_energy_compensation() const { return m_core_module->get_energy_compensation(); }
};

struct module_amplitude_sum : module_frame_sum<module_amplitude_spectrum, module_amplitude_sum>
{
    void add_requirements(graph& g) override;
};

// Median Amplitude Spectrum Module

struct module_median_amplitude_spectrum : comparable_module<module_median_amplitude_spectrum>
{
    module_median_amplitude_spectrum(long median_width)
    : m_filter(median_width), m_median_width(median_width) {}
    
    auto get_params() const { return std::make_tuple(m_median_width); }
    
    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
    const double *get_frame() const { return m_spectrum.data(); }
    
private:
    
    module_amplitude_spectrum *m_amplitude_module;
    
    median_filter<double> m_filter;
    aligned_vector<> m_spectrum;
    
    const long m_median_width;
};

// Log Bins Module

struct module_log_bins : module_core<module_log_bins>
{
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override {}
    
    const double *get_log_bins() const { return m_log_bins.data(); }
    
private:
    
    aligned_vector<> m_log_bins;
};

// Peak Detection Module

struct module_peak_detection : module_core<module_peak_detection>
{
    using peak_list = peak_set<double>;
    using peak_detector = peak_finder<double>;

    module_peak_detection(long median_width)
    : m_median_width(median_width) {}
    
    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
    const peak_list& get_peaks() const { return m_peaks; }

private:
    
    module_amplitude_spectrum *m_amplitude_module;
    module_median_amplitude_spectrum *m_median_amplitude_module;
    
    peak_detector m_detector;
    peak_list m_peaks;
    
    const long m_median_width;
};

// Amplitude Ring Buffer Module

struct module_spectrum_ring_buffer : module_core<module_spectrum_ring_buffer>
{
    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    void update_empty(const global_params& params) override;

    const double *get_frame(long frame_lag) const { return m_spectra[get_idx(frame_lag)].data(); }
    void request_lag(long lag) { m_max_lag = std::max(m_max_lag, lag); }
    
private:
    
    long get_idx(long lag) const;
    
    module_amplitude_spectrum *m_amplitude_module;
    
    std::vector<aligned_vector<>> m_spectra;
    
    long m_counter = 0;
    long m_max_lag = 0;
};

// Log Amplitude Ring Buffer Module

struct module_log_spectrum_ring_buffer : module_core<module_log_spectrum_ring_buffer>
{
    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    void update_empty(const global_params& params) override;

    const double *get_frame(long frame_lag) const { return m_spectra[get_idx(frame_lag)].data(); }
    void request_lag(long lag) { m_max_lag = std::max(m_max_lag, lag); }
    
private:
    
    long get_idx(long lag) const;
    
    module_amplitude_spectrum *m_amplitude_module;
    
    std::vector<aligned_vector<>> m_spectra;
    
    long m_counter = 0;
    long m_max_lag = 0;
};

// Autocorrelation Module

struct module_autocorrelation : module_core<module_autocorrelation>
{
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
    const double *get_coefficients() const { return m_coefficients.data(); }
    long get_length() const { return m_coefficients_size; }

private:
    
    fft_setup m_fft_setup;
    fft_split m_full_frame;
    fft_split m_half_frame;
    aligned_vector<> m_coefficients;
    
    long m_coefficients_size;
};

#endif /* _MODULES_CORE_HPP_ */
