
/*
 *  ibufconcatenate_attach.h
 *
 *	A header file containing the ibufconcatenate_info object.
 *	At it is small this is compiled into objects using it to avoid an extra object file
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#ifndef _IBUF_CONCATENATE_INFO__
#define _IBUF_CONCATENATE_INFO__

#include <ext.h>
#include <ext_obex.h>

#include <algorithm>
#include <vector>
#include <utility>

#include <AH_Locks.hpp>


// The ibufconcatenate_info object

class t_ibufconcatenate_info
{
public:
 
    t_ibufconcatenate_info(t_symbol* name)
    : buffer_name(name), max_end(0), user_count(0)
    {}
    
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
    
    void acquire() { user_count++; }
    bool release() { return --user_count == 0; }
    
private:
    
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
    
    // FIX - this is currently public to avoid errors
    
public:
    
    t_pxobject x_obj;
	
private:

	t_symbol *buffer_name;
	
    std::vector<std::pair<double, double>> timings;

	double max_end;
    
    long user_count;

    mutable read_write_lock m_lock;    
};

t_class *info_class;

static void ibufconcatenate_info_setup();
void *ibufconcatenate_info_new(t_symbol *buffer_name);
void ibufconcatenate_info_free(t_ibufconcatenate_info *x);

static void ibufconcatenate_info_setup()
{
    info_class = class_new ("ibufconcatenate_info",
                              (method) ibufconcatenate_info_new,
                              (method)ibufconcatenate_info_free,
                              sizeof(t_ibufconcatenate_info),
                              0L,
                              A_SYM,
                              0);
    
    
    class_register(CLASS_NOBOX, info_class);
}

void *ibufconcatenate_info_new(t_symbol *buffer_name)
{
    t_ibufconcatenate_info *x = (t_ibufconcatenate_info *)object_alloc(info_class);
    
    // Create the struct fully
    
    new (x) t_ibufconcatenate_info(buffer_name);
    
    // Register the buffer name in a unique namespace
    
    object_register(gensym("ibufconcatenate_attachment_namespace"), buffer_name, x);
    
    return x;
}

void ibufconcatenate_info_free(t_ibufconcatenate_info *x)
{
    object_unregister(x);
    x->~t_ibufconcatenate_info();
}

// Helper functions to attach or create the info

static __inline t_ibufconcatenate_info *attach_ibufconcatenate_info(t_symbol *name)
{
	t_ibufconcatenate_info *registered;
	t_atom argv[1];
	atom_setsym(argv, name);
	
	// first make sure the attachment object is loaded

	if (!class_findbyname(CLASS_NOBOX, gensym("ibufconcatenate_info")))
		ibufconcatenate_info_setup();
	
	// Search for or create it
	
	registered = static_cast<t_ibufconcatenate_info *>(object_findregistered (gensym("ibufconcatenate_info_namespace"), name));
	
	if (!registered)
		registered = static_cast<t_ibufconcatenate_info *>(object_new_typed(CLASS_NOBOX, gensym("ibufconcatenate_info"), 1, argv));

	if (registered)
		registered->acquire();
		
	return registered;
}

static __inline void detach_ibufconcatenate_info(t_ibufconcatenate_info *attachment)
{		
	if (!attachment)
		return;
	
	if (attachment->release())
		object_free(attachment);
}


#endif	/*_IBUF_CONCATENATE_INFO__ */
