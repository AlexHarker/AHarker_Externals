
#ifndef _MODULES_CORE_HPP_
#define _MODULES_CORE_HPP_

#include "descriptors_modules.hpp"
#include "descriptors_graph.hpp"
#include "processing_containers.hpp"

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
    long num_bins() const { return static_cast<long>(m_spectrum.size()); }

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
    long num_bins() const { return static_cast<long>(m_spectrum.size()); }
    
private:
    
    module_power_spectrum *m_power_module;
    aligned_vector m_spectrum;
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
