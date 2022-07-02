
#ifndef _DESCRIPTORS_MODULES_HPP_
#define _DESCRIPTORS_MODULES_HPP_

// N.B. We don't use <tuple> here but it will be used for get_params() in comparing modules

#include <ext.h>
#include <tuple>
#include <vector>

// Forward Declaration

class graph;

// FFT Parameters

struct fft_params
{
    long m_fft_size_log2;
    long m_hop_size;
    long m_frame_size;
    
    t_symbol *m_window_type;
    
    long fft_size() const { return 1 << m_fft_size_log2; }
};

// Global Parameters

struct global_params
{
    fft_params m_fft_params;
    
    long m_signal_length;
    
    double m_sr;
    double m_energy_threshold;
    
    // FIX - ceil??
    
    long num_frames() const { return ceil(m_signal_length / m_fft_params.m_hop_size); }
    long fft_size() const { return m_fft_params.fft_size(); }
    long fft_size_log2() const { return m_fft_params.m_fft_size_log2; }
    long hop_size() const { return m_fft_params.m_hop_size; }
    long frame_size() const { return m_fft_params.m_frame_size; }
    long num_bins() const { return (fft_size() >> 1) + 1; }
    
    t_symbol *window_type() const { return m_fft_params.m_window_type; }
    
    double bin_freq() const { return m_sr / fft_size(); }
};

// Module Arguments

class module_arguments
{
public:
    
    module_arguments(long argc, t_atom *argv)
    : m_argc(argc), m_argv(argv) {}
    
    bool get_bool(bool default_value)
    {
        return (m_argc-- > 0) ? atom_getfloat(m_argv++) : default_value;
    }
    
    long get_long(long default_value, long lo, long hi)
    {
        return (m_argc-- > 0) ? atom_getlong(m_argv++) : default_value;
    }
    
    double get_double(double default_value, double lo, double hi)
    {
        return (m_argc-- > 0) ? clip(atom_getfloat(m_argv++), lo, hi) : default_value;
    }
    
    t_symbol *get_symbol(t_symbol *default_value)
    {
        return (m_argc-- > 0) ? atom_getsym(m_argv++) : default_value;
    }
    
    long argc() const { return m_argc; }
    
private:
    
    template <class T>
    T clip(T value, T lo, T hi) { return std::min(std::max(value, lo), hi); }
    
    long m_argc;
    t_atom *m_argv;
};

// Any Module (internal or user)

struct module
{
    virtual ~module() {};
    
    virtual bool is_the_same(const module *m) const = 0;
    
    virtual void add_requirements(graph& g) {}
    
    virtual void prepare(const global_params& params) {}
    virtual void calculate(const global_params& params, const double *data, long size) = 0;
};

// A User Module

struct user_module : module
{
    virtual size_t get_output_size() const = 0;
    virtual double get_output(size_t idx) const = 0;
};

// This is the signature for setting up a user module

typedef user_module *(*user_module_setup)(const global_params&, module_arguments&);

// Comparison Helper Template

template <class T, class Module = module>
struct comparable_module : Module
{
    bool is_the_same(const module *m) const override
    {
        const T *a = static_cast<const T *>(this);
        const T *b = dynamic_cast<const T *>(m);
        
        return b && a->get_params() == b->get_params();
    }
};

// A Helper For Single Values Modules

template <class T>
struct user_module_single : comparable_module<T, user_module>
{
    size_t get_output_size() const override { return 1; }
    double get_output(size_t idx) const override { return m_value; }
    
protected:
    
    double m_value;
};

// A Helper For Multiple Values Modules

template <class T>
struct user_module_vector : comparable_module<T, user_module>
{
    size_t get_output_size() const override { return m_values.size(); }
    double get_output(size_t idx) const override { return m_values[idx]; }
    
protected:
    
    std::vector<double> m_values;
};

#endif /* _DESCRIPTORS_MODULES_HPP_ */
