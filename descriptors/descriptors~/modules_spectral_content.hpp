
#ifndef _MODULES_SPECTRAL_CONTENT_HPP_
#define _MODULES_SPECTRAL_CONTENT_HPP_

#include "modules_core.hpp"
#include "modules_spectral.hpp"
#include "modules_pitch.hpp"

// Generic Noise Measure Module

template <class T>
struct module_noise_measure : user_module_single
{
    static user_module *setup(const global_params& params, module_arguments& args)
    {
        long median_span = args.get_long(15);

        return new T(median_span);
    }
        
    module_noise_measure(long median_span)
    : m_median_span(median_span) {}
    
    bool is_the_same(const module *m) const override
    {
        const T *m_typed = dynamic_cast<const T *>(m);
        
        return m_typed && m_typed->m_median_span == m_median_span;
    }
    
protected:
    
    const long m_median_span;
};

// Noise Ratio Module

struct module_noise_ratio : module_noise_measure<module_noise_ratio>
{
    module_noise_ratio(long median_span) : module_noise_measure(median_span) {}
    
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_energy *m_energy_module;
    module_median_power_spectrum *m_median_power_module;
};

// Harmonic Ratio Module

struct module_harmonic_ratio : module_noise_measure<module_harmonic_ratio>
{
    module_harmonic_ratio(long median_span) : module_noise_measure(median_span) {}

    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_noise_ratio *m_noise_ratio_module;
};

// Spectral Peaks Module

struct module_spectral_peaks : user_module_vector
{
    static user_module *setup(const global_params& params, module_arguments& args);
        
    module_spectral_peaks(long num_peaks)
    : m_num_peaks(num_peaks) {}
    
    bool is_the_same(const module *m) const override;
    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    const long m_num_peaks;
    module_peak_detection *m_peak_detection_module;
};

// Inharmonicity Module

struct module_inharmonicity : user_module_single
{
    static user_module *setup(const global_params& params, module_arguments& args);
        
    module_inharmonicity(long num_peaks, double threshold)
    : m_num_peaks(num_peaks), m_threshold(threshold) {}
    
    bool is_the_same(const module *m) const override;
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    const long m_num_peaks;
    const double m_threshold;
    module_peak_detection *m_peak_detection_module;
    module_pitch *m_pitch_module;
};

// Roughness Module

struct module_roughness : user_module_single
{
    static user_module *setup(const global_params& params, module_arguments& args);
        
    module_roughness(long num_peaks)
    : m_num_peaks(num_peaks) {}
    
    bool is_the_same(const module *m) const override;
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    const long m_num_peaks;
    module_peak_detection *m_peak_detection_module;
};

#endif /* _MODULES_SPECTRAL_CONTENT_HPP_ */
