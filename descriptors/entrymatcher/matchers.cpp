
#include <algorithm>
#include <functional>

#include "matchers.hpp"
#include "sort.hpp"

long matchers::match(const entries::read_pointer& database, double ratio_matched, long max_matches, bool must_sort) const
{
    struct distance { double operator()(double a, double b, double scale) { return (a - b) * scale; } };
    struct ratio { double operator()(double a, double b, double scale) { return (((a > b) ? a / b : b / a) - 1.0) * scale; }};
    
    long num_items = database->num_items();
    m_num_matches = 0;
    
    m_results.resize(num_items);
    
    const auto access = database->get_accessor();
    
    if (!size() || m_audio_style)
    {
        for (long i = 0; i < num_items; i++)
            m_results[i] = result(i, 0.0);
        
        m_num_matches = num_items;
    }

    if (!size())
        return num_items;
    
    if (m_audio_style)
    {
        for (auto it = m_matchers.cbegin(); it != m_matchers.cend(); it++)
        {
           if (!m_num_matches)
               break;
            
            switch (it->m_type)
            {
                case kTestMatch:
                    if (database->get_column_label_mode(it->m_column))
                        m_num_matches = it->comparison_test<std::equal_to, t_symbol *>(m_results, m_num_matches, access);
                    else
                        m_num_matches = it->comparison_test<std::equal_to, double>(m_results, m_num_matches, access);
                    break;
                    
                case kTestLess:             m_num_matches = it->comparison_test<std::less>(m_results, m_num_matches, access);             break;
                case kTestGreater:          m_num_matches = it->comparison_test<std::greater>(m_results, m_num_matches, access);          break;
                case kTestLessEqual:        m_num_matches = it->comparison_test<std::less_equal>(m_results, m_num_matches, access);       break;
                case kTestGreaterEqual:     m_num_matches = it->comparison_test<std::greater_equal>(m_results, m_num_matches, access);    break;
                case kTestDistance:         m_num_matches = it->distance_test(false, m_results, m_num_matches, access, distance());       break;
                case kTestDistanceReject:   m_num_matches = it->distance_test(true, m_results, m_num_matches, access, distance());        break;
                case kTestRatio:            m_num_matches = it->distance_test(false, m_results, m_num_matches, access, ratio());          break;
                case kTestRatioReject:      m_num_matches = it->distance_test(true, m_results, m_num_matches, access, ratio());           break;
            }
        }
    }
    else
    {
        for (long i = 0; i < num_items; i++)
        {
            // Assume a match for each entry (for the case of no matchers)
            
            bool matched = true;
            double distance_squared = 0.0;
            
            for (auto it = m_matchers.cbegin(); it != m_matchers.cend(); it++)
            {
                switch (it->m_type)
                {
                    case kTestMatch:
                        if (database->get_column_label_mode(it->m_column))
                            matched = it->comparison_test<std::equal_to, t_symbol *>(access.get_data<t_symbol *>(i, it->m_column));
                        else
                            matched = it->comparison_test<std::equal_to, double>(access.get_data(i, it->m_column));
                        break;
                        
                    case kTestLess:             matched = it->comparison_test<std::less>(access.get_data(i, it->m_column)); break;
                    case kTestGreater:          matched = it->comparison_test<std::greater>(access.get_data(i, it->m_column)); break;
                    case kTestLessEqual:        matched = it->comparison_test<std::less_equal>(access.get_data(i, it->m_column)); break;
                    case kTestGreaterEqual:     matched = it->comparison_test<std::greater_equal>(access.get_data(i, it->m_column)); break;
                    case kTestDistance:         matched = it->distance_test(false, access.get_data(i, it->m_column), distance_squared, distance()); break;
                    case kTestDistanceReject:   matched = it->distance_test(true, access.get_data(i, it->m_column), distance_squared, distance()); break;
                    case kTestRatio:            matched = it->distance_test(false, access.get_data(i, it->m_column), distance_squared, ratio()); break;
                    case kTestRatioReject:      matched = it->distance_test(true, access.get_data(i, it->m_column), distance_squared, ratio()); break;
                }

                if (!matched)
                    break;
            }
            
            // Store the entry if it is a valid match
            
            if (matched)
                m_results[m_num_matches++] = result(i, distance_squared);
        }
    }
    
    ratio_matched = std::min(std::max(ratio_matched, 0.0), 1.0);
    max_matches = std::max(max_matches, 0L);
    long num_matches = round(m_num_matches * ratio_matched);
    num_matches = (max_matches && m_num_matches > max_matches) ? max_matches : m_num_matches;

    // FIX - better heuristics and more info on what has been sorted...
    
    if (size() && (must_sort || num_matches < m_num_matches))
    {
        if (num_matches < (database->num_items() / 8))
            num_matches = sort_top_n(num_matches, m_num_matches);
        else
            sort(m_results, m_num_matches);
    }
    
    return m_num_matches = num_matches;
}

void matchers::set_matchers(void *x, long argc, t_atom *argv, const entries::read_pointer& database)
{
    // Empty the matchers
    
    clear();
    
    // Loop over arguments
    
    while (argc)
    {
        if (argc < 2)
        {
            object_error((t_object *) x, "insufficient items in matchers message for unparsed test");
            break;
        }
        
        // Get the column and test type
        
        long column = database->column_from_specifier(argv++);
        ::TestType type = entrymatcher_test_types(argv++);
        argc -= 2;
        
        // Test for issues
        
        if (type == TEST_NONE)
        {
            object_error((t_object *) x, "invalid test / no test specified in unparsed segment of matchers message");
            break;
        }
        else if (column < 0 || column >= database->num_columns())
        {
            object_error((t_object *) x, "specified column in matchers message does not exist");
            continue;
        }
        else if (database->get_column_label_mode(column) && type != TEST_MATCH)
        {
            object_error((t_object *) x, "incorrect matcher for label type column (should be equals or ==)  column number %ld", column + 1);
            continue;
        }
        
        bool has_target = false;
        
        // Parse values
        
        if (database->get_column_label_mode(column))
        {
            // If this column is for labels store details of a valid match test (other tests are not valid)
            
            add_matcher(matchers::kTestMatch, column);
            
            for ( ; argc; argc--, argv++)
            {
                if (argc > 1 && entrymatcher_test_types(argv + 1) != TEST_NONE)
                    break;
                add_target(atom_getsym(argv));
                has_target = true;
            }
        }
        else
        {
            // If this column is for values store the parameters for any valid test
            
            double scale = 1.0;
            
            if (argc && (type == TEST_SCALE || type == TEST_WITHIN))
            {
                scale = fabs(1.0 / atom_getfloat(argv++));
                argc--;
            }
            else if (argc && (type == TEST_SCALE_RATIO || type == TEST_WITHIN_RATIO))
            {
                scale = fabs(atom_getfloat(argv++));
                scale = (scale < 1.0) ? 1.0 / scale : scale;
                scale = 1.0 / (scale - 1.0);
                argc--;
            }
            
            switch (type)
            {
                case TEST_NONE:                 break;
                case TEST_MATCH:                add_matcher(matchers::kTestMatch, column);                      break;
                case TEST_LESS_THAN:            add_matcher(matchers::kTestLess, column);                       break;
                case TEST_GREATER_THAN:         add_matcher(matchers::kTestGreater, column);                    break;
                case TEST_LESS_THAN_EQ:         add_matcher(matchers::kTestLessEqual, column);                  break;
                case TEST_GREATER_THAN_EQ:      add_matcher(matchers::kTestGreaterEqual, column);               break;
                case TEST_DISTANCE:             add_matcher(matchers::kTestDistance, column);                   break;
                case TEST_SCALE:                add_matcher(matchers::kTestDistance, column, scale);            break;
                case TEST_WITHIN:               add_matcher(matchers::kTestDistanceReject, column, scale);      break;
                case TEST_DISTANCE_RATIO:       add_matcher(matchers::kTestRatio, column);                      break;
                case TEST_SCALE_RATIO:          add_matcher(matchers::kTestRatio, column, scale);               break;
                case TEST_WITHIN_RATIO:         add_matcher(matchers::kTestRatioReject, column, scale);         break;
            }
            
            for ( ; argc; argc--, argv++)
            {
                if (argc > 1 && entrymatcher_test_types(argv + 1) != TEST_NONE)
                    break;
                add_target(atom_getfloat(argv));
                has_target = true;
            }
        }
        
        if (!has_target)
            object_error((t_object *) x, "no target values given for specified test in matchers message");
    }
}

long matchers::sort_top_n(long N, long size) const
{
    N = std::min(N, size);
    
    // Partial insertion sort (faster sorting for small numbers of n)...
    
    for (long i = 0; i < N; i++)
    {
        double min_distance = m_results[i].m_distance;
        long swap = i;
        
        for (long j = i + 1; j < size; j++)
        {
            if (m_results[j].m_distance < min_distance)
            {
                min_distance = m_results[j].m_distance;
                swap = j;
            }
        }
        
        std::swap(m_results[swap], m_results[i]);
    }
    
    return N;
}

void matchers::clear()
{
    m_matchers.clear();
}

void matchers::add_target(double value)
{
    if (m_matchers.size())
        m_matchers.back().m_values.push_back(value);
}

void matchers::add_target(t_symbol *value)
{
    if (m_matchers.size())
        m_matchers.back().m_values.push_back(value);
}

void matchers::add_matcher(TestType type, long column, double scale)
{
    m_matchers.push_back(matcher(type, column, scale));
}
