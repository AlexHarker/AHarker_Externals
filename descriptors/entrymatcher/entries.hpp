
#ifndef ENTRIES_HPP
#define ENTRIES_HPP

#include <ext.h>
#include <vector>

#include <AH_Locks.hpp>

#include "atom_types.hpp"

class entries
{
public:
    
    // Read Access (read methods with read locking)
    
    struct read_access
    {
        // Constructor
        
        read_access(const entries& data)
        : m_data(get_lock(data))
        , m_unlocked(false)
        , m_num_columns(data.num_columns())
        , m_iterator(data.m_entries.cbegin())
        , m_types(data.m_types.cbegin())
        {}
        
        ~read_access()
        {
            if (!m_unlocked)
                m_data.m_lock.release_read();
        }
        
        // Non-copyable, but movable
        
        read_access(const read_access&) = delete;
        read_access& operator=(const read_access&) = delete;
        read_access(read_access&&) = default;
      
        // Size / Labels / Info
        
        long num_items() const                                      { return m_data.num_items(); }
        long num_columns() const                                    { return m_num_columns; }
      
        t_symbol *get_column_name(long idx) const                   { return m_data.get_column_name(idx); }
        bool get_column_label_mode(long idx) const                  { return m_data.get_column_label_mode(idx); }
        void get_entry_identifier(long idx, t_atom *a) const        { return m_data.get_entry_identifier(idx, a); }
        t_custom_atom get_entry_identifier(long idx) const          { return m_data.get_entry_identifier(idx); }
        long get_entry_index(const t_atom *identifier) const        { return m_data.get_entry_index(identifier); }
        long get_column_index(const t_atom *specifier) const        { return m_data.get_column_index(specifier); }
        
        // Stats and Saving
        
        void stats(void *x, std::vector<t_atom>& output, long argc, t_atom *argv) const
        {
            m_data.stats(x, output, argc, argv);
        }

        t_dictionary *save_dictionary(bool data_one_key) const      { return m_data.save_dictionary(data_one_key); }
        void save_file(t_object *x, t_symbol *file) const           { return m_data.save_file(x, file); }

        // Get Data
        
        template <typename T = double>
        inline T get_data(long idx, long column) const                  { return get_untyped(idx, column).as<T>(); }
        inline void get_atom(t_atom *a, long idx, long column) const    { get_typed(idx, column).get_atom(a); }
        inline std::string get_string(long idx, long column) const      { return get_typed(idx, column).get_string(); }
        
    protected:
        
        // Helpers
        
        inline t_custom_atom get_typed(long idx, long column) const
        {
            return t_custom_atom(get_untyped(idx, column), m_types[idx * m_num_columns + column]);
        }
        
        inline t_untyped_atom get_untyped(long idx, long column) const
        {
            return m_iterator[idx * m_num_columns + column];
        }
        
        const entries& get_lock(const entries& data)
        {
            data.m_lock.acquire_read();
            return data;
        }
        
        // Data
        
        const entries& m_data;
        bool m_unlocked;
        const long m_num_columns;
        const std::vector<t_untyped_atom>::const_iterator m_iterator;
        const std::vector<t_custom_atom::category>::const_iterator m_types;
    };
    
    // Write Access (write methods with locking)
    
    struct write_access
    {
        // Constructor (freely copyable as the lock is transitory
        
        write_access(entries& data) : m_entries(data) {}
                
        // Reserve / Clear
        
        void reserve(long items)    { with_lock(&entries::reserve, items); }
        void clear()                { with_lock(&entries::clear); }
        
        // Setup Columns
        
        void set_column_label_modes(void *x, long argc, t_atom *argv)
        {
            with_lock(&entries::set_column_label_modes, x, argc, argv);
        }
        
        void set_column_names(void *x, long argc, t_atom *argv)
        {
            with_lock(&entries::set_column_names, x, argc, argv);
        }
        
        // Add / Remove Entries
        
        void add_entry(void *x, long argc, t_atom *argv)
        {
            with_lock(&entries::add_entry, x, argc, argv);
        }
        
        void remove_entries(void *x, long argc, t_atom *argv)
        {
            with_read_write_lock(&entries::remove_entries, x, argc, argv);
        }
        
        void remove_matched_entries(void *x, long argc, t_atom *argv)
        {
            with_read_write_lock(&entries::remove_matched_entries, x, argc, argv);
        }
        
        // Replace an Item (in an entry)
        
        void replace_item(t_atom *identifier, long column, t_atom *item)
        {
            with_lock(&entries::replace_item, identifier, column, item);
        }

        // Loading
        
        void load_file(t_object *x, t_symbol *file)             { with_lock(&entries::load_file, x, file); }
        void load_dictionary(t_object *x, t_dictionary *dict)   { with_lock(&entries::load_dictionary, x, dict); }
        
    private:
        
        // Lock Helpers
        
        template <typename Method, typename ...Args>
        void with_lock(Method method, Args&& ...args)
        {
            write_lock_hold lock(&m_entries.m_lock);
            (m_entries.*method)(args...);
        }
        
        template <typename Method, typename ...Args>
        void with_read_write_lock(Method method, Args&& ...args)
        {
            read_write_access access(m_entries);
            (m_entries.*method)(args..., access);
        }
        
        // Data
        
        entries& m_entries;
    };

    // Constructor / Names
    
    entries(t_symbol *name, long num_columns)
    : m_name(name) { m_columns.resize(num_columns); }
    
    t_symbol *get_name() const { return m_name; }

    // Accessors
    
    write_access get_write_access() { return write_access(*this); }
    read_access get_read_access() const { return read_access(*this); }
    
private:
    
    // Column Info
    
    struct column_info
    {
        column_info() : m_name(gensym("")), m_label(false) {}
        
        t_symbol *m_name;
        bool m_label;
    };
    
    // Position Info
    
    struct position_info
    {
        long m_index;
        long m_order;
    };
    
    // A Read / Write Access (starts as read with promotion to write)
    
    struct read_write_access : public read_access
    {
        read_write_access(const entries& data)
        : read_access(data), m_promoted(false) {}
        
        ~read_write_access()
        {
            if (m_promoted)
            {
                m_data.m_lock.release_write();
                m_unlocked = true;
            }
        }
        
        void promote()
        {
            m_data.m_lock.promote();
            m_promoted = true;
        }

    private:
        
        bool m_promoted;
    };
    
    // Write Methods
    
    // Global
    
    void reserve(long items);
    void clear();
    
    // Setup / Entries / Items
    
    void set_column_label_modes(void *x, long argc, t_atom *argv);
    void set_column_names(void *x, long argc, t_atom *argv);
    void add_entry(void *x, long argc, t_atom *argv);
    void remove_entries(void *x, long argc, t_atom *argv, read_write_access& access);
    void remove_matched_entries(void *x, long argc, t_atom *argv, read_write_access& access);
    void delete_entries(std::vector<long>& indices, read_write_access& access);
    void delete_entry(void *x, t_atom *identifier, read_write_access& access);
    void replace_item(t_atom *identifier, long column, t_atom *item);

    // Loading
    
    void load_file(t_object *x, t_symbol *file);
    void load_dictionary(t_object *x, t_dictionary *dict);
    
    // Data Setter
    
    inline void set_data(long idx, long column, const t_custom_atom& data)
    {
        m_entries[idx * num_columns() + column] = data.m_data;
        m_types[idx * num_columns() + column] = data.m_type;
    }
    
    // Read methods
    
    // Size / Labels / Info
    
    long num_items() const      { return static_cast<long>(m_identifiers.size()); }
    long num_columns() const    { return static_cast<long>(m_columns.size()); }
  
    t_symbol *get_column_name(long idx) const               { return m_columns[idx].m_name; }
    bool get_column_label_mode(long idx) const              { return m_columns[idx].m_label; }
    void get_entry_identifier(long idx, t_atom *a)  const   { return get_entry_identifier(idx).get_atom(a); }
    t_custom_atom get_entry_identifier(long idx) const      { return m_identifiers[idx]; }
    long get_entry_index(const t_atom *identifier) const    { return search_identifiers(identifier).m_index; }
    long get_column_index(const t_atom *specifier) const;
    
    // Stats
    
    void stats(void *x, std::vector<t_atom>& output, long argc, t_atom *argv) const;
    
    double column_min(long column) const;
    double column_max(long column) const;
    double column_mean(long column) const;
    double column_standard_deviation(long column, double mean) const;
    
    // Percentile Stats Implementation
     
     void column_sort(long column, std::vector<double>& sorted_values) const;
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
    
    // Saving
    
    void save_file(t_object *x, t_symbol *file) const;
    t_dictionary *save_dictionary(bool entries_as_one_key) const;

    // Data Getters
    
    inline t_untyped_atom get_untyped(long idx, long column) const  { return m_entries[idx * num_columns() + column]; }
    inline t_custom_atom get_typed(long idx, long column) const     { return t_custom_atom(get_untyped(idx, column), m_types[idx * num_columns() + column]); }
    inline double get_data(long idx, long column) const             { return get_untyped(idx, column).m_value; }
    inline void get_atom(t_atom *a, long idx, long column) const    { return get_typed(idx, column).get_atom(a); }

    // Ordering
    
    long get_order(long idx);
    position_info search_identifiers(const t_atom *identifier_atom) const;
   
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
