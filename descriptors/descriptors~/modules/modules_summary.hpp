
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

// Mask Storage / Time

struct mask_storage : summary_module, comparable_module<mask_storage>
{
    mask_storage() : summary_module(true) {}
    
    auto get_params() const { return std::make_tuple(); }
    void prepare(const global_params& params) override { m_mask.resize(params.num_frames()); }
    void calculate(const global_params& params, const double *data, long size) override {}
    
    std::vector<bool> &get_mask() { return m_mask; }
    
private:
    
    std::vector<bool> m_mask;
};
/*
struct mask_time : summary_module, user_module, comparable_module<mask_time, user_module>
{
    auto get_params() const { return std::make_tuple(summary_module::get_index()); }
    
    void prepare(const global_params& params) override
    {        
        double time = m_mask_time * 1000.0 * params.m_sr / params.m_hop_size;
        m_mask_span = m_mask_time < 0.0 ? 0 : time;
    }
    
    void calculate(const global_params& params, const double *data, long size) override {}
    
    void update_to_final(const module *m) override
    {
        m_mask_time = dynamic_cast<const mask_time *>(m)->m_mask_time;
    }
    
    long get_mask_span() { return m_mask_span; }
    
private:
    
    double m_mask_time = -1.0;
    long m_mask_span = 0;
};
*/
// Generic underlying finding module

template <class Condition>
struct find_n : summary_module, comparable_module<find_n<Condition>>
{
    find_n() : summary_module(), m_n(1) {}
    
    auto get_params() const { return std::make_tuple(summary_module::get_index()); }

    void add_requirements(graph& g) override
    {
        m_mask = g.add_requirement(new mask_storage());
    }
    
    void prepare(const global_params& params) override
    {
        m_pos.resize(m_n);
    }
    
    void calculate(const global_params& params, const double *data, long size) override
    {
        // FIX - This is probably coming in here

        std::vector<bool> &mask = m_mask->get_mask();
        
        long mask_span = 0;
        
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
    
    // FIX - use common mask/mask params for efficiency
    
    mask_storage *m_mask;
    
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

#endif /* _SUMMARY_MODULES_HPP_ */
