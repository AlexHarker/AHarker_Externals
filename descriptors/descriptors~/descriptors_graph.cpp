
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
        build_module(params, setup, args);
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
    
void graph::output(t_atom *argv)
{
    for (auto it = m_outputs.begin(); it != m_outputs.end(); it++)
    {
        size_t length = (*it)->get_output_size();
        
        for (size_t i = 0; i < length; i++)
            atom_setfloat(argv + i, (*it)->get_output(i));
        
        argv += length;
    }
}
    
size_t graph::size()
{
    size_t output_size = 0;
    
    for (auto it = m_outputs.begin(); it != m_outputs.end(); it++)
        output_size += (*it)->get_output_size();
    
    return output_size;
}
    
void graph::build_energy_threshold(const global_params& params)
{
    m_threshold = std::unique_ptr<energy_threshold>(new energy_threshold(params.m_energy_threshold));
}
    
void graph::build_module(const global_params& params, user_module_setup setup, module_arguments& args)
{
    auto m = (*setup)(params, args);
    m_outputs.push_back(add_requirement(m));
}
    
long graph::next_setup(const setup_list& setups, long idx, long argc, t_atom *argv, user_module_setup& setup)
{
    for (long i = idx; i < argc; i++)
        if ((setup = setups.setup_from_atom(argv + i)))
            return i;
        
    return argc;
}
