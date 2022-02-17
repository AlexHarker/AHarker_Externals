
#ifndef _MODULES_SPECTRAL_HPP_
#define _MODULES_SPECTRAL_HPP_

#include "conversion_helpers.hpp"
#include "modules_core.hpp"

// Basic Spectral Module

template <class T>
struct module_spectral : user_module_single
{
    static user_module *setup(const global_params& params, module_arguments& args)
    {
        T *m = new T();
        
        m->m_lo_freq = args.get_double(0.0);
        m->m_hi_freq = args.get_double(192000.0);

        return m;
    }
    
    module_spectral() : m_lo_freq(0.0), m_hi_freq(0.0), m_min_bin(0), m_max_bin(0) {}
    
    module_spectral(double lo_freq, double hi_freq)
    : m_lo_freq(lo_freq), m_hi_freq(hi_freq), m_min_bin(0), m_max_bin(0) {}
    
    bool is_the_same(const module *m) const override
    {
        const T *m_typed = dynamic_cast<const T *>(m);
        
        return m_typed && m_typed->m_lo_freq == m_lo_freq && m_typed->m_hi_freq == m_hi_freq;
    }
    
    void prepare(const global_params& params) override
    {
        m_min_bin = freq_to_bin(m_lo_freq, params.m_sr, params.fft_size() / 2);
        m_max_bin = freq_to_bin(m_hi_freq, params.m_sr, params.fft_size() / 2);
    }
    
protected:
    
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
    
    bool is_the_same(const module *m) const override
    {
        const T *m_typed = dynamic_cast<const T *>(m);
        
        return module_spectral<T>::is_the_same(m) && m_typed->m_report_db == m_report_db;
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
    
    module_power_spectrum *m_power_module;
    aligned_vector m_spectrum;
};

// Spectral Flatness Module

struct module_sfm : module_spectral<module_sfm>
{
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_amplitude_spectrum *m_amplitude_module;
    aligned_vector m_spectrum;
};

// Loudness Module

struct module_loudness : user_module_single
{
    static user_module *setup(const global_params& params, module_arguments& args);
    
    module_loudness(bool report_db)
    : m_report_db(report_db) {}
    
    bool is_the_same(const module *m) const override;
    void prepare(const global_params& params) override;
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_power_spectrum *m_power_module;
    aligned_vector m_loudness_curve;
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
    
    module_power_spectrum *m_power_module;
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

struct module_rolloff : user_module_single
{
    static user_module *setup(const global_params& params, module_arguments& args);
    
    bool is_the_same(const module *m) const override;
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_power_spectrum *m_power_module;
    double m_centile;
};

// Spectral Linear Shape Modules

struct module_lin_centroid : module_spectral<module_lin_centroid>
{
    module_lin_centroid(){}
    
    module_lin_centroid(double lo_freq, double hi_freq)
    : module_spectral(lo_freq, hi_freq) {}
    
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_amplitude_spectrum *m_amplitude_module;
};

struct module_lin_spread : module_spectral<module_lin_spread>
{
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;

private:
    
    module_amplitude_spectrum *m_amplitude_module;
};

struct module_lin_skewness : module_spectral<module_lin_skewness>
{
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;

private:
    
    module_amplitude_spectrum *m_amplitude_module;
};

struct module_lin_kurtosis : module_spectral<module_lin_kurtosis>
{
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;

private:
    
    module_amplitude_spectrum *m_amplitude_module;
};

// Spectral Log Shape Modules

struct module_log_centroid : module_spectral<module_log_centroid>
{
    module_log_centroid(){}
    module_log_centroid(double lo_freq, double hi_freq)
    : module_spectral(lo_freq, hi_freq) {}
    
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;

private:
    
    module_amplitude_spectrum *m_amplitude_module;
};

struct module_log_spread : module_spectral<module_log_spread>
{
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;

private:
    
    module_amplitude_spectrum *m_amplitude_module;
};

struct module_log_skewness : module_spectral<module_log_skewness>
{
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_amplitude_spectrum *m_amplitude_module;
};

struct module_log_kurtosis : module_spectral<module_log_kurtosis>
{
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_amplitude_spectrum *m_amplitude_module;
};

#endif /* _MODULES_SPECTRAL_HPP_ */
