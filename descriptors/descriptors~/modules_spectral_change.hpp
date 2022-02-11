
#ifndef _MODULES_SPECTRAL_CHANGE_HPP_
#define _MODULES_SPECTRAL_CHANGE_HPP_

#include "modules_spectral.hpp"

// Foote Module

struct module_foote : module_spectral<module_foote>
{
    static module_foote *setup(const global_params& params, long argc, t_atom *argv);
    bool is_the_same(const module *m) const override;
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_spectrum_ring_buffer *m_ring_buffer_module;
    
    bool m_forward_only;
    long m_frames_back;
};

// Flux Module

struct module_flux : module_spectral<module_flux>
{
    static module_flux *setup(const global_params& params, long argc, t_atom *argv);
    bool is_the_same(const module *m) const override;
    void add_requirements(graph& g) override;
    void calculate(const global_params& params, const double *frame, long size) override;
    
private:
    
    module_spectrum_ring_buffer *m_ring_buffer_module;
    
    bool m_forward_only;
    bool m_normalise_spectrum;
    long m_frames_back;
};

#endif /* _MODULES_SPECTRAL_CHANGE_HPP_ */
