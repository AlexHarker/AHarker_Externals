
#ifndef __DESCRIPTORS_LEVEL_MODULES_HPP__
#define __DESCRIPTORS_LEVEL_MODULES_HPP__

#include <Statistics.hpp>

#include "descriptors_modules.hpp"
#include "descriptors_conversion_helpers.h"

template <class T>
struct module_level : user_module_single
{
    static user_module *setup(const global_params& params, long argc, t_atom *argv)
    {
        T *m = new T();
        
        m->m_report_db = argc ? atom_getfloat(argv) : true;
        
        return m;
    }
    
    bool is_the_same(const module *m) const override
    {
        const T *m_typed = dynamic_cast<const T *>(m);
        
        return m_typed && m_typed->m_report_db == m_report_db;
    }

    void set(double value)
    {
        m_value = m_report_db ? atodb(value) : value;
    }
    
private:
    
    bool m_report_db;
};

struct module_average_abs_amp : module_level<module_average_abs_amp>
{
    void calculate(const double *frame, long size) override
    {
        set(statSumAbs(frame, size) / size);
    }
};

struct module_average_rms_amp : module_level<module_average_rms_amp>
{
    void calculate(const double *frame, long size) override
    {
        set(statRMS(frame, size));
    }
};

struct module_peak_amp : module_level<module_peak_amp>
{
    void calculate(const double *frame, long size) override
    {
        set(statMax(frame, size));
    }
};

#endif /* __DESCRIPTORS_LEVEL_MODULES_HPP__ */
