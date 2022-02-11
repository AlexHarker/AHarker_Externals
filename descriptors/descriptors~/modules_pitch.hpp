
#ifndef _MODULES_PITCH_HPP_
#define _MODULES_PITCH_HPP_

#include "modules_spectral.hpp"

// Pitch Modules

template <class T>
struct module_pitch_base : user_module_single
{
    module_pitch_base() : m_threshold(0.68) {}
    module_pitch_base(double threshold) : m_threshold(threshold) {}

    static user_module *setup(const global_params& params, long argc, t_atom *argv)
    {
        T *m = new T();
        
        m->m_threshold = argc > 0 ? atom_getfloat(argv + 2) : 0.68;

        return m;
    }
    
    bool is_the_same(const module *m) const override
    {
        const T *m_typed = dynamic_cast<const T *>(m);
        return m_typed && m_typed->m_threshold == m_threshold;
    }
    
protected:
    
    double m_threshold;
};

// Autocorrelation Module

struct module_autocorrelation : module
{
    bool is_the_same(const module *m) const override
    {
        return dynamic_cast<const module_autocorrelation *>(m);
    }

    void prepare(const global_params& params) override
    {
        m_fft_setup.resize(params.m_fft_size_log2);
        m_full_frame.resize(params.fft_size());;
        m_half_frame.resize(params.fft_size());;
        m_coefficients.resize(params.fft_size());
    }
    
    void complex_mul_conjugate(long length, float scale)
    {
        using VecType = SIMDType<double, SIMDLimits<double>::max_size>;
        
        auto full_frame = m_full_frame.get();
        auto half_frame = m_half_frame.get();

        VecType *in_real1 = reinterpret_cast<VecType *>(full_frame.realp);
        VecType *in_imag1 = reinterpret_cast<VecType *>(full_frame.imagp);
        VecType *in_real2 = reinterpret_cast<VecType *>(half_frame.realp);
        VecType *in_imag2 = reinterpret_cast<VecType *>(half_frame.imagp);
        
        VecType vscale(scale);
        
        float nyquist1 = full_frame.imagp[0];
        float nyquist2 = half_frame.imagp[0];
            
        full_frame.imagp[0] = 0.f;
        half_frame.imagp[0] = 0.f;
        
        for (long i = 0; i < length >> 2; i++)
        {
            VecType r1 = in_real1[i];
            VecType i1 = in_imag1[i];
            VecType r2 = in_real2[i];
            VecType i2 = in_imag2[i];
            
            in_real1[i] = ((r1 * r2) + (i1 * i2)) * vscale;
            in_imag1[i] = ((i1 * r2) - (r1 * i2)) * vscale;
        }
        
        full_frame.imagp[0] = (nyquist1 * nyquist2) * scale;
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        long fft_size = get_length();
            
        double scale = 0.0;
        
        // Calculate normalisation factor
        
        for (long i = 0; i < (size >> 1); i++)
            scale += frame[i] * frame[i];
            
        scale = 0.25 / ((double)  fft_size * scale);
            
        // Do ffts straight into position with zero padding (one half the size of the other)
        
        hisstools_rfft(m_fft_setup.get(), frame, &m_full_frame.get(), size, m_fft_setup.size());
        hisstools_rfft(m_fft_setup.get(), frame, &m_half_frame.get(), (size >> 1), m_fft_setup.size());
            
        // Calculate ac coefficients
            
        complex_mul_conjugate(size >> 1, scale);
            
        // Inverse fft
            
        hisstools_rifft(m_fft_setup.get(), &m_full_frame.get(), m_coefficients.data(), m_fft_setup.size());
    }
    
    const double *get_coefficients() const { return m_coefficients.data(); }
    long get_length() const { return static_cast<long>(m_coefficients.size()); }

private:
    
    fft_setup m_fft_setup;
    fft_split m_full_frame;
    fft_split m_half_frame;
    aligned_vector m_coefficients;
};

// Pitch Module

struct module_pitch : module_pitch_base<module_pitch>
{
    module_pitch() {}
    module_pitch(double threshold) : module_pitch_base(threshold) {}

    void add_requirements(graph& g) override
    {
        m_autocorrelation_module = g.add_requirement(new module_autocorrelation());
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        auto ac_coeff = m_autocorrelation_module->get_coefficients();
        long length = m_autocorrelation_module->get_length();
        
        bool no_pitch = false;
        long bin_freq = 0;
        long from = 0;
        long to = 0;
        long i;
        
        double confidence = 0.0;
        double max_val = 0.0;
        
        const double zero_lag = ac_coeff[0];
        const double compare_thresh = zero_lag * m_threshold;
        
        // Ignore first peak
        
        for (i = 1; i < length; i++)
            if (ac_coeff[i] < compare_thresh)
                break;
        
        long end_first = i;
        
        // Find start of second peak
        
        for (; i < length; i++)
            if (ac_coeff[i] > compare_thresh)
                break;
        
        from = i;
        
        // Find end of second peak
        
        for (; i < length; i++)
            if (ac_coeff[i] < compare_thresh)
                break;
        
        if (i == length)
        {
            from = end_first;
            to = length;
            no_pitch = true;
        }
        else
        {
            to = i;
        }
        
        for (i = from; i < to; i++)
        {
            const double current_val = ac_coeff[i];
            
            if (current_val > max_val)
            {
                max_val = current_val;
                bin_freq = i;
            }
        }
        
        confidence = max_val;
        
        // Do the correction if we have found a pitch (using parabolic interpolation)
        
        if (!no_pitch)
        {
            double prev_val = ac_coeff[bin_freq - 1];
            double next_val = ac_coeff[bin_freq + 1] ;
            double beta = 0.5 * (next_val - prev_val);
            double gamma = (2.0  * max_val) - next_val - prev_val;
            m_value = params.m_sr / (bin_freq + (beta / gamma));
        }
        else
        {
            m_value = DBL_MAX;
        }
        
        m_confidence = std::min(confidence, 1.0);
    }

    double get_confidence() const { return m_confidence; }
    
private:
    
    module_autocorrelation *m_autocorrelation_module;
    double m_confidence;
};

// Confidence Module

struct module_confidence : module_pitch_base<module_confidence>
{
    void add_requirements(graph& g) override
    {
        m_pitch_module = g.add_requirement(new module_pitch(m_threshold));
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        m_value = m_pitch_module->get_confidence();
    }
    
private:
    
    module_pitch *m_pitch_module;
};

// Brightness Modules

template <class T>
struct module_brightness : module_spectral<T>
{
    static user_module *setup(const global_params& params, long argc, t_atom *argv)
    {
        T *m = dynamic_cast<T *>(module_spectral<T>::setup(params, argc, argv));
        
        m->m_threshold = argc > 2 ? atom_getfloat(argv + 2) : 0.68;
        
        return m;
    }
    
    bool is_the_same(const module *m) const override
    {
        const T *m_typed = dynamic_cast<const T *>(m);
        return module_spectral<T>::is_the_same(m) && m_typed->m_threshold == m_threshold;
    }
    
protected:
    
    double m_threshold;
};

// Brightness Linear

struct module_lin_brightness : module_brightness<module_lin_brightness>
{
    void add_requirements(graph& g) override
    {
        m_pitch_module = g.add_requirement(new module_pitch(m_threshold));
        m_centroid_module = g.add_requirement(new module_lin_centroid(m_lo_freq, m_hi_freq));
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        const double pitch = m_pitch_module->get_output(0);
        const double centroid = m_centroid_module->get_output(0);
        
        //if (pitch > 10. && pitch != DBL_MAX && centroid != DBL_MAX)
        m_value = centroid / pitch;
    }

private:
    
    module_lin_centroid *m_centroid_module;
    module_pitch *m_pitch_module;
};

// Brightness Log

struct module_log_brightness : module_brightness<module_log_brightness>
{
    void add_requirements(graph& g) override
    {
        m_pitch_module = g.add_requirement(new module_pitch(m_threshold));
        m_centroid_module = g.add_requirement(new module_log_centroid(m_lo_freq, m_hi_freq));
    }
    
    void calculate(const global_params& params, const double *frame, long size) override
    {
        const double pitch = m_pitch_module->get_output(0);
        const double centroid = m_centroid_module->get_output(0);
        
        //if (pitch > 10. && pitch != DBL_MAX && centroid != DBL_MAX)
        m_value = centroid / pitch;
    }

private:
    
    module_log_centroid *m_centroid_module;
    module_pitch *m_pitch_module;
};

#endif /* _MODULES_PITCH_HPP_ */
