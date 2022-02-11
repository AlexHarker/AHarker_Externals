
#ifndef _MODULES_PITCH_HPP_
#define _MODULES_PITCH_HPP_

#include "modules_spectral.hpp"

// Pitch Modules

template <class T>
struct module_pitch_base : user_module_single
{
    module_pitch_base(double threshold) : m_threshold(threshold) {}

    static user_module *setup(const global_params& params, long argc, t_atom *argv)
    {
        const double threshold = argc > 0 ? atom_getfloat(argv) : 0.68;
        return new T(threshold);
    }
    
    bool is_the_same(const module *m) const override
    {
        const T *m_typed = dynamic_cast<const T *>(m);
        return m_typed && m_typed->m_threshold == m_threshold;
    }
    
protected:
    
    double m_threshold;
};

// Pitch Module

struct module_pitch : module_pitch_base<module_pitch>
{
    module_pitch(double threshold) : module_pitch_base(threshold) {}

    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;

    double get_confidence() const { return m_confidence; }
    
private:
    
    module_autocorrelation *m_autocorrelation_module;
    double m_confidence;
};

// Confidence Module

struct module_confidence : module_pitch_base<module_confidence>
{
    module_confidence(double threshold) : module_pitch_base(threshold) {}

    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
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
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;

private:
    
    module_lin_centroid *m_centroid_module;
    module_pitch *m_pitch_module;
};

// Brightness Log

struct module_log_brightness : module_brightness<module_log_brightness>
{
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;

private:
    
    module_log_centroid *m_centroid_module;
    module_pitch *m_pitch_module;
};

#endif /* _MODULES_PITCH_HPP_ */
