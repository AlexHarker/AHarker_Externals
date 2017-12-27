
/*
 *  schedulemessage
 *
 *	schedulemessage has two potential uses:
 *
 *	1 - To move messages from the low priority thread to high priority scheduler thread (if it it exists).
 *	2 - To delay any message (like delay, or pipe but for any messages).
 *
 *	See documentations for details of usage and intricatcies of timing / threading issues.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>


void *this_class;


typedef struct schedulemessage{
    
	t_object a_obj;
	
	double delay;
	
	void *message_out;
	
} t_schedulemessage;


t_symbol *ps_int;
t_symbol *ps_float;
t_symbol *ps_bang;


void *schedulemessage_new(double delay);
void schedulemessage_free(t_schedulemessage *x);

void schedulemessage_output(t_schedulemessage *x, t_symbol *msg, long argc, t_atom *argv);
void schedulemessage_int(t_schedulemessage *x, t_atom_long value);
void schedulemessage_float(t_schedulemessage *x, double float_in);
void schedulemessage_bang(t_schedulemessage *x);
void schedulemessage_anything(t_schedulemessage *x, t_symbol *msg, long argc, t_atom *argv);
void schedulemessage_delay(t_schedulemessage *x, double delay);

void schedulemessage_assist(t_schedulemessage *x, void *b, long m, long a, char *s);


int C74_EXPORT main()
{	
	this_class = class_new("schedulemessage", 
							(method) schedulemessage_new, 
							(method) schedulemessage_free, 
							sizeof(t_schedulemessage), 
							NULL, 
							A_DEFFLOAT,
							0);
	

	class_addmethod(this_class, (method)schedulemessage_int, "int", A_LONG, 0);
	class_addmethod(this_class, (method)schedulemessage_float, "float", A_FLOAT, 0);
	class_addmethod(this_class, (method)schedulemessage_delay, "ft1", A_FLOAT, 0);
	class_addmethod(this_class, (method)schedulemessage_bang, "bang", 0);
	class_addmethod(this_class, (method)schedulemessage_anything, "list", A_GIMME, 0);
	class_addmethod(this_class, (method)schedulemessage_anything, "anything", A_GIMME, 0);
	class_addmethod(this_class, (method)schedulemessage_assist, "assist", A_CANT, 0);
	
	class_register(CLASS_BOX, this_class);
	 
	ps_int = gensym("int");
	ps_float = gensym("float");
	ps_bang = gensym("bang");
	
	return 0;
}

void *schedulemessage_new(double delay)
{
    t_schedulemessage *x = (t_schedulemessage *) object_alloc(this_class);
	
	floatin(x, 1);
	
	x->message_out = outlet_new(x, 0);
	x->delay = delay;
	
    return x;
}

void schedulemessage_free(t_schedulemessage *x)
{
}

void schedulemessage_output(t_schedulemessage *x, t_symbol *msg, long argc, t_atom *argv)
{
    if (msg == ps_bang)
    {
        outlet_bang(x->message_out);
        return;
    }
		
    if (msg == ps_int)
    {
        if (argc)
            outlet_int(x->message_out, atom_getlong(argv));
        return;
    }
		
    if (msg == ps_float)
    {
        if (argc)
            outlet_float(x->message_out, atom_getfloat(argv));
        return;
    }
		
    outlet_anything(x->message_out, msg, argc, argv);
}

void schedulemessage_int(t_schedulemessage *x, t_atom_long value)
{
	t_atom out_atom;
	
	atom_setlong(&out_atom, value);
	
	schedulemessage_anything(x, ps_int, 1, &out_atom);
}

void schedulemessage_float(t_schedulemessage *x, double float_in)
{
	t_atom out_atom;
	
	atom_setfloat(&out_atom, float_in);
	
	schedulemessage_anything(x, ps_float, 1, &out_atom);
}

void schedulemessage_bang(t_schedulemessage *x)
{
	schedulemessage_anything(x, ps_bang, 0, 0);
}

void schedulemessage_anything(t_schedulemessage *x, t_symbol *msg, long argc, t_atom *argv)
{
	double delay = x->delay;
	
	if (!isr() || delay > 0)
		schedule_fdelay(x, (method) schedulemessage_output, delay, msg, argc, argv);
	else
		schedulemessage_output(x, msg, argc, argv);
}

void schedulemessage_delay(t_schedulemessage *x, double delay)
{
	x->delay = delay;
}

void schedulemessage_assist(t_schedulemessage *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
    {
		sprintf(s, "Scheduled Message");
	}
	else
	{
		switch (a)
		{
			case 0:
				sprintf(s, "Input (anything)");
				break;
				
			case 1:
				sprintf(s, "Set Delay Time In Milliseconds");
				break;
		}
	}
}
