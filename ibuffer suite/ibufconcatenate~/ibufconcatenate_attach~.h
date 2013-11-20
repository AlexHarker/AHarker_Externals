
/*
 *  ibufconcatenate_attach.h
 *
 *	A header file for ibufconcatenate_attach.c.
 *	This code contains the object structure for the attachment object and routines for attaching and detaching to/from the object.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#ifndef _IBUF_CONCATENATE_ATTACH__
#define _IBUF_CONCATENATE_ATTACH__


#include <ext.h>
#include <ext_obex.h>


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
	
} t_ibufconcatenate_attach;


static __inline t_ibufconcatenate_attach *new_ibufconcatenate_attachment(t_symbol *name)
{
	t_ibufconcatenate_attach *registered;
	t_atom argv[1];
	atom_setsym(argv, name);
	
	// first make sure the attachment object is loaded

	if (!class_findbyname(CLASS_NOBOX, gensym("ibufconcatenate_attach~")))
	{
		void *forceload = newinstance(gensym("ibufconcatenate_attach~"), 1, argv);
		if (forceload)
			freeobject(forceload);
	}
	
	// Search for or create it (may still return zero if the .mxo is not present)
	
	registered = object_findregistered (gensym("ibufconcatenate_attachment_namespace"), name);
	
	if (!registered)
		registered = object_new_typed(CLASS_NOBOX, gensym("ibufconcatenate_attach~"), 1, argv);

	if (registered)
		registered->user_count++;
		
	return registered;
}


static __inline void detach_ibufconcatenate_attachment(t_ibufconcatenate_attach *attachment)
{		
	if (!attachment)
		return;
	
	if (--(attachment->user_count) == 0)
		object_free(attachment);
}


#endif	/*_IBUF_CONCATENATE_ATTACH__ */
