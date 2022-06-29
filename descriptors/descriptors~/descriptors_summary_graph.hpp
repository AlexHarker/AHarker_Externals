
#ifndef _DESCRIPTORS_SUMMARY_GRAPH_HPP_
#define _DESCRIPTORS_SUMMARY_GRAPH_HPP_

#include "descriptors_graph.hpp"

#include <type_traits>

// A type to indicate that a module is involved in calculating a summary (not per frame descriptors)

struct summary_module
{
    summary_module(bool no_index = false) : m_no_index(no_index), m_index(-1) {}
    virtual ~summary_module() {}

    // N.B. - used by specifiersthis should only ever be called after a dynamic cast and comparison check
    
    virtual void update_to_final(const module *m) {}
    
    size_t get_index() const { return m_index; }
    void set_index(size_t index) { m_index = m_no_index ? -1 : index; }
        
    bool m_no_index;
    size_t m_index;
};

// A type for adding specifiers that apply to a single descriptor

template <class T>
struct summary_specifier : summary_module, comparable_module<T, user_module>
{
    void calculate(const global_params& params, const double *data, long size) override {}
    
    size_t get_output_size() const override { return 0; }
    double get_output(size_t idx) const override { return 0.0; }
};

// The summary graph

class summary_graph : private graph
{    
public:
    
    summary_graph() : m_last_summary_idx(-1) {}
    
    void build(const setup_list& setups, const global_params& params, long argc, t_atom *argv);
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
