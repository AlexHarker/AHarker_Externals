
#include "descriptors_summary_graph.hpp"
#include "modules/modules_summary.hpp"

// Summary Graph
    
summary_graph::summary_graph(t_object *x, const setup_list& setups, const global_params& params, long argc, t_atom *argv)
: m_num_summary_descriptors(0), m_last_summary_idx(-1)
{
    user_module_setup next;
        
    long argc_begin = next_setup(setups, 0, argc, argv, next);
        
    build_energy_threshold(params);
        
    while (argc_begin < argc)
    {
        user_module_setup setup = next;
            
        long argc_end = next_setup(setups, argc_begin + 1, argc, argv, next);
        module_arguments args(x, argc_end - argc_begin, argv + argc_begin);
                
        auto m = (*setup)(params, args);
        auto summary = dynamic_cast<summary_module *>(m);
        
        if (summary)
        {
            summary->set_index(graph::size() - 1);
            if (summary->is_descriptor())
                m_num_summary_descriptors++;
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
    
    // N.B. for the summary graph prepare needs calling per input, but we call it here first
    
    prepare(params);
}
    
void summary_graph::prepare(const global_params& params)
{
    graph::prepare(params);
        
    for (auto it = m_summary_modules.begin(); it != m_summary_modules.end(); it++)
        (*it)->prepare(params);
    
    m_frame.resize(params.frame_size());
    m_temp_data.resize(graph::size() * params.num_frames());
}
    
void summary_graph::run(const global_params& params, ibuffer_data& data, long buffer_offset, long buffer_chan)
{
    // Loop over hops

    for (long i = 0, offset = 0; i < params.num_frames(); i++, offset += params.hop_size())
    {
        long frame_available = std::min(params.frame_size(), params.m_signal_length - offset);
        
        // Get samples
        
        ibuffer_get_samps(data, m_frame.data(), buffer_offset + offset, frame_available, buffer_chan);

        for (long j = frame_available; j < params.frame_size(); j++)
            m_frame[j] = 0.0;
        
        // Run per frame
    
        if (graph::run(params, m_frame.data()))
        {
            for (long j = 0; j < graph::size(); j++)
                m_temp_data[j * params.num_frames() + i] = graph::get_output(j, 0);
        }
        else
        {
            for (long j = 0; j < graph::size(); j++)
                m_temp_data[j * params.num_frames() + i] = infinity();
        }
    }
    
    // Run stats and summary descriptors
        
    for (auto it = m_summary_modules.begin(); it != m_summary_modules.end(); it++)
    {
        auto index = dynamic_cast<summary_module *>(it->get())->get_index();
        
        if (index == -1)
            (*it)->calculate(params, nullptr, 0);
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
