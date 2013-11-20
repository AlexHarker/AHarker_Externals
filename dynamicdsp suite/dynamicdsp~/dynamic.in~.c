
/*
 *  dynamic.in~
 *
 *	dynamic.in~ acts like in~ but for patchers loaded inside a dynamicdsp~ object.
 *	
 *	Unlike in~ the user can change the signal inlet which the object refers to by sending an int to the object.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <dynamicdsp~.h>


void *this_class;


typedef struct _dynamic_in
{
    t_pxobject x_obj;
	
	long declared_sig_ins;
	void **sig_ins;
	
	long valid;
	long inlet_num;
	
} t_dynamic_in;


void dynamic_in_free(t_dynamic_in *x);
void *dynamic_in_new();
void dynamic_in_assist(t_dynamic_in *x, void *b, long m, long a, char *s);

void dynamic_in_dsp(t_dynamic_in *x, t_signal **sp, short *count);
void dynamic_in_dsp64(t_dynamic_in *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

void dynamic_in_int(t_dynamic_in *x, long inlet_num);
t_int *dynamic_in_perform(t_int *w);
void dynamic_in_perform64(t_dynamic_in *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

int main(void)
{
	this_class = class_new("dynamic.in~",
						   (method)dynamic_in_new, 
						   (method)dynamic_in_free, 
						   sizeof(t_dynamic_in), 
						   NULL, 
						   A_DEFLONG, 
						   0);
    
	class_addmethod(this_class, (method)dynamic_in_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)dynamic_in_dsp64, "dsp64", A_CANT, 0);
    class_addmethod(this_class, (method)dynamic_in_assist, "assist", A_CANT, 0);
	class_addmethod(this_class, (method)dynamic_in_int, "int", A_LONG, 0);
    
	class_dspinit(this_class);
	
	class_register(CLASS_BOX, this_class);
	
	return 0;
}


void dynamic_in_free(t_dynamic_in *x)
{
	dsp_free(&x->x_obj);
}


void *dynamic_in_new(long inlet_num)
{
    t_dynamic_in *x = (t_dynamic_in *)object_alloc(this_class);
	void *dynamicdsp_parent = Get_Dynamic_Object();
	long declared_sig_ins;

    dsp_setup((t_pxobject *)x, 1);
    outlet_new((t_object *)x,"signal");
	
	x->inlet_num = inlet_num;
	x->valid = 0;
	
	dynamicdsp_parent = Get_Dynamic_Object();
	declared_sig_ins = Dynamic_Get_Declared_Sigins(dynamicdsp_parent);

	x->sig_ins = Dynamic_Get_Sigins(dynamicdsp_parent);
	
	if (inlet_num <= declared_sig_ins && inlet_num >= 1)
		x->valid = 1;
		
	x->declared_sig_ins = declared_sig_ins;
	
    return (x);
}


void dynamic_in_assist(t_dynamic_in *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
		sprintf(s,"(signal) Signal Input %ld of Patcher", x->inlet_num);
    else 
		sprintf(s,"(int) Inlet Number");
}


void dynamic_in_dsp(t_dynamic_in *x, t_signal **sp, short *count)
{
	dsp_add(dynamic_in_perform, 3, sp[1]->s_vec, sp[0]->s_n, x);
}


void dynamic_in_dsp64(t_dynamic_in *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	object_method(dsp64, gensym("dsp_add64"), x, dynamic_in_perform64, 0, NULL);		// scalar routine
}


void dynamic_in_int(t_dynamic_in *x, long inlet_num)
{
	x->valid = 0;
	x->inlet_num = inlet_num;
	
	if (inlet_num >= 1 && inlet_num <= x->declared_sig_ins)	
		x->valid = 1;
	else
		x->valid = 0;
}


t_int *dynamic_in_perform(t_int *w)
{	
    float *from;
    float *out1 = (float *)(w[1]);
    int vec_size = w[2];
    t_dynamic_in *x = (t_dynamic_in *)(w[3]);
	long i;

	if (x->valid)
	{
		from = x->sig_ins[x->inlet_num - 1];
		for (i = 0; i < vec_size; i++)				
			*out1++ = *from++;
	}
	else
	{
		for (i = 0; i < vec_size; i++)
			*out1++ = 0;
	}
	
	return w + 4;
}


void dynamic_in_perform64(t_dynamic_in *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
    double *from, *out1;
	long i;
	
    out1 = outs[0];
	
	if (x->valid)
	{
		from = x->sig_ins[x->inlet_num - 1];
		for (i = 0; i < vec_size; i++)				
			*out1++ = *from++;
	}
	else
	{
		for (i = 0; i < vec_size; i++)
			*out1++ = 0;
	}
}
