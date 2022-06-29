
#include "descriptors_summary_graph.hpp"
#include "modules/modules_summary.hpp"

// Summary Graph
    
void summary_graph::build(const setup_list& setups, const global_params& params, long argc, t_atom *argv)
{
    user_module_setup next;
        
    long argc_begin = next_setup(setups, 0, argc, argv, next);
        
    build_energy_threshold(params);
        
    while (argc_begin < argc)
    {
        user_module_setup setup = next;
            
        long argc_end = next_setup(setups, argc_begin + 1, argc, argv, next);
        module_arguments args(argc_end - (argc_begin + 1), argv + argc_begin + 1);
                
        auto m = (*setup)(params, args);
        auto summary = dynamic_cast<summary_module *>(m);
        
        if (summary)
        {
            summary->set_index(graph::size() - 1);
            if (m->get_output_size())
            {
                if (summary->get_index() == -1)
                    check_last_descriptor_summary();
                else
                    m_last_summary_idx = graph::size() - 1;
                m_summary_outputs.push_back(add_requirement(m));
            }
            else
                add_requirement(m);
        }
        else
        {
            assert(m->get_output_size() == 1);
            check_last_descriptor_summary();
            graph::add_user_module(m);
        }
        
        argc_begin = argc_end;
    }

    check_last_descriptor_summary();
    
    // N.B. for the summary graph prepare needs calling per input, so we skip it here
}
    
void summary_graph::prepare(const global_params& params)
{
    graph::prepare(params);
        
    for (auto it = m_summary_modules.begin(); it != m_summary_modules.end(); it++)
        (*it)->prepare(params);
    
    m_frame.resize(params.m_frame_size);
    m_temp_data.resize(graph::size() * params.num_frames());
}
    
void summary_graph::run(const global_params& params, const double *input)
{
    // Loop over hops

    for (long i = 0, offset = 0; i < params.num_frames(); i++, offset += params.m_hop_size)
    {
        long frame_available = std::min(params.m_frame_size, params.m_signal_length - offset);
        
        // Get samples
        
        for (long j = 0; j < frame_available; j++)
            m_frame[j] = input[offset + j];
        for (long j = frame_available; j < params.m_frame_size; j++)
            m_frame[j] = 0.0;
        
        // Run per frame
    
        graph::run(params, m_frame.data());
        
        for (long j = 0; j < graph::size(); j++)
            m_temp_data[j * params.num_frames() + i] = graph::get_output(j, 0);
    }
    
    // Run stats and summary descriptors
        
    for (auto it = m_summary_modules.begin(); it != m_summary_modules.end(); it++)
    {
        // FIX - make this nicer...

        auto index = dynamic_cast<summary_module *>(it->get())->get_index();
        
        if (index == -1)
            (*it)->calculate(params, input, params.m_signal_length);
        else
            (*it)->calculate(params, m_temp_data.data() + index * params.num_frames(), params.num_frames());
    }
}

module *summary_graph::add_requirement_impl(module *m)
{
    auto summary = dynamic_cast<summary_module *>(m);
    
    // Check against summary modules

    if (summary)
    {
        // Set index before we compare
        
        summary->set_index(graph::size() - 1);
        
        for (auto it = m_summary_modules.begin(); it != m_summary_modules.end(); it++)
        {
            if ((*it)->is_the_same(m))
            {
                dynamic_cast<summary_module*>(it->get())->update_to_final(m);
                delete m;
                return it->get();
            }
        }
        
        m->add_requirements(*this);
        m_summary_modules.push_back(std::unique_ptr<module>(m));
        
        return m;
    }
    
    return graph::add_requirement_impl(m);
}

void summary_graph::check_last_descriptor_summary()
{
    if (graph::size() - 1 != m_last_summary_idx)
    {
        auto summary = new stat_module_mean();
        summary->set_index(graph::size() - 1);
        m_last_summary_idx = graph::size() - 1;
        m_summary_outputs.push_back(add_requirement(summary));
    }
}
