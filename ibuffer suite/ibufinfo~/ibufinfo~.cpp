
/*
 *  ibufinfo~
 *
 *	Returns length, sample rate and number of channels for ibuffer~ or buffer~ objects.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <ext.h>
#include <ext_obex.h>

#include <ibuffer_access.hpp>

t_class *this_class;

typedef struct _ibufinfo
{
    t_object x_obj;
	
	t_symbol *name;
	
	void *length_outlet;
	void *sr_outlet;
	void *chans_outlet;
	
} t_ibufinfo;


void *ibufinfo_new(t_symbol *buffer_name);
void ibufinfo_assist(t_ibufinfo *x, void *b, long m, long a, char *s);

void ibufinfo_set(t_ibufinfo *x, t_symbol *msg, long argc, t_atom *argv);
void ibufinfo_set_internal(t_ibufinfo *x, t_symbol *name);
void ibufinfo_bang(t_ibufinfo *x);


int C74_EXPORT main(void)
{
	this_class = class_new ("ibufinfo~",
							(method) ibufinfo_new, 
							NULL,
							sizeof (t_ibufinfo), 
							NULL,
							A_DEFSYM, 
							0);
	
	class_addmethod(this_class, (method)ibufinfo_set, "set", A_GIMME, 0);	
	class_addmethod(this_class, (method)ibufinfo_bang, "bang", 0);
	
	class_addmethod(this_class, (method)ibufinfo_assist, "assist", A_CANT, 0);
	
	class_register(CLASS_BOX, this_class);
	
	ibuffer_init();
	
	return 0;
}

void *ibufinfo_new(t_symbol *buffer_name)
{	
	t_ibufinfo *x = (t_ibufinfo *)object_alloc(this_class);
    
	x->chans_outlet = intout(x);
   	x->sr_outlet = floatout(x);
	x->length_outlet = floatout(x);
	
	x->name = buffer_name;
	
	return (x);
}

void ibufinfo_assist(t_ibufinfo *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
    {
		switch (a)
		{
			case 0:
				sprintf(s,"Length (ms)");
				break;
			case 1:
				sprintf(s,"Sample Rate");
				break;
			case 2:
				sprintf(s,"Channels");
				break;
		}
	}
	else 
	{
        sprintf(s, "Buffer Name / bang");
    }
}

void ibufinfo_bang(t_ibufinfo *x)
{
	ibufinfo_set_internal(x, x->name);
}

void ibufinfo_set(t_ibufinfo *x, t_symbol *msg, long argc, t_atom *argv)
{
	ibufinfo_set_internal(x, argc ? atom_getsym(argv) : 0);
}

void ibufinfo_set_internal(t_ibufinfo *x, t_symbol *name)
{
    x->name = name;
    
	ibuffer_data data(name);
	
	if (data.length)
	{
		if (data.sample_rate)
		{
			outlet_int(x->chans_outlet, data.num_chans);
			outlet_float(x->sr_outlet, data.sample_rate);
			outlet_float(x->length_outlet, (data.length / data.sample_rate) * 1000);
		}
		else 
		{
			outlet_int(x->chans_outlet, 0);
			outlet_float(x->sr_outlet, 0);
			outlet_float(x->length_outlet, 0);
		}		
	}
	else
		error("ibufinfo~: named (i)buffer~ does not exist");
}


