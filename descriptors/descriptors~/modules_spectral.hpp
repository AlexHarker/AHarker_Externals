
#ifndef _MODULES_SPECTRAL_HPP_
#define _MODULES_SPECTRAL_HPP_

#include "descriptors_conversion_helpers.h"

#include "modules_core.hpp"
#include <SIMDSupport.hpp>
#include <Statistics.hpp>

#include <algorithm>
#include <cmath>

using VecType = SIMDType<double, SIMDLimits<double>::max_size>;

// Basic Spectral Module

template <class T>
struct module_spectral : user_module_single
{
    static user_module *setup(const global_params& params, long argc, t_atom *argv)
    {
        T *m = new T();
        
        m->m_lo_freq = argc > 0 ? atom_getfloat(argv + 0) : 0.0;
        m->m_hi_freq = argc > 1 ? atom_getfloat(argv + 1) : 192000.0;

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
        get_bin_range(m_min_bin, m_max_bin, m_lo_freq, m_hi_freq, params.m_sr, params.fft_size() / 2);
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
    static user_module *setup(const global_params& params, long argc, t_atom *argv)
    {
        T *m = dynamic_cast<T *>(module_spectral<T>::setup(params, argc, argv));
        
        m->m_report_db = argc > 2 ? atom_getfloat(argv + 2) : true;
        
        return m;
    }
    
    bool is_the_same(const module *m) const override
    {
        const T *m_typed = dynamic_cast<const T *>(m);
        
        return module_spectral<T>::is_the_same(m) && m_typed->m_report_db == m_report_db;
    }
    
protected:
    
    bool m_report_db;
};

// Energy Ratio Module

// USES ENERGY OR AMPS?

struct module_energy_ratio : module_spectral<module_energy_ratio>
{
    void add_requirements(graph& g) override
    {
        m_power_module = g.add_requirement(new module_power_spectrum());
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        const double *power = m_power_module->get_frame();
        long num_bins = m_power_module->num_bins();
                
        m_value = statSumSquares(power + m_min_bin, m_max_bin - m_min_bin) / statSumSquares(power, num_bins);
    }
    
private:
    
    module_power_spectrum *m_power_module;
    aligned_vector m_spectrum;
};

// Spectral Flatness Module

// USES ENERGY OR AMPS?

struct module_sfm : module_spectral<module_sfm>
{
    void add_requirements(graph& g) override
    {
        m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        const double *amplitudes = m_amplitude_module->get_frame();
                
        m_value = statFlatness(amplitudes + m_min_bin, m_max_bin - m_min_bin);
    }
    
private:
    
    module_amplitude_spectrum *m_amplitude_module;
    aligned_vector m_spectrum;
};

// Loudness Module

struct module_loudness : user_module_single
{
    static user_module *setup(const global_params& params, long argc, t_atom *argv)
    {
        module_loudness *m = new module_loudness();
        
        m->m_report_db = argc ? atom_getfloat(argv) : true;
        
        return m;
    }
    
    bool is_the_same(const module *m) const override
    {
        const module_loudness *m_typed = dynamic_cast<const module_loudness *>(m);
        
        return m_typed && m_typed->m_report_db == m_report_db;
    }
    
    void prepare(const global_params& params) override
    {
        // FIX - check sr
        
        long size = (params.fft_size() >> 1) + 1;
        
        if (m_loudness_curve.size() != size)
        {
            m_loudness_curve.resize((params.fft_size() >> 1) + 1);
            
            double *curve = m_loudness_curve.data();
            
            const double freq_mul = (params.m_sr / 1000.0) / params.fft_size();
            
            for (long i = 0; i < size; i++)
            {
                const double f = i * freq_mul;
                const double fo = f - 3.3;
                const double fo2 = fo * fo;
                const double f2 = f * f;
                const double f4 = f2 * f2;
                const double a = pow(10.0, (-3.64 * pow(f, -0.8) + 6.5 * exp(-0.6 * fo2) - (0.001 * f4)) / 20.0);
                
                curve[i] = a * a;
            }
        }
    }
    
    void add_requirements(graph& g) override
    {
        m_power_module = g.add_requirement(new module_power_spectrum());
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        const double *curve_frame = m_loudness_curve.data();
        const double *power_frame = m_power_module->get_frame();
                
        const VecType *curve = reinterpret_cast<const VecType *>(curve_frame);
        const VecType *power = reinterpret_cast<const VecType *>(power_frame);
        VecType sum(0.0);

        long nyquist = m_power_module->num_bins() - 1;
        long loop_size = nyquist / VecType::size;
        
        // Calculate amplitude spectrum
        
        for (long i = 0; i < loop_size; i++)
            sum += power[i] * curve[i];
        
        // Nyquist Value
       
        double p = power_frame[nyquist] * curve_frame[nyquist];

        // Sum
        
        double store_sum[VecType::size];
        sum.store(store_sum);
        
        for (int i = 0; i < VecType::size; i++)
            p += store_sum[i];
        
        // FIX - requires energy compensation
                
        m_value = m_report_db ? pow_to_db(p) : sqrt(p);
    }
    
private:
    
    module_power_spectrum *m_power_module;
    aligned_vector m_loudness_curve;
    bool m_report_db;
};

// Energy Module

struct module_energy : module_spectral_db<module_energy>
{
    void add_requirements(graph& g) override
    {
        m_power_module = g.add_requirement(new module_power_spectrum());
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        const double *power = m_power_module->get_frame();

        const double energy = statSumSquares(power + m_min_bin, m_max_bin - m_min_bin);

        // FIX - requires energy compensation
                
        m_value = m_report_db ? pow_to_db(energy) : energy;
    }
    
private:
    
    module_power_spectrum *m_power_module;
};

// Spectral Crest Module

struct module_spectral_crest : module_spectral_db<module_spectral_crest>
{
    void add_requirements(graph& g) override
    {
        m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        const double *amps = m_amplitude_module->get_frame();

        const double crest = statCrest(amps + m_min_bin, m_max_bin - m_min_bin);

        // FIX - check this
                
        m_value = m_report_db ? atodb(crest) : crest;
    }
    
private:
    
    module_amplitude_spectrum *m_amplitude_module;
};

// Rolloff Module

struct module_rolloff : user_module_single
{
    static user_module *setup(const global_params& params, long argc, t_atom *argv)
    {
        module_rolloff *m = new module_rolloff();
        
        m->m_centile = (argc ? atom_getfloat(argv) : 0.95) * 100.0;
        
        return m;
    }
    
    bool is_the_same(const module *m) const override
    {
        const module_rolloff *m_typed = dynamic_cast<const module_rolloff *>(m);
        
        return m_typed && m_typed->m_centile == m_centile;
    }
    
    void add_requirements(graph& g) override
    {
        m_power_module = g.add_requirement(new module_power_spectrum());
    }
    
    void prepare(const global_params& params) override
    {
        m_bin_freq = params.m_sr / params.fft_size();
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        const double bin = statPDFPercentile(m_power_module->get_frame(), m_centile, m_power_module->num_bins());
        
        m_value = bin * m_bin_freq;
    }
    
private:
    
    module_power_spectrum *m_power_module;
    double m_centile;
    double m_bin_freq;
};


// Spectral Linear Shape Modules

struct module_lin_centroid : module_spectral<module_lin_centroid>
{
    module_lin_centroid() {}
    
    module_lin_centroid(double lo_freq, double hi_freq)
    : module_spectral(lo_freq, hi_freq) {}
    
    void add_requirements(graph& g) override
    {
        m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
    }
    
    void prepare(const global_params& params) override
    {
        module_spectral::prepare(params);
        m_bin_freq = params.m_sr / params.fft_size();
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        m_value = statCentroid(m_amplitude_module->get_frame() + m_min_bin, m_max_bin - m_min_bin) * m_bin_freq;
    }

private:
    
    module_amplitude_spectrum *m_amplitude_module;
    double m_bin_freq;
};

struct module_lin_spread : module_spectral<module_lin_spread>
{
    void add_requirements(graph& g) override
    {
        m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
    }
    
    void prepare(const global_params& params) override
    {
        module_spectral::prepare(params);
        m_bin_freq = params.m_sr / params.fft_size();
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        m_value = statSpread(m_amplitude_module->get_frame() + m_min_bin, m_max_bin - m_min_bin) * m_bin_freq;
    }

private:
    
    module_amplitude_spectrum *m_amplitude_module;
    double m_bin_freq;
};

struct module_lin_skewness : module_spectral<module_lin_skewness>
{
    void add_requirements(graph& g) override
    {
        m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        m_value = statSkewness(m_amplitude_module->get_frame() + m_min_bin, m_max_bin - m_min_bin);
    }

private:
    
    module_amplitude_spectrum *m_amplitude_module;
};

struct module_lin_kurtosis : module_spectral<module_lin_kurtosis>
{
    void add_requirements(graph& g) override
    {
        m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        m_value = statKurtosis(m_amplitude_module->get_frame() + m_min_bin, m_max_bin - m_min_bin);
    }

private:
    
    module_amplitude_spectrum *m_amplitude_module;
};

// Spectral Log Shape Modules

struct module_log_centroid : module_spectral<module_log_centroid>
{
    module_log_centroid() {}
    
    module_log_centroid(double lo_freq, double hi_freq)
    : module_spectral(lo_freq, hi_freq) {}
    
    void add_requirements(graph& g) override
    {
        m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
    }
    
    void prepare(const global_params& params) override
    {
        module_spectral::prepare(params);
        m_bin_freq = params.m_sr / params.fft_size();
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        m_value = statLogCentroid(m_amplitude_module->get_frame() + m_min_bin, m_max_bin - m_min_bin) * m_bin_freq;
    }

private:
    
    module_amplitude_spectrum *m_amplitude_module;
    double m_bin_freq;
};

struct module_log_spread : module_spectral<module_log_spread>
{
    void add_requirements(graph& g) override
    {
        m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
    }
    
    void prepare(const global_params& params) override
    {
        module_spectral::prepare(params);
        m_bin_freq = params.m_sr / params.fft_size();
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        m_value = statLogSpread(m_amplitude_module->get_frame() + m_min_bin, m_max_bin - m_min_bin);
    }

private:
    
    module_amplitude_spectrum *m_amplitude_module;
    double m_bin_freq;
};

struct module_log_skewness : module_spectral<module_log_skewness>
{
    void add_requirements(graph& g) override
    {
        m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        m_value = statLogSkewness(m_amplitude_module->get_frame() + m_min_bin, m_max_bin - m_min_bin);
    }

private:
    
    module_amplitude_spectrum *m_amplitude_module;
};

struct module_log_kurtosis : module_spectral<module_log_kurtosis>
{
    void add_requirements(graph& g) override
    {
        m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        m_value = statLogKurtosis(m_amplitude_module->get_frame() + m_min_bin, m_max_bin - m_min_bin);
    }

private:
    
    module_amplitude_spectrum *m_amplitude_module;
};

#endif /* _MODULES_SPECTRAL_HPP_ */
