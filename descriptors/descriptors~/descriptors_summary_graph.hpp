
#ifndef _DESCRIPTORS_SUMMARY_GRAPH_HPP_
#define _DESCRIPTORS_SUMMARY_GRAPH_HPP_

#include "descriptors_graph.hpp"

// The summary graph

class summary_graph : private graph
{    
public:
    
    summary_graph() : m_last_summary_idx(-1) {}
    
    void build(t_object *x, const setup_list& setups, const global_params& params, long argc, t_atom *argv);
    void prepare(const global_params& params);
    void run(const global_params& params, const double *input);
    
    void output(t_atom *argv) { graph::output(m_summary_outputs, argv); }
    size_t size() { return graph::size(m_summary_outputs); }
    
private:
    
    module *add_requirement_impl(module *m) override;
    
    void check_last_descriptor_summary();
    
    std::vector<std::unique_ptr<module>> m_summary_modules;
    std::vector<user_module *> m_summary_outputs;
    std::vector<double> m_frame;
    std::vector<double> m_temp_data;
    
    size_t m_last_summary_idx;
};

#endif /* _DESCRIPTORS_SUMMARY_GRAPH_HPP_ */
