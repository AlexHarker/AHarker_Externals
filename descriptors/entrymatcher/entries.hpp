
#ifndef ENTRIES_HPP
#define ENTRIES_HPP

#include <ext.h>
#include <vector>

#include "atom_types.hpp"

#include <AH_Locks.hpp>

class entries
{
    struct column_info
    {
        column_info() : m_name(gensym("")), m_label(false) {}
        
        t_symbol *m_name;
        bool m_label;
    };
    
public:
    
    class accessor
    {
        friend entries;
        
    public:
    
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
        
        accessor(const entries& database)
        : m_num_columns(database.num_columns())
        , m_iterator(database.m_entries.cbegin())
        , m_types(database.m_types.cbegin())
        {}
        
        const long m_num_columns;
        const std::vector<t_untyped_atom>::const_iterator m_iterator;
        const std::vector<t_custom_atom::category>::const_iterator m_types;
    };
    
    struct read_pointer
    {
        read_pointer(const entries *ptr) : m_ptr(ptr)  { m_ptr->m_lock.acquire_read(); }
        read_pointer(const read_pointer&) = delete;
        read_pointer& operator=(const read_pointer&) = delete;
        read_pointer(read_pointer&&) = default;
        ~read_pointer()                                      { destroy(); }
        
        void destroy()
        {
            if (m_ptr)
            {
                m_ptr->m_lock.release_read();
                m_ptr = nullptr;
            }
        }
        
        const entries *operator->() const { return m_ptr; }
        
    protected:
        
        const entries *m_ptr;
    };
    
    struct write_pointer
    {
        write_pointer(entries *ptr) : m_ptr(ptr) {}
        write_pointer(const write_pointer&) = delete;
        write_pointer& operator=(const write_pointer&) = delete;
        write_pointer(write_pointer&&) = default;
        entries *operator->() const { return m_ptr; }
        
    protected:
        
        entries *m_ptr;
    };

    entries(t_symbol *name, long num_columns)
    : m_name(name) { m_columns.resize(num_columns); }
    
    accessor get_accessor() const { return accessor(*this); }

    void reserve(long items);
    void clear();
    
    size_t num_items() const            { return m_identifiers.size(); }
    size_t num_columns() const          { return m_columns.size(); }
    
    void set_column_label_modes(void *x, long argc, t_atom *argv);
    void set_column_names(void *x, long argc, t_atom *argv);
    void add_entry(void *x, long argc, t_atom *argv);
    void replace_item(t_atom *identifier, long column, t_atom *item);
    void remove_entries(void *x, long argc, t_atom *argv);
    void remove_matched_entries(void *x, long argc, t_atom *argv);
    
    t_symbol *get_name() const                              { return m_name; }
    t_symbol *get_column_name(long idx) const               { return m_columns[idx].m_name; }
    bool get_column_label_mode(long idx) const              { return m_columns[idx].m_label; }
    void get_entry_identifier(t_atom *a, long idx) const    { return get_entry_identifier(idx).get_atom(a); }
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
    void load(t_object *x, t_symbol *file);

    t_dictionary *save_dictionary(bool entries_as_one_key) const;
    void load_dictionary(t_object *x, t_dictionary *dict);

    inline void get_atom(t_atom *a, long idx, long column) const    { return get_typed(idx, column).get_atom(a); }
    inline std::string get_string(long idx, long column) const      { return get_typed(idx, column).get_string(); }
    
private:

    struct read_write_pointer : public read_pointer
    {
        read_write_pointer(const entries *ptr)
        : read_pointer(ptr), m_promoted(false) {}
        
        ~read_write_pointer()
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
        
        read_write_pointer(const read_write_pointer&) = delete;
        read_write_pointer& operator=(const read_write_pointer&) = delete;
        
    private:
        
        bool m_promoted;
    };
    
    inline t_untyped_atom get_untyped(long idx, long column) const  { return m_entries[idx * num_columns() + column]; }
    inline t_custom_atom get_typed(long idx, long column) const     { return t_custom_atom(get_untyped(idx, column), m_types[idx * num_columns() + column]); }
    inline double get_data(long idx, long column) const             { return get_untyped(idx, column).m_value; }
    
    void clear(write_lock_hold &lock);
    void set_column_label_modes(write_lock_hold &lock, void *x, long argc, t_atom *argv);
    void set_column_names(write_lock_hold &lock, void *x, long argc, t_atom *argv);
    void add_entry(write_lock_hold &lock, void *x, long argc, t_atom *argv);
    void remove_entry(void *x, t_atom *identifier);
    void remove_entries(read_write_pointer& read_locked_database, const std::vector<long>& indices);
    
    template <const double& func(const double&, const double&)>
    struct binary_functor
    {
        const double operator()(const double a, const double b) { return func(a, b); }
    };
    
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
