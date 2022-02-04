
/*
 *  ibufconcatenate_attach.h
 *
 *  A header file containing the ibufconcatenate_info object.
 *  At it is small this is compiled into objects using it to avoid an extra object file
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#ifndef _IBUFCONCATENATE_INFO_HPP_
#define _IBUFCONCATENATE_INFO_HPP_


#include <ext.h>
#include <ext_obex.h>

#include <algorithm>
#include <vector>
#include <utility>

#include <AH_Lifecycle.hpp>
#include <AH_Locks.hpp>
#include <AH_Private_Object.hpp>


// Class and Object Structure

t_class *info_class;

class t_ibufconcatenate_info
{
public:
    
    // Constructor
    
    t_ibufconcatenate_info(t_symbol* name)
    : buffer_name(name), max_end(0)
    {}
    
    // Class for Read Access

    class read_access : private read_lock_hold
    {
    public:
        
        read_access(const t_ibufconcatenate_info *info)
        : read_lock_hold(&info->m_lock), m_info(info)
        {}
        
        bool get_item(long item, double &beg, double &end) const
        {
            return m_info->get_item(item, beg, end);
        }
        
        t_symbol *buffername() const    { return m_info->buffername(); }
        long size() const               { return m_info->size(); }
        double get_max_end() const      { return m_info->get_max_end(); }
        
        const t_ibufconcatenate_info *m_info;
    };
    
    // Class for Write Access
    
    class write_access : private write_lock_hold
    {
    public:
        
        write_access(t_ibufconcatenate_info *info)
        : write_lock_hold(&info->m_lock), m_info(info)
        {}
        
        void clear() { m_info->clear(); }
        
        long add_item(double beg, double end)
        {
            return m_info->add_item(beg, end);
        }
        
        bool get_item(long item, double &beg, double &end) const
        {
            return m_info->get_item(item, beg, end);
        }
        
        t_symbol *buffername() const    { return m_info->buffername(); }
        long size() const               { return m_info->size(); }
        double get_max_end() const      { return m_info->get_max_end(); }
        
        t_ibufconcatenate_info *m_info;
    };

private:
    
    // Implementation
    
    void clear()
    {
        timings.clear();
        max_end = 0.0;
    }
    
    long add_item(double beg, double end)
    {
        if (beg > end)
            std::swap(beg, end);
        
        timings.push_back({beg, end});
        
        max_end = std::max(max_end, end);
        
        return size() - 1;
    }
    
    bool get_item(long item, double &beg, double &end) const
    {
        if (item < 0 || item >= size())
            return false;
        
        beg = timings[item].first;
        end = timings[item].second;
        
        return true;
    }
    
    t_symbol *buffername() const
    {
        return buffer_name;
    }
    
    long size() const
    {
        return static_cast<long>(timings.size());
    }
    
    double get_max_end() const
    {
        return max_end;
    }

public:
    
    // N.B. This will be accessed by the Max API
    
    t_pxobject x_obj;
    t_private_count m_count;
        
private:

    t_symbol *buffer_name;
    
    std::vector<std::pair<double, double>> timings;
    
    double max_end;
        
    mutable read_write_lock m_lock;
};

// Function Prototypes

static void ibufconcatenate_info_setup();
void *ibufconcatenate_info_new(t_symbol *buffer_name);
void ibufconcatenate_info_free(t_ibufconcatenate_info *x);

// Namespace Utility

static t_symbol *get_namespace()
{
    static t_symbol *namespace_sym = gensym("ibufconcatenate_info_namespace");
    
    return namespace_sym;
}

// Setup (like main)

static void ibufconcatenate_info_setup()
{
    info_class = class_new("ibufconcatenate_info",
                           (method) ibufconcatenate_info_new,
                           (method) ibufconcatenate_info_free,
                           sizeof(t_ibufconcatenate_info),
                           (method) nullptr,
                           A_SYM,
                           0);
    
    
    class_register(CLASS_NOBOX, info_class);
}

// New / Free

void *ibufconcatenate_info_new(t_symbol *buffer_name)
{
    t_ibufconcatenate_info *x = (t_ibufconcatenate_info *) object_alloc(info_class);
    
    // Create the struct fully
    
    create_object(*x, buffer_name);
    
    return x;
}

void ibufconcatenate_info_free(t_ibufconcatenate_info *x)
{
    object_unregister(x);
    destroy_object(*x);
}

// Attach / Detach

static inline t_ibufconcatenate_info *attach_ibufconcatenate_info(t_symbol *name, t_ibufconcatenate_info *prev)
{
    t_ibufconcatenate_info *registered;
    t_atom argv[1];
    atom_setsym(argv, name);
    
    t_symbol *object_name = gensym("ibufconcatenate_info");
    
    // first make sure the attachment object is loaded
    
    if (!class_findbyname(CLASS_NOBOX, object_name))
        ibufconcatenate_info_setup();
    
    // Search for or create it
    
    registered = private_object_find_retain(prev, name, get_namespace());
    
    if (!registered)
        registered = private_object_create<t_ibufconcatenate_info>(object_name, name, get_namespace(), 1, argv);
    
    return registered;
}

static inline void detach_ibufconcatenate_info(t_ibufconcatenate_info *attachment)
{
    private_object_release(attachment);
}

#endif /*_IBUFCONCATENATE_INFO_HPP_ */

