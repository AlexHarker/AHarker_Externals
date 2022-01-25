
#include <algorithm>
#include <functional>

#include "matchers.hpp"
#include "sort.hpp"

// Test Symbols

t_symbol *ps_distance = gensym("distance");
t_symbol *ps_match = gensym("match");
t_symbol *ps_scale = gensym("scale");
t_symbol *ps_within = gensym("within");
t_symbol *ps_less = gensym("less");
t_symbol *ps_greater = gensym("greater");
t_symbol *ps_lesseq = gensym("lesseq");
t_symbol *ps_greatereq = gensym("greatereq");
t_symbol *ps_distance_ratio = gensym("distance_ratio");
t_symbol *ps_scale_ratio = gensym("scale_ratio");
t_symbol *ps_within_ratio = gensym("within_ratio");

t_symbol *ps_sym_distance = gensym("-");
t_symbol *ps_sym_match = gensym("==");
t_symbol *ps_sym_scale = gensym("%");
t_symbol *ps_sym_within = gensym("<->");
t_symbol *ps_sym_less = gensym("<");
t_symbol *ps_sym_greater = gensym(">");
t_symbol *ps_sym_lesseq = gensym("<=");
t_symbol *ps_sym_greatereq = gensym(">=");
t_symbol *ps_sym_distance_ratio = gensym("/");
t_symbol *ps_sym_scale_ratio = gensym("%%");
t_symbol *ps_sym_within_ratio = gensym("</>");

// Attempts to match an atom with any of the symbols representing a valid test type

TestType test_type(t_atom *argv)
{
    if (atom_gettype(argv) != A_SYM)
        return TEST_NONE;
    
    if (atom_getsym(argv) == ps_match || atom_getsym(argv) == ps_sym_match) return TEST_MATCH;
    if (atom_getsym(argv) == ps_distance || atom_getsym(argv) == ps_sym_distance) return TEST_DISTANCE;
    if (atom_getsym(argv) == ps_less || atom_getsym(argv) == ps_sym_less) return TEST_LESS_THAN;
    if (atom_getsym(argv) == ps_greater || atom_getsym(argv) == ps_sym_greater) return TEST_GREATER_THAN;
    if (atom_getsym(argv) == ps_lesseq || atom_getsym(argv) == ps_sym_lesseq) return TEST_LESS_THAN_EQ;
    if (atom_getsym(argv) == ps_greatereq || atom_getsym(argv) == ps_sym_greatereq) return TEST_GREATER_THAN_EQ;
    if (atom_getsym(argv) == ps_scale || atom_getsym(argv) == ps_sym_scale) return TEST_SCALE;
    if (atom_getsym(argv) == ps_within || atom_getsym(argv) == ps_sym_within) return TEST_WITHIN;
    if (atom_getsym(argv) == ps_distance_ratio || atom_getsym(argv) == ps_sym_distance_ratio) return TEST_DISTANCE_RATIO;
    if (atom_getsym(argv) == ps_scale_ratio || atom_getsym(argv) == ps_sym_scale_ratio) return TEST_SCALE_RATIO;
    if (atom_getsym(argv) == ps_within_ratio || atom_getsym(argv) == ps_sym_within_ratio) return TEST_WITHIN_RATIO;
    
    return TEST_NONE;
}

long matchers::match(const entries::read_pointer& database, double ratio_matched, long max_matches, bool must_sort) const
{
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
        for (auto it = m_matchers.cbegin(); it != m_matchers.cend() && m_num_matches; it++)
            m_num_matches = it->match(m_results, m_num_matches, access);
    }
    else
    {
        // Assume a match for each entry (for the case of no matchers)

        for (long i = 0; i < num_items; i++)
        {
            bool matched = true;
            double sum = 0.0;
            
            for (auto it = m_matchers.cbegin(); it != m_matchers.cend() && matched; it++)
                matched = it->match(i, access, sum);
            
            // Store the entry if it is a valid match
            
            if (matched)
                m_results[m_num_matches++] = result(i, sum);
        }
    }
    
    ratio_matched = std::min(std::max(ratio_matched, 0.0), 1.0);
    max_matches = std::max(max_matches, 0L);
    long num_matches = static_cast<long>(round(m_num_matches * ratio_matched));
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
        TestType type = test_type(argv++);
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
                if (argc > 1 && test_type(argv + 1) != TEST_NONE)
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
                if (argc > 1 && test_type(argv + 1) != TEST_NONE)
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
        std::swap(m_results[i], (*std::min_element(m_results.begin() + i, m_results.begin() + size)));
    
    return N;
}

void matchers::clear()
{
    m_matchers.clear();
}

void matchers::add_target(double value)
{
    if (m_matchers.size())
        m_matchers.back().m_targets.push_back(value);
}

void matchers::add_target(t_symbol *value)
{
    if (m_matchers.size())
        m_matchers.back().m_targets.push_back(value);
}

void matchers::add_matcher(e_test type, long column, double scale)
{
    m_matchers.push_back(matcher(type, column, scale));
}
