
#ifndef _DESCRIPTORS_MODULES_HPP_
#define _DESCRIPTORS_MODULES_HPP_

#include <ext.h>
#include <vector>

// Global Parameters

struct global_params
{
    long m_fft_size;
    long m_frame_size;
    long m_hop_size;
};

// Any Module (internal or user)

struct module
{
    module();
    virtual ~module() = 0;
    
    //bool operator == (const module& = 0);
    
    virtual void prepare() {}
    virtual void calculate(double *frame, long size) = 0;
    
    virtual void requires(std::vector<module *> modules) {}
};

// A User Module

struct user_module : module
{
    virtual size_t get_output_size() const = 0;
    virtual double get_output(size_t idx) const = 0;
};

// This is the signature for setting up a user module

typedef user_module *(*user_module_setup)(const global_params& globals, long argc, t_atom *argv);

// A Helper For Single Values Modules

struct user_module_single : user_module
{
    size_t get_output_size() const override { return 1; }
    double get_output(size_t idx) const override { return m_value; }
    
private:
    
    double m_value;
};

// A Helper For Multiple Values Modules

struct user_module_vector : user_module
{
    size_t get_output_size() const override { return m_values.size(); }
    double get_output(size_t idx) const override { return m_values[idx]; }
    
private:
    
    std::vector<double> m_values;
};

#endif /* _DESCRIPTORS_MODULES_HPP_ */
