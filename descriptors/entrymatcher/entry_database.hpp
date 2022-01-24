
#ifndef ENTRY_DATABASE_HPP
#define ENTRY_DATABASE_HPP

#include <ext.h>
#include <vector>

#include "custom_atom.hpp"

#include <AH_Locks.hpp>

class EntryDatabase
{
    struct ColumnInfo
    {
        ColumnInfo() : mName(gensym("")), mLabel(false) {}
        
        t_symbol *mName;
        bool mLabel;
    };
    
public:
    
    class RawAccessor
    {
        friend EntryDatabase;
        
    public:
        
        inline UntypedAtom getData(long idx, long column) const                  { return mIterator[idx * mNumColumns + column]; }

    protected:
        
        RawAccessor(const EntryDatabase& database) : mNumColumns(database.numColumns()), mIterator(database.mEntries.begin()) {}
        
        const long mNumColumns;
        const std::vector<UntypedAtom>::const_iterator mIterator;
    };
    
    class AtomAccessor : private RawAccessor
    {
        friend EntryDatabase;
        
    public:
        
        inline void getDataAtom(t_atom *a, long idx, long column) const       { CustomAtom(getData(idx, column), mTypes[idx * mNumColumns + column]).getAtom(a); }
        
    private:
        
        AtomAccessor(const EntryDatabase& database) : RawAccessor(database), mTypes(database.mTypes.begin()) {}
        
        const std::vector<CustomAtom::Type>::const_iterator mTypes;
    };
    
    struct ReadPointer
    {
        ReadPointer(const EntryDatabase *ptr) : mPtr(ptr)   { mPtr->mLock.acquire_read(); }
        ReadPointer(const ReadPointer&) = delete;
        ReadPointer& operator=(const ReadPointer&) = delete;
        ReadPointer( ReadPointer&&) = default;
        ~ReadPointer()                                      { destroy(); }
        
        void destroy()
        {
            if (mPtr)
            {
                mPtr->mLock.release_read();
                mPtr = nullptr;
            }
        }
        
        const EntryDatabase *operator->() const { return mPtr; }
        
    protected:
        
        const EntryDatabase *mPtr;
    };
    
    struct WritePointer
    {
        WritePointer(EntryDatabase *ptr) : mPtr(ptr) {}
        WritePointer(const WritePointer&) = delete;
        WritePointer& operator=(const WritePointer&) = delete;
        WritePointer(WritePointer&&) = default;
        EntryDatabase *operator->() const { return mPtr; }
        
    protected:
        
        EntryDatabase *mPtr;
    };

    EntryDatabase(t_symbol *name, long numCols) : mName(name) { mColumns.resize(numCols); }
    
    RawAccessor rawAccessor() const { return RawAccessor(*this); }
    AtomAccessor atomAccessor() const { return AtomAccessor(*this); }

    void reserve(long items);
    void clear();
    
    size_t numItems() const         { return mIdentifiers.size(); }
    size_t numColumns() const       { return mColumns.size(); }
    
    void setColumnLabelModes(void *x, long argc, t_atom *argv);
    void setColumnNames(void *x, long argc, t_atom *argv);
    void addEntry(void *x, long argc, t_atom *argv);
    void replaceItem(t_atom *identifier, long column, t_atom *item);
    void removeEntries(void *x, long argc, t_atom *argv);
    void removeMatchedEntries(void *x, long argc, t_atom *argv);
    
    t_symbol *getName() const                                 { return mName; }
    t_symbol *getColumnName(long idx) const                   { return mColumns[idx].mName; }
    bool getColumnLabelMode(long idx) const                   { return mColumns[idx].mLabel; }
    void getEntryIdentifier(t_atom *a, long idx) const        { return getEntryIdentifier(idx).getAtom(a); }
    CustomAtom getEntryIdentifier(long idx) const             { return mIdentifiers[idx];}
    long getEntryIndex(const t_atom *identifier) const
    {
        long order;
        return searchIdentifiers(identifier, order);
    }
    
    long columnFromSpecifier(const t_atom *specifier) const;
    
    void stats(void *x, std::vector<t_atom>& output, long argc, t_atom *argv) const;
    double columnMin(long column) const;
    double columnMax(long column) const;
    double columnMean(long column) const;
    double columnStandardDeviation(long column) const;
    double columnPercentile(long column, double percentile) const;
    double columnMedian(long column) const;
    
    void view(t_object *database_object) const;
    void save(t_object *x, t_symbol *fileSpecifier) const;
    void load(t_object *x, t_symbol *fileSpecifier);

    t_dictionary *saveDictionary(bool entriesAsOneKey) const;
    void loadDictionary(t_object *x, t_dictionary *dict);

    inline UntypedAtom getData(long idx, long column) const                 { return mEntries[idx * numColumns() + column]; }
    inline CustomAtom getTypedData(long idx, long column) const             { return CustomAtom(getData(idx, column), mTypes[idx * numColumns() + column]); }
    inline void getDataAtom(t_atom *a, long idx, long column) const         { return getTypedData(idx, column).getAtom(a); }

private:

    struct ReadWritePointer : public ReadPointer
    {
        ReadWritePointer(const EntryDatabase *ptr) : ReadPointer(ptr), mPromoted(false)   {}
        
        ~ReadWritePointer()
        {
            if (mPromoted)
            {
                mPtr->mLock.release_write();
                mPtr = nullptr;
            }
        }
        
        void promoteToWrite()
        {
            mPtr->mLock.promote();
            mPromoted = true;
        }
        
        ReadWritePointer(const ReadWritePointer&) = delete;
        ReadWritePointer& operator=(const ReadWritePointer&) = delete;
        
    private:
        
        bool mPromoted;
    };
    
    void clear(write_lock_hold &lock);
    void setColumnLabelModes(write_lock_hold &lock, void *x, long argc, t_atom *argv);
    void setColumnNames(write_lock_hold &lock, void *x, long argc, t_atom *argv);
    void addEntry(write_lock_hold &lock, void *x, long argc, t_atom *argv);
    void removeEntry(void *x, t_atom *identifier);
    void removeEntries(ReadWritePointer& readLockedDatabase, const std::vector<long>& indices);
    
    template <const double& func(const double&, const double&)>
    struct BinaryFunctor
    {
        const double operator()(const double a, const double b) { return func(a, b); }
    };
    
    double columnStandardDeviation(long column, double mean) const;
    void columnSortValues(long column, std::vector<double>& sortedValues) const;
    double findPercentile(std::vector<double>& sortedValues, double percentile) const;

    template <class Op> double columnCalculate(long column, const double startValue, Op op) const
    {
        double value = startValue;
    
        if (column < 0 || mColumns[column].mLabel || !numItems())
            return std::numeric_limits<double>::quiet_NaN();
        
        for (long i = 0; i < numItems(); i++)
            value = op(value, getData(i, column));
        
        return value;
    }

    long getOrder(long idx);
    long searchIdentifiers(const t_atom *identifierAtom, long& idx) const;
    
    inline void setData(long idx, long column, const CustomAtom& data)
    {
        mEntries[idx * numColumns() + column] = data.mData;
        mTypes[idx * numColumns() + column] = data.mType;
    }
   
    // Data
    
    t_symbol *mName;
    std::vector<ColumnInfo> mColumns;
    std::vector<CustomAtom> mIdentifiers;
    std::vector<long> mOrder;
    std::vector<UntypedAtom> mEntries;
    std::vector<CustomAtom::Type> mTypes;

    mutable read_write_lock mLock;
};

#endif
