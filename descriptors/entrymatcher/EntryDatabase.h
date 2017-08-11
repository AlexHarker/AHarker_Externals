
#ifndef ENTRYDATABASE_H
#define ENTRYDATABASE_H

#include "ext.h"
#include <vector>

struct FloatSym
{
    enum Type { kSymbol, kDouble, kInt };
    
    FloatSym() :  mType(kDouble), mValue(0.0) {}
    FloatSym(double val) : mType(kDouble), mValue(val) {}
    FloatSym(t_atom_long val) : mType(kInt), mValue(val) {}
    FloatSym(t_symbol *sym) : mType(kSymbol), mSymbol(sym) {}
    FloatSym(const t_atom *a) { *this = atom_gettype(a) == A_SYM ? FloatSym(atom_getsym(a)) : FloatSym(atom_getfloat(a)); }
    
    t_atom getAtom() const
    {
        t_atom a;
        
        switch (mType)
        {
            case kSymbol:   atom_setsym(&a, mSymbol);   break;
            case kDouble:   atom_setfloat(&a, mValue);  break;
            case kInt:      atom_setlong(&a, mValue);   break;

        }
        
        return a;
    }
    
    Type mType;
    union {
        double mValue;
        t_symbol *mSymbol;
    };
};

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
        
        inline FloatSym getData(long idx, long column) const { return mIterator[idx * mNumColumns + column]; }

    private:
        
        const long mNumColumns;
        const std::vector<const FloatSym>::iterator mIterator;
    };
    
    EntryDatabase(long numCols)     { mColumns.resize(numCols); }
    
    void reserve(long items)
    {
        mIdentifiers.reserve(items);
        mEntries.reserve(items * numColumns());
    }
    
    void clear()
    {
        mEntries.clear();
        mIdentifiers.clear();
    }
    
    size_t numItems() const         { return mIdentifiers.size(); }
    size_t numColumns() const       { return mColumns.size(); }
    
    Accessor accessor() const       { return Accessor(*this); }
    
    void labelModes(long argc, t_atom *argv);
    void names(long argc, t_atom *argv);
    void entry(long argc, t_atom *argv);
    
    t_symbol *getName(long idx) const       { return mColumns[idx].mName; }
    bool getLabelMode(long idx) const       { return mColumns[idx].mLabel; }
    t_atom getIdentifier(long idx) const    { return mIdentifiers[idx]; }
    
    void lookup(std::vector<t_atom>& output, long idx, long argc, t_atom *argv) const;
    long itemFromIdentifier(t_atom& identifier) const;
    long columnFromSpecifier(const t_atom& specifier) const;

    inline FloatSym getData(long idx, long column) const       { return mEntries[idx * numColumns() + column]; }
    inline t_atom getDataAtom(long idx, long column) const     { return getData(idx, column).getAtom(); }
    
private:

    bool compareIdentifiers(const t_atom& identifier1, const t_atom& identifier2) const;

    inline void addItem(const t_atom& identifier)
    {
        long size = numItems();
        mEntries.resize((size + 1) * numColumns());
        mIdentifiers.push_back(identifier);
    }
    
    inline void setData(long idx, long column, const FloatSym& data)   { mEntries[idx * numColumns() + column] = data; }
    
    // Data
    
    std::vector<ColumnInfo> mColumns;
    std::vector<t_atom> mIdentifiers;
    std::vector<FloatSym> mEntries;
};


#endif
