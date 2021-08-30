
/*
 *  tsah~ (trigger sample-hold)
 *
 *	tsah~ is like sah~ but the output value is updated whenever the trigger input is non-zero.
 *  This means that the output can be updated once per sample if desired.
 *
 *  Copyright 2010-21 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>


// Globals and Object Structure

t_class *this_class;

constexpr int MAXIMUM_NUM_OUTLETS = 64;

struct t_tsah
{
    t_pxobject x_obj;
	
	float *sig_ins[MAXIMUM_NUM_OUTLETS];
	float *sig_outs[MAXIMUM_NUM_OUTLETS];
	
	double last_outputs[MAXIMUM_NUM_OUTLETS];
	
	long num_outlets;
};

// Function Prototypes

void *tsah_new(t_atom_long num_outlets);
void tsah_free(t_tsah *x);
void tsah_assist(t_tsah *x, void *b, long m, long a, char *s);

t_int *tsah_perform(t_int *w);
t_int *tsah_perform_multiple(t_int *w);

void tsah_perform64(t_tsah *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void tsah_perform_multiple64(t_tsah *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void tsah_perform_multiple64_unroll(t_tsah *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

void tsah_dsp(t_tsah *x, t_signal **sp, short *count);
void tsah_dsp64(t_tsah *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

// Main

int C74_EXPORT main()
{	
	this_class = class_new("tsah~",
				(method) tsah_new,
				(method) tsah_free,
				sizeof(t_tsah), 
                nullptr,
				A_DEFLONG, 
				0);
	
	class_addmethod(this_class, (method) tsah_assist, "assist", A_CANT, 0);
	class_addmethod(this_class, (method) tsah_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method) tsah_dsp64, "dsp64", A_CANT, 0);
	
	class_dspinit(this_class);
	class_register(CLASS_BOX, this_class);
		
	return 0;
}

// New / Free

void *tsah_new(t_atom_long num_outlets)
{	
	long i;
	
	t_tsah *x = (t_tsah *) object_alloc(this_class);
    
	num_outlets = num_outlets < 1 ? 1 : num_outlets;
    
    if (num_outlets > MAXIMUM_NUM_OUTLETS)
    {
        object_error((t_object *) x, "the maximum number of outlets is %ld", MAXIMUM_NUM_OUTLETS);
        num_outlets = MAXIMUM_NUM_OUTLETS;
    }
	
	dsp_setup((t_pxobject *)x, num_outlets + 1);

	for (i = 0; i < num_outlets; i++)
	{
		outlet_new((t_object *)x, "signal");
		x->last_outputs[i] = 0.;
	}
	
	x->x_obj.z_misc = Z_NO_INPLACE;		// due to working method!!
	x->num_outlets = num_outlets;
	
	return x;
}

void tsah_free(t_tsah *x)
{
	dsp_free(&x->x_obj);
}

// Perform

// Standard Perform (one input to sample)

t_int *tsah_perform(t_int *w)
{		
	float *in = (float *) w[1];
	float *trigger = (float *) w[2];
	float *out = (float *) w[3];
	long vec_size = w[4];
	t_tsah *x = (t_tsah *) w[5];
	
	float output = x->last_outputs[0];
	
	while (vec_size--)
	{
		*out++ = output = *trigger++ ? *in : output;
		in++;
	}
	
	x->last_outputs[0] = output;
	
	return w + 6;
}

// Multiple Perform (multiple inputs to sample)

t_int *tsah_perform_multiple(t_int *w)
{			
	long vec_size = (long) w[2];
	t_tsah *x = (t_tsah *) w[3];
	
	double *last_outputs = x->last_outputs;
    long num_outlets = x->num_outlets;
	
	for (long i = 0; i < num_outlets; i++)
	{
		long vec_size_temp = vec_size;
		float output_temp = last_outputs[i];
		
        float *trigger_ptr = (float *) w[1];
		float *out_ptr = x->sig_outs[i];
		float *in_ptr = x->sig_ins[i];
		
		while (vec_size_temp--) 
		{
			*out_ptr++ = output_temp = *trigger_ptr++ ? *in_ptr : output_temp;
			in_ptr++;
		}
		
		last_outputs[i] = output_temp;
	}
	
	return w + 4;
}

// 64 Bit Standard Perform (one input to sample)

void tsah_perform64(t_tsah *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{			
	double *in = ins[0];
	double *trigger = ins[1];
	double *out = outs[0];
	
	double output = x->last_outputs[0];
	
	while (vec_size--)
	{
		*out++ = output = *trigger++ ? *in : output;
		in++;
	}
	
	x->last_outputs[0] = output;
}

// 64 Bit Multiple Perform (multiple inputs to sample)

void tsah_perform_multiple64(t_tsah *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{			
    double *last_outputs = x->last_outputs;
    long num_outlets = x->num_outlets;
	
	for (long i = 0; i < num_outlets; i++)
	{
		long vec_size_temp = vec_size;
		double output_temp = last_outputs[i];
		
		double *trigger_ptr = ins[num_outlets];
		double *out_ptr = outs[i];
		double *in_ptr = ins[i];
		
		while (vec_size_temp--) 
		{
			*out_ptr++ = output_temp = *trigger_ptr++ ? *in_ptr : output_temp;
			in_ptr++;
		}
		
		last_outputs[i] = output_temp;
	}
}

// 64 Bit Multiple Perform Loop Unrolled (multiple inputs to sample)

void tsah_perform_multiple64_unroll(t_tsah *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{			
	double *last_outputs = x->last_outputs;
    long num_outlets = x->num_outlets;
	
	for (long i = 0; i < num_outlets; i++)
	{
		long vec_size_temp = vec_size >> 2;
		double output_temp = last_outputs[i];
		
		double *trigger_ptr = ins[num_outlets];
		double *out_ptr = outs[i];
		double *in_ptr = ins[i];
		
		while (vec_size_temp--) 
		{
			*out_ptr++ = output_temp = *trigger_ptr++ ? *in_ptr : output_temp;
			in_ptr++;
			*out_ptr++ = output_temp = *trigger_ptr++ ? *in_ptr : output_temp;
			in_ptr++;
			*out_ptr++ = output_temp = *trigger_ptr++ ? *in_ptr : output_temp;
			in_ptr++;
			*out_ptr++ = output_temp = *trigger_ptr++ ? *in_ptr : output_temp;
			in_ptr++;
		}
		
		last_outputs[i] = output_temp;
	}
}

// DSP

void tsah_dsp(t_tsah *x, t_signal **sp, short *count)
{
    long i;
    
    if (x->num_outlets > 1)
    {
        for (i = 0; i < x->num_outlets; i++)
        {
            x->sig_ins[i] = (float *) sp[i]->s_vec;
            x->sig_outs[i] = (float *) sp[x->num_outlets + i + 1]->s_vec;
        }
        
        dsp_add(tsah_perform_multiple, 3, sp[x->num_outlets]->s_vec, sp[0]->s_n, x);
    }
    else
        dsp_add(tsah_perform, 5, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n, x);
}

void tsah_dsp64(t_tsah *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{		
	if (x->num_outlets > 1)
	{
		if (maxvectorsize >= 4)
			object_method(dsp64, gensym("dsp_add64"), x, tsah_perform_multiple64_unroll, 0, nullptr);
		else
			object_method(dsp64, gensym("dsp_add64"), x, tsah_perform_multiple64, 0, nullptr);
	}
	else
		object_method(dsp64, gensym("dsp_add64"), x, tsah_perform64, 0, nullptr);
}

// Assist

void tsah_assist(t_tsah *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
    {
        switch (a)
        {
            case 0:
                sprintf(s,"(signal) Signal To Sample");
                break;
                
            case 1:
                sprintf(s,"(signal) Trigger");
                break;
        }
    }
    else
        sprintf(s,"(signal) Held Values");
}
