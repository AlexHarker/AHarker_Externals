
#ifndef ENTRYDATABASE_H
#define ENTRYDATABASE_H

#include "ext.h"
#include <vector>
#include <list>
#include <map>

#include "CustomAtom.h"

class EntryDatabase
{
    struct ColumnInfo
    {
        ColumnInfo() : mName(gensym("")), mLabel(false) {}
        
        t_symbol *mName;
        bool mLabel;
    };
    
    struct Lock
    {
        Lock() : mAtomicLock(0) {}
        ~Lock() { acquire(); }
        
        void acquire() { while(!attempt()); }
        bool attempt() { return ATOMIC_COMPARE_SWAP32(0, 1, &mAtomicLock); }
        void release() { ATOMIC_COMPARE_SWAP32(1, 0, &mAtomicLock); }
        
    private:
        
        t_int32_atomic mAtomicLock;
    };
    
    struct HoldLock
    {
        HoldLock() : mLock(NULL) {}
        HoldLock(Lock *lock) : mLock(lock)  { mLock->acquire(); }
        ~HoldLock()                         { if (mLock) mLock->release(); }
                
    private:
        
        Lock *mLock;
    };

public:
    
    struct Entry
    {
        Entry(CustomAtom identifier, long numColumns) : mIdentifier(identifier)
        {
            mData.resize(numColumns);
            mTypes.resize(numColumns);
        };
        
        inline UntypedAtom getData(long column) const                   { return mData[column]; }
        inline CustomAtom getTypedData(long column) const               { return CustomAtom(getData(column), mTypes[column]); }
        inline void getDataAtom(t_atom *a, long column) const           { return getTypedData(column).getAtom(a); }
        inline void getIdentifierAtom(t_atom *a) const                  { return mIdentifier.getAtom(a); }
        
        
        inline void setData(long column, const CustomAtom& data)
        {
            mData[column] = data.mData;
            mTypes[column] = data.mType;
        }
        
        // Data
        
        CustomAtom mIdentifier;
        std::vector<UntypedAtom> mData;
        std::vector<CustomAtom::Type> mTypes;
    };

    class Accessor
    {
        friend EntryDatabase;
        
    public:
        
        const std::list<Entry>::const_iterator begin() const    { return mBegin; }
        const std::list<Entry>::const_iterator end() const      { return mEnd; }
        
    protected:
        
        Accessor(const EntryDatabase& database) : mBegin(database.mEntryList.cbegin()), mEnd(database.mEntryList.cend()) {}
        
        const std::list<Entry>::const_iterator mBegin;
        const std::list<Entry>::const_iterator mEnd;
    };
    
    struct ReadPointer
    {
        ReadPointer(const EntryDatabase *ptr) : mPtr(ptr)
        {
            HoldLock lock(&mPtr->mReadLock);
            if (mPtr->mReadCount++ == 0)
                mPtr->mWriteLock.acquire();
        }
        
        ~ReadPointer()
        {
            if (--mPtr->mReadCount == 0)
                mPtr->mWriteLock.release();
        }
        
        const EntryDatabase *operator->() const { return mPtr; }
        
    private:
        
        const EntryDatabase *mPtr;
    };
    
    EntryDatabase(t_symbol *name, long numCols) : mName(name) { mColumns.resize(numCols); }
    
    Accessor accessor() const { return Accessor(*this); }

    void reserve(long items);
    void clear();
    
    size_t numItems() const         { return mEntryList.size(); }
    size_t numColumns() const       { return mColumns.size(); }
    
    void setColumnLabelModes(void *x, long argc, t_atom *argv);
    void setColumnNames(void *x, long argc, t_atom *argv);
    void addEntry(void *x, long argc, t_atom *argv);
    void removeEntries(void *x, long argc, t_atom *argv);
    void removeMatchedEntries(void *x, long argc, t_atom *argv);
    
    t_symbol *getColumnName(long idx) const                   { return mColumns[idx].mName; }
    bool getColumnLabelMode(long idx) const                   { return mColumns[idx].mLabel; }
    
    long columnFromSpecifier(const t_atom *specifier) const;
    
    double columnMin(const t_atom *specifier) const;
    double columnMax(const t_atom *specifier) const;
    double columnMean(const t_atom *specifier) const;
    double columnStandardDeviation(const t_atom *specifier) const;
    double columnPercentile(const t_atom *specifier, double percentile) const;
    double columnMedian(const t_atom *specifier) const;
    
    void view(t_object *database_object) const;
    void save(t_object *x, t_symbol *fileSpecifier) const;
    void load(t_object *x, t_symbol *fileSpecifier);

    std::list<Entry>::const_iterator findItem(const t_atom *identifierAtom) const;

private:

    std::list<Entry>::iterator searchIdentifiers(const t_atom *identifierAtom);

    void clear(HoldLock &lock);
    void setColumnLabelModes(HoldLock &lock, void *x, long argc, t_atom *argv);
    void setColumnNames(HoldLock &lock, void *x, long argc, t_atom *argv);
    void addEntry(HoldLock &lock, void *x, long argc, t_atom *argv);
    void removeEntry(HoldLock &lock, void *x, t_atom *identifier);
    
    template <const double& func(const double&, const double&)>
    struct BinaryFunctor
    {
        const double operator()(const double a, const double b) { return func(a, b); }
    };
    
    template <class Op> double columnCalculate(const t_atom *specifier, const double startValue, Op op) const
    {
        double value = startValue;
        long column = columnFromSpecifier(specifier);
    
        if (column < 0 || mColumns[column].mLabel || !numItems())
            return std::numeric_limits<double>::quiet_NaN();
        
        for (std::list<Entry>::const_iterator it = mEntryList.cbegin(); it != mEntryList.cend(); it++)
            value = op(value, it->getData(column));
        
        return value;
    }
    
    // Data
    
    t_symbol *mName;
    std::vector<ColumnInfo> mColumns;
    std::list<Entry> mEntryList;
    std::map<CustomAtom, std::list<Entry>::iterator> mMap;

    mutable Lock mWriteLock;
    mutable Lock mReadLock;
    mutable long mReadCount;
};


#endif
