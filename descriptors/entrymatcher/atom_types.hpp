
#ifndef ATOM_TYPES_HPP
#define ATOM_TYPES_HPP

#include <ext.h>
#include <vector>
#include <string>

// Atom Without Type Information

union t_untyped_atom
{
    // Constructors from Different Types
    
    t_untyped_atom() : m_integer(0) {}
    t_untyped_atom(double val) : m_value(val) {}
    t_untyped_atom(t_atom_long val) : m_integer(val) {}
    t_untyped_atom(t_symbol *val) : m_symbol(val) {}
    
    // Casts To Possible Types
    
    operator double() const         { return m_value; }
    operator t_atom_long() const    { return m_integer; }
    operator t_symbol *() const     { return m_symbol; }
    
    // Data
    
    double m_value;
    t_atom_long m_integer;
    t_symbol *m_symbol;
};

// An Atom with Type Information

struct t_custom_atom
{
    // Definitions
    
    enum CompareResult { kLess, kGreater, kEqual };
    enum Type : unsigned char { kSymbol, kDouble, kTranslatedInt, kInt  };
    
    // Constructors from Different Types

    t_custom_atom() : m_data(0.0), m_type(kDouble) {}
    t_custom_atom(double val) : m_data(val), m_type(kDouble) {}
    t_custom_atom(t_symbol *sym) : m_data(sym), m_type(kSymbol) {}
    
    t_custom_atom(t_atom_long val, bool translate = true) : m_type(translate ? kTranslatedInt : kInt)
    {
        if (translate)
            m_data.m_value = static_cast<double>(val);
        else
            m_data.m_integer = val;
    }
    
    t_custom_atom(const t_atom *a, bool translate = true)
    {
        switch (atom_gettype(a))
        {
            case A_SYM:     *this = t_custom_atom(atom_getsym(a));                 break;
            case A_FLOAT:   *this = t_custom_atom(atom_getfloat(a));               break;
            case A_LONG:    *this = t_custom_atom(atom_getlong(a), translate);     break;
                
            default:
                *this = t_custom_atom();
        }
    }
    
    t_custom_atom(const t_untyped_atom& a, const Type type) : m_data(a), m_type(type) {}
    
    // Get as Standard Atom
    
    void inline get_atom(t_atom *a) const
    {
        switch (m_type)
        {
            case kSymbol:           atom_setsym(a, m_data.m_symbol);    break;
            case kDouble:           atom_setfloat(a, m_data.m_value);   break;
            case kInt:              atom_setlong(a, m_data.m_integer);      break;
            case kTranslatedInt:    atom_setlong(a, static_cast<t_atom_long>(m_data.m_value));    break;
        }
    }
    
    // Get As a String
    
    std::string inline get_string() const
    {
        switch (m_type)
        {
            case kSymbol:           return m_data.m_symbol->s_name;
            case kDouble:           return std::to_string(m_data.m_value);
            case kInt:              return std::to_string(m_data.m_integer);
            case kTranslatedInt:    return std::to_string((t_atom_long) m_data.m_value);
        }
    }
    
    // Compare t_custom_atoms
    
    friend inline CompareResult compare(const t_custom_atom& a, const t_custom_atom& b)
    {
        if (a.m_type == b.m_type)
        {
            switch (a.m_type)
            {
                case kDouble:
                case kTranslatedInt:  return a.m_data.m_value == b.m_data.m_value ? kEqual : a.m_data.m_value < b.m_data.m_value ? kLess : kGreater;
                case kInt:            return a.m_data.m_integer == b.m_data.m_integer ? kEqual : a.m_data.m_integer < b.m_data.m_integer ? kLess : kGreater;
                case kSymbol:         return a.m_data.m_symbol == b.m_data.m_symbol ? kEqual :a.m_data.m_symbol < b.m_data.m_symbol ? kLess : kGreater;
            }
        }
        
        return (a.m_type < b.m_type) ? kLess : kGreater;
    }
    
    bool operator < (const t_custom_atom& a)
    {
        Type type1 = m_type == kTranslatedInt ? kDouble : m_type;
        Type type2 = a.m_type == kTranslatedInt ? kDouble : a.m_type;

        if (type1 == type2)
        {
            switch (m_type)
            {
                case kDouble:
                case kTranslatedInt:  return m_data.m_value < a.m_data.m_value;
                case kInt:            return m_data.m_integer < a.m_data.m_integer;
                case kSymbol:         return strcmp(m_data.m_symbol->s_name, a.m_data.m_symbol->s_name) < 0;
            }
        }
        
        if (type1 == kSymbol) return true;
        if (type2 == kSymbol) return false;
        
        if (type1 == kInt)
            return m_data.m_integer < a.m_data.m_value;
        else
            return m_data.m_value < a.m_data.m_integer;
    }
    
    bool operator > (const t_custom_atom& a)
    {
        Type type1 = m_type == kTranslatedInt ? kDouble : m_type;
        Type type2 = a.m_type == kTranslatedInt ? kDouble : a.m_type;
        
        if (type1 == type2)
        {
            switch (m_type)
            {
                case kDouble:
                case kTranslatedInt:  return m_data.m_value > a.m_data.m_value;
                case kInt:            return m_data.m_integer > a.m_data.m_integer;
                case kSymbol:         return strcmp(m_data.m_symbol->s_name, a.m_data.m_symbol->s_name) > 0;
            }
        }
        
        if (type1 == kSymbol) return false;
        if (type2 == kSymbol) return true;
        
        if (type1 == kInt)
            return m_data.m_integer > a.m_data.m_value;
        else
            return m_data.m_value > a.m_data.m_integer;
    }
    
    // Casts To Possible Types

    operator double() const         { return m_data.m_value; }
    operator t_atom_long() const    { return m_data.m_integer; }
    operator t_symbol *() const     { return m_data.m_symbol; }
    
    // Data
    
    t_untyped_atom m_data;
    Type m_type;
};

#endif
