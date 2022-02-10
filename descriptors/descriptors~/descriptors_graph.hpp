
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
    
    void build()
    {
        
        for (auto it = m_modules.begin(); it != m_modules.end(); it++)
            (*it)->prepare();
    }
    
    void run(double *frame, long size)
    {
        for (auto it = m_modules.begin(); it != m_modules.end(); it++)
            (*it)->calculate(frame, size);
    }
    
    size_t size()
    {
        size_t output_size = 0;
        
        for (auto it = m_outputs.begin(); it != m_outputs.end(); it++)
            output_size += (*it)->get_output_size();
        
        return output_size;
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
    std::vector<std::unique_ptr<user_module>> m_outputs;
};

#endif /* _DESCRIPTORS_GRAPH_HPP_ */
