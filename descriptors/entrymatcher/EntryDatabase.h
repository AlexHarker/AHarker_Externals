
#ifndef ENTRYDATABASE_H
#define ENTRYDATABASE_H

#include "ext.h"
#include <vector>

struct FloatSym
{
    enum CompareResult { kLess, kGreater, kEqual };
    enum Type { kSymbol, kDouble, kTranlatedInt, kInt  };
    
    FloatSym() :  mType(kDouble), mValue(0.0) {}
    FloatSym(double val) : mType(kDouble), mValue(val) {}
    FloatSym(t_symbol *sym) : mType(kSymbol), mSymbol(sym) {}
    
    FloatSym(t_atom_long val, bool translate = true) : mType(translate ? kInt : kTranlatedInt)
    {
        if (translate)
            mValue = val;
        else
            mInt = val;
    }
    
    FloatSym(const t_atom *a, bool translate = true)
    {
        switch (atom_gettype(a))
        {
            case A_SYM:     *this = FloatSym(atom_getsym(a));               break;
            case A_FLOAT:   *this = FloatSym(atom_getfloat(a));             break;
            case A_LONG:    *this = FloatSym(atom_getlong(a), translate);   break;

            default:
                *this = FloatSym();
        }
    }
    
    t_atom inline getAtom() const
    {
        t_atom a;
        
        switch (mType)
        {
            case kSymbol:           atom_setsym(&a, mSymbol);   break;
            case kDouble:           atom_setfloat(&a, mValue);  break;
            case kInt:              atom_setlong(&a, mInt);     break;
            case kTranlatedInt:     atom_setlong(&a, mValue);   break;
        }
        
        return a;
    }
    
    friend inline CompareResult compare(const FloatSym& a, const FloatSym& b)
    {
        if (a.mType == b.mType)
        {
            switch (a.mType)
            {
                case FloatSym::kDouble:
                case FloatSym::kTranlatedInt:   return a.mValue == b.mValue ? kEqual : a.mValue < b.mValue ? kLess : kGreater;
                case FloatSym::kInt:            return a.mInt == b.mInt ? kEqual : a.mInt < b.mInt ? kLess : kGreater;
                case FloatSym::kSymbol:         return a.mSymbol == b.mSymbol ? kEqual :a.mSymbol < b.mSymbol ? kLess : kGreater;
            }
        }
        
        return (a.mType < b.mType) ? kLess : kGreater;
    }
    
    Type mType;
    union
    {
        double mValue;
        t_atom_long mInt;
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
        mOrder.reserve(items);
        mEntries.reserve(items * numColumns());
    }
    
    void clear()
    {
        mEntries.clear();
        mIdentifiers.clear();
        mOrder.clear();
    }
    
    size_t numItems() const         { return mIdentifiers.size(); }
    size_t numColumns() const       { return mColumns.size(); }
    
    Accessor accessor() const       { return Accessor(*this); }
    
    void labelModes(long argc, t_atom *argv);
    void names(long argc, t_atom *argv);
    void entry(long argc, t_atom *argv);
    void remove(long idx);
    
    t_symbol *getName(long idx) const       { return mColumns[idx].mName; }
    bool getLabelMode(long idx) const       { return mColumns[idx].mLabel; }
    t_atom getIdentifier(long idx) const    { return getIdentifierInternal(idx).getAtom(); }
    
    void lookup(std::vector<t_atom>& output, long idx, long argc, t_atom *argv) const;
    long columnFromSpecifier(const t_atom *specifier) const;
    
    long itemFromIdentifier(const t_atom *identifier) const
    {
        long order;
        return searchIdentifiers(FloatSym(identifier, false), order);
    }

    inline FloatSym getData(long idx, long column) const       { return mEntries[idx * numColumns() + column]; }
    inline t_atom getDataAtom(long idx, long column) const     { return getData(idx, column).getAtom(); }
    
private:

    long searchIdentifiers(const FloatSym& identifier, long& idx) const;

    FloatSym getIdentifierInternal(long idx) const                      { return mIdentifiers[idx];}
    inline void setData(long idx, long column, const FloatSym& data)    { mEntries[idx * numColumns() + column] = data; }
    
    // Data
    
    std::vector<ColumnInfo> mColumns;
    std::vector<FloatSym> mIdentifiers;
    std::vector<long> mOrder;
    std::vector<FloatSym> mEntries;
};


#endif
