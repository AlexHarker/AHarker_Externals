
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

// Length

struct summary_module_length : summary_module_single<summary_module_length>
{
    summary_module_length() : summary_module_single(true) {}
    
    static user_module *setup(const global_params& params, module_arguments& args)
    {
        return new summary_module_length();
    }
    
    auto get_params() const { return std::make_tuple(summary_module::get_index()); }

    void calculate(const global_params& params, const double *data, long size) override
    {
        m_value = 1000.0  * params.m_signal_length / params.m_sr;
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

// FIX - all of these need to ignore spurios values I think

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
/*
struct stat_module_ratio_above : stats_module_single<stat_module_ratio_above>
{
    // FIX - set threshold
    
    void calculate(const double *data, long size) override
    {
        m_value = stat_ratio_above(data, m_threshold, size);
    }
    
private:
    
    double m_threshold;
};

struct stat_module_ratio_below : stats_module_single<stat_module_ratio_below>
{
    // FIX - set threshold
    
    void calculate(const double *data, long size) override
    {
        m_value = stat_ratio_below(data, m_threshold, size);
    }
    
private:
    
    double m_threshold;
};

// FIX - should be multiple versions (with masks)

struct stat_module_min : stats_module_single<stat_module_min>
{
    void calculate(const double *data, long size) override
    {
        m_value = stat_min(data, size);
    }
};

struct stat_module_max : stats_module_single<stat_module_max>
{
    void calculate(const double *data, long size) override
    {
        m_value = stat_max(data, size);
    }
};

struct stat_module_min_pos : stats_module_single<stat_module_min_pos>
{
    void calculate(const double *data, long size) override
    {
        m_value = stat_min_pos(data, size);
    }
};

struct stat_module_max_pos : stats_module_single<stat_module_max_pos>
{
    void calculate(const double *data, long size) override
    {
        m_value = stat_max_pos(data, size);
    }
};
*/

#endif /* _SUMMARY_MODULES_HPP_ */
