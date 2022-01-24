
#ifndef ATOM_TYPES_HPP
#define ATOM_TYPES_HPP

#include <ext.h>
#include <functional>
#include <vector>
#include <string>

// Untyped Atom (condensed memory storage - the type will likely be stored separately)

union t_untyped_atom
{
    // Constructors
    
    t_untyped_atom()                : m_integer(0) {}
    t_untyped_atom(double val)      : m_value(val) {}
    t_untyped_atom(t_atom_long val) : m_integer(val) {}
    t_untyped_atom(t_symbol *val)   : m_symbol(val) {}
    
    // Reads (typed)

    template <typename T>
    T as() const;

    template<>
    double as() const       { return m_value; }

    template<>
    t_atom_long as() const  { return m_integer; }
    
    template<>
    t_symbol *as() const    { return m_symbol; }
    
    template<>
    const char *as() const  { return m_symbol->s_name; }
    
    t_atom_long deconform() const { return static_cast<t_atom_long>(m_value); }
    
    // Data
    
    double m_value;
    t_atom_long m_integer;
    t_symbol *m_symbol;
};

// An Custom Atom with Type Information

struct t_custom_atom
{
    // Definitions
               
    enum class ordering { lower, higher, equal };
    enum class category : unsigned char { floating, integral, conformed, symbolic  };
    
    // Constructors

    t_custom_atom()                 : m_data(0.0), m_type(category::floating) {}
    t_custom_atom(double val)       : m_data(val), m_type(category::floating) {}
    t_custom_atom(t_symbol *sym)    : m_data(sym), m_type(category::symbolic) {}
    
    t_custom_atom(t_atom_long val, bool conform = true)
    : m_type(conform ? category::conformed : category::integral)
    {
        if (conform)
            m_data.m_value = static_cast<double>(val);
        else
            m_data.m_integer = val;
    }
    
    t_custom_atom(const t_untyped_atom& a, const category type)
    : m_data(a), m_type(type) {}
    
    t_custom_atom(const t_atom *a, bool conform = true)
    {
        switch (atom_gettype(a))
        {
            case A_SYM:     *this = t_custom_atom(atom_getsym(a));                 break;
            case A_FLOAT:   *this = t_custom_atom(atom_getfloat(a));               break;
            case A_LONG:    *this = t_custom_atom(atom_getlong(a), conform);       break;
            default:        *this = t_custom_atom();
        }
    }
    
    // Symbol Check
    
    bool is_symbol() const { return m_type == category::symbolic; }
    
    // Typed Reads (force type)

    template <typename T>
    T as() const { return m_data.as<T>(); }
    
    t_atom_long deconform() const { return m_data.deconform(); }
    
    // Get Atom
    
    void inline get_atom(t_atom *a) const
    {
        switch (m_type)
        {
            case category::floating:    atom_setfloat(a, as<double>());         break;
            case category::integral:    atom_setlong(a, as<t_atom_long>());     break;
            case category::conformed:   atom_setlong(a, deconform());           break;
            case category::symbolic:    atom_setsym(a, as<t_symbol *>());       break;
            default:                    atom_setfloat(a, 0.0);
        }
    }
    
    // Get as String
    
    std::string inline get_string() const
    {
        switch (m_type)
        {
            case category::floating:    return std::to_string(as<double>());
            case category::integral:    return std::to_string(as<t_atom_long>());
            case category::conformed:   return std::to_string(deconform());
            case category::symbolic:    return as<const char *>();
            default:                    return {};
        }
    }
    
    // Comparisons and ordering (compare is consistent across runs, but ordering is faster)
    
    friend inline ordering order(const t_custom_atom& a, const t_custom_atom& b)
    {
        if (a.m_type == b.m_type)
        {
            switch (a.m_type)
            {
                case category::integral:    return order(a.as<t_atom_long>(), b.as<t_atom_long>());
                case category::symbolic:    return order(a.as<t_symbol *>(), b.as<t_symbol *>());
                default:                    return order(a.as<double>(), b.as<double>());

            }
        }
        
        return (a.m_type < b.m_type) ? ordering::lower : ordering::higher;
    }
    
    bool operator < (const t_custom_atom& a) { return comparison<std::less>(a); }
    bool operator > (const t_custom_atom& a) { return comparison<std::greater>(a); }

    // Data

    t_untyped_atom m_data;
    category m_type;
    
private:

    // Implementation (ordering and comparisons)
    
    template <typename T>
    static ordering order(const T& a, const T& b)
    {
        return a == b ? ordering::equal : a < b ? ordering::lower : ordering::higher;
    }
    
    template <template <typename U> class Op>
    bool comparison(const t_custom_atom& a)
    {
        category type1 = m_type == category::conformed ? category::floating : m_type;
        category type2 = a.m_type == category::conformed ? category::floating : a.m_type;

        if (type1 == type2)
        {
            switch (m_type)
            {
                case category::integral:    return Op<double>()(as<t_atom_long>(), a.as<t_atom_long>());
                case category::symbolic:    return Op<int>()(strcmp(as<const char *>(), a.as<const char *>()), 0);
                default:                    return Op<double>()(as<double>(), a.as<double>());
            }
        }
        
        if (type1 == category::symbolic)
            return Op<int>()(0, 1);
        
        if (type2 == category::symbolic)
            return Op<int>()(1, 0);
        
        if (type1 == category::integral)
            return Op<double>()(static_cast<double>(as<t_atom_long>()), as<double>());
        else
            return Op<double>()(as<double>(), static_cast<double>(as<t_atom_long>()));
    }
};

#endif
