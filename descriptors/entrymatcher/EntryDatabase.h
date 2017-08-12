
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
        
    public:
        
        Accessor(const EntryDatabase& database) : mNumColumns(database.numColumns()), mIterator(database.mEntries.begin()) {}
        
        inline t_atom getDataAtom(long idx, long column) const  { return getData(idx, column).getAtom(); }
        inline CustomAtom getData(long idx, long column) const  { return mIterator[idx * mNumColumns + column]; }

    private:
        
        const long mNumColumns;
        const std::vector<const CustomAtom>::iterator mIterator;
    };
    
    EntryDatabase(long numCols)     { mColumns.resize(numCols); }
    
    void reserve(long items);
    void clear();
    
    size_t numItems() const         { return mIdentifiers.size(); }
    size_t numColumns() const       { return mColumns.size(); }
    
    Accessor accessor() const       { return Accessor(*this); }
    
    void labelModes(long argc, t_atom *argv);
    void names(long argc, t_atom *argv);
    void addEntry(long argc, t_atom *argv);
    void removeEntry(long idx);
    
    t_symbol *getName(long idx) const       { return mColumns[idx].mName; }
    bool getLabelMode(long idx) const       { return mColumns[idx].mLabel; }
    t_atom getIdentifier(long idx) const    { return getIdentifierInternal(idx).getAtom(); }
    
    long columnFromSpecifier(const t_atom *specifier) const;
    
    long itemFromIdentifier(const t_atom *identifier) const
    {
        long order;
        return searchIdentifiers(CustomAtom(identifier, false), order);
    }

    inline CustomAtom getData(long idx, long column) const     { return mEntries[idx * numColumns() + column]; }
    inline t_atom getDataAtom(long idx, long column) const     { return getData(idx, column).getAtom(); }
    
private:

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
