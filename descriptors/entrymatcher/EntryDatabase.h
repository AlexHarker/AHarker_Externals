
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
    
public:
    
    class Accessor
    {
        friend EntryDatabase;
        
    public:
        
        inline CustomAtom getData(long idx, long column) const              { return mIterator[idx * mNumColumns + column]; }
        inline void getDataAtom(t_atom *a, long idx, long column) const     { return getData(idx, column).getAtom(a); }

    private:
        
        Accessor(const EntryDatabase& database) : mNumColumns(database.numColumns()), mIterator(database.mEntries.begin()) {}

        const long mNumColumns;
        const std::vector<const CustomAtom>::iterator mIterator;
    };
    
    EntryDatabase(long numCols)     { mColumns.resize(numCols); }
    Accessor accessor() const       { return Accessor(*this); }

    void reserve(long items);
    void clear();
    
    size_t numItems() const         { return mIdentifiers.size(); }
    size_t numColumns() const       { return mColumns.size(); }
    
    void setLabelModes(void *x, long argc, t_atom *argv);
    void setNames(void *x, long argc, t_atom *argv);
    void addEntry(void *x, long argc, t_atom *argv);
    void removeEntry(void *x, t_atom *identifier);
    
    t_symbol *getName(long idx) const                   { return mColumns[idx].mName; }
    bool getLabelMode(long idx) const                   { return mColumns[idx].mLabel; }
    void getIdentifier(t_atom *a, long idx) const       { return getIdentifierInternal(idx).getAtom(a); }
    
    inline CustomAtom getData(long idx, long column) const              { return mEntries[idx * numColumns() + column]; }
    inline void getDataAtom(t_atom *a, long idx, long column) const     { return getData(idx, column).getAtom(a); }
    
    long columnFromSpecifier(const t_atom *specifier) const;
    
    double columnMin(const t_atom *specifier) const;
    double columnMax(const t_atom *specifier) const;
    double columnMean(const t_atom *specifier) const;
    double columnStandardDeviation(const t_atom *specifier) const;
    double columnPercentile(const t_atom *specifier, double percentile) const;
    double columnMedian(const t_atom *specifier) const;
    
    void view() const;
    void save(t_object *x, t_symbol *fileSpecifier) const;
    void load(t_object *x, t_symbol *fileSpecifier);

    long itemFromIdentifier(const t_atom *identifier) const
    {
        long order;
        return searchIdentifiers(CustomAtom(identifier, false), order);
    }

private:

    template <const double& func(const double&, const double&)>
    struct BinaryFunctor
    {
        const double operator()(const double a, const double b) { return func(a, b); }
    };
    
    template <class Op> double columnCalculate(const t_atom *specifier, const double startValue, Op op) const
    {
        double value = startValue;
        long column = columnFromSpecifier(specifier);
    
        if (column >= 0 && !mColumns[column].mLabel)
        {
            for (long i = 0; i < numItems(); i++)
                op(value, getData(i, column).mValue);
        }
        
        return value;
    }

    long searchIdentifiers(const CustomAtom& identifier, long& idx) const;

    CustomAtom getIdentifierInternal(long idx) const                    { return mIdentifiers[idx];}
    
    inline void setData(long idx, long column, const CustomAtom& data)  { mEntries[idx * numColumns() + column] = data; }
    
    // Data
    
    std::vector<ColumnInfo> mColumns;
    std::vector<CustomAtom> mIdentifiers;
    std::vector<long> mOrder;
    std::vector<CustomAtom> mEntries;
};


#endif
