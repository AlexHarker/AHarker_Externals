
#include <algorithm>
#include <functional>

#include "matchers.hpp"
#include "sort.hpp"

// Test Symbols

t_symbol *ps_match = gensym("match");
t_symbol *ps_less = gensym("less");
t_symbol *ps_greater = gensym("greater");
t_symbol *ps_lesseq = gensym("lesseq");
t_symbol *ps_greatereq = gensym("greatereq");
t_symbol *ps_distance = gensym("distance");
t_symbol *ps_scale = gensym("scale");
t_symbol *ps_within = gensym("within");
t_symbol *ps_distance_ratio = gensym("distance_ratio");
t_symbol *ps_scale_ratio = gensym("scale_ratio");
t_symbol *ps_within_ratio = gensym("within_ratio");

t_symbol *ps_sym_match = gensym("==");
t_symbol *ps_sym_less = gensym("<");
t_symbol *ps_sym_greater = gensym(">");
t_symbol *ps_sym_lesseq = gensym("<=");
t_symbol *ps_sym_greatereq = gensym(">=");
t_symbol *ps_sym_distance = gensym("-");
t_symbol *ps_sym_scale = gensym("%");
t_symbol *ps_sym_within = gensym("<->");
t_symbol *ps_sym_distance_ratio = gensym("/");
t_symbol *ps_sym_scale_ratio = gensym("%%");
t_symbol *ps_sym_within_ratio = gensym("</>");

// Attempts to match an atom with any of the symbols representing a valid test type

matchers::test matchers::test_type(t_atom *argv)
{
    if (atom_gettype(argv) != A_SYM)
        return test::none;
    
    t_symbol *s = atom_getsym(argv);
    
    if (s == ps_match           || s == ps_sym_match)           return test::match;
    if (s == ps_less            || s == ps_sym_less)            return test::less;
    if (s == ps_greater         || s == ps_sym_greater)         return test::greater;
    if (s == ps_lesseq          || s == ps_sym_lesseq)          return test::less_eq;
    if (s == ps_greatereq       || s == ps_sym_greatereq)       return test::greater_eq;
    if (s == ps_distance        || s == ps_sym_distance)        return test::diff;
    if (s == ps_scale           || s == ps_sym_scale)           return test::diff;
    if (s == ps_within          || s == ps_sym_within)          return test::diff_reject;
    if (s == ps_distance_ratio  || s == ps_sym_distance_ratio)  return test::ratio;
    if (s == ps_scale_ratio     || s == ps_sym_scale_ratio)     return test::ratio;
    if (s == ps_within_ratio    || s == ps_sym_within_ratio)    return test::ratio_reject;
    
    return test::none;
}

bool matchers::needs_scale(t_atom *argv)
{
    if (atom_gettype(argv) != A_SYM)
        return false;
    
    t_symbol *s = atom_getsym(argv);
    
    if (s == ps_scale           || s == ps_sym_scale)           return true;
    if (s == ps_within          || s == ps_sym_within)          return true;
    if (s == ps_scale_ratio     || s == ps_sym_scale_ratio)     return true;
    if (s == ps_within_ratio    || s == ps_sym_within_ratio)    return true;
    
    return false;
}

long matchers::match(const accessor& database, double ratio_matched, long max_matches, bool must_sort) const
{
    long num_items = database.num_items();
    m_num_matches = 0;
    
    m_results.resize(num_items);
        
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
            m_num_matches = it->match(m_results, m_num_matches, database);
    }
    else
    {
        // Assume a match for each entry (for the case of no matchers)

        for (long i = 0; i < num_items; i++)
        {
            bool matched = true;
            double sum = 0.0;
            
            for (auto it = m_matchers.cbegin(); it != m_matchers.cend() && matched; it++)
                matched = it->match(i, database, sum);
            
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
        if (num_matches < (database.num_items() / 8))
        {
            num_matches = std::min(num_matches, m_num_matches);
            
            // Partial insertion sort (faster sorting for small numbers of n)...
            
            for (long i = 0; i < num_matches; i++)
                std::swap(m_results[i], (*std::min_element(m_results.begin() + i, m_results.begin() + m_num_matches)));
           
        }
        else
            sort(m_results, m_num_matches);
    }
    
    return m_num_matches = num_matches;
}

void matchers::set_matchers(void *x, long argc, t_atom *argv, const accessor& database)
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
        
        long column = database.column_from_specifier(argv++);
        bool get_scale = needs_scale(argv);
        test type = test_type(argv++);
        argc -= 2;
        
        // Test for issues
        
        if (type == test::none)
        {
            object_error((t_object *) x, "invalid test / no test specified in unparsed segment of matchers message");
            break;
        }
        else if (column < 0 || column >= database.num_columns())
        {
            object_error((t_object *) x, "specified column in matchers message does not exist");
            continue;
        }
        else if (database.get_column_label_mode(column) && type != test::match)
        {
            object_error((t_object *) x, "incorrect matcher for label type column (should be equals or ==)  column number %ld", column + 1);
            continue;
        }
        
        bool has_target = false;
        
        // Parse values
        
        if (database.get_column_label_mode(column))
        {
            // If this column is for labels store details of a valid match test (other tests are not valid)
            
            add_matcher(test::match, column);
            
            for ( ; argc; argc--, argv++)
            {
                if (argc > 1 && test_type(argv + 1) != test::none)
                    break;
                add_target(atom_getsym(argv));
                has_target = true;
            }
        }
        else
        {
            // If this column is for values store the parameters for any valid test
            
            double scale = 1.0;
            
            if (argc && get_scale && (type == test::diff || type == test::diff_reject))
            {
                scale = fabs(1.0 / atom_getfloat(argv++));
                argc--;
            }
            else if (argc && get_scale && (type == test::ratio || type == test::ratio_reject))
            {
                scale = fabs(atom_getfloat(argv++));
                scale = (scale < 1.0) ? 1.0 / scale : scale;
                scale = 1.0 / (scale - 1.0);
                argc--;
            }
            
            add_matcher(type, column, scale);
            
            for ( ; argc; argc--, argv++)
            {
                if (argc > 1 && test_type(argv + 1) != test::none)
                    break;
                add_target(atom_getfloat(argv));
                has_target = true;
            }
        }
        
        if (!has_target)
            object_error((t_object *) x, "no target values given for specified test in matchers message");
    }
}
