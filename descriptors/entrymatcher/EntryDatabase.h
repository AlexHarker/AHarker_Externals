
#ifndef ENTRYDATABASE_H
#define ENTRYDATABASE_H

#include "ext.h"
#include <vector>

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
    
    class RawAccessor
    {
        friend EntryDatabase;
        
    public:
        
        inline UntypedAtom getData(long idx, long column) const                  { return mIterator[idx * mNumColumns + column]; }

    protected:
        
        RawAccessor(const EntryDatabase& database) : mNumColumns(database.numColumns()), mIterator(database.mEntries.begin()) {}
        
        const long mNumColumns;
        const std::vector<const UntypedAtom>::iterator mIterator;
    };
    
    class AtomAccessor : private RawAccessor
    {
        friend EntryDatabase;
        
    public:
        
        inline void getDataAtom(t_atom *a, long idx, long column) const       { CustomAtom(getData(idx, column), mTypes[idx * mNumColumns + column]).getAtom(a); }
        
    private:
        
        AtomAccessor(const EntryDatabase& database) : RawAccessor(database), mTypes(database.mTypes.begin()) {}
        
        const std::vector<const CustomAtom::Type>::iterator mTypes;
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
    
    RawAccessor rawAccessor() const { return RawAccessor(*this); }
    AtomAccessor atomAccessor() const { return AtomAccessor(*this); }

    void reserve(long items);
    void clear();
    
    size_t numItems() const         { return mIdentifiers.size(); }
    size_t numColumns() const       { return mColumns.size(); }
    
    void setColumnLabelModes(void *x, long argc, t_atom *argv);
    void setColumnNames(void *x, long argc, t_atom *argv);
    void addEntry(void *x, long argc, t_atom *argv);
    void removeEntries(void *x, long argc, t_atom *argv);
    void removeMatchedEntries(void *x, long argc, t_atom *argv);
    
    t_symbol *getColumnName(long idx) const                   { return mColumns[idx].mName; }
    bool getColumnLabelMode(long idx) const                   { return mColumns[idx].mLabel; }
    void getEntryIdentifier(t_atom *a, long idx) const        { return getIdentifierInternal(idx).getAtom(a); }
    long getEntryIndex(const t_atom *identifier) const
    {
        long order;
        return searchIdentifiers(identifier, order);
    }
    
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

private:

    inline UntypedAtom getData(long idx, long column) const                 { return mEntries[idx * numColumns() + column]; }
    inline CustomAtom getTypedData(long idx, long column) const             { return CustomAtom(getData(idx, column), mTypes[idx * numColumns() + column]); }
    inline void getDataAtom(t_atom *a, long idx, long column) const         { return getTypedData(idx, column).getAtom(a); }
    
    void clear(HoldLock &lock);
    void setColumnLabelModes(HoldLock &lock, void *x, long argc, t_atom *argv);
    void setColumnNames(HoldLock &lock, void *x, long argc, t_atom *argv);
    void addEntry(HoldLock &lock, void *x, long argc, t_atom *argv);
    void removeEntry(HoldLock &lock, void *x, t_atom *identifier);
    void removeEntries(HoldLock &lock, const std::vector<long>& indices);
    
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
        
        for (long i = 0; i < numItems(); i++)
            value = op(value, getData(i, column));
        
        return value;
    }

    long getOrder(long idx);
    long searchIdentifiers(const t_atom *identifierAtom, long& idx) const;

    CustomAtom getIdentifierInternal(long idx) const                    { return mIdentifiers[idx];}
    
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

    mutable Lock mWriteLock;
    mutable Lock mReadLock;
    mutable long mReadCount;
};


#endif
