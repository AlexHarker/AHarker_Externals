
#ifndef _SUMMARY_MODULES_HPP_
#define _SUMMARY_MODULES_HPP_

#include "descriptors_summary_graph.hpp"

#include "peak_finder.hpp"

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
        return new T(args.get_long(1, 1, 256));
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

    summary_module_spectral_peaks(long N) : summary_module_vector(true), m_num_peaks(N)
    {
        m_values.resize(N * 2);
    }

    static user_module *setup(const global_params& params, module_arguments& args);

    auto get_params() const { return std::make_tuple(summary_module::get_index(), m_num_peaks); }

    void add_requirements(graph& g) override;
    void prepare(const global_params& params) override;
    void calculate(const global_params& params, const double *data, long size) override;
    
private:
    
    spectrum_average *m_spectrum;
    
    peak_list m_peaks;
    peak_detector m_detector;
    long m_num_peaks;
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

// Specifiers

// Mask Time

struct specifier_mask_time : comparable_summary_specifier<specifier_mask_time>
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

struct specifier_threshold : comparable_summary_specifier<specifier_threshold>
{
    enum class mode { abs, peak_mul, peak_add, peak_db, mean_mul, mean_add, mean_db };
    
    specifier_threshold(double threshold = std::numeric_limits<double>::infinity(), mode type = mode::abs)
    : m_threshold(threshold), m_type(type) {}
    
    static user_module *setup(const global_params& params, module_arguments& args);

    auto get_params() const { return std::make_tuple(summary_module::get_index()); }
    void update_to_final(const module *m) override;

    void calculate(const global_params& params, const double *data, long size) override;
            
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
        constexpr auto infinity = std::numeric_limits<double>::infinity();
        
        const long *positions = m_finder->get_positions();
        
        if (Pos)
        {
            for (long i = 0; i < vector_module::get_n(); i++)
                vector_module::m_values[i] = frame_to_ms(params, positions[i]);
        }
        else
        {
            for (long i = 0; i < vector_module::get_n(); i++)
                vector_module::m_values[i] = (positions[i] == -1) ? infinity : data[positions[i]];
        }
    }
        
private:
               
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

// Longest Crossings

// Generic underlying longest crossings module

template <template <class T> class Op1, template <class T> class Op2>
struct longest_crossings : summary_module, comparable_module<longest_crossings<Op1, Op2>>
{
    struct crossing
    {
        crossing() : m_cross1(-1), m_cross2(-1) {}
        crossing(long cross1, long cross2) : m_cross1(cross1), m_cross2(cross2) {}
        
        long length() const { return m_cross2 - m_cross1; }
        
        long m_cross1;
        long m_cross2;
    };
    
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
        constexpr double infinity = std::numeric_limits<double>::infinity();
        
        const auto crossings = m_crossings.data();
        
        double threshold = m_threshold->get_threshold();
        long mask_span = m_mask_time->get_mask_span();

        long cross1, cross2, insert;
        long num_found = 0;
        
        for (long i = 0; i < size; i = cross2)
        {
            // Find the next crossing over the threshold
            
            for (cross1 = i; cross1 < size; cross1++)
                if (data[cross1] != infinity && Op1<double>()(data[cross1], threshold))
                    break;
            
            // Exit if we've reached the end without finding a crossing point
            
            if (cross1 == size)
                break;
            
            // Jump by the mask size then find out when the value crosses back through the threshold
            
            for (cross2 = cross1 + mask_span; cross2 < size; cross2++)
                if (data[cross2] != infinity && Op2<double>()(data[cross2], threshold))
                    break;

            crossing cross(cross1, std::max(cross2, size));

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

// Generic longest crossings user module

template <template <class T> class Op1, template <class T> class Op2, bool Both>
struct stat_longest_crossings_user : summary_module_vector_n<stat_longest_crossings_user<Op1, Op2, Both>>
{
    using crossings_module = longest_crossings<Op1, Op2>;
    using vector_module = summary_module_vector_n<stat_longest_crossings_user<Op1, Op2, Both>>;
    
    stat_longest_crossings_user(long N) : vector_module(N)
    {
        vector_module::m_values.resize(Both ? N * 2 : N);
    }
    
    void add_requirements(graph& g) override
    {
        m_crossings = g.add_requirement(new crossings_module());
        m_crossings->set_n(vector_module::get_n());
    }
    
    void calculate(const global_params& params, const double *data, long size) override
    {
        const auto *crossings = m_crossings->get_crossings();
        
        if (Both)
        {
            for (long i = 0; i < vector_module::get_n(); i++)
            {
                vector_module::m_values[i * 2 + 0] = frame_to_ms(params, crossings[i].m_cross1);
                vector_module::m_values[i * 2 + 1] = frame_to_ms(params, crossings[i].m_cross2);
            }
        }
        else
        {
            for (long i = 0; i < vector_module::get_n(); i++)
                vector_module::m_values[i] = frame_to_ms(params, crossings[i].m_cross1);
        }
    }
    
private:
    
    crossings_module *m_crossings;
};
                                      
// Final user module types

using stat_module_longest_above = stat_longest_crossings_user<std::greater, std::less, false>;
using stat_module_longest_below = stat_longest_crossings_user<std::less, std::greater, false>;
using stat_module_longest_above_both = stat_longest_crossings_user<std::greater, std::less, true>;
using stat_module_longest_below_both = stat_longest_crossings_user<std::less, std::greater, true>;

#endif /* _SUMMARY_MODULES_HPP_ */
