
#ifndef CUSTOMATOM_H
#define CUSTOMATOM_H

#include "ext.h"
#include <vector>
#include <string>

union UntypedAtom
{
    UntypedAtom() : mInt(0) {}
    UntypedAtom(double val) : mValue(val) {}
    UntypedAtom(t_atom_long val) : mInt(val) {}
    UntypedAtom(t_symbol *val) : mSymbol(val) {}
    
    operator double() const         { return mValue; }
    operator t_atom_long() const    { return mInt; }
    operator t_symbol *() const     { return mSymbol; }
    
    double mValue;
    t_atom_long mInt;
    t_symbol *mSymbol;
};

struct CustomAtom
{
    enum CompareResult { kLess, kGreater, kEqual };
    enum Type : unsigned char { kSymbol, kDouble, kTranslatedInt, kInt  };
    
    CustomAtom() : mData(0.0), mType(kDouble) {}
    CustomAtom(double val) : mData(val), mType(kDouble) {}
    CustomAtom(t_symbol *sym) : mData(sym), mType(kSymbol) {}
    
    CustomAtom(t_atom_long val, bool translate = true) : mType(translate ? kTranslatedInt : kInt)
    {
        if (translate)
            mData.mValue = val;
        else
            mData.mInt = val;
    }
    
    CustomAtom(const t_atom *a, bool translate = true)
    {
        switch (atom_gettype(a))
        {
            case A_SYM:     *this = CustomAtom(atom_getsym(a));                 break;
            case A_FLOAT:   *this = CustomAtom(atom_getfloat(a));               break;
            case A_LONG:    *this = CustomAtom(atom_getlong(a), translate);     break;
                
            default:
                *this = CustomAtom();
        }
    }
    
    CustomAtom(const UntypedAtom& a, const Type type) : mData(a), mType(type) {}
    
    void inline getAtom(t_atom *a) const
    {
        switch (mType)
        {
            case kSymbol:           atom_setsym(a, mData.mSymbol);    break;
            case kDouble:           atom_setfloat(a, mData.mValue);   break;
            case kInt:              atom_setlong(a, mData.mInt);      break;
            case kTranslatedInt:    atom_setlong(a, mData.mValue);    break;
        }
    }
    
    std::string inline getString() const
    {
        switch (mType)
        {
            case kSymbol:           return mData.mSymbol->s_name;
            case kDouble:           return std::to_string(mData.mValue);
            case kInt:              return std::to_string(mData.mInt);
            case kTranslatedInt:    return std::to_string((t_atom_long) mData.mValue);
        }
    }
    
    friend inline CompareResult compare(const CustomAtom& a, const CustomAtom& b)
    {
        if (a.mType == b.mType)
        {
            switch (a.mType)
            {
                case kDouble:
                case kTranslatedInt:  return a.mData.mValue == b.mData.mValue ? kEqual : a.mData.mValue < b.mData.mValue ? kLess : kGreater;
                case kInt:            return a.mData.mInt == b.mData.mInt ? kEqual : a.mData.mInt < b.mData.mInt ? kLess : kGreater;
                case kSymbol:         return a.mData.mSymbol == b.mData.mSymbol ? kEqual :a.mData.mSymbol < b.mData.mSymbol ? kLess : kGreater;
            }
        }
        
        return (a.mType < b.mType) ? kLess : kGreater;
    }
    
    friend inline bool operator < (const CustomAtom& a, const CustomAtom& b)
    {
        if (a.mType == b.mType)
        {
            switch (a.mType)
            {
                case kDouble:
                case kTranslatedInt:  return a.mData.mValue < b.mData.mValue;
                case kInt:            return a.mData.mInt < b.mData.mInt;
                case kSymbol:         return a.mData.mSymbol < b.mData.mSymbol;
            }
        }
        
        return (a.mType < b.mType);
    }
    
    operator double() const         { return mData.mValue; }
    operator t_atom_long() const    { return mData.mInt; }
    operator t_symbol *() const     { return mData.mSymbol; }
    
    UntypedAtom mData;
    Type mType;
};

#endif
