
#ifndef _MODULES_LEVEL_HPP_
#define _MODULES_LEVEL_HPP_

#include <Statistics.hpp>

#include "descriptors_modules.hpp"
#include "descriptors_conversion_helpers.h"

template <class T>
struct module_level : user_module_single
{
    module_level() : m_report_db(true) {}
    
    static user_module *setup(const global_params& params, long argc, t_atom *argv)
    {
        T *m = new T();
        
        m->m_report_db = argc ? atom_getfloat(argv) : true;
        
        return m;
    }
    
    bool is_the_same(const module *m) const override
    {
        return dynamic_cast<const T *>(m);
    }
    
    double get_output(size_t idx) const override
    {
        return m_report_db ? atodb(m_value) : m_value;
    }

private:
    
    bool m_report_db;
};

struct module_average_abs_amp : module_level<module_average_abs_amp>
{
    void calculate(const global_params& params, const double *frame, long size) override
    {
        m_value = statSumAbs(frame, size) / size;
    }
};

struct module_average_rms_amp : module_level<module_average_rms_amp>
{
    void calculate(const global_params& params, const double *frame, long size) override
    {
        m_value = statRMS(frame, size);
    }
};

struct module_peak_amp : module_level<module_peak_amp>
{
    void calculate(const global_params& params, const double *frame, long size) override
    {
        m_value = statMax(frame, size);
    }
};

#endif /* _MODULES_LEVEL_HPP_ */
