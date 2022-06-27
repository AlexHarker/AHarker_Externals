
#ifndef _DESCRIPTORS_GRAPH_HPP_
#define _DESCRIPTORS_GRAPH_HPP_

#include "descriptors_modules.hpp"
#include "modules/modules_core.hpp"
#include "modules/modules_level.hpp"

#include <ext.h>
#include <memory>
#include <vector>

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

class graph
{
public:
    
    template <class T>
    T *add_requirement(T *m)
    {
        // Check the window for energy threshold first
        
        if (m_threshold->get_window_module()->is_the_same(m))
        {
            delete m;
            return dynamic_cast<T *>(m_threshold->get_window_module());
        }
        
        // Check other modules
        
        for (auto it = m_modules.begin(); it != m_modules.end(); it++)
        {
            if ((*it)->is_the_same(m))
            {
                delete m;
                return dynamic_cast<T *>(it->get());
            }
        }
        
        m->add_requirements(*this);
        m_modules.push_back(std::unique_ptr<module>(m));
        
        return m;
    }
    
    void build(const setup_list& setups, const global_params& params, long argc, t_atom *argv);
    void prepare(const global_params& params);
    bool run(const global_params& params, const double *frame);
    
    void output(t_atom *argv);
    size_t size();
    
protected:
    
    void build_energy_threshold(const global_params& params);
    void build_module(const global_params& params, user_module_setup setup, module_arguments& args);
    
    long next_setup(const setup_list& setups, long idx, long argc, t_atom *argv, user_module_setup& setup);
    
    std::unique_ptr<energy_threshold> m_threshold;
    std::vector<std::unique_ptr<module>> m_modules;
    std::vector<user_module *> m_outputs;
};

#endif /* _DESCRIPTORS_GRAPH_HPP_ */
