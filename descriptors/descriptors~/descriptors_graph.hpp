
#ifndef _DESCRIPTORS_GRAPH_HPP_
#define _DESCRIPTORS_GRAPH_HPP_

#include "descriptors_modules.hpp"
#include "modules/modules_core.hpp"
#include "modules/modules_level.hpp"

#include <ext.h>
#include <memory>
#include <vector>

// Setup List (for adding modules)

class setup_list
{
public:
    
    struct user_setups
    {
        t_symbol *m_name;
        user_module_setup m_setup;
    };
    
    void add_module(const char *name, user_module_setup setup);
    user_module_setup setup_from_atom(t_atom *a) const;
    
private:
    
    std::vector<user_setups> m_setups;
};

// Energy Threshold Class

class energy_threshold
{
public:
    
    energy_threshold(double threshold)
    : m_energy_module(false)
    {}
    
    void prepare(const global_params& params);
    bool check(const global_params& params, const double *frame);
    
    module_window *get_window_module() { return &m_window; }
    
private:
    
    module_window m_window;
    module_average_rms_amp m_energy_module;
};

// The Main Graph Class

class graph
{
public:
    
    virtual ~graph() {};
    
    template <class T>
    T *add_requirement(T *m)
    {
        return dynamic_cast<T *>(add_requirement_impl(m));
    }
    
    void build(t_object *x, const setup_list& setups, const global_params& params, long argc, t_atom *argv);
    void prepare(const global_params& params);
    bool run(const global_params& params, const double *frame);
    
    void output(t_atom *argv) { output(m_outputs, argv); }
    size_t output_size() { return output_size(m_outputs); }
    size_t size() { return m_outputs.size(); }
    
protected:
    
    void build_energy_threshold(const global_params& params);
    void add_user_module(user_module *m);
    
    long next_setup(const setup_list& setups, long idx, long argc, t_atom *argv, user_module_setup& setup);
    
    void output(std::vector<user_module *>& output_modules, t_atom *argv);
    size_t output_size(std::vector<user_module *>& output_modules);
    
    double get_output(long idx, long jdx) { return m_outputs[idx]->get_output(jdx); }
        
    // This allows the summary graph to override the implementation for adding requirements
    
    virtual module *add_requirement_impl(module *m);
    
private:

    std::unique_ptr<energy_threshold> m_threshold;
    std::vector<std::unique_ptr<module>> m_modules;
    std::vector<user_module *> m_outputs;
};

#endif /* _DESCRIPTORS_GRAPH_HPP_ */
