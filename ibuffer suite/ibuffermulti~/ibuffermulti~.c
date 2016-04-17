
/*
 *  ibuffermulti~
 *
 *	ibuffermulti~ can be used to dynamically load audio files into multiple ibuffer~s.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


// FIX - threadsafety for resizing

#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>


void *this_class;


#define BUFFER_GROW_SIZE 512


typedef struct _ibuffermulti
{
    t_pxobject x_obj;
	
	t_object **buffers;
	
	long num_items;
    long max_num_items;
	
	void *number_out;	
	
} t_ibuffermulti;


t_symbol *ps_set;
t_symbol *ps_replace;
t_symbol *ps_valid;


void *ibuffermulti_new ();
void ibuffermulti_free (t_ibuffermulti *x);
void ibuffermulti_assist (t_ibuffermulti *x, void *b, long m, long a, char *s);

void ibuffermulti_clear (t_ibuffermulti *x);
void ibuffermulti_doclear(t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv);
void ibuffermulti_load (t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv);
void ibuffermulti_doload (t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv);


int C74_EXPORT main (void)
{
	this_class = class_new ("ibuffermulti~", (method) ibuffermulti_new, (method)ibuffermulti_free, sizeof(t_ibuffermulti), 0L, 0);
	
	class_addmethod (this_class, (method)ibuffermulti_clear, "clear", 0);
	class_addmethod (this_class, (method)ibuffermulti_load, "load", A_GIMME, 0);
	class_addmethod (this_class, (method)ibuffermulti_assist, "assist", A_CANT, 0);
	
	class_dspinit (this_class);
	class_register (CLASS_BOX, this_class);
	
	ps_set = gensym("set");
	ps_replace = gensym("replace");
	ps_valid = gensym("valid");
	
	return 0;
}


void *ibuffermulti_new ()
{	
    t_ibuffermulti *x = (t_ibuffermulti *) object_alloc (this_class);
	
	dsp_setup((t_pxobject *)x, 0);
	
	x->num_items =  0;
    x->max_num_items =  0;
	x->number_out = intout(x);
	
    x->buffers = (t_object **) malloc(BUFFER_GROW_SIZE * sizeof(t_object *));
    
    return (x);
}


void ibuffermulti_free (t_ibuffermulti *x)
{
	ibuffermulti_clear(x);
	dsp_free(&x->x_obj);
}


void ibuffermulti_assist (t_ibuffermulti *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET) 
	{
		sprintf(s,"Num of Buffers Loaded");
	}
    else 
	{
		sprintf(s,"File Operations");
    }
}


void ibuffermulti_clear (t_ibuffermulti *x)
{
	defer(x, (method) ibuffermulti_doclear, 0, 0, 0);
}


void ibuffermulti_doclear (t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv)
{
	for (long i = 0; i < x->num_items; i++)
		object_free(x->buffers[i]);
	
	x->num_items = 0;
	outlet_int(x->number_out, 0);
}


void ibuffermulti_load (t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv)
{
	if (argc)
		defer(x, (method) ibuffermulti_doload, s, argc, argv);
}


void ibuffermulti_doload (t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv)
{	
	t_object *current_buffer;
	t_atom return_val;
	long *valid_val;

	// First make sure the ibuffer object is loaded
	
	if (!class_findbyname(CLASS_BOX, gensym("ibuffer~")))
	{
		void *force_load = newinstance(gensym("ibuffer~"), 0, 0);
		if (force_load)
			freeobject(force_load);
	}
	
    if (x->num_items >= x->max_num_items)
    {
        t_object **grow_buffers = realloc(x->buffers, (x->max_num_items + BUFFER_GROW_SIZE) * sizeof(t_object *));
        
        if (!grow_buffers)
        {
            object_error((t_object *) x, "could not allocate space for new ibuffer~s");
            return;
        }

        x->buffers = grow_buffers;
        x->max_num_items += BUFFER_GROW_SIZE;
    }
    
	// Return a properly allocated object (may still return zero if the .mxo is not present)
	
	x->buffers[x->num_items] = current_buffer = object_new_typed(CLASS_BOX, gensym("ibuffer~"), 0, 0);
	
	// Now load the buffer 
	
	if (current_buffer && argc > 1 && atom_gettype(argv) == A_SYM && atom_gettype(argv + 1) == A_SYM)
	{
		// Set name, then load file - finally check validity
		
		object_method_typed(current_buffer, ps_set, 1, argv, &return_val);
		object_method_typed(current_buffer, ps_replace, argc - 1, argv + 1, &return_val);		
		valid_val = (long *) object_method(current_buffer, ps_valid);
		
		if (valid_val && *valid_val)
			x->num_items++;
		else
		{
			if (!valid_val)
				post ("invalid ibuffer~ object");
			freeobject(current_buffer);
		}
	}
	
	outlet_int(x->number_out, x->num_items);
}
