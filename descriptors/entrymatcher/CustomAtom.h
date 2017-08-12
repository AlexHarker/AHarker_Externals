
#ifndef CUSTOMATOM_H
#define CUSTOMATOM_H

#include "ext.h"
#include <vector>

struct CustomAtom
{
    enum CompareResult { kLess, kGreater, kEqual };
    enum Type { kSymbol, kDouble, kTranlatedInt, kInt  };
    
    CustomAtom() :  mType(kDouble), mValue(0.0) {}
    CustomAtom(double val) : mType(kDouble), mValue(val) {}
    CustomAtom(t_symbol *sym) : mType(kSymbol), mSymbol(sym) {}
    
    CustomAtom(t_atom_long val, bool translate = true) : mType(translate ? kInt : kTranlatedInt)
    {
        if (translate)
            mValue = val;
        else
            mInt = val;
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
    
    friend inline CompareResult compare(const CustomAtom& a, const CustomAtom& b)
    {
        if (a.mType == b.mType)
        {
            switch (a.mType)
            {
                case kDouble:
                case kTranlatedInt:   return a.mValue == b.mValue ? kEqual : a.mValue < b.mValue ? kLess : kGreater;
                case kInt:            return a.mInt == b.mInt ? kEqual : a.mInt < b.mInt ? kLess : kGreater;
                case kSymbol:         return a.mSymbol == b.mSymbol ? kEqual :a.mSymbol < b.mSymbol ? kLess : kGreater;
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

#endif
