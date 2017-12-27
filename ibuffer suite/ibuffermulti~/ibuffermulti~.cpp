
/*
 *  ibuffermulti~
 *
 *	ibuffermulti~ can be used to dynamically load audio files into multiple ibuffer~s.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


// N.B. - alterations to the object structure only happen in the low priority thread for threadsafety


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <vector>

t_class *this_class;


typedef struct _ibuffermulti
{
    t_pxobject x_obj;
	
    std::vector<t_object *> buffers;
	
	void *number_out;	
	
} t_ibuffermulti;


t_symbol *ps_set;
t_symbol *ps_replace;
t_symbol *ps_valid;


void *ibuffermulti_new();
void ibuffermulti_free(t_ibuffermulti *x);
void ibuffermulti_assist(t_ibuffermulti *x, void *b, long m, long a, char *s);

void ibuffermulti_user_clear(t_ibuffermulti *x);
void ibuffermulti_deferred_clear(t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv);
void ibuffermulti_clear(t_ibuffermulti *x);
void ibuffermulti_deferred_delete(t_object *x, t_symbol *s, short argc, t_atom *argv);
void ibuffermulti_user_load(t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv);
void ibuffermulti_load(t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv);


int C74_EXPORT main()
{
	this_class = class_new ("ibuffermulti~", (method) ibuffermulti_new, (method)ibuffermulti_free, sizeof(t_ibuffermulti), 0L, 0);
	
	class_addmethod (this_class, (method)ibuffermulti_user_clear, "clear", 0);
	class_addmethod (this_class, (method)ibuffermulti_user_load, "load", A_GIMME, 0);
	class_addmethod (this_class, (method)ibuffermulti_assist, "assist", A_CANT, 0);
	
	class_dspinit(this_class);
	class_register(CLASS_BOX, this_class);
	
	ps_set = gensym("set");
	ps_replace = gensym("replace");
	ps_valid = gensym("valid");
	
	return 0;
}

void *ibuffermulti_new()
{	
    t_ibuffermulti *x = (t_ibuffermulti *) object_alloc (this_class);
	
	dsp_setup((t_pxobject *)x, 0);
	
    x->number_out = intout(x);
	
    // Call constructor for vector
    
    new(&x->buffers) std::vector<t_object *>();
    
    return x;
}

void ibuffermulti_free(t_ibuffermulti *x)
{
    dsp_free(&x->x_obj);
	ibuffermulti_clear(x);
    
    // Deconstruct vector
    
    x->buffers.~vector<t_object *>();
}

void ibuffermulti_assist(t_ibuffermulti *x, void *b, long m, long a, char *s)
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

void ibuffermulti_user_clear(t_ibuffermulti *x)
{
	defer(x, (method) ibuffermulti_deferred_clear, 0, 0, 0);
}

void ibuffermulti_deferred_clear(t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv)
{
    ibuffermulti_clear(x);
    
    outlet_int(x->number_out, 0);
}

void ibuffermulti_clear(t_ibuffermulti *x)
{
	for (long i = 0; i < x->buffers.size(); i++)
        defer(x, (method) ibuffermulti_deferred_delete, 0, 0, 0);
	
    x->buffers.clear();
}

void ibuffermulti_deferred_delete(t_object *x, t_symbol *s, short argc, t_atom *argv)
{
    object_free(x);
}

void ibuffermulti_user_load(t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv)
{
	if (argc)
		defer(x, (method) ibuffermulti_load, s, argc, argv);
}

void ibuffermulti_load(t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv)
{
	// First make sure the ibuffer object is loaded
	
	if (!class_findbyname(CLASS_BOX, gensym("ibuffer~")))
	{
		void *force_load = newinstance(gensym("ibuffer~"), 0, 0);
		if (force_load)
			freeobject(static_cast<t_object *>(force_load));
	}
    
	// Return a properly allocated object (may still return NULL if the .mxo is not present)
	
	t_object *current_buffer = reinterpret_cast<t_object *>(object_new_typed(CLASS_BOX, gensym("ibuffer~"), 0, 0));
    bool buffer_loaded = false;

	// Now load the buffer 
	
	if (current_buffer && argc > 1 && atom_gettype(argv) == A_SYM && atom_gettype(argv + 1) == A_SYM)
	{
        t_atom return_val;

		// Set name, then load file - finally check validity
		
		object_method_typed(current_buffer, ps_set, 1, argv, &return_val);
		object_method_typed(current_buffer, ps_replace, argc - 1, argv + 1, &return_val);		
		long *valid_val = (long *) object_method(current_buffer, ps_valid);
		
		if (valid_val && *valid_val)
            buffer_loaded = true;
	}
	
    if (buffer_loaded)
        x->buffers.push_back(current_buffer);
    else
    {
        object_error(reinterpret_cast<t_object *>(x), "could not load ibuffer~");
        freeobject(current_buffer);
    }
    
	outlet_int(x->number_out, x->buffers.size());
}
