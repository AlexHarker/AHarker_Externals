
#ifndef MATCHERS_HPP
#define MATCHERS_HPP

#include "atom_types.hpp"
#include "entry_database.hpp"

#include <limits>

class matchers
{
public:
    
    enum class test { none, match, less, greater, less_eq, greater_eq, diff, ratio, diff_reject, ratio_reject };
        
    // Constructor and Capacity
    
    matchers() : m_num_matches(0), m_audio_style(false) {}
        
    long size() const   { return static_cast<long>(m_matchers.size()); }
    void clear()        { m_matchers.clear(); }

    // Utilities
    
    static test test_type(t_atom *argv);
    static bool needs_scale(t_atom *argv);

    // Add / Set Matchers
    
    void add_matcher(test type, long column, double scale = 1.0)
    {
        m_matchers.push_back(matcher(type, column, scale));
    }
    
    void set_matchers(void *x, long argc, t_atom *argv, const entries::read_pointer& database);
    
    // Add / Set Targets
    
    void add_target(double value)
    {
        if (m_matchers.size())
            m_matchers.back().add_target(value);
    }

    void add_target(t_symbol *value)
    {
        if (m_matchers.size())
            m_matchers.back().add_target(value);
    }

    void set_target(long idx, double value)
    {
        if (idx >= 0 && idx < size())
            m_matchers[idx].set_target(value);
    }
    
    // Matching
    
    void set_audio_style(bool style) { m_audio_style = style; }

    long match(const entries::read_pointer& database, double ratio_matched, long max_matches, bool must_sort) const;
    
    // Results
    
    long get_num_matches() const            { return m_num_matches; }
    long get_index(long idx) const          { return m_results[idx].m_index; }
    double get_distance(long idx) const     { return sqrt(m_results[idx].m_distance); }
    
private:

    // Implementation
    
    // Results
    
    struct result
    {
        result() {}
        result(long index, double distance) : m_index(index), m_distance(distance) {}
        
        friend bool operator < (const result& a, const result& b) { return a.m_distance < b.m_distance; }
        friend bool operator > (const result& a, const result& b) { return a.m_distance > b.m_distance; }

        long m_index;
        double m_distance;
    };

    // Useful Types
    
    using accessor = entries::accessor;
    using target_set = std::vector<t_custom_atom>;
    using results_set = std::vector<result>;

    // Matcher Class
    
    class matcher
    {
    public:
        
        // Constructor
        
        matcher(test type, long column, double scale = 1.0)
        : m_type(type), m_column(column), m_scale(scale) {}
         
        // Matching
        
        bool match(long idx, const accessor& access, double& sum) const
        {
            switch (m_type)
            {
                case test::match:
                    if (access->get_column_label_mode(m_column))
                        return compare<std::equal_to, t_symbol *>(access.get_data<t_symbol *>(idx, m_column));
                    else
                        return compare<std::equal_to, double>(access.get_data(idx, m_column));
                    
                case test::less:          return compare<std::less>(access.get_data(idx, m_column));
                case test::greater:       return compare<std::greater>(access.get_data(idx, m_column));
                case test::less_eq:       return compare<std::less_equal>(access.get_data(idx, m_column));
                case test::greater_eq:    return compare<std::greater_equal>(access.get_data(idx, m_column));
                case test::diff:          return measure<difference, false>(access.get_data(idx, m_column), sum);
                case test::diff_reject:   return measure<difference, true>(access.get_data(idx, m_column), sum);
                case test::ratio:         return measure<excess_ratio, false>(access.get_data(idx, m_column), sum);
                case test::ratio_reject:  return measure<excess_ratio, true>(access.get_data(idx, m_column), sum);
                    
                default:                  return true;
            }
        }
        
        long match(results_set& results, long size, const accessor& access) const
        {
            switch (m_type)
            {
                case test::match:
                    if (access->get_column_label_mode(m_column))
                        return compare_loop<std::equal_to, t_symbol *>(results, size, access);
                    else
                        return compare_loop<std::equal_to, double>(results, size, access);
                    
                case test::less:          return compare_loop<std::less>(results, size, access);
                case test::greater:       return compare_loop<std::greater>(results, size, access);
                case test::less_eq:       return compare_loop<std::less_equal>(results, size, access);
                case test::greater_eq:    return compare_loop<std::greater_equal>(results, size, access);
                case test::diff:          return measure_loop<difference, false>(results, size, access);
                case test::diff_reject:   return measure_loop<difference, true>(results, size, access);
                case test::ratio:         return measure_loop<excess_ratio, false>(results, size, access);
                case test::ratio_reject:  return measure_loop<excess_ratio, true>(results, size, access);
                    
                default:                  return size;
            }
        }
        
        // Set Targets
        
        void set_target(double value) { m_targets.resize(1, value); }
        void add_target(double value) { m_targets.push_back(value); }
        void add_target(t_symbol *value) { m_targets.push_back(value); }
        
    private:
        
        // Single Matcher Implementation
        
        // Functors
  
        struct difference
        {
            double operator()(double a, double b) { return a - b; }
        };
        
        struct excess_ratio
        {
            double operator()(double a, double b) { return (a > b ? a / b : b / a) - 1.0; }
        };
        
        template <template <typename U> class Op, typename T>
        struct comparison
        {
            inline bool operator()(const T& value, const T& target, double scale, double sum)
            {
                return Op<T>()(value, target);
            }
         
            inline bool operator()(const T& value, const target_set targets, double scale, double sum) const
            {
                for (auto it = targets.cbegin(); it != targets.cend(); it++)
                    if (Op<T>()(value, it->as<T>()))
                        return true;
            
                return false;
            }
        };
        
        template <class Op, bool Reject>
        struct measurement
        {
            static double square(const double x) { return x * x; }
            
            inline bool operator()(const double& value, const double& target, double scale, double& sum)
            {
                double distance = square(Op()(value, target) * scale);

                sum += distance;
                
                return (!Reject || distance <= 1.0);
            }
            
            inline bool operator()(const double& value, const target_set targets, double scale, double& sum) const
            {
                double distance = std::numeric_limits<double>::infinity();

                for (auto it = targets.cbegin(); it != targets.cend(); it++)
                    distance = std::min(distance, square(Op()(value, it->as<double>()) * scale));
                
                sum += distance;
                
                return !Reject || distance <= 1.0;
            }
        };
        
        // Loop Over Database
        
        template <typename T, typename Op>
        inline long loop(results_set& results, long size, const accessor& access) const
        {
            auto data = [&](long i) { return access.get_data<T>(results[i].m_index, m_column); };
            
            long matched = 0;
            
            if (m_targets.size() == 1)
            {
                T target = m_targets[0].as<T>();
                
                for (long i = 0; i < size; i++)
                    if (Op()(data(i), target, m_scale, results[i].m_distance))
                        results[matched++] = results[i];
            }
            else
            {
                for (long i = 0; i < size; i++)
                    if (Op()(data(i), m_targets, m_scale, results[i].m_distance))
                        results[matched++] = results[i];
            }
            
            return matched;
        }
        
        // Single and Looped Function Calls
        
        template <template <typename U> class Op, typename T = double>
        bool compare(const T& value) const
        {
            return comparison<Op, T>()(value, m_targets, m_scale, 0.0);
        }
    
        template <typename Op, bool Reject>
        bool measure(const double& value, double& distance_sum) const
        {
            return measurement<Op, Reject>()(value, m_targets, m_scale, distance_sum);
        }
    
        template <template <typename U> class Op, typename T = double>
        long compare_loop(results_set& results, long size, const accessor& access) const
        {
            return loop<T, comparison<Op, T>>(results, size, access);
        }
        
        template <typename Op, bool Reject>
        long measure_loop(results_set& results, long size, const accessor& access) const
        {
            return loop<double, measurement<Op, Reject>>(results, size, access);
        }
    
        // Matching Data
        
        test m_type;
        long m_column;
        target_set m_targets;
        double m_scale;
    };
    
    // Data
            
    mutable long m_num_matches;
    mutable results_set m_results;
    
    std::vector<matcher> m_matchers;
    
    bool m_audio_style;
};

#endif
