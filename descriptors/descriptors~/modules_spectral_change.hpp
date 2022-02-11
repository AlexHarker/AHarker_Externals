
#ifndef _MODULES_SPECTRAL_CHANGE_HPP_
#define _MODULES_SPECTRAL_CHANGE_HPP_

#include "modules_spectral.hpp"
#include "descriptors_graph.hpp"

#include <algorithm>

// A Generic Spectral Change Module

template <class T>
struct module_spectral_change : module_spectral<T>
{
    void add_requirements(graph& g) override
    {
        m_ring_buffer_module = g.add_requirement(new module_spectrum_ring_buffer());
        m_ring_buffer_module->request_lag(m_frame_lag);
    }

    void set_lag(long lag)
    {
        m_frame_lag = std::max(0L, lag);
    }
    
protected:
    
    module_spectrum_ring_buffer *m_ring_buffer_module;
    long m_frame_lag;
};

// Foote Module

struct module_foote : module_spectral_change<module_foote>
{
    static user_module *setup(const global_params& params, long argc, t_atom *argv);
    bool is_the_same(const module *m) const override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    bool m_forward_only;
};

// Flux Module

struct module_flux : module_spectral_change<module_flux>
{
    static user_module *setup(const global_params& params, long argc, t_atom *argv);
    bool is_the_same(const module *m) const override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
        
    bool m_forward_only;
    bool m_normalise_spectrum;
    bool m_square_flag;
};

// MKL Module

struct module_mkl : module_spectral_change<module_mkl>
{
    static user_module *setup(const global_params& params, long argc, t_atom *argv);
    bool is_the_same(const module *m) const override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
        
    double m_threshold;
    bool m_forward_only;
    bool m_normalise_spectrum;
    bool m_weight_second_frame;
};

#endif /* _MODULES_SPECTRAL_CHANGE_HPP_ */
