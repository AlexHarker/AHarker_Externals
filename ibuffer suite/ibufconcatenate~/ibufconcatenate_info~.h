
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

// The ibufconcatenate_info object

#define MAX_ITEMS 8192

typedef struct _ibufconcatenate_attach
{
    t_pxobject x_obj;
	
	t_symbol *buffer_name;
	
	double *starts;
	double *ends;

	long samp_offset;
    
	long num_items;
	
	long user_count;
	
} t_ibufconcatenate_info;


t_class *attach_class;

static void ibufconcatenate_info_setup();
void *ibufconcatenate_info_new(t_symbol *buffer_name);
void ibufconcatenate_info_free(t_ibufconcatenate_info *x);

static void ibufconcatenate_info_setup()
{
    attach_class = class_new ("ibufconcatenate_info~",
                              (method) ibufconcatenate_info_new,
                              (method)ibufconcatenate_info_free,
                              sizeof(t_ibufconcatenate_info),
                              0L,
                              A_SYM,
                              0);
    
    
    class_register(CLASS_NOBOX, attach_class);
}

void *ibufconcatenate_info_new(t_symbol *buffer_name)
{
    t_ibufconcatenate_info *x = (t_ibufconcatenate_info *)object_alloc (attach_class);
    
    // Set default variables (and allocate some memory)
    
    x->buffer_name = buffer_name;
    x->num_items = 0;
    x->samp_offset = 0;
    x->user_count = 0;
    
    x->starts = static_cast<double *>(malloc(sizeof(double) * MAX_ITEMS * 2));
    x->ends = x->starts + MAX_ITEMS;
    
    // Register the buffer name in a unique namespace
    
    object_register(gensym("ibufconcatenate_attachment_namespace"), buffer_name, x);
    
    return(x);
}

void ibufconcatenate_info_free(t_ibufconcatenate_info *x)
{
    object_unregister(x);
    free(x->starts);
}

// Helper functions to create and deal with the info

static __inline t_ibufconcatenate_info *new_ibufconcatenate_info(t_symbol *name)
{
	t_ibufconcatenate_info *registered;
	t_atom argv[1];
	atom_setsym(argv, name);
	
	// first make sure the attachment object is loaded

	if (!class_findbyname(CLASS_NOBOX, gensym("ibufconcatenate_attach~")))
		ibufconcatenate_info_setup();
	
	// Search for or create it
	
	registered = static_cast<t_ibufconcatenate_info *>(object_findregistered (gensym("ibufconcatenate_info_namespace"), name));
	
	if (!registered)
		registered = static_cast<t_ibufconcatenate_info *>(object_new_typed(CLASS_NOBOX, gensym("ibufconcatenate_info~"), 1, argv));

	if (registered)
		registered->user_count++;
		
	return registered;
}

static __inline void detach_ibufconcatenate_info(t_ibufconcatenate_info *attachment)
{		
	if (!attachment)
		return;
	
	if (--(attachment->user_count) == 0)
		object_free(attachment);
}


#endif	/*_IBUF_CONCATENATE_INFO__ */
