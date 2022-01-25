
#ifndef MATCHERS_HPP
#define MATCHERS_HPP

#include "atom_types.hpp"
#include "entry_database.hpp"

enum TestType
{
    TEST_NONE,
    TEST_MATCH,
    TEST_LESS_THAN,
    TEST_GREATER_THAN,
    TEST_LESS_THAN_EQ,
    TEST_GREATER_THAN_EQ,
    TEST_DISTANCE,
    TEST_SCALE,
    TEST_WITHIN,
    TEST_DISTANCE_RATIO,
    TEST_SCALE_RATIO,
    TEST_WITHIN_RATIO
};

class matchers
{
public:
    
    enum e_test { kTestMatch, kTestLess, kTestGreater, kTestLessEqual, kTestGreaterEqual, kTestDistance, kTestRatio, kTestDistanceReject, kTestRatioReject };
    
private:
        
    struct result
    {
        result() {}
        result(long index, double distance) : m_index(index), m_distance(distance) {}
        
        friend bool operator < (const result& a, const result& b) { return a.m_distance < b.m_distance; }
        friend bool operator > (const result& a, const result& b) { return a.m_distance > b.m_distance; }

        long m_index;
        double m_distance;
    };

    using accessor = entries::accessor;
    using results_set = std::vector<result>;
    using target_set = std::vector<t_custom_atom>;

    struct matcher
    {
        matcher(e_test type, long column, double scale = 1.0)
        : m_type(type), m_column(column), m_scale(scale) {}
                
        struct distance { double operator()(double a, double b) { return a - b; }};
        struct ratio { double operator()(double a, double b) { return (a > b ? a / b : b / a) - 1.0; }};
        
        template <template <typename U> class Op, typename T>
        struct compare
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
        struct interval
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
                double distance = HUGE_VAL;

                for (auto it = targets.cbegin(); it != targets.cend(); it++)
                    distance = std::min(distance, square(Op()(value, it->as<double>()) * scale));
                
                sum += distance;
                
                return !Reject || distance <= 1.0;
            }
        };
                
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
        
        template <template <typename U> class Op, typename T = double>
        bool comparison(const T& value) const
        {
            return compare<Op, T>()(value, m_targets, m_scale, 0.0);
        }
    
        template <template <typename U> class Op, typename T = double>
        long comparison_loop(results_set& results, long size, const accessor& access) const
        {
            return loop<T, compare<Op, T>>(results, size, access);
        }
        
        template <typename Op, bool Reject>
        bool measure(const double& value, double& distance_sum) const
        {
            return interval<Op, Reject>()(value, m_targets, m_scale, distance_sum);
        }
        
        template <typename Op, bool Reject>
        long measure_loop(results_set& results, long size, const accessor& access) const
        {
            return loop<double, interval<Op, Reject>>(results, size, access);
        }
    
        bool match(long idx, const accessor& access, double& sum) const
        {
            switch (m_type)
            {
                case kTestMatch:
                    if (access->get_column_label_mode(m_column))
                        return comparison<std::equal_to, t_symbol *>(access.get_data<t_symbol *>(idx, m_column));
                    else
                        return comparison<std::equal_to, double>(access.get_data(idx, m_column));
                    
                case kTestLess:             return comparison<std::less>(access.get_data(idx, m_column));
                case kTestGreater:          return comparison<std::greater>(access.get_data(idx, m_column));
                case kTestLessEqual:        return comparison<std::less_equal>(access.get_data(idx, m_column));
                case kTestGreaterEqual:     return comparison<std::greater_equal>(access.get_data(idx, m_column));
                case kTestDistance:         return measure<distance, false>(access.get_data(idx, m_column), sum);
                case kTestDistanceReject:   return measure<distance, true>(access.get_data(idx, m_column), sum);
                case kTestRatio:            return measure<ratio, false>(access.get_data(idx, m_column), sum);
                case kTestRatioReject:      return measure<ratio, true>(access.get_data(idx, m_column), sum);
            }
        }
        
        long match(results_set& results, long size, const accessor& access) const
        {
            switch (m_type)
            {
                case kTestMatch:
                    if (access->get_column_label_mode(m_column))
                        return comparison_loop<std::equal_to, t_symbol *>(results, size, access);
                    else
                        return comparison_loop<std::equal_to, double>(results, size, access);
                    
                case kTestLess:             return comparison_loop<std::less>(results, size, access);
                case kTestGreater:          return comparison_loop<std::greater>(results, size, access);
                case kTestLessEqual:        return comparison_loop<std::less_equal>(results, size, access);
                case kTestGreaterEqual:     return comparison_loop<std::greater_equal>(results, size, access);
                case kTestDistance:         return measure_loop<distance, false>(results, size, access);
                case kTestDistanceReject:   return measure_loop<distance, true>(results, size, access);
                case kTestRatio:            return measure_loop<ratio, false>(results, size, access);
                case kTestRatioReject:      return measure_loop<ratio, true>(results, size, access);
            }
        }
        
        e_test m_type;
        long m_column;
        target_set m_targets;
        double m_scale;
    };
    
    using matcher_set = std::vector<struct matcher>;
    
public:
    
    matchers() : m_num_matches(0), m_audio_style(false) {}
    
    long match(const entries::read_pointer& database, double ratio_matched = 1.0, long max_matches = 0, bool must_sort = true) const;
    
    size_t size() const { return m_matchers.size(); }
    
    void clear()
    {
        m_matchers.clear();
    }
    
    void set_target(long idx, double value)
    {
        if (idx >= 0 && idx < size())
        {
            m_matchers[idx].m_targets.resize(1);
            m_matchers[idx].m_targets[0] = value;
        }
    }
    
    long get_num_matches() const            { return m_num_matches; }
    long get_index(long idx) const          { return m_results[idx].m_index; }
    double get_distance(long idx) const     { return sqrt(m_results[idx].m_distance); }
    
    void add_target(double value)
    {
        if (m_matchers.size())
            m_matchers.back().m_targets.push_back(value);
    }

    void add_target(t_symbol *value)
    {
        if (m_matchers.size())
            m_matchers.back().m_targets.push_back(value);
    }

    void add_matcher(e_test type, long column, double scale = 1.0)
    {
        m_matchers.push_back(matcher(type, column, scale));
    }
    
    void set_matchers(void *x, long argc, t_atom *argv, const entries::read_pointer& database);
    void set_audio_style(bool style) { m_audio_style = style; }
    
    static TestType test_type(t_atom *argv);

private:
        
    mutable long m_num_matches;
    
    mutable results_set m_results;
    
    matcher_set m_matchers;
    
    bool m_audio_style;
};

#endif
