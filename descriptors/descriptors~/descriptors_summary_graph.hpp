
#ifndef _DESCRIPTORS_SUMMARY_GRAPH_HPP_
#define _DESCRIPTORS_SUMMARY_GRAPH_HPP_

#include "descriptors_graph.hpp"

#include <type_traits>

// A type to indicate that a module calculates a summary (not per frame descriptors)

struct summary_module
{
    size_t get_index() const { return m_index; }
    void set_index(size_t index) { m_index = index; }
    
    virtual ~summary_module() {}
    
    size_t m_index = -1;
};

class summary_graph : private graph
{    
public:
    
    void build(const setup_list& setups, const global_params& params, long argc, t_atom *argv);
    void prepare(const global_params& params);
    void run(const global_params& params, const double *input);
    
    void output(t_atom *argv) { graph::output(m_summary_outputs, argv); }
    size_t size() { return graph::size(m_summary_outputs); }
    
private:
    
    module *add_requirement_impl(module *m) override;
    
    std::vector<std::unique_ptr<module>> m_summary_modules;
    std::vector<user_module *> m_summary_outputs;
    std::vector<double> m_frame;
    std::vector<double> m_temp_data;
};

#endif /* _DESCRIPTORS_SUMMARY_GRAPH_HPP_ */
