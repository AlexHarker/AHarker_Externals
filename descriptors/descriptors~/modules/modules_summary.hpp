
#ifndef _SUMMARY_MODULES_HPP_
#define _SUMMARY_MODULES_HPP_

#include "processing_containers.hpp"
#include "utility_definitions.hpp"

#include "../descriptors_graph.hpp"
#include "../descriptors_summary_modules.hpp"

#include "../library/peak_finder.hpp"

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

template <class T>
struct summary_module_vector_n : summary_module_vector<T>
{
    summary_module_vector_n(long N) : m_n(N) {}
    
    static user_module *setup(const global_params& params, module_arguments& args)
    {
        return new T(args.get_long("number of values", 1, 1, 256));
    }
    
    auto get_params() const { return std::make_tuple(summary_module::get_index(), get_n()); }
    
protected:
    
    long get_n() const { return m_n; }
    
    long m_n;
};

// Helpers

double frame_to_ms(const global_params& params, double frame);

// Duration

struct summary_module_duration : summary_module_single<summary_module_duration>
{
    summary_module_duration() : summary_module_single(true) {}
    
    static user_module *setup(const global_params& params, module_arguments& args);
    
    auto get_params() const { return std::make_tuple(summary_module::get_index()); }

    void calculate(const global_params& params, const double *data, long size) override;
    
    bool is_descriptor() const override { return true; };
};

// Summary Spectral Peaks 

struct summary_module_spectral_peaks : summary_module_vector<summary_module_spectral_peaks>
{
    using peak_list = peak_set<double>;
    using peak_detector = peak_finder<double>;

    struct spectrum_average : module_core<spectrum_average>
    {
        void add_requirements(graph& g) override;
        void prepare(const global_params& params) override;
        void calculate(const global_params& params, const double *frame, long size) override;
        
        const double *get_average() const { return m_spectrum.data(); }
        
    private:
        
        module_amplitude_spectrum *m_amplitude_module;
        aligned_vector<> m_spectrum;
    };

    summary_module_spectral_peaks(long N, long median_width, double median_gain, double range)
    : summary_module_vector(true), m_filter(median_width), m_num_peaks(N)
    , m_median_width(median_width), m_median_gain(median_gain), m_range(range)
    {
        m_values.resize(N * 2);
    }

    static user_module *setup(const global_params& params, module_arguments& args);

    auto get_params() const { return std::make_tuple(summary_module::get_index(), m_num_peaks); }

    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *data, long size) override;
    
    bool is_descriptor() const override { return true; };

private:
    
    spectrum_average *m_spectrum;
    
    peak_list m_peaks;
    peak_detector m_detector;
    median_filter<double> m_filter;

    aligned_vector<> m_median_spectrum;

    const long m_num_peaks;
    const long m_median_width;
    const double m_median_gain;
    const double m_range;
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

// Mean / Median / Centroid / Stddev / Range 

struct stat_module_mean : stat_module_simple<stat_module_mean>
{
    void calculate(const global_params& params, const double *data, long size) override;
};

struct stat_module_median : stat_module_simple<stat_module_median>
{
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *data, long size) override;
    
private:
    
    aligned_vector<long> m_indices;
};

struct stat_module_centroid : stat_module_simple<stat_module_centroid>
{
    void calculate(const global_params& params, const double *data, long size) override;
};

struct stat_module_stddev : stat_module_simple<stat_module_stddev>
{
    void add_requirements(graph& g) override
    {
        m_mean = g.add_requirement(new stat_module_mean());
    }
    
    void calculate(const global_params& params, const double *data, long size) override;
    
private:
    
    stat_module_mean *m_mean;
};

struct stat_module_range : stat_module_simple<stat_module_range>
{
    void calculate(const global_params& params, const double *data, long size) override;
};

// Finding Stats

// Mask Storage

struct storage_mask : summary_module, module_core<storage_mask>
{
    storage_mask() : summary_module(true) {}
    
    void prepare(const global_params& params) override { m_mask.resize(params.num_frames()); }
    void calculate(const global_params& params, const double *data, long size) override {}
    
    std::vector<bool> &get_reset_mask(const double *data, long size)
    {
        for (long i = 0; i < size; i++)
            m_mask[i] = data[i] == infinity();
        
        return m_mask;
    }
    
private:
    
    std::vector<bool> m_mask;
};

// Specifiers

// Mask Time

struct specifier_mask_time : summary_specifier<specifier_mask_time>
{
    specifier_mask_time(double time = -1.0) : m_mask_time(time) {}

    static user_module *setup(const global_params& params, module_arguments& args);
        
    auto get_params() const { return std::make_tuple(summary_module::get_index()); }
    void update_to_final(const module *m) override;

    void prepare(const global_params& params) override;
            
    long get_mask_span() { return m_mask_span; }
    
private:
    
    double m_mask_time = -1.0;
    long m_mask_span = 0;
};

// Threshold

struct specifier_threshold : summary_specifier<specifier_threshold>
{
    enum class mode { abs, peak_mul, peak_add, peak_db, mean_mul, mean_add, mean_db };
    
    specifier_threshold(double threshold = infinity(), mode type = mode::abs)
    : m_threshold(threshold), m_type(type) {}
    
    static user_module *setup(const global_params& params, module_arguments& args);

    auto get_params() const { return std::make_tuple(summary_module::get_index()); }
    void update_to_final(const module *m) override;

    void calculate(const global_params& params, const double *data, long size) override;
            
    double get_threshold() { return m_calculated_threshold; }
    
private:
    
    double m_threshold = infinity();
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
        std::vector<bool> &mask = m_mask->get_reset_mask(data, size);
        long mask_span = m_mask_time->get_mask_span();
        
        // Reset the mask
        
        for (long j = 0; j < size; j++)
            mask[j] = data[j] == infinity();
                
        // Loop over values
        
        for (long i = 0; i < m_n; i++)
        {
            double threshold = 0.0;
            long pos = -1;
            
            // Find the next value of interest
            
            for (long j = 0; j < size; j++)
            {
                long test_pos = j;
                
                if (!mask[j] && m_cond(data, j, size, pos == -1, threshold))
                {
                    pos = test_pos;
                    threshold = data[test_pos];
                }
            }
             
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
struct stat_find_n_user : summary_module_vector_n<stat_find_n_user<Condition, Pos>>
{
    using finder_module = find_n<Condition>;
    using vector_module = summary_module_vector_n<stat_find_n_user<Condition, Pos>>;
    
    stat_find_n_user(long N) : vector_module(N)
    {
        vector_module::m_values.resize(N);
    }
    
    void add_requirements(graph& g) override
    {
        m_finder = g.add_requirement(new finder_module());
        m_finder->set_n(vector_module::get_n());
    }
    
    void calculate(const global_params& params, const double *data, long size) override
    {
        const long *positions = m_finder->get_positions();
        
        if (Pos)
        {
            for (long i = 0; i < vector_module::get_n(); i++)
                vector_module::m_values[i] = frame_to_ms(params, positions[i]);
        }
        else
        {
            for (long i = 0; i < vector_module::get_n(); i++)
                vector_module::m_values[i] = (positions[i] == -1) ? infinity() : data[positions[i]];
        }
    }
        
private:
               
    finder_module *m_finder;
};

// Condition templates

template <template <class T> class Op>
struct condition_min_max
{
    bool operator()(const double *data, long idx, long size, bool skip_threshold, double threshold)
    {
        return skip_threshold || Op<double>()(data[idx], threshold);
    }
};

template <template <class T> class Op1, template <class T> class Op2>
struct condition_peak_trough
{
    bool reverse_op(double a, double b) { return Op2<double>()(a, b); }

    bool operator()(const double *data, long& idx, long size, bool skip_threshold, double threshold)
    {
        const double value = data[idx];
        
        if ((skip_threshold || Op1<double>()(value, threshold)) && (!idx || Op1<double>()(value, data[idx - 1])))
        {
            // Now search right until the value changes
         
            for (; idx < size - 1; idx++)
                if (value != data[idx + 1])
                    break;
                  
            long clipped_pos = std::min(idx + 1, size - 1);
            
            if (data[clipped_pos] == infinity() || !Op2<double>()(value, data[clipped_pos]))
            {
                idx++;
                return true;
            }
        }
        
        return false;
    }
};

using is_max = condition_min_max<std::greater>;
using is_min = condition_min_max<std::less>;
using is_peak = condition_peak_trough<std::greater, std::less>;
using is_trough = condition_peak_trough<std::less, std::greater>;

// Final user module types

using stat_module_max = stat_find_n_user<is_max, false>;
using stat_module_min = stat_find_n_user<is_min, false>;
using stat_module_max_pos = stat_find_n_user<is_max, true>;
using stat_module_min_pos = stat_find_n_user<is_min, true>;
using stat_module_peak = stat_find_n_user<is_peak, false>;
using stat_module_trough = stat_find_n_user<is_trough, false>;
using stat_module_peak_pos = stat_find_n_user<is_peak, true>;
using stat_module_trough_pos = stat_find_n_user<is_trough, true>;

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
            if (data[i] != infinity())
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

// Crossings

struct crossing
{
    crossing() : m_pos(-1), m_cross1(-1), m_cross2(-1) {}
    crossing(long pos, long cross1, long cross2) : m_pos(pos), m_cross1(cross1), m_cross2(cross2) {}
    
    long length() const { return m_cross2 - m_cross1; }

    long m_pos;
    long m_cross1;
    long m_cross2;
};

// Generic underlying longest crossings module

template <class Condition>
struct extreme_crossings : summary_module, comparable_module<extreme_crossings<Condition>>
{
    extreme_crossings() : summary_module(), m_n(1) {}
    
    auto get_params() const { return std::make_tuple(summary_module::get_index()); }
    
    void add_requirements(graph& g) override
    {
        m_mask = g.add_requirement(new storage_mask());
        m_mask_time = g.add_requirement(new specifier_mask_time());
        m_threshold = g.add_requirement(new specifier_threshold());
    }
    
    void prepare(const global_params& params) override
    {
        m_crossings.resize(m_n);
    }
    
    void calculate(const global_params& params, const double *data, long size) override
    {
        const auto crossings = m_crossings.data();
        
        std::vector<bool> &mask = m_mask->get_reset_mask(data, size);
        double threshold = m_threshold->get_threshold();
        long mask_span = m_mask_time->get_mask_span();
                  
        long num_found = 0;
        
        // Loop per extremity
        
        for (long i = 0; i < m_n; i++)
        {
            // Assume there is not another valid value
            
            double compare = threshold;
            long pos = -1;
                        
            // Search for the next most extreme value
            
            for (long j = 0; j < size; j++)
            {
                long test_pos = j;
                
                if (!mask[j] && m_cond(data, j, size, false, compare))
                {
                    compare = data[test_pos];
                    pos = test_pos;
                }
            }
            
            // If pos is invalid then we are done
            
            if (pos == -1)
                break;
                    
            long cross1, cross2;
    
            // Search earlier then later for crossing points
                
            for (cross1 = pos; cross1 > 0 ; cross1--)
                if (data[i] != infinity() && m_cond.reverse_op(data[cross1], threshold))
                    break;
                                
            for (cross2 = pos + 1; cross2 < size - 1; cross2++)
                if (data[i] != infinity() && m_cond.reverse_op(data[cross2], threshold))
                    break;
            
            // Update mask points and do masking
            
            for (long j = std::max(pos - mask_span, cross1); j < std::min(pos + mask_span + 1, cross2); j++)
                mask[j] = true;
                        
            crossings[num_found++] = crossing(pos, cross1, cross2);
        }
        
        // Store defaults if there aren't any more values to be found
        
        for (long i = num_found; i < m_n; i++)
            crossings[i] = crossing();
    }
    
    void set_n(long N) { m_n = std::max(m_n, N); }
    
    const crossing *get_crossings() const { return m_crossings.data(); }
    long get_n() const { return m_n; }
    
private:
    
    storage_mask *m_mask;
    specifier_mask_time *m_mask_time;
    specifier_threshold *m_threshold;
    
    aligned_vector<crossing> m_crossings;
    long m_n;
    
    Condition m_cond;
};

// Longest Crossings

// Generic underlying longest crossings module

template <template <class T> class Op1, template <class T> class Op2>
struct longest_crossings : summary_module, comparable_module<longest_crossings<Op1, Op2>>
{
    longest_crossings() : summary_module(), m_n(1) {}
    
    auto get_params() const { return std::make_tuple(summary_module::get_index()); }
    
    void add_requirements(graph& g) override
    {
        m_mask_time = g.add_requirement(new specifier_mask_time());
        m_threshold = g.add_requirement(new specifier_threshold());
    }
    
    void prepare(const global_params& params) override
    {
        m_crossings.resize(m_n);
    }
    
    void calculate(const global_params& params, const double *data, long size) override
    {
        const auto crossings = m_crossings.data();
        
        double threshold = m_threshold->get_threshold();
        long mask_span = m_mask_time->get_mask_span();
        
        long cross1, cross2, insert;
        long num_found = 0;
        
        for (long i = 0; i < size; i = cross2)
        {
            // Find the next crossing over the threshold
            
            for (cross1 = i; cross1 < size; cross1++)
                if (data[cross1] != infinity() && Op1<double>()(data[cross1], threshold))
                    break;
            
            // Exit if we've reached the end without finding a crossing point
            
            if (cross1 == size)
                break;
            
            // Jump by the mask size then find out when the value crosses back through the threshold
            
            for (cross2 = cross1 + mask_span; cross2 < size; cross2++)
                if (data[cross2] != infinity() && Op2<double>()(data[cross2], threshold))
                    break;
            
            crossing cross(-1, cross1, std::min(cross2, size));
            
            // If this is longer than any of the stored values then move others and store this crossing
            
            for (insert = 0; insert < num_found; insert++)
                if (cross.length() > crossings[insert].length())
                    break;
            
            num_found = std::min(num_found + 1, m_n);
            
            if (insert < num_found)
            {
                for (long j = num_found - 1; j >= insert + 1; j--)
                    crossings[j] = crossings[j - 1];
                
                crossings[insert] = cross;
            }
        }
        
        // Store defaults if there aren't any more values to be found
        
        for (long i = num_found; i < m_n; i++)
            crossings[i] = crossing();
    }
    
    void set_n(long N) { m_n = std::max(m_n, N); }
    
    const crossing *get_crossings() const { return m_crossings.data(); }
    long get_n() const { return m_n; }
    
private:
    
    specifier_mask_time *m_mask_time;
    specifier_threshold *m_threshold;
    
    aligned_vector<crossing> m_crossings;
    long m_n;
};

// Generic crossings user module

enum class cross_mode { value, pos, cross, crossings, length };

template <class Crossings, cross_mode Mode>
struct stat_crossings_user : summary_module_vector_n<stat_crossings_user<Crossings, Mode>>
{
    using vector_module = summary_module_vector_n<stat_crossings_user<Crossings, Mode>>;
    
    stat_crossings_user(long N) : vector_module(N)
    {
        vector_module::m_values.resize(Mode == cross_mode::crossings ? N * 2 : N);
    }
    
    void add_requirements(graph& g) override
    {
        m_crossings = g.add_requirement(new Crossings());
        m_crossings->set_n(vector_module::get_n());
    }
    
    void calculate(const global_params& params, const double *data, long size) override
    {
        const auto *crossings = m_crossings->get_crossings();
        
        switch (Mode)
        {
            case cross_mode::value:
                for (long i = 0; i < vector_module::get_n(); i++)
                    vector_module::m_values[i] = (crossings[i].m_pos == -1) ? infinity() : data[crossings[i].m_pos];
                break;
                
            case cross_mode::pos:
                for (long i = 0; i < vector_module::get_n(); i++)
                    vector_module::m_values[i] = frame_to_ms(params, crossings[i].m_pos);
                break;
                
            case cross_mode::cross:
                for (long i = 0; i < vector_module::get_n(); i++)
                    vector_module::m_values[i] = frame_to_ms(params, crossings[i].m_cross1);
                break;
                
            case cross_mode::length:
                for (long i = 0; i < vector_module::get_n(); i++)
                    vector_module::m_values[i] = frame_to_ms(params, crossings[i].m_cross2 - crossings[i].m_cross1);
                break;
                
            case cross_mode::crossings:
                for (long i = 0; i < vector_module::get_n(); i++)
                {
                    vector_module::m_values[i * 2 + 0] = frame_to_ms(params, crossings[i].m_cross1);
                    vector_module::m_values[i * 2 + 1] = frame_to_ms(params, crossings[i].m_cross2);
                }
                break;
        }
    }
    
private:
    
    Crossings *m_crossings;
};

using extreme_above = extreme_crossings<is_peak>;
using extreme_below = extreme_crossings<is_trough>;
using longest_above = longest_crossings<std::greater, std::less>;
using longest_below = longest_crossings<std::less, std::greater>;

// Final user module types

using stat_module_crossing_peak = stat_crossings_user<extreme_above, cross_mode::value>;
using stat_module_crossing_trough = stat_crossings_user<extreme_below, cross_mode::value>;
using stat_module_crossing_peak_pos = stat_crossings_user<extreme_above, cross_mode::pos>;
using stat_module_crossing_trough_pos = stat_crossings_user<extreme_below, cross_mode::pos>;
using stat_module_cross_above = stat_crossings_user<extreme_above, cross_mode::cross>;
using stat_module_cross_below = stat_crossings_user<extreme_below, cross_mode::cross>;
using stat_module_crossings_above = stat_crossings_user<extreme_above, cross_mode::crossings>;
using stat_module_crossings_below = stat_crossings_user<extreme_below, cross_mode::crossings>;
using stat_module_longest_above = stat_crossings_user<longest_above, cross_mode::cross>;
using stat_module_longest_below = stat_crossings_user<longest_below, cross_mode::cross>;
using stat_module_longest_above_both = stat_crossings_user<longest_above, cross_mode::crossings>;
using stat_module_longest_below_both = stat_crossings_user<longest_below, cross_mode::crossings>;

#endif /* _SUMMARY_MODULES_HPP_ */
