
/*
 *  ibufconcatenate_attach~
 *
 *	The ibufconcatenate_attach~ object is a no box class that is used to attach ibufconcatenate objects to a buffer name and store date.
 *	This means different objects can share the same metadata about a buffer's contents.
 *	It is loaded automatically by the objects requiring it, but must be present in the search path.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include "ibufconcatenate_attach~.h"


void *attach_class;


void *ibufconcatenate_attach_new(t_symbol *buffer_name);
void ibufconcatenate_attach_free(t_ibufconcatenate_attach *x);

int main (void)
{
    attach_class = class_new ("ibufconcatenate_attach~",
							(method) ibufconcatenate_attach_new, 
							(method)ibufconcatenate_attach_free, 
							sizeof(t_ibufconcatenate_attach), 
							0L,
							A_SYM,
							0);
	
	
	class_register(CLASS_NOBOX, attach_class);
	
	return 0;
}


void *ibufconcatenate_attach_new(t_symbol *buffer_name)
{
    t_ibufconcatenate_attach *x = (t_ibufconcatenate_attach *)object_alloc (attach_class);
	
	// Set default variables (and allocate some memory)
	
	x->buffer_name = buffer_name;
	x->num_items = 0;
	x->samp_offset = 0;
	x->user_count = 0;
	
	x->starts = malloc(sizeof(double) * MAX_ITEMS * 2);
	x->ends = x->starts + MAX_ITEMS;
	
	// Register the buffer name in a unique namespace
	
	object_register(gensym("ibufconcatenate_attachment_namespace"), buffer_name, x);
	
	return(x);
}


void ibufconcatenate_attach_free(t_ibufconcatenate_attach *x)
{
	object_unregister(x);
	free(x->starts);
}



