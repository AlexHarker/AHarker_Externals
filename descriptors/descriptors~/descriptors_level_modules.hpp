
#ifndef __DESCRIPTORS_LEVEL_MODULES_HPP__
#define __DESCRIPTORS_LEVEL_MODULES_HPP__

#include <Statistics.hpp>

#include "descriptors_modules.hpp"
#include "descriptors_conversion_helpers.h"

struct module_level : user_module_single
{
    template <class T>
    static T *setup(long argc, t_atom *argv)
    {
        T *m = new T();
        
        if (argc)
            m->m_report_db = atom_getfloat(argv);
        
        return m;
    }
    
    void set(double value)
    {
        m_value = m_report_db ? atodb(value) : value;
    }
    
private:
    
    bool m_report_db;
};

struct module_average_abs_amp : module_level
{
    static user_module *setup(const global_params& params, long argc, t_atom *argv)
    {
        return module_level::setup<module_average_abs_amp>(argc, argv);
    }
    
    void calculate(const double *frame, long size) override
    {
        set(statSumAbs(frame, size) / size);
    }
};

struct module_average_rms_amp : module_level
{
    static user_module *setup(const global_params& params, long argc, t_atom *argv)
    {
        return module_level::setup<module_average_rms_amp>(argc, argv);
    }
    
    void calculate(const double *frame, long size) override
    {
        set(statRMS(frame, size));
    }
};

struct module_peak_amp : module_level
{
    static user_module *setup(const global_params& params, long argc, t_atom *argv)
    {
        return module_level::setup<module_peak_amp>(argc, argv);
    }
    
    void calculate(const double *frame, long size) override
    {
        set(statMax(frame, size));
    }
};

#endif /* __DESCRIPTORS_LEVEL_MODULES_HPP__ */
