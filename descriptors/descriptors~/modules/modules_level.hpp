
#ifndef _MODULES_LEVEL_HPP_
#define _MODULES_LEVEL_HPP_

#include "descriptors_modules.hpp"
#include "conversion_helpers.hpp"

template <class T>
struct module_level : user_module_single
{
    module_level(bool report_db) : m_report_db(report_db) {}
    
    static user_module *setup(const global_params& params, module_arguments& args)
    {
        return new T(args.get_bool(true));
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
    
    const bool m_report_db;
};

// Level Modules

// Abs Module

struct module_average_abs_amp : module_level<module_average_abs_amp>
{
    module_average_abs_amp(bool report_db) : module_level(report_db) {}
    
    void calculate(const global_params& params, const double *frame, long size) override;
};

// RMS Module

struct module_average_rms_amp : module_level<module_average_rms_amp>
{
    module_average_rms_amp(bool report_db) : module_level(report_db) {}

    void calculate(const global_params& params, const double *frame, long size) override;
};

// Peak Amp Module

struct module_peak_amp : module_level<module_peak_amp>
{
    module_peak_amp(bool report_db) : module_level(report_db) {}

    void calculate(const global_params& params, const double *frame, long size) override;
};

#endif /* _MODULES_LEVEL_HPP_ */
