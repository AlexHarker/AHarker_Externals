
#ifndef _MODULES_SPECTRAL_CONTENT_HPP_
#define _MODULES_SPECTRAL_CONTENT_HPP_

#include "modules_core.hpp"
#include "modules_spectral.hpp"

// Generic Noise Measure Module

template <class T>
struct module_noise_measure : user_module_single
{
    static user_module *setup(const global_params& params, long argc, t_atom *argv)
    {
        long median_span = argc > 0 ? atom_getlong(argv + 0) : 15;

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

struct module_harmonic_ratio : module_noise_measure<module_noise_ratio>
{
    module_harmonic_ratio(long median_span) : module_noise_measure(median_span) {}

    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_noise_ratio *m_noise_ratio_module;
};

#endif /* _MODULES_SPECTRAL_CONTENT_HPP_ */
