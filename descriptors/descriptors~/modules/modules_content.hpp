
#ifndef _MODULES_SPECTRAL_CONTENT_HPP_
#define _MODULES_SPECTRAL_CONTENT_HPP_

#include "modules_core.hpp"
#include "modules_spectral.hpp"
#include "modules_pitch.hpp"

#include <limits>

// Generic Noise Measure Module

template <class T>
struct module_noise_measure : user_module_single<T>
{
    static user_module *setup(const global_params& params, module_arguments& args)
    {
        long median_span = args.get_long("median span", 15, 0, std::numeric_limits<long>::max());

        return new T(median_span);
    }
        
    module_noise_measure(long median_span)
    : m_median_span(median_span) {}
    
    auto get_params() const { return std::make_tuple(m_median_span); }
    
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
    
    module_power_sum *m_power_sum_module;
    module_median_amplitude_spectrum *m_median_amplitude_module;
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

struct module_spectral_peaks : user_module_vector<module_spectral_peaks>
{
    static user_module *setup(const global_params& params, module_arguments& args);
        
    module_spectral_peaks(long num_peaks, long median_span, double median_gain, double range, bool report_db)
    : m_num_peaks(num_peaks), m_median_span(median_span), m_median_gain(median_gain), m_range(range), m_report_db(report_db) {}
    
    auto get_params() const { return std::make_tuple(m_num_peaks, m_median_span); }

    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_peak_detection *m_peak_detection_module;

    const long m_num_peaks;
    const long m_median_span;
    const double m_median_gain;
    const double m_range;
    const bool m_report_db;
};

// Inharmonicity Module

struct module_inharmonicity : user_module_single<module_inharmonicity>
{
    static user_module *setup(const global_params& params, module_arguments& args);
        
    module_inharmonicity(long num_peaks, long median_span, double median_gain, double range, double threshold)
    : m_num_peaks(num_peaks), m_median_span(median_span), m_median_gain(median_gain), m_range(range), m_threshold(threshold) {}
    
    auto get_params() const { return std::make_tuple(m_num_peaks, m_threshold); }

    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_peak_detection *m_peak_detection_module;
    module_pitch *m_pitch_module;
    
    const long m_num_peaks;
    const long m_median_span;
    const double m_median_gain;
    const double m_range;
    const double m_threshold;
};

// Roughness Module

struct module_roughness : user_module_single<module_roughness>
{
    static user_module *setup(const global_params& params, module_arguments& args);
        
    module_roughness(long num_peaks, long median_span, double median_gain, double range)
    : m_num_peaks(num_peaks), m_median_span(median_span), m_median_gain(median_gain), m_range(range) {}
    
    auto get_params() const { return std::make_tuple(m_num_peaks); }

    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_peak_detection *m_peak_detection_module;

    const long m_num_peaks;
    const long m_median_span;
    const double m_median_gain;
    const double m_range;
};

#endif /* _MODULES_SPECTRAL_CONTENT_HPP_ */
