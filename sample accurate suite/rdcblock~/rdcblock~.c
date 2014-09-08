
/*
 *  rdcblock~ (reset-able dc blocking filter)
 *
 *	rdcblock~ is a simple dc blocking filter in which the filter memory can be reset on a sample-accurate basis, using a dedicated reset input.
 *
 *	There are two reset modes, one that zeros the memory, another that resets to the current input value.
 *
 *	Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Denormals.h>

void *this_class;


typedef struct _rdcblock
{
    t_pxobject x_obj;
	
	double in_mem;
	double y;
	
	t_atom_long mode;
	
} t_rdcblock;


void *rdcblock_new (t_atom_long mode);
void rdcblock_free (t_rdcblock *x);
void rdcblock_assist (t_rdcblock *x, void *b, long m, long a, char *s);

t_int *rdcblock_perform (t_int *w);
t_int *rdcblock_perform_inval (t_int *w);
void rdcblock_dsp (t_rdcblock *x, t_signal **sp, short *count);

void rdcblock_perform64 (t_rdcblock *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void rdcblock_perform_inval64 (t_rdcblock *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void rdcblock_dsp64 (t_rdcblock *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);


int C74_EXPORT main (void)
{	
	this_class = class_new("rdcblock~",
				(method)rdcblock_new,
				(method)rdcblock_free,
				sizeof(t_rdcblock), 
				NULL, 
				A_DEFLONG, 
				0);
	
	class_addmethod(this_class, (method)rdcblock_assist, "assist", A_CANT, 0);
	class_addmethod(this_class, (method)rdcblock_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)rdcblock_dsp64, "dsp64", A_CANT, 0);
	
	class_dspinit(this_class);
	class_register(CLASS_BOX, this_class);
		
	return 0;
}


void *rdcblock_new(t_atom_long mode)
{
    t_rdcblock *x = (t_rdcblock *)object_alloc(this_class);
    
    dsp_setup((t_pxobject *)x, 2);
	outlet_new((t_object *)x, "signal");
	
	x->in_mem = x->y = 0.;
	x->mode = mode;
	
	return (x);
}


void rdcblock_free(t_rdcblock *x)
{
	dsp_free(&x->x_obj);
}


t_int *rdcblock_perform (t_int *w)
{		
	float *in1 = (float *) w[2];
	float *in2 = (float *) w[3];
	float *out = (float *) w[4];
	long vec_size = w[5];
    t_rdcblock *x = (t_rdcblock *) w[6];
	
	double in;
	
	// Recall memory
	
	double in_mem = x->in_mem;
	double y = x->y;
	
	float out_val;
	
	while (vec_size--) 
	{
		in = *in1++;
		
		// Sample accurate reset
		
		if (*in2++)						
			in_mem = y = 0.;
		
		// Filter and shift memory
		
		y = in - in_mem + (0.99 * y);
		in_mem = in;
		
		out_val = (float) y;
		*out++ = AH_FIX_DENORM_FLOAT(out_val);
		
#ifndef __APPLE__
		y = AH_FIX_DENORM_DOUBLE(y);
#endif
	}
	
	// Store memory
	
	x->in_mem = in_mem;
	x->y = y;
	
	return w + 7;
}


t_int *rdcblock_perform_inval (t_int *w)
{		
	float *in1 = (float *) w[2];
	float *in2 = (float *) w[3];
	float *out = (float *) w[4];
	long vec_size = w[5];
    t_rdcblock *x = (t_rdcblock *) w[6];
	
	double in;
	
	// Recall memory

	double in_mem = x->in_mem;
	double y = x->y;
	
	float out_val;
	
	while (vec_size--) 
	{
		in = *in1++;
		
		// Sample accurate reset
		
		if (*in2++)
		{
			in_mem = in;
			y = 0.;
		}
		
		// Filter and shift memory

		y = in - in_mem + (0.99 * y);
		in_mem = in;
		
		out_val = (float) y;
		*out++ = AH_FIX_DENORM_FLOAT(out_val);
		
#ifndef __APPLE__
		y = AH_FIX_DENORM_DOUBLE(y);
#endif		
	}

	// Store memory

	x->in_mem = in_mem;
	x->y = y;
	
	return w + 7;
}


void rdcblock_dsp(t_rdcblock *x, t_signal **sp, short *count)
{				
	if (x->mode)
		dsp_add(denormals_perform, 6, rdcblock_perform_inval, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n, x);
	else
		dsp_add(denormals_perform, 6, rdcblock_perform, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n, x);
}


void rdcblock_perform64 (t_rdcblock *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{		
	double *in1 = ins[0];
	double *in2 = ins[1];
	double *out = outs[0];
	
	// Recall memory
	
	double in_mem = x->in_mem;
	double y = x->y;
	double in, out_val;
	
	while (vec_size--) 
	{
		in = *in1++;
		
		// Sample accurate reset
		
		if (*in2++)						
			in_mem = y = 0.;
		
		// Filter and shift memory
		
		y = in - in_mem + (0.99 * y);
		in_mem = in;
		
		out_val = AH_FIX_DENORM_DOUBLE(y);
		*out++ = out_val;
		y = out_val;
	}
	
	// Store memory
	
	x->in_mem = in_mem;
	x->y = y;
}


void rdcblock_perform_inval64 (t_rdcblock *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{		
	double *in1 = ins[0];
	double *in2 = ins[1];
	double *out = outs[0];
		
	// Recall memory
	
	double in_mem = x->in_mem;
	double y = x->y;
	double in, out_val;
	
	while (vec_size--) 
	{
		in = *in1++;
		
		// Sample accurate reset
		
		if (*in2++)
		{
			in_mem = in;
			y = 0.;
		}
		
		// Filter and shift memory
		
		y = in - in_mem + (0.99 * y);
		in_mem = in;
		
		out_val = AH_FIX_DENORM_DOUBLE(y);
		*out++ = out_val;
		y = out_val;
	}
	
	// Store memory
	
	x->in_mem = in_mem;
	x->y = y;
}


void rdcblock_dsp64 (t_rdcblock *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{				
	if (x->mode)
		object_method(dsp64, gensym("dsp_add64"), x, rdcblock_perform_inval64, 0, NULL);
	else 
		object_method(dsp64, gensym("dsp_add64"), x, rdcblock_perform64, 0, NULL);
}


void rdcblock_assist(t_rdcblock *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET)
	{
		switch (a)
		{
			case 0:
				sprintf(s,"(signal) Input");
				break;
				
			case 1:
				sprintf(s,"(signal) Reset Trigger");
				break;
		}
	}
	else
		sprintf(s,"(signal) Output");
}



