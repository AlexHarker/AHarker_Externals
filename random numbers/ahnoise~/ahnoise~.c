
/*
 *  ahnoise~
 *
 *	ahnoise~ is a high quality noise generator with random time seeding. 
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Random.h>

void *this_class;


typedef struct _ahnoise
{
	t_pxobject a_obj;
	
	t_rand_gen gen;
	
} t_ahnoise;


void *ahnoise_new ();
void ahnoise_free (t_ahnoise *x);
void ahnoise_assist (t_ahnoise *x, void *b, long m, long a, char *s);

t_int *ahnoise_perform (t_int *w);
void ahnoise_dsp (t_ahnoise *x, t_signal **sp, short *count);

void ahnoise_perform64 (t_ahnoise *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void ahnoise_dsp64 (t_ahnoise *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);


int C74_EXPORT main (void)
{	
	this_class = class_new("ahnoise~",
						   (method)ahnoise_new,
						   (method)ahnoise_free,
						   sizeof(t_ahnoise), 
						   NULL, 
						   0);
	
	class_addmethod(this_class, (method)ahnoise_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)ahnoise_dsp64, "dsp64", A_CANT, 0);
	class_addmethod(this_class, (method)ahnoise_assist, "assist", A_CANT, 0);
	
	class_dspinit(this_class);
	
	class_register(CLASS_BOX, this_class);
	
	return 0;
}


void *ahnoise_new()
{
    t_ahnoise *x = (t_ahnoise *)object_alloc(this_class);

    dsp_setup((t_pxobject *)x, 0);
	outlet_new((t_object *)x, "signal");

	rand_seed(&x->gen);
	
	return (x);
}


void ahnoise_free(t_ahnoise *x)
{
	dsp_free(&x->a_obj);	
}


t_int *ahnoise_perform (t_int *w)
{	
	// Set pointers
	
	float *out = (float *) w[1];
	long vec_size = w[2];
	t_ahnoise *x = (t_ahnoise *) w[3];
	
	t_rand_gen *gen = &x->gen;
		
    // Get random values and convert from [0 to 1] to [-1.0 to 1.0]
    
	while (vec_size--)
		*out++ = (float) ((rand_double(gen) * 2.0) - 1.0);
		
	return w + 4;
}


void ahnoise_dsp(t_ahnoise *x, t_signal **sp, short *count)
{				
	dsp_add(ahnoise_perform, 3, sp[0]->s_vec, sp[0]->s_n, x);
}


void ahnoise_perform64 (t_ahnoise *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
	// Set pointers
	
	double *out = outs[0];
	t_rand_gen *gen = &x->gen;
	
    // Get random values and convert from [0 to 1] to [-1.0 to 1.0]

	while (vec_size--)
		*out++ = (rand_double(gen) * 2.0) - 1.0;
}


void ahnoise_dsp64 (t_ahnoise *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{				
	object_method(dsp64, gensym("dsp_add64"), x, ahnoise_perform64, 0, NULL);
}


void ahnoise_assist(t_ahnoise *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_OUTLET) 
	{
		sprintf(s,"(signal) Noise");
	}
    else 
	{
		sprintf(s,"(signal) Dummy");
    }
}

