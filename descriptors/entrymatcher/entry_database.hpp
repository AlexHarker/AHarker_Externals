
#ifndef ENTRY_DATABASE_HPP
#define ENTRY_DATABASE_HPP

#include <ext.h>
#include <vector>

#include "custom_atom.hpp"

#include <AH_Locks.hpp>

class EntryDatabase
{
    struct column_info
    {
        column_info() : m_name(gensym("")), m_label(false) {}
        
        t_symbol *m_name;
        bool m_label;
    };
    
public:
    
    class raw_accessor
    {
        friend EntryDatabase;
        
    public:
        
        inline t_untyped_atom get_data(long idx, long column) const
        {
            return m_iterator[idx * m_num_columns + column];
        }

    protected:
        
        raw_accessor(const EntryDatabase& database)
        : m_num_columns(database.numColumns()), m_iterator(database.m_entries.cbegin()) {}
        
        const long m_num_columns;
        const std::vector<t_untyped_atom>::const_iterator m_iterator;
    };
    
    class atom_accessor : private raw_accessor
    {
        friend EntryDatabase;
        
    public:
        
        inline void get_data_atom(t_atom *a, long idx, long column) const
        {
            t_custom_atom(get_data(idx, column), m_types[idx * m_num_columns + column]).get_atom(a);
        }
        
    private:
        
        atom_accessor(const EntryDatabase& database)
        : raw_accessor(database), m_types(database.m_types.cbegin()) {}
        
        const std::vector<t_custom_atom::Type>::const_iterator m_types;
    };
    
    struct ReadPointer
    {
        ReadPointer(const EntryDatabase *ptr) : m_ptr(ptr)  { m_ptr->m_lock.acquire_read(); }
        ReadPointer(const ReadPointer&) = delete;
        ReadPointer& operator=(const ReadPointer&) = delete;
        ReadPointer( ReadPointer&&) = default;
        ~ReadPointer()                                      { destroy(); }
        
        void destroy()
        {
            if (m_ptr)
            {
                m_ptr->m_lock.release_read();
                m_ptr = nullptr;
            }
        }
        
        const EntryDatabase *operator->() const { return m_ptr; }
        
    protected:
        
        const EntryDatabase *m_ptr;
    };
    
    struct write_pointer
    {
        write_pointer(EntryDatabase *ptr) : m_ptr(ptr) {}
        write_pointer(const write_pointer&) = delete;
        write_pointer& operator=(const write_pointer&) = delete;
        write_pointer(write_pointer&&) = default;
        EntryDatabase *operator->() const { return m_ptr; }
        
    protected:
        
        EntryDatabase *m_ptr;
    };

    EntryDatabase(t_symbol *name, long numCols) : m_name(name) { m_columns.resize(numCols); }
    
    raw_accessor rawAccessor() const { return raw_accessor(*this); }
    atom_accessor atomAccessor() const { return atom_accessor(*this); }

    void reserve(long items);
    void clear();
    
    size_t numItems() const         { return m_identifiers.size(); }
    size_t numColumns() const       { return m_columns.size(); }
    
    void setColumnLabelModes(void *x, long argc, t_atom *argv);
    void setColumnNames(void *x, long argc, t_atom *argv);
    void addEntry(void *x, long argc, t_atom *argv);
    void replaceItem(t_atom *identifier, long column, t_atom *item);
    void removeEntries(void *x, long argc, t_atom *argv);
    void removeMatchedEntries(void *x, long argc, t_atom *argv);
    
    t_symbol *getName() const                                   { return m_name; }
    t_symbol *getColumnName(long idx) const                     { return m_columns[idx].m_name; }
    bool getColumnLabelMode(long idx) const                     { return m_columns[idx].m_label; }
    void getEntryIdentifier(t_atom *a, long idx) const          { return getEntryIdentifier(idx).get_atom(a); }
    t_custom_atom getEntryIdentifier(long idx) const            { return m_identifiers[idx];}
    long getEntryIndex(const t_atom *identifier) const
    {
        long order;
        return search_identifiers(identifier, order);
    }
    
    long columnFromSpecifier(const t_atom *specifier) const;
    
    void stats(void *x, std::vector<t_atom>& output, long argc, t_atom *argv) const;
    double column_min(long column) const;
    double column_max(long column) const;
    double column_mean(long column) const;
    double column_standard_deviation(long column) const;
    double column_percentile(long column, double percentile) const;
    double column_median(long column) const;
    
    void view(t_object *database_object) const;
    void save(t_object *x, t_symbol *fileSpecifier) const;
    void load(t_object *x, t_symbol *fileSpecifier);

    t_dictionary *saveDictionary(bool entriesAsOneKey) const;
    void loadDictionary(t_object *x, t_dictionary *dict);

    inline t_untyped_atom getData(long idx, long column) const              { return m_entries[idx * numColumns() + column]; }
    inline t_custom_atom getTypedData(long idx, long column) const          { return t_custom_atom(getData(idx, column), m_types[idx * numColumns() + column]); }
    inline void getDataAtom(t_atom *a, long idx, long column) const         { return getTypedData(idx, column).get_atom(a); }

private:

    struct ReadWritePointer : public ReadPointer
    {
        ReadWritePointer(const EntryDatabase *ptr) : ReadPointer(ptr), m_promoted(false) {}
        
        ~ReadWritePointer()
        {
            if (m_promoted)
            {
                m_ptr->m_lock.release_write();
                m_ptr = nullptr;
            }
        }
        
        void promoteToWrite()
        {
            m_ptr->m_lock.promote();
            m_promoted = true;
        }
        
        ReadWritePointer(const ReadWritePointer&) = delete;
        ReadWritePointer& operator=(const ReadWritePointer&) = delete;
        
    private:
        
        bool m_promoted;
    };
    
    void clear(write_lock_hold &lock);
    void setColumnLabelModes(write_lock_hold &lock, void *x, long argc, t_atom *argv);
    void setColumnNames(write_lock_hold &lock, void *x, long argc, t_atom *argv);
    void add_entry(write_lock_hold &lock, void *x, long argc, t_atom *argv);
    void remove_entry(void *x, t_atom *identifier);
    void remove_entries(ReadWritePointer& readLockedDatabase, const std::vector<long>& indices);
    
    template <const double& func(const double&, const double&)>
    struct BinaryFunctor
    {
        const double operator()(const double a, const double b) { return func(a, b); }
    };
    
    double column_standard_deviation(long column, double mean) const;
    void column_sort_values(long column, std::vector<double>& sortedValues) const;
    double find_percentile(std::vector<double>& sortedValues, double percentile) const;

    template <class Op>
    double column_calculate(long column, const double startValue, Op op) const
    {
        double value = startValue;
    
        if (column < 0 || m_columns[column].m_label || !numItems())
            return std::numeric_limits<double>::quiet_NaN();
        
        for (long i = 0; i < numItems(); i++)
            value = op(value, getData(i, column));
        
        return value;
    }

    long get_order(long idx);
    long search_identifiers(const t_atom *identifierAtom, long& idx) const;
    
    inline void set_data(long idx, long column, const t_custom_atom& data)
    {
        m_entries[idx * numColumns() + column] = data.m_data;
        m_types[idx * numColumns() + column] = data.m_type;
    }
   
    // Data
    
    t_symbol *m_name;
    
    std::vector<column_info> m_columns;
    std::vector<t_custom_atom> m_identifiers;
    std::vector<long> m_order;
    std::vector<t_untyped_atom> m_entries;
    std::vector<t_custom_atom::Type> m_types;

    mutable read_write_lock m_lock;
};

#endif
