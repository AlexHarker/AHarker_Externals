
/*
 *  dynamic.request~
 *
 *	dynamic.requests~ acts somewhat like a combination of dynamic.in~ and sah~. 
 *
 *	For a patcher loaded within a dynamicdsp~ object, a non-zero signal input cause dynamic.request~ to update its output by fetching a value from the associated signal inlet.
 *	The value is then held until a new trigger is received. This allows efficient parameter updating for sample accurate processing.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <dynamicdsp~.h>


void *this_class;


typedef struct _dynamic_request
{
    t_pxobject x_obj;
	
	long num_sig_ins;
	void **sig_ins;
	
	t_atom_long inlet_num;
    bool valid;
	
	double lastval;
	
} t_dynamic_request;


void dynamic_request_free(t_dynamic_request *x);
void *dynamic_request_new(t_atom_long inlet_num);
void dynamic_request_assist(t_dynamic_request *x, void *b, long m, long a, char *s);

void dynamic_request_dsp64(t_dynamic_request *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void dynamic_request_dsp(t_dynamic_request *x, t_signal **sp, short *count);

void dynamic_request_int(t_dynamic_request *x, t_atom_long inlet_num);
t_int *dynamic_request_perform(t_int *w);
t_int *dynamic_request_perform_small(t_int *w);
void dynamic_request_perform64(t_dynamic_request *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void dynamic_request_perform_small64(t_dynamic_request *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);


int C74_EXPORT main()
{
	this_class = class_new("dynamic.request~",
						   (method)dynamic_request_new, 
						   (method)dynamic_request_free, 
						   sizeof(t_dynamic_request), 
						   NULL, 
						   A_DEFLONG, 
						   0);
    
	class_addmethod(this_class, (method)dynamic_request_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)dynamic_request_dsp64, "dsp64", A_CANT, 0);
    class_addmethod(this_class, (method)dynamic_request_assist, "assist", A_CANT, 0);
	class_addmethod(this_class, (method)dynamic_request_int, "int", A_LONG, 0);
    
	class_dspinit(this_class);
	
	class_register(CLASS_BOX, this_class);
	
	return 0;
}

void dynamic_request_free(t_dynamic_request *x)
{
	dsp_free(&x->x_obj);
}

void *dynamic_request_new(t_atom_long inlet_num)
{
    t_dynamic_request *x = (t_dynamic_request *)object_alloc(this_class);
	void *dynamicdsp_parent = Get_Dynamic_Object();
	long num_sig_ins = Dynamic_Get_Num_Sig_Ins(dynamicdsp_parent);
	
    x->inlet_num = -1;
    x->valid = false;
    x->sig_ins = Dynamic_Get_Sig_In_Ptrs(dynamicdsp_parent);
    x->num_sig_ins = num_sig_ins;
    x->lastval = 0;

    if (inlet_num <= num_sig_ins && inlet_num >= 1)
    {
        x->valid = true;
        x->inlet_num = inlet_num;
    }
    
    dsp_setup((t_pxobject *)x, 1);
    outlet_new((t_object *)x,"signal");

    return (x);
}

void dynamic_request_assist(t_dynamic_request *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
		sprintf(s,"(signal) Requested Input %ld of Patcher", (long) x->inlet_num);
    else 
		sprintf(s,"(signal) Request Value / (int) Inlet Number");
}

void dynamic_request_dsp(t_dynamic_request *x, t_signal **sp, short *count)
{
	if (x->valid)
	{
		if (sp[0]->s_n > 4)
			dsp_add(dynamic_request_perform, 4, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n, x);
		else 
			dsp_add(dynamic_request_perform_small, 4, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n, x);
	}
}

void dynamic_request_dsp64(t_dynamic_request *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	if (x->valid)
	{
		if (maxvectorsize > 4)
			object_method(dsp64, gensym("dsp_add64"), x, dynamic_request_perform64, 0, NULL);		
		else 
			object_method(dsp64, gensym("dsp_add64"), x, dynamic_request_perform_small64, 0, NULL);		
	}
}

// Unlike dynamic.in~ the request object is either valid when dsp goes on or not, and can only be set to valid inputs (this speeds things up)

void dynamic_request_int(t_dynamic_request *x, t_atom_long inlet_num)
{		
	if (inlet_num >= 1 && inlet_num <= x->num_sig_ins)
	{
		x->inlet_num = inlet_num;
		x->valid = true;
	}
}

// Manually loop unrolled for speed (can't be used for single sample vectors) - if the vs is above 4 then we use this version

t_int *dynamic_request_perform(t_int *w)
{	
	float *in1 = (float *)(w[1]);
    float *out1 = (float *)(w[2]);
    long vec_size = w[3];
    t_dynamic_request *x = (t_dynamic_request *)(w[4]);
	
	float *from;
	float lastval = x->lastval;
	
	from = x->sig_ins[x->inlet_num - 1];
	
	for (long i = 0; i < vec_size >> 1; i++)
	{
		if (*in1++)
			lastval = *from;
		*out1++ = lastval;
		from++;
		if (*in1++)
			lastval = *from;
		*out1++ = lastval;
		from++;
	}
	
	x->lastval = lastval;
	
	return w + 5;
}

// Non loop unrolled version for small vector sizes

t_int *dynamic_request_perform_small(t_int *w)
{	    
	float *in1 = (float *)(w[1]);
    float *out1 = (float *)(w[2]);
    long vec_size = w[3];
	t_dynamic_request *x = (t_dynamic_request *)(w[4]);
	
	float *from;
	float lastval = x->lastval;
	
	from = x->sig_ins[x->inlet_num - 1];
	
	for (long i = 0; i < vec_size; i++)
	{
		if (*in1++)
			lastval = *from;
		*out1++ = lastval;
		from++;
	}
	
	x->lastval = lastval;
	
	return w + 5;
}

// 64 Bit Manually loop unrolled for speed (can't be used for single sample vectors) - if the vs is above 4 then we use this version

void dynamic_request_perform64(t_dynamic_request *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
	double *in1 = ins[0];
    double *out1 = outs[0];
	double *from;
	
	double lastval = x->lastval;
			
	from = x->sig_ins[x->inlet_num - 1];
	
	for (long i = 0; i < vec_size >> 1; i++)
	{
		if (*in1++)
			lastval = *from;
		*out1++ = lastval;
		from++;
		if (*in1++)
			lastval = *from;
		*out1++ = lastval;
		from++;
	}
	
	x->lastval = lastval;
}

// 64 Bit Non loop unrolled version for small vector sizes

void dynamic_request_perform_small64(t_dynamic_request *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	    
	double *in1 = ins[0];
    double *out1 = outs[0];
	double *from;
	
	double lastval = x->lastval;
		
	from = x->sig_ins[x->inlet_num - 1];
	
	for (long i = 0; i < vec_size; i++)
	{
		if (*in1++)
			lastval = *from;
		*out1++ = lastval;
		from++;
	}
	
	x->lastval = lastval;
}
