
#include <algorithm>
#include <functional>
#include <cmath>
#include <limits>

#include "entries.hpp"
#include "matchers.hpp"
#include "sort.hpp"

t_symbol *ps_mean = gensym("mean");
t_symbol *ps_min = gensym("min");
t_symbol *ps_minimum = gensym("minimum");
t_symbol *ps_max = gensym("max");
t_symbol *ps_maximum = gensym("maximum");
t_symbol *ps_median = gensym("median");
t_symbol *ps_stddev = gensym("stddev");
t_symbol *ps_deviation = gensym("deviation");
t_symbol *ps_standard_dev = gensym("standard_dev");
t_symbol *ps_standard_deviation= gensym("standard_deviation");
t_symbol *ps_centile = gensym("centile");
t_symbol *ps_percentile = gensym("percentile");

t_symbol *ps_identfier = gensym("identifier");

// Column Setup

// Set Column Label Modes

void entries::set_column_label_modes(void *x, long argc, t_atom *argv)
{
    bool label_modes_changed = false;
    
    if (argc > num_columns())
    {
        object_error((t_object *) x, "more label modes than columns");
        argc = num_columns();
    }
    
    for (long i = 0; i < argc; i++)
    {
        bool label = atom_getlong(argv++) ? true : false;
        label_modes_changed |= label != m_columns[i].m_label;
        m_columns[i].m_label = label;
    }
    
    if (label_modes_changed)
    {
        if (num_items())
            object_error((t_object *) x, "data cleared due to label mode changes");
        clear();
    }
}

// Set Column Names

void entries::set_column_names(void *x, long argc, t_atom *argv)
{
    if (argc > num_columns())
    {
        object_error((t_object *) x, "more names than columns");
        argc = num_columns();
    }
        
    for (long i = 0; i < argc; i++)
        m_columns[i].m_name = atom_getsym(argv++);
}

// Find Entries / Columns

// Search Identifiers

entries::position_info entries::search_identifiers(const t_atom *identifier_atom) const
{
    using ordering = t_custom_atom::ordering;
    
    t_custom_atom identifier(identifier_atom, false);
    
    long idx = num_items() / 2;
    long gap = std::max(1L, idx);
    
    while (gap && idx < num_items())
    {
        gap = std::max(1L, gap / 2);
        
        switch (order(identifier, get_entry_identifier(m_order[idx])))
        {
            case ordering::equal:
                return { m_order[idx], idx };
                
            case ordering::higher:
                idx += gap;
                break;
                
            case ordering::lower:
                if (gap == 1 && (!idx || order(identifier, get_entry_identifier(m_order[idx - 1])) == ordering::higher))
                    gap = 0;
                else
                    idx -= gap;
                break;
        }
    }
    
    return { -1, idx };
}

// Get a Column Index (for a specifier)

long entries::get_column_index(const t_atom *specifier) const
{
    if (atom_gettype(specifier) != A_SYM)
    {
        t_atom_long column = atom_getlong(specifier) - 1;
        return (column >= -1 && column < num_columns()) ? static_cast<long>(column) : -2;
    }
    
    t_symbol *column_name = atom_getsym(specifier);
    
    if (column_name == ps_identfier)
        return -1;
    
    for (long i = 0; i < num_columns(); i++)
        if (column_name == m_columns[i].m_name)
            return i;
    
    return -2;
}

// Reserve / Clear

// Reserve

void entries::reserve(long items)
{
    m_identifiers.reserve(items);
    m_order.reserve(items);
    m_entries.reserve(items * num_columns());
    m_types.reserve(items * num_columns());
}

// Clear

void entries::clear()
{
    m_identifiers.clear();
    m_order.clear();
    m_entries.clear();
    m_types.clear();
}

// Adding / Removing Entries

// Add

void entries::add_entry(void *x, long argc, t_atom *argv)
{
    if (!argc--)
    {
        object_error((t_object *) x, "no arguments for entry");
        return;
    }
    
    if (argc > num_columns())
        object_error((t_object *) x, "more items than columns");
    
    // Get the identifier, order position and find any prexisting entry with this identifier
    
    t_atom *identifier = argv++;
    auto position = search_identifiers(identifier);
    long idx = position.m_index;

    // Make a space for a new entry in the case that this identifier does *not* exist
    
    if (idx < 0)
    {
        idx = num_items();
        m_identifiers.push_back(t_custom_atom(identifier, false));
        m_order.insert(m_order.begin() + position.m_order, idx);
        m_entries.resize((idx + 1) * num_columns());
        m_types.resize((idx + 1) * num_columns());
    }

    // Store data of the correct data type but store null data for any unspecified columns / incorrect types
    
    for (long i = 0; i < num_columns(); i++, argv++)
    {
        t_custom_atom data(argv);
        
        if (i >= argc || m_columns[i].m_label != data.is_symbol())
        {
            if (i < argc)
                object_error((t_object *) x, "incorrect type in entry - column number %ld", i + 1);
            
            set_data(idx, i, m_columns[i].m_label ? t_custom_atom(gensym("")) : t_custom_atom());
        }
        else
            set_data(idx, i, data);
    }
}

// Removal Entry Error

void entry_does_not_exist(void *x, t_atom *argv)
{
    t_custom_atom a(argv, false);
    object_error((t_object *) x, "no entry %s to remove", a.get_string().c_str());
}

// Remove Entries from identifiers (with read pointer - ugraded on next method call to write)

void entries::remove_entries(void *x, long argc, t_atom *argv, read_write_access& access)
{
    if (!argc)
    {
        object_error((t_object *) x, "no identifiers given for remove message");
        return;
    }
    
    if (argc == 1)
    {
        delete_entry(x, argv, access);
        return;
    }
    
    std::vector<long> indices;
    indices.reserve(argc);
        
    for (long i = 0; i < argc; i++)
    {
        long index = get_entry_index(argv + i);
        
        if (index < 0)
            entry_does_not_exist(x, argv + i);
        else
            indices.push_back(index);
    }
    
    if (indices.size())
        delete_entries(indices, access);
}

// Remove Matching Entries (based on a specific set of matchers)

void entries::remove_matched_entries(void *x, long argc, t_atom *argv, read_write_access& access)
{
    if (!argc)
        return;
            
    matchers matching(x, argc, argv, access);
    long num_matches = matching.match(access, 1.0, 0, false);
        
    if (num_matches && matching.size())
    {
        std::vector<long> indices(num_matches);
        
        for (long i = 0; i < num_matches; i++)
            indices[i] = matching.get_index(i);
        
        delete_entries(indices, access);
    }
}

// Helpers for Mass Removal

template <class Iterator>
void move_data(Iterator it, long in_begin, long in_end, long out_idx, long item_size = 1)
{
    std::copy(it + (in_begin * item_size), it + (in_end * item_size), it + (out_idx * item_size));
}

long entries::get_order(long idx)
{
    t_atom identifier;

    m_identifiers[idx].get_atom(&identifier);

    return search_identifiers(&identifier).m_order;
}

// Delete Multiple Entries (from a list of sorted indices (upgrading a read pointer to a write lock))

void entries::delete_entries(std::vector<long>& indices, read_write_access& access)
{
    // Predicates
    
    struct non_consecutive { bool operator()(long &a, const long &b) { return b > a + 1; } };
    struct for_deletion { bool operator()(const long& a) { return a == -1; } };

    // Sort indices
    
    sort(indices, static_cast<long>(indices.size()));
            
    // Setup new order vector
    
    std::vector<long> new_order(num_items());
    long out_idx = indices[0], idx = 0, jdx = 0;

    for (auto it = indices.begin(); it <= indices.end() + 1; it++)
    {
        for ( ; idx < (it < indices.end() ? *it : num_items()); idx++, jdx++)
            new_order[get_order(idx)] = jdx;
        
        it = std::adjacent_find(it, indices.end(), non_consecutive());
       
        // Mark for deletion

        for ( ; idx < (it != indices.end() ? (*it) + 1 : *(it - 1) + 1); idx++)
            new_order[get_order(idx)] = -1;
    }
        
    access.promote();

    // Remove data
    
    for (auto it = indices.begin(); it < indices.end(); )
    {
        it = std::adjacent_find(it, indices.end(), non_consecutive());

        long kdx = it < indices.end() ? (*it) + 1: *(it - 1) + 1;
        long ldx = ++it < indices.end() ? (*it) : num_items();
        
        move_data(m_identifiers.begin(), kdx, ldx, out_idx);
        move_data(m_entries.begin(), kdx, ldx, out_idx, num_columns());
        move_data(m_types.begin(), kdx, ldx, out_idx, num_columns());
        
        out_idx += (ldx - kdx);
    }

    // Swap order vectors and move data in m_order as marked earlier
    
    std::swap(m_order, new_order);
    
    auto out_it = std::find_if(m_order.begin(), m_order.end(), for_deletion());
    
    for (auto it = out_it; it != m_order.end(); )
    {
        auto jt = std::find_if_not(it, m_order.end(), for_deletion());
        it = std::find_if(jt, m_order.end(), for_deletion());
        
        out_it = std::copy(jt, it, out_it);
    }
    
    // Resize storage
        
    m_identifiers.resize(out_idx);
    m_entries.resize(out_idx * num_columns());
    m_types.resize(out_idx * num_columns());
    m_order.resize(out_idx);
}

// Delete a Single Entry

void entries::delete_entry(void *x, t_atom *identifier, read_write_access& access)
{
    auto position = search_identifiers(identifier);
    long idx = position.m_index;
    long data_offset = idx * num_columns();
    
    if (idx < 0)
    {
        entry_does_not_exist(x, identifier);
        return;
    }
 
    access.promote();

    m_identifiers.erase(m_identifiers.begin() + idx);
    m_order.erase(m_order.begin() + position.m_order);
    m_entries.erase(m_entries.begin() + data_offset, m_entries.begin() + data_offset + num_columns());
    m_types.erase(m_types.begin() + data_offset, m_types.begin() + data_offset + num_columns());
 
    auto check_and_adjust_order = [&](long &a) { if (a > idx) a--; };

    auto it = m_order.begin();
    
    // Unrolled order updating for speed
    
    while (it < (m_order.end() - 3))
    {
        check_and_adjust_order(*it++);
        check_and_adjust_order(*it++);
        check_and_adjust_order(*it++);
        check_and_adjust_order(*it++);
    }
    
    while (it != m_order.end())
        check_and_adjust_order(*it++);
}

// Replace One Item (within an entry)

void entries::replace_item(t_atom *identifier, long column, t_atom *item)
{
    t_custom_atom data = item;
    
    long idx = get_entry_index(identifier);
    
    if (idx < 0)
        return;
        
    if (m_columns[column].m_label == data.is_symbol())
        set_data(idx, column, data);
    else
        return;
}

// Stats Calculations

// Utility Binary Functor Struct (for min/max statistics)

template <const double& func(const double&, const double&)>
struct binary_functor
{
    const double operator()(const double a, const double b) { return func(a, b); }
};

// User Function (handles errors and variable queries)

void entries::stats(void *x, std::vector<t_atom>& output, long argc, t_atom *argv) const
{
    if (argc < 2)
    {
        object_error((t_object *) x, "no statistics listed");
        return;
    }
    
    std::vector<double> values;
    double mean = 0.0;
    bool mean_calculated = false;
    bool sorted = false;
    long i;

    long column = get_column_index(argv++);
    argc--;

    if (column < 0)
    {
        object_error((t_object *) x, "invalid column for statistics calculation");
        return;
    }
        
    // Resize assuming each element is a statistic
        
    output.resize(argc);

    // Loop over statistics and calculate
    
    for (i = 0; argc > 0; i++)
    {
        t_symbol *test = atom_getsym(argv++);
        argc--;
        
        if (test == ps_mean || test == ps_stddev || test == ps_deviation || test == ps_standard_dev || test == ps_standard_deviation)
        {
            if (!mean_calculated)
            {
                mean = column_mean(column);
                mean_calculated = true;
            }
            
            if (test == ps_mean)
                atom_setfloat(output.data() + i, mean);
            else
                atom_setfloat(output.data() + i, column_standard_deviation(column, mean));
        }
        else if (test == ps_median || test == ps_centile || test == ps_percentile)
        {
            double percentile = 50.0;
            
            if (!sorted)
            {
                column_sort(column, values);
                sorted = true;
            }
            
            if (test != ps_median)
            {
                if (argc--)
                    percentile = atom_getfloat(argv++);
                else
                    object_error((t_object *) x, "no percentile given for percentile statistic");
            }
            
            atom_setfloat(output.data() + i, find_percentile(values, percentile));
        }
        else if (test == ps_min || test == ps_minimum)
            atom_setfloat(output.data() + i, column_min(column));
        else if (test == ps_max || test == ps_maximum)
            atom_setfloat(output.data() + i, column_max(column));
        else
            object_error((t_object *) x, "unknown statistic type");
    }
    
    // Resize to the correct output size
    
    output.resize(i);
}

// Specific Statistics

double entries::column_min(long column) const
{
    return column_calculate(column, std::numeric_limits<double>::infinity(), binary_functor<std::min<double>>());
}

double entries::column_max(long column) const
{
    return column_calculate(column, -std::numeric_limits<double>::infinity(), binary_functor<std::max<double>>());
}

double entries::column_mean(long column) const
{
    return column_calculate(column, 0.0, std::plus<double>()) / num_items();
}

double entries::column_standard_deviation(long column, double mean) const
{
    struct variance
    {
        variance(double mean) : m_mean(mean) {}
        double operator()(double sum, double data) { return sum + ((data - m_mean) * (data - m_mean)); }
        double m_mean;
    };
    
    return sqrt(column_calculate(column, 0.0, variance(mean)) / num_items());
}

void entries::column_sort(long column, std::vector<double>& sorted_values) const
{
    sorted_values.resize(num_items());

    for (long i = 0; i < num_items(); i++)
        sorted_values[i] = get_data(i, column);
    
    sort(sorted_values, num_items());
}

double entries::find_percentile(std::vector<double>& sorted_values, double percentile) const
{
    long n_items = static_cast<long>(sorted_values.size());
    
    if (!n_items)
        return std::numeric_limits<double>::quiet_NaN();
    
    long idx = std::max(0L, std::min(static_cast<long>(round(n_items * (percentile / 100.0))), n_items - 1));
    
    return sorted_values[idx];
}

// Saving and Loading

// File Save

void entries::save_file(t_object *x, t_symbol *file) const
{
    char filepath[MAX_PATH_CHARS];
    char filename[MAX_FILENAME_CHARS];
    short path;
    
    t_fourcc type;
    t_fourcc types = 'JSON';
    
    if (file && file != gensym(""))
    {
        strncpy_zero(filepath, file->s_name, MAX_PATH_CHARS);
        if (path_frompotentialpathname(filepath, &path, filename))
            return;
    }
    else
    {
        strcpy(filename, m_name->s_name);
        if (saveasdialog_extended(filename, &path, &type, &types, 1))
            return;
    }
    
    t_dictionary *dict = save_dictionary(true);
    dictionary_write(dict, filename, path);
    object_free(dict);
}

// File Load

void entries::load_file(t_object *x, t_symbol *file)
{
    char filename[MAX_PATH_CHARS];
    short path;

    t_dictionary *dict = nullptr;

    t_fourcc type;
    t_fourcc types = 'JSON';
    
    if (file && file != gensym(""))
    {
        strncpy_zero(filename, file->s_name, MAX_PATH_CHARS);
        if (locatefile_extended(filename, &path, &type, &types, 1))
            return;
    }
    else
    {
        strcpy(filename, "");
        if (open_dialog(filename, &path, &type, &types, 1))
            return;
    }
    
    dictionary_read(filename, path, &dict);
    load_dictionary(x, dict);
    object_free(dict);
}

// Dictionary Save

t_dictionary *entries::save_dictionary(bool data_one_key) const
{
    std::vector<t_atom> args(num_columns() + 1);
    t_dictionary *dict = dictionary_new();
    t_dictionary *dict_meta = dictionary_new();
    t_dictionary *dict_data = dictionary_new();
    
    // Metadata
    
    dictionary_appendlong(dict_meta, gensym("num_columns"), num_columns());
    
    for (long i = 0; i < num_columns(); i++)
        atom_setsym(args.data() + i, m_columns[i].m_name);
    
    dictionary_appendatoms(dict_meta, gensym("names"), num_columns(), args.data());
    
    for (long i = 0; i < num_columns(); i++)
        atom_setlong(args.data() + i, m_columns[i].m_label);
    
    dictionary_appendatoms(dict_meta, gensym("labelmodes"), num_columns(), args.data());
    dictionary_appenddictionary(dict, gensym("metadata"), (t_object *) dict_meta);
    
    // Data
    
    if (data_one_key)
    {
        std::vector<t_atom> entries(num_items());

        for (long i = 0; i < num_items(); i++)
        {
            t_dictionary *dict_entry = dictionary_new();
            
            get_entry_identifier(args.data(), i);
            
            for (long j = 0; j < num_columns(); j++)
                get_atom(args.data() + j + 1, i, j);
            
            dictionary_appendatoms(dict_entry, gensym("entry"), num_columns() + 1, args.data());
            atom_setobj(entries.data() + i, dict_entry);
        }
        
        dictionary_appendatoms(dict_data, gensym("all_entries"), num_items(), entries.data());
    }
    else
    {
        for (long i = 0; i < num_items(); i++)
        {
            std::string str("entry_" + std::to_string(i + 1));
            
            get_entry_identifier(args.data(), i);

            for (long j = 0; j < num_columns(); j++)
                get_atom(args.data() + j + 1, i, j);
            
            dictionary_appendatoms(dict_data, gensym(str.c_str()), num_columns() + 1, args.data());
        }
    }
    dictionary_appenddictionary(dict, gensym("data"), (t_object *) dict_data);
    
    return dict;
}

// Dictionary Load

void entries::load_dictionary(t_object *x, t_dictionary *dict)
{
    t_max_err err = MAX_ERR_NONE;
    
    t_dictionary *dict_meta = nullptr;
    t_dictionary *dict_data = nullptr;
    
    t_atom *argv;
    long argc;
    
    if (!dict)
        return;
    
    dictionary_getdictionary(dict, gensym("metadata"), (t_object **) &dict_meta);
    dictionary_getdictionary(dict, gensym("data"), (t_object **) &dict_data);
    
    if (dict_meta && dict_data)
    {
        clear();
        
        t_atom_long new_num_columns;
        dictionary_getlong(dict_meta, gensym("num_columns"), &new_num_columns);
        
        if (new_num_columns != num_columns())
        {
            m_columns.clear();
            m_columns.resize(new_num_columns);
        }
        
        dictionary_getatoms(dict_meta, gensym("names"), &argc, &argv);
        set_column_names(x, argc, argv);
        
        dictionary_getatoms(dict_meta, gensym("labelmodes"), &argc, &argv);
        set_column_label_modes(x, argc, argv);
        
        // Data
        
        if (dictionary_getatoms(dict_data, gensym("all_entries"), &argc, &argv) == MAX_ERR_NONE)
        {
            for (long i = 0; i < argc; i++)
            {
                t_atom *entry_argv;
                long entry_argc;
                
                t_dictionary *dict_entry = (t_dictionary *) atom_getobj(argv + i);
                if ((err = dictionary_getatoms(dict_entry, gensym("entry"), &entry_argc, &entry_argv)) == MAX_ERR_NONE)
                    add_entry(x, entry_argc, entry_argv);
            }
        }
        else
        {
            for (long i = 0; err == MAX_ERR_NONE; i++)
            {
                std::string str("entry_" + std::to_string(i + 1));
                if ((err = dictionary_getatoms(dict_data, gensym(str.c_str()), &argc, &argv)) == MAX_ERR_NONE)
                    add_entry(x, argc, argv);
            }
        }
    }
}
