
/*
 *  depack
 *
 *	depack is a non-typed version of unpack for situations in which you need to unpack a list of unknown, or varying types.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>


const static int maximum_outlets = 256;


t_class *this_class;


typedef struct depack{
    
	t_object a_obj;
	
	long num_outlets;
    
	void *outlet_array[maximum_outlets];
		
} t_depack;


void *depack_new(t_atom_long num_outlets);
void depack_free(t_depack *x);

void depack_do_args(t_depack *x, short argc, t_atom *argv, long offset);

void depack_int(t_depack *x, t_atom_long value);
void depack_float(t_depack *x, double value);
void depack_list(t_depack *x, t_symbol *msg, long argc, t_atom *argv);
void depack_anything(t_depack *x, t_symbol *msg, long argc, t_atom *argv);

void depack_assist(t_depack *x, void *b, long m, long a, char *s);


int C74_EXPORT main()
{	
	this_class = class_new("depack", 
							(method) depack_new, 
							(method) depack_free, 
							sizeof(t_depack), 
							NULL, 
							A_DEFLONG,
							0);
	
    class_addmethod(this_class, (method)depack_int, "int", A_LONG, 0);
	class_addmethod(this_class, (method)depack_float, "float", A_FLOAT, 0);
	class_addmethod(this_class, (method)depack_list, "list", A_GIMME, 0);
	class_addmethod(this_class, (method)depack_anything, "anything", A_GIMME, 0);
	class_addmethod(this_class, (method)depack_assist, "assist", A_CANT, 0);
	
	class_register(CLASS_BOX, this_class);
	
	return 0;
}

void depack_free(t_depack *x)
{
}

void *depack_new(t_atom_long num_outlets)
{
    t_depack *x = (t_depack *) object_alloc(this_class);
	void **outlet_array;
	long i;
	
	if (num_outlets < 1)
    {
        object_error((t_object *) x, "the minimum number of outlets is 1");
        num_outlets = 1;
    }
    
	if (num_outlets > maximum_outlets)
    {
        object_error((t_object *) x, "the maximum number of outlets is %ld", maximum_outlets);
        num_outlets = maximum_outlets;
    }
	
	x->num_outlets = num_outlets;
	
	outlet_array = x->outlet_array;
	
	for (i = num_outlets - 1; i >= 0; i--)
		outlet_array[i] = outlet_new(x, 0);
	
    return x;
}

void depack_do_args(t_depack *x, short argc, t_atom *argv, long offset)
{
	long i;
	long num_outlets = x->num_outlets;
	void **outlet_array = x->outlet_array;
	
	if (argc > num_outlets - offset) argc = num_outlets - offset;
	
	for (i = argc - 1; i >= 0; i--)
	{
		switch (atom_gettype(argv + i))
		{
			case A_SYM:
				outlet_anything(outlet_array[i + offset], atom_getsym(argv + i), 0, 0);
				break;
			case A_FLOAT:
				outlet_float(outlet_array[i + offset], atom_getfloat(argv + i));
				break;
			case A_LONG:
				outlet_int(outlet_array[i + offset], atom_getlong(argv + i));
				break;
		}
	}
}

void depack_float(t_depack *x, double value)
{
    outlet_float(x->outlet_array[0], value);
} 

void depack_int(t_depack *x, t_atom_long value)
{ 	
    outlet_int(x->outlet_array[0], value);
}

void depack_list(t_depack *x, t_symbol *msg, long argc, t_atom *argv)
{
	depack_do_args(x, argc, argv, 0);
}

void depack_anything(t_depack *x, t_symbol *msg, long argc, t_atom *argv)
{
	depack_do_args(x, argc, argv, 1);
	outlet_anything(x->outlet_array[0], msg, 0, 0);
}

void depack_assist(t_depack *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
		sprintf(s, "Outlet %ld", a + 1);
    else 
        sprintf(s, "Input (anything)");
}
