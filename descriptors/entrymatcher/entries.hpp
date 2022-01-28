
#ifndef ENTRIES_HPP
#define ENTRIES_HPP

#include <ext.h>
#include <vector>

#include "atom_types.hpp"

#include <AH_Locks.hpp>

class entries
{
public:
    
    // Read Access (read methods with read locking)
    
    struct read_access
    {
        read_access(const entries& database)
        : m_ptr(get_lock(database))
        , m_num_columns(database.num_columns())
        , m_iterator(database.m_entries.cbegin())
        , m_types(database.m_types.cbegin())
        {}
        
        read_access(const read_access&) = delete;
        read_access& operator=(const read_access&) = delete;
        read_access(read_access&&) = default;
        ~read_access()                                      { destroy(); }
        
        void destroy()
        {
            if (m_ptr)
            {
                m_ptr->m_lock.release_read();
                m_ptr = nullptr;
            }
        }
        
        long num_items() const                                  { return m_ptr->num_items(); }
        long num_columns() const                                { return m_num_columns; }
      
        t_symbol *get_column_name(long idx) const               { return m_ptr->get_column_name(idx); }
        bool get_column_label_mode(long idx) const              { return m_ptr->get_column_label_mode(idx); }
        void get_entry_identifier(long idx, t_atom *a) const    { return m_ptr->get_entry_identifier(idx, a); }
        t_custom_atom get_entry_identifier(long idx) const      { return m_ptr->get_entry_identifier(idx); }
        long get_entry_index(const t_atom *identifier) const    { return m_ptr->get_entry_index(identifier); }
       
        long column_from_specifier(const t_atom *specifier) const { return m_ptr->column_from_specifier(specifier); }
        
        void stats(void *x, std::vector<t_atom>& output, long argc, t_atom *argv) const
        {
            m_ptr->stats(x, output, argc, argv);
        }

        t_dictionary *save_dictionary(bool entries_as_one_key) const { return m_ptr->save_dictionary(entries_as_one_key); }
        
        inline std::string get_string(long idx, long column) const { return m_ptr->get_string(idx, column); }
        
        void save(t_object *x, t_symbol *file) const { return m_ptr->save(x, file); }

        template <typename T = double>
        inline T get_data(long idx, long column) const
        {
            return get_untyped(idx, column).as<T>();
        }
        
        inline void get_atom(t_atom *a, long idx, long column) const
        {
            t_custom_atom(get_untyped(idx, column), m_types[idx * m_num_columns + column]).get_atom(a);
        }
        
    protected:
        
        inline t_untyped_atom get_untyped(long idx, long column) const
        {
            return m_iterator[idx * m_num_columns + column];
        }
        
        const entries *get_lock(const entries& ptr)
        {
            ptr.m_lock.acquire_read();
            return &ptr;
        }
        
        const entries *m_ptr;
        const long m_num_columns;
        const std::vector<t_untyped_atom>::const_iterator m_iterator;
        const std::vector<t_custom_atom::category>::const_iterator m_types;
    };
    
    struct write_access
    {
        write_access(entries& data) : m_entries(data) {}
                
        // Writing operations
        
        void reserve(long items)    { call_with_lock(&entries::reserve, items); }
        void clear()                { call_with_lock(&entries::clear); }
        
        void set_column_label_modes(void *x, long argc, t_atom *argv)
        {
            call_with_lock(&entries::set_column_label_modes, x, argc, argv);
        }
        
        void set_column_names(void *x, long argc, t_atom *argv)
        {
            call_with_lock(&entries::set_column_names, x, argc, argv);
        }
        
        void add_entry(void *x, long argc, t_atom *argv)
        {
            call_with_lock(&entries::add_entry, x, argc, argv);
        }
        
        void remove_entries(void *x, long argc, t_atom *argv)
        {
            void (entries::*method)(void *, long, t_atom *) = &entries::remove_entries;
            call_with_lock(method, x, argc, argv);
        }
        
        void remove_matched_entries(void *x, long argc, t_atom *argv)
        {
            call_with_lock(&entries::remove_matched_entries, x, argc, argv);
        }
        void replace_item(t_atom *identifier, long column, t_atom *item)
        {
            call_with_lock(&entries::replace_item, identifier, column, item);
        }

        void load(t_object *x, t_symbol *file)                  { call_with_lock(&entries::load, x, file); }
        void load_dictionary(t_object *x, t_dictionary *dict)   { call_with_lock(&entries::load_dictionary, x, dict); }
        
    private:
        
        template <typename Method, typename ...Args>
        void call_with_lock(Method method, Args...args)
        {
            write_lock_hold lock(&m_entries.m_lock);
            (m_entries.*method)(args...);
        }
        
        entries& m_entries;
    };

    entries(t_symbol *name, long num_columns)
    : m_name(name) { m_columns.resize(num_columns); }
    
    t_symbol *get_name() const { return m_name; }

    write_access get_write_access() { return write_access(*this); }
    read_access get_read_access() const { return read_access(*this); }

private:
    
    struct column_info
    {
        column_info() : m_name(gensym("")), m_label(false) {}
        
        t_symbol *m_name;
        bool m_label;
    };
    
    struct read_write_access : public read_access
    {
        read_write_access(const entries& data)
        : read_access(data), m_promoted(false) {}
        
        ~read_write_access()
        {
            if (m_promoted)
            {
                m_ptr->m_lock.release_write();
                m_ptr = nullptr;
            }
        }
        
        void promote()
        {
            m_ptr->m_lock.promote();
            m_promoted = true;
        }

    private:
        
        bool m_promoted;
    };
    
    // Read methods
    
    long num_items() const      { return static_cast<long>(m_identifiers.size()); }
    long num_columns() const    { return static_cast<long>(m_columns.size()); }
  
    t_symbol *get_column_name(long idx) const               { return m_columns[idx].m_name; }
    bool get_column_label_mode(long idx) const              { return m_columns[idx].m_label; }
    void get_entry_identifier(long idx, t_atom *a)  const   { return get_entry_identifier(idx).get_atom(a); }
    t_custom_atom get_entry_identifier(long idx) const      { return m_identifiers[idx];}
    
    long get_entry_index(const t_atom *identifier) const
    {
        long order;
        return search_identifiers(identifier, order);
    }
    
    long column_from_specifier(const t_atom *specifier) const;
    
    void stats(void *x, std::vector<t_atom>& output, long argc, t_atom *argv) const;
    
    double column_min(long column) const;
    double column_max(long column) const;
    double column_mean(long column) const;
    double column_standard_deviation(long column) const;
    double column_percentile(long column, double percentile) const;
    double column_median(long column) const;
    
    void view(t_object *database_object) const;
    void save(t_object *x, t_symbol *file) const;
    t_dictionary *save_dictionary(bool entries_as_one_key) const;

    // Data getters
    
    inline t_untyped_atom get_untyped(long idx, long column) const  { return m_entries[idx * num_columns() + column]; }
    inline t_custom_atom get_typed(long idx, long column) const     { return t_custom_atom(get_untyped(idx, column), m_types[idx * num_columns() + column]); }
    inline double get_data(long idx, long column) const             { return get_untyped(idx, column).m_value; }
    inline void get_atom(t_atom *a, long idx, long column) const    { return get_typed(idx, column).get_atom(a); }
    inline std::string get_string(long idx, long column) const      { return get_typed(idx, column).get_string(); }
    
    // Write methods
    
    void reserve(long items);
    void clear();
    
    void set_column_label_modes(void *x, long argc, t_atom *argv);
    void set_column_names(void *x, long argc, t_atom *argv);
    void add_entry(void *x, long argc, t_atom *argv);
    void remove_entry(void *x, t_atom *identifier);
    void remove_entries(read_write_access& read_locked_database, const std::vector<long>& indices);
    void remove_entries(void *x, long argc, t_atom *argv);
    void remove_matched_entries(void *x, long argc, t_atom *argv);
    void replace_item(t_atom *identifier, long column, t_atom *item);

    void load(t_object *x, t_symbol *file);
    void load_dictionary(t_object *x, t_dictionary *dict);
    
    double column_standard_deviation(long column, double mean) const;
    void column_sort_values(long column, std::vector<double>& sorted_values) const;
    double find_percentile(std::vector<double>& sorted_values, double percentile) const;

    template <class Op>
    double column_calculate(long column, const double start_value, Op op) const
    {
        double value = start_value;
    
        if (column < 0 || m_columns[column].m_label || !num_items())
            return std::numeric_limits<double>::quiet_NaN();
        
        for (long i = 0; i < num_items(); i++)
            value = op(value, get_data(i, column));
        
        return value;
    }

    long get_order(long idx);
    long search_identifiers(const t_atom *identifier_atom, long& idx) const;
    
    inline void set_data(long idx, long column, const t_custom_atom& data)
    {
        m_entries[idx * num_columns() + column] = data.m_data;
        m_types[idx * num_columns() + column] = data.m_type;
    }
   
    // Data
    
    t_symbol *m_name;
    
    std::vector<column_info> m_columns;
    std::vector<t_custom_atom> m_identifiers;
    std::vector<long> m_order;
    std::vector<t_untyped_atom> m_entries;
    std::vector<t_custom_atom::category> m_types;

    mutable read_write_lock m_lock;
};

#endif
