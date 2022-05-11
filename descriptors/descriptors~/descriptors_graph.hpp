
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
    
    void add_module(const char *name, user_module_setup setup)
    {
        m_setups.push_back({ gensym(name), setup });
    }
    
    user_module_setup setup_from_atom(t_atom *a) const
    {
        for (auto it = m_setups.begin(); it != m_setups.end(); it++)
            if (atom_gettype(a) == A_SYM && atom_getsym(a) == it->m_name)
                return it->m_setup;
                
        return nullptr;
    }
    
private:
    
    std::vector<user_setups> m_setups;
};

class energy_threshold
{
public:
    
    energy_threshold(double threshold)
    : m_energy_module(false)
    {}
    
    void prepare(const global_params& params)
    {
        m_window.prepare(params);
        m_energy_module.prepare(params);
    }
    
    bool check(const global_params& params, const double *frame)
    {
        m_window.calculate(params, frame, params.m_frame_size);
        
        if (!params.m_energy_threshold)
            return false;
            
        m_energy_module.calculate(params, m_window.get_frame(), params.m_frame_size);
        
        double level = m_energy_module.get_output(0) * m_window.get_rms_compensation();
    
        return level < params.m_energy_threshold;
    }
    
    module_window *get_window_module()
    {
        return &m_window;
    }
    
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
    
    void build(const setup_list& setups, const global_params& params, long argc, t_atom *argv)
    {
        user_module_setup next;
        
        long argc_begin = next_setup(setups, 0, argc, argv, next);
        
        m_threshold = std::unique_ptr<energy_threshold>(new energy_threshold(params.m_energy_threshold));
        
        while (argc_begin < argc)
        {
            user_module_setup setup = next;
            
            long argc_end = next_setup(setups, argc_begin + 1, argc, argv, next);
            module_arguments args(argc_end - (argc_begin + 1), argv + argc_begin + 1);
            auto m = (*setup)(params, args);
            m_outputs.push_back(add_requirement(m));
            argc_begin = argc_end;
        }
            
        prepare(params);
    }
    
    void prepare(const global_params& params)
    {
        if (m_threshold)
            m_threshold->prepare(params);
        
        for (auto it = m_modules.begin(); it != m_modules.end(); it++)
            (*it)->prepare(params);
    }
    
    bool run(const global_params& params, const double *frame)
    {
        if (m_threshold->check(params, frame))
            return false;
            
        for (auto it = m_modules.begin(); it != m_modules.end(); it++)
            (*it)->calculate(params, frame, params.m_frame_size);
        
        return true;
    }
    
    void output(t_atom *argv)
    {
        for (auto it = m_outputs.begin(); it != m_outputs.end(); it++)
        {
            size_t length = (*it)->get_output_size();
            
            for (size_t i = 0; i < length; i++)
                atom_setfloat(argv + i, (*it)->get_output(i));
                
            argv += length;
        }
    }
    
    size_t size()
    {
        size_t output_size = 0;
        
        for (auto it = m_outputs.begin(); it != m_outputs.end(); it++)
            output_size += (*it)->get_output_size();
        
        return output_size;
    }
    
    long next_setup(const setup_list& setups, long idx, long argc, t_atom *argv, user_module_setup& setup)
    {
        for (long i = idx; i < argc; i++)
            if ((setup = setups.setup_from_atom(argv + i)))
                return i;
        
        return argc;
    }

    std::unique_ptr<energy_threshold> m_threshold;
    std::vector<std::unique_ptr<module>> m_modules;
    std::vector<user_module *> m_outputs;
};

#endif /* _DESCRIPTORS_GRAPH_HPP_ */
