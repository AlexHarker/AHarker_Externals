
#ifndef __DESCRIPTORS_SPECTRAL_MODULES_HPP__
#define __DESCRIPTORS_SPECTRAL_MODULES_HPP__

#include "descriptors_conversion_helpers.h"
#include "descriptors_modules.hpp"
#include "descriptors_graph.hpp"

#include <HISSTools_FFT/HISSTools_FFT.h>
#include <SIMDSupport.hpp>

#include <cmath>

using VecType = SIMDType<double, SIMDLimits<double>::max_size>;

// UNDERLYING RAII DATA TYPES

struct aligned_vector
{
    aligned_vector() : m_ptr(nullptr), m_size(0) {}
    ~aligned_vector() { deallocate_aligned(m_ptr); }
    
    double *data() const { return m_ptr; }
    
    void resize(size_t size)
    {
        if (size != m_size)
        {
            deallocate_aligned(m_ptr);
            m_ptr = allocate_aligned<double>(size);
            m_size = size;
        }
    }
    
    size_t size() const { return m_size; }
    
private:

    double *m_ptr;
    size_t m_size;
};

struct fft_split
{
    void resize(size_t size)
    {
        m_vector.resize(size);
        m_split.realp = m_vector.data();
        m_split.imagp = m_vector.data() + (size >> 1);
    }
    
    FFT_SPLIT_COMPLEX_D& get() { return m_split; }
    const FFT_SPLIT_COMPLEX_D& get() const { return m_split; }

    size_t size() const { return m_vector.size(); }

private:

    aligned_vector m_vector;
    FFT_SPLIT_COMPLEX_D m_split;
};

struct fft_setup
{
    fft_setup() : m_size(0) {}
    ~fft_setup() { hisstools_destroy_setup(m_fft_setup); }
    
    void resize(size_t size)
    {
        if (size != m_size)
        {
            hisstools_destroy_setup(m_fft_setup);
            hisstools_create_setup(&m_fft_setup, size);
            m_size = size;
        }
    }
    
    FFT_SETUP_D& get() { return m_fft_setup; }

    size_t size() const { return m_size; }

    FFT_SETUP_D m_fft_setup;
    size_t m_size;
};

// MODULES

struct module_fft : module
{
    bool is_the_same(const module *m) const override
    {
        return dynamic_cast<const module_fft *>(m);
    }

    void prepare(const global_params& params) override
    {
        m_fft_frame.resize(params.fft_size());
        m_fft_setup.resize(params.m_fft_size_log2);
    }
    
    void calculate(const double *frame, long size) override
    {
        // Apply window
/*
        for (j = 0; j < window_size >> 2; j++)
            v_windowed_frame[j] = F32_VEC_MUL_OP(v_raw_frame[j], v_window[j]);
        for (j <<= 2; j < window_size; j++)
            windowed_frame[j] = raw_frame[j] * window[j];
*/
        // Do FFT (as well as any zero padding)
        
        hisstools_rfft(m_fft_setup.get(), frame, &m_fft_frame.get(), size, m_fft_setup.size());
    }
    
    const FFT_SPLIT_COMPLEX_D& get_frame() const { return m_fft_frame.get(); }
    
private:
    
    fft_setup m_fft_setup;
    fft_split m_fft_frame;
};

// Power Spectrum

struct module_power_spectrum : module
{
    bool is_the_same(const module *m) const override
    {
        return dynamic_cast<const module_power_spectrum *>(m);
    }

    void add_requirements(graph& g) override
    {
        m_fft_module = g.add_requirement(new module_fft());
    }
    
    void prepare(const global_params& params) override
    {
        m_spectrum.resize((params.fft_size() >> 1) + 1);
    }
    
    void calculate(const double *frame, long size) override
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
    
    long num_bins() const { return static_cast<long>(m_spectrum.size()); }

    const double *get_frame() const { return m_spectrum.data(); }

private:
    
    module_fft *m_fft_module;
    aligned_vector m_spectrum;
};

// Amplitude Spectrum

struct module_amplitude_spectrum : module
{
    bool is_the_same(const module *m) const override
    {
        return dynamic_cast<const module_amplitude_spectrum *>(m);
    }

    void add_requirements(graph& g) override
    {
        m_power_module = g.add_requirement(new module_power_spectrum());
    }
    
    void prepare(const global_params& params) override
    {
        m_spectrum.resize((params.fft_size() >> 1) + 1);
    }
    
    void calculate(const double *frame, long size) override
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
    
    long num_bins() const { return static_cast<long>(m_spectrum.size()); }

    const double *get_frame() const { return m_spectrum.data(); }

private:
    
    module_power_spectrum *m_power_module;
    aligned_vector m_spectrum;
};

// Basic Spectral Module

template <class T>
struct module_spectral : user_module_single
{
    static T *setup(long argc, t_atom *argv)
    {
        T *m = new T();
        
        m->m_lo_freq = argc > 0 ? atom_getfloat(argv + 0) : 0.0;
        m->m_hi_freq = argc > 1 ? atom_getfloat(argv + 1) : 192000.0;

        return m;
    }
    
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

// Energy Ratio Module

// USES ENERGY OR AMPS?

struct module_energy_ratio : module_spectral<module_energy_ratio>
{
    static user_module *setup(const global_params& params, long argc, t_atom *argv)
    {
        return module_spectral::setup(argc, argv);
    }
    
    void add_requirements(graph& g) override
    {
        m_power_module = g.add_requirement(new module_power_spectrum());
    }
    
    void calculate(const double *frame, long size) override
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
    static user_module *setup(const global_params& params, long argc, t_atom *argv)
    {
        return module_spectral::setup(argc, argv);
    }
    
    void add_requirements(graph& g) override
    {
        m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
    }
    
    void calculate(const double *frame, long size) override
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
    
    void calculate(const double *frame, long size) override
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

struct module_energy : module_spectral<module_energy>
{
    static user_module *setup(const global_params& params, long argc, t_atom *argv)
    {
        module_energy *m = module_spectral::setup(argc, argv);
        
        m->m_report_db = argc > 2 ? atom_getfloat(argv + 2) : true;
        
        return m;
    }
    
    bool is_the_same(const module *m) const override
    {
        const module_energy *m_typed = dynamic_cast<const module_energy *>(m);
        
        return module_spectral::is_the_same(m) && m_typed->m_report_db == m_report_db;
    }
    
    void add_requirements(graph& g) override
    {
        m_power_module = g.add_requirement(new module_power_spectrum());
    }
    
    void calculate(const double *frame, long size) override
    {
        const double *power = m_power_module->get_frame();

        const double energy = statSumSquares(power + m_min_bin, m_max_bin - m_min_bin);

        // FIX - requires energy compensation
                
        m_value = m_report_db ? pow_to_db(energy) : energy;
    }
    
private:
    
    module_power_spectrum *m_power_module;
    bool m_report_db;
};



#endif /* __DESCRIPTORS_SPECTRAL_MODULES_HPP__ */
