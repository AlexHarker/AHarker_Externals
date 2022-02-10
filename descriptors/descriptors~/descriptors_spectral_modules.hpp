
#ifndef __DESCRIPTORS_SPECTRAL_MODULES_HPP__
#define __DESCRIPTORS_SPECTRAL_MODULES_HPP__

#include "descriptors_modules.hpp"
#include "descriptors_graph.hpp"

#include <HISSTools_FFT/HISSTools_FFT.h>
#include <SIMDSupport.hpp>

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
        long nyquist = (m_spectrum.size() - 1);
        long loop_size = nyquist / VecType::size;
        
        // Calculate power spectrum
        
        for (long i = 0; i < loop_size; i++)
            power[i] = (real[i] * real[i]) + (imag[i] * imag[i]);
        
        // Fix DC and Nyquist Values
        
        m_spectrum.data()[0] = fft_frame.realp[0] * fft_frame.realp[0];
        m_spectrum.data()[nyquist] = fft_frame.imagp[0] * fft_frame.imagp[0];
    }
    
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
        long nyquist = (m_spectrum.size() - 1);
        long loop_size = nyquist / VecType::size;
        
        // Calculate amplitude spectrum
        
        for (long i = 0; i < loop_size; i++)
            amps[i] = sqrt(power[i]);
        
        // Do Nyquist Value
        
        m_spectrum.data()[nyquist] = sqrt(power_frame[nyquist]);
    }
    
private:
    
    module_power_spectrum *m_power_module;
    aligned_vector m_spectrum;
};

#endif /* __DESCRIPTORS_SPECTRAL_MODULES_HPP__ */
