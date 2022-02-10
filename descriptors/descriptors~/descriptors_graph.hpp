
#ifndef _DESCRIPTORS_GRAPH_HPP_
#define _DESCRIPTORS_GRAPH_HPP_

#include "descriptors_modules.hpp"

#include <ext.h>
#include <memory>
#include <vector>

class graph
{
    struct user_setups
    {
        t_symbol *m_name;
        user_module_setup m_setup;
    };
    
public:
    
    void add_module(const char *name, user_module_setup setup)
    {
        m_installed_modules.push_back({ gensym(name), setup });
    }
    
    module *add_requirement(module *m)
    {
        // FIX - search for the same module in the graph and only add if needed
        
        m->add_requirements(*this);
        m_modules.push_back(std::unique_ptr<module>(m));
        
        return m;
    }
    
    void build(global_params& params, long argc, t_atom *argv)
    {
        user_module_setup next;
        
        long argc_begin = next_setup(0, argc, argv, next);
        
        while (argc_begin < argc)
        {
            user_module_setup setup = next;
            
            long argc_end = next_setup(argc_begin + 1, argc, argv, next);
            auto m = ((*setup)(params, argc_end - (argc_begin + 1), argv + argc_begin + 1));
            m_outputs.push_back(dynamic_cast<user_module *>(add_requirement(m)));
            argc_begin = argc_end;
        }
            
        for (auto it = m_modules.begin(); it != m_modules.end(); it++)
            (*it)->prepare(params);
    }
    
    void run(double *frame, long size)
    {
        for (auto it = m_modules.begin(); it != m_modules.end(); it++)
            (*it)->calculate(frame, size);
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
    
    long next_setup(long idx, long argc, t_atom *argv, user_module_setup& setup)
    {
        for (long i = idx; i < argc; i++)
            if ((setup = setup_from_atom(argv + i)))
                return i;
        
        return argc;
    }
    
    user_module_setup setup_from_atom(t_atom *a)
    {
        for (auto it = m_installed_modules.begin(); it != m_installed_modules.end(); it++)
            if (atom_gettype(a) == A_SYM && atom_getsym(a) == it->m_name)
                return it->m_setup;
                
        return nullptr;
    }
    
    std::vector<user_setups> m_installed_modules;
    std::vector<std::unique_ptr<module>> m_modules;
    std::vector<user_module *> m_outputs;
};

#endif /* _DESCRIPTORS_GRAPH_HPP_ */
