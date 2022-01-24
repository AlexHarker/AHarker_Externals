
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

TestType test_type(t_atom *argv);



class matchers
{
public:
    
    enum TestType { kTestMatch, kTestLess, kTestGreater, kTestLessEqual, kTestGreaterEqual, kTestDistance, kTestRatio, kTestDistanceReject, kTestRatioReject };
    
private:
    
    struct result
    {
        result() {}
        result(long index, double distance) : m_index(index), m_distance(distance) {}
        
        friend bool operator > (const result& a, const result& b) { return a.m_distance > b.m_distance; }
        
        long m_index;
        double m_distance;
    };
    
    struct matcher
    {
        matcher(TestType type, long column, double scale = 1.0)
        : m_type(type), m_column(column), m_scale(scale) {}

        template <template <typename U> class Op, typename T = double>
        inline bool comparison_test(const T& value) const
        {
            for (auto it = m_values.cbegin(); it != m_values.cend(); it++)
                if (Op<T>()(value, it->as<T>()))
                    return true;
            
            return false;
        }
        
        template <typename Op>
        inline bool distance_test(bool reject, const double value, double& overall_distance, Op op) const
        {
            double distance = HUGE_VAL;

            for (auto it = m_values.cbegin(); it != m_values.cend(); it++)
            {
                double current_distance = op(value, it->as<double>(), m_scale);
                current_distance *= current_distance;
                if (current_distance < distance)
                    distance = current_distance;
            }
            
            overall_distance += distance;
            return !reject || distance <= 1.0;
        }
        
        template <template <typename U> class Op, typename T = double>
        inline long comparison_test(std::vector<result>& results, long num_matches, const entries::accessor& access) const
        {
            long matched = 0;
            
            if (m_values.size() == 1)
            {
                T comparison_value = m_values[0].as<T>();
                
                for (long i = 0; i < num_matches; i++)
                {
                    long idx = results[i].m_index;
                    
                    if (Op<T>()(access.get_data<T>(idx, m_column), comparison_value))
                        results[matched++] = result(idx, results[i].m_distance);
                }
            }
            else
            {
                for (long i = 0; i < num_matches; i++)
                {
                    long idx = results[i].m_index;
                    T value = access.get_data<T>(idx, m_column);
                    
                    for (auto it = m_values.cbegin(); it != m_values.cend(); it++)
                    {
                        if (Op<T>()(value, it->as<T>()))
                        {
                            results[matched++] = result(idx, results[i].m_distance);
                            break;
                        }
                    }
                }
            }
            
            return matched;
        }
        
        template <typename Op>
        inline long distance_test(bool reject, std::vector<result>& results, long num_matches, const entries::accessor& access, Op op) const
        {
            long matched = 0;
            
            if (m_values.size() == 1)
            {
                double comparison_value = m_values[0].as<double>();
                
                for (long i = 0; i < num_matches; i++)
                {
                    long idx = results[i].m_index;
                    double distance = op(comparison_value, access.get_data(idx, m_column), m_scale);
                    distance *= distance;
                    
                    if (!reject || distance <= 1.0)
                        results[matched++] = result(idx, results[i].m_distance + distance);
                }
            }
            else
            {
                for (long i = 0; i < num_matches; i++)
                {
                    long idx = results[i].m_index;
                    double value = access.get_data(idx, m_column);
                    double distance = HUGE_VAL;
                    
                    for (auto it = m_values.cbegin(); it != m_values.cend(); it++)
                    {
                        double current_distance = op(it->as<double>(), value, m_scale);
                        current_distance *= current_distance;
                        if (current_distance < distance)
                            distance = current_distance;
                    }
                    
                    if (!reject || distance <= 1.0)
                        results[matched++] = result(idx, results[i].m_distance + distance);
                }
            }
            
            return matched;
        }
        
        TestType m_type;
        long m_column;
        std::vector<t_custom_atom> m_values;
        double m_scale;
    };
    
public:
    
    matchers() : m_num_matches(0), m_audio_style(false) {}
    
    long match(const entries::read_pointer& database, double ratio_matched = 1.0, long max_matches = 0, bool must_sort = true) const;
    
    size_t size() const { return m_matchers.size(); }
    
    void clear();
    
    void set_target(long idx, double value)
    {
        if (idx >= 0 && idx < size())
        {
            m_matchers[idx].m_values.resize(1);
            m_matchers[idx].m_values[0] = value;
        }
    }
    
    long get_num_matches() const            { return m_num_matches; }
    long get_index(long idx) const          { return m_results[idx].m_index; }
    double get_distance(long idx) const     { return sqrt(m_results[idx].m_distance); }
    
    void add_target(double value);
    void add_target(t_symbol *value);
    void add_matcher(TestType type, long column, double scale = 1.0);
    
    void set_matchers(void *x, long argc, t_atom *argv, const entries::read_pointer& database);
    void set_audio_style(bool style) { m_audio_style = style; }
    
private:
    
    long sort_top_n(long N, long size) const;
    
    mutable long m_num_matches;
    
    mutable std::vector<result> m_results;
    
    std::vector<matcher> m_matchers;
    
    bool m_audio_style;
};

#endif
