
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
    
    FloatSym(t_atom *a)
    {
        if (atom_gettype(a) == A_SYM)
            *this = FloatSym(atom_getsym(a));
        else
            *this = FloatSym(atom_getfloat(a));
    }
    
    t_atom getAtom() const
    {
        t_atom a;
        
        switch (mType)
        {
            case kSymbol:   atom_setsym(&a, mSymbol); break;
            case kDouble:   atom_setfloat(&a, mValue); break;
            case kInt:   atom_setlong(&a, mValue); break;

        }
        
        return a;
    }
    
    Type mType;
    union {
        double mValue;
        t_symbol *mSymbol;
    };
};

class Matcher
{
    
public:

    enum TestType { kTestMatch, kTestLess, kTestGreater, kTestLessEqual, kTestGreaterEqual, kTestDistance, kTestRatio };
    
    Matcher(TestType type, long column, double scale = 1.0, bool reject = false) : mType(type), mColumn(column), mScale(scale), mReject(reject)
    {
        mValues.reserve(5);
    }
    
    inline bool match(const FloatSym& data, double& overallDistance) const;
    
    long getcolumn() const      { return mColumn; }
    void add(double value)      { mValues.push_back(value); }
    void add(t_symbol *value)   { mValues.push_back(value); }

private:
    
    template <typename Op> bool comparison(double value, Op op) const
    {
        for (std::vector<const FloatSym>::iterator it = mValues.begin(); it != mValues.end(); it++)
            if (op(value, (*it).mValue)) return true;
        
        return false;
    }
    
    TestType mType;
    long mColumn;
    double mScale;
    bool mReject;
    std::vector<FloatSym> mValues;
};


class EntryDatabase
{
    struct ColumnInfo
    {
        ColumnInfo() : mName(gensym("")), mLabel(false) {}
        
        t_symbol *mName;
        bool mLabel;
    };
    
    struct Entry
    {
        Entry(const t_atom& identifier, const long size)
        {
            t_atom zeroAtom;
            atom_setfloat(&zeroAtom, 0.0);
            
            mIdentifier = identifier;
            mData.resize(size);
        }
        
        t_atom mIdentifier;
        std::vector<FloatSym> mData;
    };
    
public:
    
    EntryDatabase(long numCols)     { mColumns.resize(numCols); }
    
    void reserve(long items)        { mEntries.reserve(items); }
    
    void clear()                    { mEntries.clear(); }
    size_t numItems() const         { return mEntries.size(); }
    size_t numColumns() const       { return mColumns.size(); }
    
    void labelModes(long argc, t_atom *argv);
    void names(long argc, t_atom *argv);
    void entry(long argc, t_atom *argv);
    
    t_symbol *getName(long idx) const       { return mColumns[idx].mName; }
    bool getLabelMode(long idx) const       { return mColumns[idx].mLabel; }
    t_atom getIdentifier(long idx) const    { return mEntries[idx].mIdentifier; }
    
    void lookup(std::vector<t_atom>& output, long idx, long argc, t_atom *argv) const;
    long itemFromIdentifier(t_atom& identifier) const;
    long columnFromSpecifier(const t_atom& specifier) const;
    long calculate(std::vector<Matcher>& matchers, std::vector<long>& indices, std::vector<double>& distances) const;

private:

    bool compareIdentifiers(const t_atom& identifier1, const t_atom& identifier2) const;

    // Data
    
    std::vector<ColumnInfo> mColumns;
    std::vector<Entry> mEntries;
};

#endif
