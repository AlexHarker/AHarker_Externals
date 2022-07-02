
#ifndef _MODULES_SPECTRAL_HPP_
#define _MODULES_SPECTRAL_HPP_

#include "conversion_helpers.hpp"
#include "utility_definitions.hpp"
#include "modules_core.hpp"

// Basic Spectral Module

template <class T>
struct module_spectral : user_module_single<T>
{
    static user_module *setup(const global_params& params, module_arguments& args)
    {
        T *m = new T();
        
        m->m_lo_freq = args.get_double(0.0, 0.0, infinity());
        m->m_hi_freq = args.get_double(192000.0, 0.0, infinity());

        return m;
    }
    
    module_spectral() : m_lo_freq(0.0), m_hi_freq(0.0), m_min_bin(0), m_max_bin(0) {}
    
    module_spectral(double lo_freq, double hi_freq)
    : m_lo_freq(lo_freq), m_hi_freq(hi_freq), m_min_bin(0), m_max_bin(0) {}
    
    auto get_params() const { return std::make_tuple(m_lo_freq, m_hi_freq); }
    
    void prepare(const global_params& params) override
    {
        m_min_bin = freq_to_bin(m_lo_freq, params.m_sr, params.fft_size() / 2);
        m_max_bin = freq_to_bin(m_hi_freq, params.m_sr, params.fft_size() / 2);
    }
    
protected:
    
    long bin_count() const { return m_max_bin - m_min_bin; }
    
    double m_lo_freq;
    double m_hi_freq;
    long m_min_bin;
    long m_max_bin;
};

// Spectral Module With dB flag

template <class T>
struct module_spectral_db : module_spectral<T>
{
    static user_module *setup(const global_params& params, module_arguments& args)
    {
        T *m = dynamic_cast<T *>(module_spectral<T>::setup(params, args));
        
        m->m_report_db = args.get_bool(true);
        
        return m;
    }
    
    module_spectral_db() : module_spectral<T>(), m_report_db(true) {}
    
    module_spectral_db(double lo_freq, double hi_freq, bool report_db)
    : module_spectral<T>(lo_freq, hi_freq), m_report_db(report_db) {}
    
    auto get_params() const
    {
        return std::make_tuple(module_spectral<T>::get_params(), m_report_db);
    }
    
protected:
    
    bool m_report_db;
};

// Energy Ratio Module

struct module_energy_ratio : module_spectral<module_energy_ratio>
{
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_power_sum *m_power_sum_module;
    aligned_vector<> m_spectrum;
};

// Spectral Flatness Module

struct module_sfm : module_spectral<module_sfm>
{
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_amplitude_spectrum *m_amplitude_module;
    module_amplitude_sum *m_amplitude_sum_module;
    aligned_vector<> m_spectrum;
};

// Loudness Module

struct module_loudness : user_module_single<module_loudness>
{
    static user_module *setup(const global_params& params, module_arguments& args);
    
    module_loudness(bool report_db)
    : m_sr(0), m_report_db(report_db) {}
    
    auto get_params() const { return std::make_tuple(m_report_db); }
    
    void prepare(const global_params& params) override;
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_power_spectrum *m_power_module;
    aligned_vector<> m_loudness_curve;
    double m_sr;
    const bool m_report_db;
};

// Energy Module

struct module_energy : module_spectral_db<module_energy>
{
    module_energy() {}
    module_energy(double lo_freq, double hi_freq, bool report_db)
    : module_spectral_db(lo_freq, hi_freq, report_db) {}
    
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_power_sum *m_power_sum_module;
};

// Spectral Crest Module

struct module_spectral_crest : module_spectral_db<module_spectral_crest>
{
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_amplitude_spectrum *m_amplitude_module;
};

// Rolloff Module

struct module_rolloff : user_module_single<module_rolloff>
{
    static user_module *setup(const global_params& params, module_arguments& args);
    
    module_rolloff(double ratio)
    : m_ratio(ratio) {}
    
    auto get_params() const { return std::make_tuple(m_ratio); }

    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_power_spectrum *m_power_module;
    module_power_sum *m_power_sum_module;
    const double m_ratio;
};

// Spectral Linear Shape Modules

struct module_lin_centroid : module_spectral<module_lin_centroid>
{
    module_lin_centroid(){}
    
    module_lin_centroid(double lo_freq, double hi_freq)
    : module_spectral(lo_freq, hi_freq) {}
    
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
    double get_raw_centroid() const { return m_raw; }
    double get_sum() const { return m_sum; }
    
    const double *get_frame() const { return m_amplitude_module->get_frame(); }

private:
    
    module_amplitude_spectrum *m_amplitude_module;
    module_amplitude_sum *m_amplitude_sum_module;
    
    double m_raw;
    double m_sum;
};

struct module_lin_spread : module_spectral<module_lin_spread>
{
    module_lin_spread(){}
    
    module_lin_spread(double lo_freq, double hi_freq)
    : module_spectral(lo_freq, hi_freq) {}
    
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;

    const double *get_frame() const { return m_centroid_module->get_frame(); }

    double get_raw_spread() const { return m_raw; }
    double get_raw_centroid() const { return m_centroid_module->get_raw_centroid(); }
    double get_sum() const { return m_centroid_module->get_sum(); }
    
private:
    
    module_lin_centroid *m_centroid_module;
    
    double m_raw;
};

struct module_lin_skewness : module_spectral<module_lin_skewness>
{
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;

private:
    
    module_lin_spread *m_spread_module;
};

struct module_lin_kurtosis : module_spectral<module_lin_kurtosis>
{
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;

private:
    
    module_lin_spread *m_spread_module;
};

// Spectral Log Shape Modules

struct module_log_centroid : module_spectral<module_log_centroid>
{
    module_log_centroid(){}
    
    module_log_centroid(double lo_freq, double hi_freq)
    : module_spectral(lo_freq, hi_freq) {}
    
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;

    double get_raw_centroid() const { return m_raw; }
    double get_sum() const { return m_sum; }
    
    const double *get_frame() const { return m_amplitude_module->get_frame(); }
    const double *get_log_bins() const { return m_log_bins_module->get_log_bins(); }

private:
    
    module_log_bins *m_log_bins_module;
    module_amplitude_spectrum *m_amplitude_module;
    module_amplitude_sum *m_amplitude_sum_module;
    
    double m_raw;
    double m_sum;
};

struct module_log_spread : module_spectral<module_log_spread>
{
    module_log_spread(){}
    
    module_log_spread(double lo_freq, double hi_freq)
    : module_spectral(lo_freq, hi_freq) {}
    
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;

    const double *get_frame() const { return m_centroid_module->get_frame(); }
    const double *get_log_bins() const { return m_centroid_module->get_log_bins(); }

    double get_raw_spread() const { return m_raw; }
    double get_raw_centroid() const { return m_centroid_module->get_raw_centroid(); }
    double get_sum() const { return m_centroid_module->get_sum(); }

private:
    
    module_log_centroid *m_centroid_module;
    
    double m_raw;
};

struct module_log_skewness : module_spectral<module_log_skewness>
{
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_log_spread *m_spread_module;
};

struct module_log_kurtosis : module_spectral<module_log_kurtosis>
{
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_log_spread *m_spread_module;
};

#endif /* _MODULES_SPECTRAL_HPP_ */
