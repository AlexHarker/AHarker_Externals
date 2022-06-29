
#ifndef _SUMMARY_MODULES_HPP_
#define _SUMMARY_MODULES_HPP_

#include "descriptors_modules.hpp"

#include <Statistics.hpp>

#include <algorithm>
#include <tuple>
#include <vector>

template <class T>
struct summary_module_single : summary_module, user_module_single<T>
{
    summary_module_single(bool no_index = false) : summary_module(no_index) {}
};

template <class T>
struct summary_module_vector : summary_module, user_module_vector<T>
{
    summary_module_vector(bool no_index = false) : summary_module(no_index) {}
};

// Duration

struct summary_module_duration : summary_module_single<summary_module_duration>
{
    summary_module_duration() : summary_module_single(true) {}
    
    static user_module *setup(const global_params& params, module_arguments& args)
    {
        return new summary_module_duration();
    }
    
    auto get_params() const { return std::make_tuple(summary_module::get_index()); }

    void calculate(const global_params& params, const double *data, long size) override
    {
        m_value = 1000.0 * params.m_signal_length / params.m_sr;
    }
};

// Stats

template <class T>
struct stat_module_simple : summary_module_single<T>
{
    static user_module *setup(const global_params& params, module_arguments& args)
    {
        return new T();
    }
    
    auto get_params() const { return std::make_tuple(summary_module::get_index()); }
};

// Mean

// FIX - all of these need to ignore spurious values I think

struct stat_module_mean : stat_module_simple<stat_module_mean>
{
    void calculate(const global_params& params, const double *data, long size) override
    {
        m_value = stat_mean(data, size);
    }
};

struct stat_module_centroid : stat_module_simple<stat_module_centroid>
{
    // FIX - could be more efficient
    
    void calculate(const global_params& params, const double *data, long size) override
    {
        m_value = stat_centroid(data, size);
    }
};

struct stat_module_stddev : stat_module_simple<stat_module_stddev>
{
    // FIX - could be more efficient
    
    void calculate(const global_params& params, const double *data, long size) override
    {
        m_value = stat_standard_deviation(data, size);
    }
};

struct stat_module_range : stat_module_simple<stat_module_range>
{
    void calculate(const global_params& params, const double *data, long size) override
    {
        m_value = stat_max(data, size) - stat_min(data, size);
    }
};

struct stat_module_median : stat_module_simple<stat_module_median>
{
    void prepare(const global_params& params) override
    {
        m_indices.resize(params.num_frames());
    }
    
    void calculate(const global_params& params, const double *data, long size) override
    {
        long *indices = m_indices.data();
        
        sort_ascending(indices, data, size);
        m_value = data[indices[size >> 1]];
    }
    
private:
    
    aligned_vector<long> m_indices;
};

// Finding Stats

// Mask Storage

struct storage_mask : summary_module, comparable_module<storage_mask>
{
    storage_mask() : summary_module(true) {}
    
    auto get_params() const { return std::make_tuple(); }
    void prepare(const global_params& params) override { m_mask.resize(params.num_frames()); }
    void calculate(const global_params& params, const double *data, long size) override {}
    
    std::vector<bool> &get_mask() { return m_mask; }
    
private:
    
    std::vector<bool> m_mask;
};

// Mask Time

struct specifier_mask_time : comparable_summary_specifier<specifier_mask_time>
{
    static user_module *setup(const global_params& params, module_arguments& args)
    {
        double time = args.get_double(0.0, 0.0, std::numeric_limits<double>::infinity());
        return new specifier_mask_time(time);
    }
    
    specifier_mask_time(double time = -1.0) : m_mask_time(time) {}
    
    auto get_params() const { return std::make_tuple(summary_module::get_index()); }
    
    void prepare(const global_params& params) override
    {        
        double time = m_mask_time * params.m_sr / (1000.0 * params.m_hop_size);
        m_mask_span = m_mask_time < 0.0 ? 0 : time;
    }
        
    void update_to_final(const module *m) override
    {
        auto time = dynamic_cast<const specifier_mask_time *>(m)->m_mask_time;
        if (time > 0.0)
            m_mask_time = time;
    }
    
    long get_mask_span() { return m_mask_span; }
    
private:
    
    double m_mask_time = -1.0;
    long m_mask_span = 0;
};

// Threshold

struct specifier_threshold : comparable_summary_specifier<specifier_threshold>
{
    enum class mode { abs, peak_mul, peak_add, peak_db, mean_mul, mean_add, mean_db };
    
    static user_module *setup(const global_params& params, module_arguments& args)
    {
        double threshold = args.get_double(0.0, -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
        t_symbol *type_specifier = args.get_symbol(gensym("abs"));
        mode type = mode::abs;
        
        if (type_specifier == gensym("mean_mul"))
            type = mode::mean_mul;
        else if (type_specifier == gensym("mean_add"))
            type = mode::mean_add;
        else if (type_specifier == gensym("mean_db"))
            type = mode::mean_db;
        else if (type_specifier == gensym("peak_mul"))
            type = mode::peak_mul;
        else if (type_specifier == gensym("peak_add"))
            type = mode::peak_add;
        else if (type_specifier == gensym("peak_db"))
            type = mode::peak_db;
        
        return new specifier_threshold(threshold, type);
    }
    
    specifier_threshold(double threshold = std::numeric_limits<double>::infinity(), mode type = mode::abs)
    : m_threshold(threshold), m_type(type) {}
    
    void calculate(const global_params& params, const double *data, long size) override
    {
        const bool use_mean = m_type == mode::mean_mul || m_type == mode::mean_add || m_type == mode::mean_db;
        const bool use_peak = m_type == mode::peak_mul || m_type == mode::peak_add || m_type == mode::peak_db;
        const bool use_db = m_type == mode::mean_db || m_type == mode::peak_db;
        
        const double specified = use_db ? dbtoa(m_threshold) : m_threshold;
        
        double stat = -std::numeric_limits<double>::infinity();
        
        if (use_mean)
        {
            stat = 0.0;
            long num_valid = 0;
            
            for (long i = 0; i < size; i++)
            {
                if (data[i] != std::numeric_limits<double>::infinity())
                {
                    stat += data[i];
                    num_valid++;
                }
            }
            
            if (num_valid)
                stat /= num_valid;
        }
        else if (use_peak)
        {
            for (long i = 0; i < size; i++)
            {
                if (data[i] != std::numeric_limits<double>::infinity())
                    stat = std::max(stat, data[i]);
            }
        }
            
        switch (m_type)
        {
            case mode::abs:
                m_calculated_threshold = specified;
                break;
            case mode::mean_mul:
            case mode::mean_db:
            case mode::peak_mul:
            case mode::peak_db:
                m_calculated_threshold = specified * stat;
                break;
            case mode::mean_add:
            case mode::peak_add:
                m_calculated_threshold = specified + stat;
                break;
        }
    }
    
    auto get_params() const { return std::make_tuple(summary_module::get_index()); }
    
    void update_to_final(const module *m) override
    {
        auto b = dynamic_cast<const specifier_threshold *>(m);
        if (b->m_threshold != std::numeric_limits<double>::infinity())
        {
            m_threshold = b->m_threshold;
            m_type = b->m_type;
        }
    }
    
    double get_threshold() { return m_calculated_threshold; }
    
private:
    
    double m_threshold = std::numeric_limits<double>::infinity();
    double m_calculated_threshold = -1.0;
    mode m_type = mode::abs;
};

// Generic underlying finding module

template <class Condition>
struct find_n : summary_module, comparable_module<find_n<Condition>>
{
    find_n() : summary_module(), m_n(1) {}
    
    auto get_params() const { return std::make_tuple(summary_module::get_index()); }

    void add_requirements(graph& g) override
    {
        m_mask = g.add_requirement(new storage_mask());
        m_mask_time = g.add_requirement(new specifier_mask_time());
    }
    
    void prepare(const global_params& params) override
    {
        m_pos.resize(m_n);
    }
    
    void calculate(const global_params& params, const double *data, long size) override
    {
        std::vector<bool> &mask = m_mask->get_mask();
        long mask_span = m_mask_time->get_mask_span();
        
        // Reset the mask
        
        for (long j = 0; j < size; j++)
            mask[j] = data[j] == std::numeric_limits<double>::infinity();
                
        // Loop over values
        
        for (long i = 0; i < m_n; i++)
        {
            long pos = -1;
            
            // Find the next value of interest
            
            for (long j = 0; j < size; j++)
                if (!mask[j] && m_cond(data, j, pos, size))
                    pos = j;
             
            // If valid apply the mask and store, else exit early
            
            if (pos != -1)
            {
                for (long j = std::max(0L, pos - mask_span); j < std::min(pos + mask_span + 1, size); j++)
                    mask[j] = true;
                
                m_pos.data()[i] = pos;
            }
            else
            {
                for (; i < m_n; i++)
                    m_pos.data()[i] = -1;
            }
        }
    }
    
    void set_n(long N) { m_n = std::max(m_n, N); }
    
    const long *get_positions() const { return m_pos.data(); }
    long get_n() const { return m_n; }
    
private:
        
    storage_mask *m_mask;
    specifier_mask_time *m_mask_time;
    
    aligned_vector<long> m_pos;
    long m_n;
    Condition m_cond;
};

// Generic finding user module

template <class Condition, bool Pos>
struct stat_find_n_user : summary_module_vector<stat_find_n_user<Condition, Pos>>
{
    using finder_module = find_n<Condition>;
    using vector_module = summary_module_vector<stat_find_n_user<Condition, Pos>>;
    
    stat_find_n_user(long N)
    {
        vector_module::m_values.resize(N);
    }
    
    static user_module *setup(const global_params& params, module_arguments& args)
    {
        long N = args.get_long(1, 1, 256);
        return new stat_find_n_user(N);
    }
    
    auto get_params() const { return std::make_tuple(summary_module::get_index(), get_n()); }
    
    void add_requirements(graph& g) override
    {
        m_finder = g.add_requirement(new finder_module());
        m_finder->set_n(get_n());
    }
    
    void calculate(const global_params& params, const double *data, long size) override
    {
        constexpr auto infinity = std::numeric_limits<double>::infinity();
        
        const long *positions = m_finder->get_positions();
        
        if (Pos)
        {
            const double frame_to_ms = 1000.0 * params.m_hop_size / params.m_sr;
            
            for (long i = 0; i < vector_module::m_values.size(); i++)
            {
                long pos = positions[i];
                vector_module::m_values[i] = (pos == -1) ? infinity : pos * frame_to_ms;
            }
        }
        else
        {
            for (long i = 0; i < vector_module::m_values.size(); i++)
            {
                long pos = positions[i];
                vector_module::m_values[i] = (pos == -1) ? infinity : data[pos];
            }
        }
    }
        
private:
           
    long get_n() const { return static_cast<long>(vector_module::m_values.size()); }
    
    finder_module *m_finder;
};

// Condition templates

template <template <class T> class Op>
struct condition_single
{
    bool operator()(const double *data, long i, long pos, long size)
    {
        return pos == -1 || Op<double>()(data[i], data[pos]);
    }
};

template <template <class T> class Op>
struct condition_triple
{
    bool operator()(const double *data, long i, long pos, long size)
    {
        Op<double> op;
        bool is_triple = i && i < size && op(data[i], data[i-1]) && op(data[i], data[i+1]);
        return is_triple && condition_single<Op>()(data, i, pos, size);
    }
};

// Final user module types

using stat_module_max = stat_find_n_user<condition_single<std::greater>, false>;
using stat_module_min = stat_find_n_user<condition_single<std::less>, false>;
using stat_module_max_pos = stat_find_n_user<condition_single<std::greater>, true>;
using stat_module_min_pos = stat_find_n_user<condition_single<std::less>, true>;
using stat_module_peak = stat_find_n_user<condition_triple<std::greater>, false>;
using stat_module_trough = stat_find_n_user<condition_triple<std::less>, false>;
using stat_module_peak_pos = stat_find_n_user<condition_triple<std::greater>, true>;
using stat_module_trough_pos = stat_find_n_user<condition_triple<std::less>, true>;

// Ratio user module

template <template <class T> class Op>
struct stat_module_ratio : stat_module_simple<stat_module_ratio<Op>>
{
    using base = stat_module_simple<stat_module_ratio<Op>>;
    
    void add_requirements(graph& g) override
    {
        m_threshold = g.add_requirement(new specifier_threshold());
    }
    
    void calculate(const global_params& params, const double *data, long size) override
    {
        double threshold = m_threshold->get_threshold();
        
        long num_cond = 0;
        long num_valid = 0;
        
        for (long i = 0; i < size; i++)
        {
            if (data[i] != std::numeric_limits<double>::infinity())
            {
                if (Op<double>()(data[i], threshold))
                    num_cond++;
                num_valid++;
            }
        }
        
        base::m_value = num_valid ? static_cast<double>(num_cond) / num_valid : 0.0;
    }
    
    specifier_threshold *m_threshold;
};

// Final user module types

using stat_module_ratio_above = stat_module_ratio<std::greater>;
using stat_module_ratio_below = stat_module_ratio<std::less>;

#endif /* _SUMMARY_MODULES_HPP_ */
