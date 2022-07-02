
#ifndef DESCRIPTORS_SUMMARY_MODULES_H
#define DESCRIPTORS_SUMMARY_MODULES_H

#include "descriptors_modules.hpp"

// A type to indicate that a module is involved in calculating a summary (not per frame descriptors)

struct summary_module
{
    summary_module(bool no_index = false) : m_no_index(no_index), m_index(-1) {}
    virtual ~summary_module() {}
    
    // N.B. - used by specifiersthis should only ever be called after a dynamic cast and comparison check
    
    virtual void update_to_final(const module *m) {}
    
    size_t get_index() const { return m_index; }
    void set_index(size_t index) { m_index = m_no_index ? -1 : index; }
    
    bool m_no_index;
    size_t m_index;
};

// A type for adding specifiers that apply to a single descriptor

template <class T>
struct summary_specifier : summary_module, comparable_module<T, user_module>
{
    void calculate(const global_params& params, const double *data, long size) override {}
    
    size_t get_output_size() const override { return 0; }
    double get_output(size_t idx) const override { return 0.0; }
};

#endif /* DESCRIPTORS_SUMMARY_MODULES_H */
