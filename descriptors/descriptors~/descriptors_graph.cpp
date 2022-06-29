
#include "descriptors_graph.hpp"

// Setup List

void setup_list::add_module(const char *name, user_module_setup setup)
{
    m_setups.push_back({ gensym(name), setup });
}

user_module_setup setup_list::setup_from_atom(t_atom *a) const
{
    for (auto it = m_setups.begin(); it != m_setups.end(); it++)
        if (atom_gettype(a) == A_SYM && atom_getsym(a) == it->m_name)
            return it->m_setup;
    
    return nullptr;
}

// Energy Threshold

void energy_threshold::prepare(const global_params& params)
{
    m_window.prepare(params);
    m_energy_module.prepare(params);
}
    
bool energy_threshold::check(const global_params& params, const double *frame)
{
    m_window.calculate(params, frame, params.m_frame_size);
        
    if (!params.m_energy_threshold)
        return false;
        
    m_energy_module.calculate(params, m_window.get_frame(), params.m_frame_size);
        
    double level = m_energy_module.get_output(0) * m_window.get_rms_compensation();
        
    return level < params.m_energy_threshold;
}

// Graph
    
module *graph::add_requirement_impl(module *m)
{
    // Check the window for energy threshold first
    
    if (m_threshold->get_window_module()->is_the_same(m))
    {
        delete m;
        return m_threshold->get_window_module();
    }
    
    // Check other modules
    
    for (auto it = m_modules.begin(); it != m_modules.end(); it++)
    {
        if ((*it)->is_the_same(m))
        {
            delete m;
            return it->get();
        }
    }
    
    m->add_requirements(*this);
    m_modules.push_back(std::unique_ptr<module>(m));
    
    return m;
}

void graph::build(const setup_list& setups, const global_params& params, long argc, t_atom *argv)
{
    user_module_setup next;
        
    long argc_begin = next_setup(setups, 0, argc, argv, next);
        
    build_energy_threshold(params);
        
    while (argc_begin < argc)
    {
        user_module_setup setup = next;
            
        long argc_end = next_setup(setups, argc_begin + 1, argc, argv, next);
        module_arguments args(argc_end - (argc_begin + 1), argv + argc_begin + 1);
        add_user_module((*setup)(params, args));
        argc_begin = argc_end;
    }
        
    prepare(params);
}
    
void graph::prepare(const global_params& params)
{
    if (m_threshold)
        m_threshold->prepare(params);
        
    for (auto it = m_modules.begin(); it != m_modules.end(); it++)
        (*it)->prepare(params);
}
    
bool graph::run(const global_params& params, const double *frame)
{
    if (m_threshold->check(params, frame))
        return false;
        
    for (auto it = m_modules.begin(); it != m_modules.end(); it++)
        (*it)->calculate(params, frame, params.m_frame_size);
        
    return true;
}
        
void graph::build_energy_threshold(const global_params& params)
{
    m_threshold = std::unique_ptr<energy_threshold>(new energy_threshold(params.m_energy_threshold));
}
    
void graph::add_user_module(user_module *m)
{
    m_outputs.push_back(add_requirement(m));
}
    
long graph::next_setup(const setup_list& setups, long idx, long argc, t_atom *argv, user_module_setup& setup)
{
    for (long i = idx; i < argc; i++)
        if ((setup = setups.setup_from_atom(argv + i)))
            return i;
        
    return argc;
}

void graph::output(std::vector<user_module *>& output_modules, t_atom *argv)
{
    for (auto it = output_modules.begin(); it != output_modules.end(); it++)
    {
        size_t length = (*it)->get_output_size();
        
        for (size_t i = 0; i < length; i++)
            atom_setfloat(argv + i, (*it)->get_output(i));
        
        argv += length;
    }
}

size_t graph::size(std::vector<user_module *>& output_modules)
{
    size_t output_size = 0;
    
    for (auto it = output_modules.begin(); it != output_modules.end(); it++)
        output_size += (*it)->get_output_size();
    
    return output_size;
}
