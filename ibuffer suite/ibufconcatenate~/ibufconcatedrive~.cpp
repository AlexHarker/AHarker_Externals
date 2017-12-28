
/*
 *  ibufconcatedrive~
 *
 *	The ibufconcatedrive~ object is high resolution drive object (accumulator) that is internally clipped according to the given items (or chunks) associated with a buffer.
 *	Typically this object forms part of a sample accurate sampler or granulator, and is used in conjunction with play~ or preferably hr.play~.
 *
 *	See the helpfile documentation for more on how this object can be used in practice.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include "ibufconcatenate_info~.h"

#include <limits>


t_class *this_class;


typedef struct _ibufconcatedrive
{
    t_pxobject x_obj;
	
    double sr_const;
	double accum;
	double lo;
	double hi;
	
	t_ibufconcatenate_info *attachment;
		
} t_ibufconcatedrive;


void *ibufconcatedrive_new (t_symbol *buffernmae, double init_val);
void ibufconcatedrive_set(t_ibufconcatedrive *x, t_symbol *msg, short argc, t_atom *argv);
void ibufconcatedrive_free (t_ibufconcatedrive *x);
void ibufconcatedrive_assist (t_ibufconcatedrive *x, void *b, long m, long a, char *s);

void ibufconcatedrive_dsp (t_ibufconcatedrive *x, t_signal **sp, short *count);
void ibufconcatedrive_dsp64(t_ibufconcatedrive *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

t_int *ibufconcatedrive_perform (t_int *w);
void ibufconcatedrive_perform64 (t_ibufconcatedrive *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);


int C74_EXPORT main()
{	
	this_class = class_new("ibufconcatedrive~",
				(method)ibufconcatedrive_new,
				(method)ibufconcatedrive_free,
				sizeof(t_ibufconcatedrive), 
				NULL, 
				A_SYM,
				A_DEFFLOAT, 
				0);
	
	class_addmethod(this_class, (method)ibufconcatedrive_set, "set", A_GIMME, 0);
	class_addmethod(this_class, (method)ibufconcatedrive_assist, "assist", A_CANT, 0);
	class_addmethod(this_class, (method)ibufconcatedrive_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)ibufconcatedrive_dsp64, "dsp64", A_CANT, 0);
	   
	class_dspinit(this_class);
	class_register(CLASS_BOX, this_class);
		
	return 0;
}


void *ibufconcatedrive_new(t_symbol *buffer_name, double init_val)
{
    t_ibufconcatedrive *x = (t_ibufconcatedrive *)object_alloc(this_class);
	
    dsp_setup((t_pxobject *)x, 4);
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");
	
	// Set default variables and intial output value
	
	x->accum = init_val;
	
	x->lo = 0;
	x->hi = 0;
	
	x->attachment = new_ibufconcatenate_info(buffer_name);
	if (!x->attachment)
		return 0;
	
	return (x);
}


void ibufconcatedrive_set(t_ibufconcatedrive *x, t_symbol *msg, short argc, t_atom *argv)
{
	t_symbol *buffer_name = argc ? atom_getsym(argv): 0;
	
	if (buffer_name)
	{
		detach_ibufconcatenate_info(x->attachment);
		x->attachment = new_ibufconcatenate_info(buffer_name);
	}
}


void ibufconcatedrive_free(t_ibufconcatedrive *x)
{
	dsp_free(&x->x_obj);
	detach_ibufconcatenate_info(x->attachment);
}


t_int *ibufconcatedrive_perform (t_int *w)
{	
	// Set pointers
	
	float *in = (float *) w[1];
	float *item_in = (float *) w[2];
	float *reset_in = (float *) w[3];
	float *reset_trigger = (float *) w[4];
	float *out_lo_res = (float *) w[5];
	float *out_hi_res = (float *) w[6];
	long vec_size = (long) w[7];
	t_ibufconcatedrive *x = (t_ibufconcatedrive *) w[8];
	
	double *starts;
	double *ends;
	double lo;
	double hi;
	double sr_const;
	double accum;
	
	float faccum;
	
	long num_items;
	long item;
	
	if (!x->attachment)
		return w + 9;
	
	starts = x->attachment->starts;
	ends = x->attachment->ends;

	lo = x->lo;
	hi = x->hi;
	sr_const = x->sr_const;
	accum = x->accum;
		
	num_items = x->attachment->num_items;
	
	while (vec_size--)
	{		
		// Get new buffer chunk data
		
		if (*reset_trigger++)
		{
			// Get item and reset to the given input value
			
			item = (long) *item_in;
			accum = *reset_in;
			
			if (item < 1 || item > num_items)
			{
				lo = 0.;
                hi = std::numeric_limits<float>::max();
			}
			else 
			{
				lo = starts[item - 1];
				hi = ends[item - 1];
			}
		}
		else 
		{
			// Accumulate (do drive)
			
			accum += *in++ * sr_const;
		}

		reset_in++;
		item_in++;
		
		// Clip the output to within the given bounds of the current chunk
		
		if (accum < lo)
			accum = lo;
		if (accum > hi)
			accum = hi;
		
		// Output
		
		*out_lo_res++ = faccum = accum;
		*out_hi_res++ = accum - faccum;
	}
			
	x->accum = accum;
	x->lo = lo;
	x->hi = hi;
			
	return w + 9;
}


void ibufconcatedrive_perform64 (t_ibufconcatedrive *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
	// Set pointers
	
	double *in = ins[0];
	double *item_in = ins[1];
	double *reset_in = ins[2];
	double *reset_trigger = ins[3];
	double *out_lo_res = outs[0];
	double *out_hi_res = outs[1];

	double *starts;
	double *ends;
	double lo;
	double hi;
	double sr_const;
	double accum;
		
	long num_items;
	long item;
	
	if (!x->attachment)
		return;
	
	starts = x->attachment->starts;
	ends = x->attachment->ends;
	
	lo = x->lo;
	hi = x->hi;
	sr_const = x->sr_const;
	accum = x->accum;
	
	num_items = x->attachment->num_items;
	
	while (vec_size--)
	{		
		// Get new buffer chunk data
		
		if (*reset_trigger++)
		{
			// Get item and reset to the given input value
			
			item = (long) *item_in;
			accum = *reset_in;
			
			if (item < 1 || item > num_items)
			{
				lo = 0.;
				hi = std::numeric_limits<double>::max();;
			}
			else 
			{
				lo = starts[item - 1];
				hi = ends[item - 1];
			}
		}
		else 
		{
			// Accumulate (do drive)
			
			accum += *in++ * sr_const;
		}
		
		reset_in++;
		item_in++;
		
		// Clip the output to within the given bounds of the current chunk
		
		if (accum < lo)
			accum = lo;
		if (accum > hi)
			accum = hi;
		
		// Output
		
		*out_lo_res++ = accum;
		*out_hi_res++ = 0.;
	}
	
	x->accum = accum;
	x->lo = lo;
	x->hi = hi;
}


void ibufconcatedrive_dsp(t_ibufconcatedrive *x, t_signal **sp, short *count)
{				
	x->sr_const = 1000. / sp[0]->s_sr;
	dsp_add(ibufconcatedrive_perform, 8, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[0]->s_n, x);
}


void ibufconcatedrive_dsp64(t_ibufconcatedrive *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	x->sr_const = 1000. / samplerate;
	object_method(dsp64, gensym("dsp_add64"), x, ibufconcatedrive_perform64, 0, NULL);
}


void ibufconcatedrive_assist(t_ibufconcatedrive *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_OUTLET)
	{
		switch (a)
		{
			case 0:
				sprintf(s,"(signal) Output (ms)");
				break;
				
			case 1:
				sprintf(s,"(signal) High Resolution Output (ms)");
				break;
		}
	}
	else 
	{
		switch (a)
		{
			case 0:
				sprintf(s,"(signal) Speed");
				break;
			
			case 1:
				sprintf(s,"(signal) Item In");
				break;
				
			case 2:
				sprintf(s,"(signal) Reset Value (ms)");
				break;
				
			case 3:
				sprintf(s,"(signal) Reset Trigger");
				break;
		}
	}
}
