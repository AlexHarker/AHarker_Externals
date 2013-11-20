
/*
 *  randomvals and randomvals~
 *
 *	randomvals and its audio rate counterpart (randomvals~) generate high-quality pseudorandom numbers of a flat or gaussian-based distribution between 0 and 1.
 *
 *	The audio rate version requires triggering to cause output in order to conserve CPU (it is intended for sample-accurate control, rather than noise generation).
 *	The gaussian mode allows for the combination of multiple curves, each with its own parameters for mean (between 0 and 1), deviation and weight.
 *	The implementation here is agnostic in relation to the underlying pseudorandom number generator (which is dealt with in AH_Random.h>.
 *	It is expected however, that the underlying generator be of high quality, with a reasonably small state (only a handful of bytes), and a long cycle.
 *	More distributions may be added at a later stage.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Random.h>


void *this_class;


typedef struct _randomvals
{
	
#ifdef MSP_VERSION
	t_pxobject a_obj;
#else
	t_object a_obj;
#endif
	
	t_rand_gen gen;
	
	double means[64];
	double devs[64];
	double weights[64];
	double lo_bounds[64];
	double hi_bounds[64];
	
	long num_params;
	
	void *the_outlet;
	
} t_randomvals;


void *randomvals_new ();
void randomvals_free (t_randomvals *x);
void randomvals_assist (t_randomvals *x, void *b, long m, long a, char *s);
void randomvals_list (t_randomvals *x, t_symbol *msg, short argc, t_atom *argv);

#ifdef MSP_VERSION

void randomvals_dsp (t_randomvals *x, t_signal **sp, short *count);
t_int *randomvals_perform (t_int *w);

void randomvals_perform64 (t_randomvals *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void randomvals_dsp64(t_randomvals *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

#else

void randomvals_int (t_randomvals *x, long in);

#endif


int main (void)
{
	
#ifdef MSP_VERSION
	
	this_class = class_new("randomvals~",
						   (method)randomvals_new,
						   (method)randomvals_free,
						   sizeof(t_randomvals), 
						   NULL, 
						   A_DEFLONG, 
						   0);
	
	class_addmethod(this_class, (method)randomvals_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)randomvals_dsp64, "dsp64", A_CANT, 0);
	
	class_dspinit(this_class);
	
#else
	
	this_class = class_new("randomvals",
						   (method)randomvals_new,
						   (method)randomvals_free,
						   sizeof(t_randomvals), 
						   NULL, 
						   A_DEFLONG, 
						   0);
	
	class_addmethod(this_class, (method)randomvals_int, "int", A_LONG, 0);
	
#endif
	
	class_addmethod(this_class, (method)randomvals_list, "list", A_GIMME, 0);
	class_addmethod(this_class, (method)randomvals_assist, "assist", A_CANT, 0);
	
	
	class_register(CLASS_BOX, this_class);
	
	return 0;
}


void *randomvals_new()
{
    t_randomvals *x = (t_randomvals *)object_alloc(this_class);

	t_atom list_atoms[3];
#ifdef MSP_VERSION
    dsp_setup((t_pxobject *)x, 1);
	outlet_new((t_object *)x, "signal");
#else
	x->the_outlet = floatout(x);
#endif
	
	rand_seed(&x->gen);
	
	atom_setfloat(list_atoms + 0, 0.5);
	atom_setfloat(list_atoms + 1, 1.);
	atom_setfloat(list_atoms + 2, 1.);	
	
	randomvals_list(x, gensym("list"), 3, list_atoms);
	
	return (x);
}


void randomvals_free(t_randomvals *x)
{
	
#ifdef MSP_VERSION	
	dsp_free(&x->a_obj);
#endif
	
}


void randomvals_list (t_randomvals *x, t_symbol *msg, short argc, t_atom *argv)
{
	double *means = x->means;
	double *devs = x->devs;
	double *weights = x->weights;
	double *lo_bounds = x->lo_bounds;
	double *hi_bounds = x->hi_bounds;
	int i;
	
	double mean, dev, weight, lo_bound, hi_bound;
	double weight_val = 0;
	double inf = HUGE_VAL;
	
	long num_params = argc / 3;
	
	if (!num_params)
		return;
	
	for (i = 0; i < num_params; i++)
	{
		// Calculate parameters to store
		
		mean = atom_getfloat(argv++);
		if (mean < 0) 
			mean = 0;
		if (mean > 1)
			mean = 1;
		
		means[i] = mean;
		
		dev = atom_getfloat(argv++);
		if (dev < 0)
			dev = 0;
		
		devs[i] = dev;
		
		weight = atom_getfloat(argv++);
		if (weight < 0)
			weight = 0;
		
		lo_bound = erf(-mean / (dev * sqrt(2.)));
		hi_bound = erf((1.- mean) / (dev * sqrt(2.)));
		
		// N.B. inf is fine as an input, but nan is not...
		
		if (lo_bound != lo_bound)
			lo_bound = erf(-inf);
		if (hi_bound != hi_bound)
			hi_bound = erf(inf);
		
		lo_bounds[i] = lo_bound;
		hi_bounds[i] = hi_bound;
		
		weight_val += weight;
		weights[i] = weight_val;
	}
	
	x->num_params = num_params;
}


#ifdef MSP_VERSION

t_int *randomvals_perform (t_int *w)
{	
	// Set pointers
	
	float *in = (float *) w[1];
	float *out = (float *) w[2];
	long vec_size = w[3];
	t_randomvals *x = (t_randomvals *) w[4];
	
	
	t_rand_gen *gen = &x->gen;
	
	double *means = x->means;
	double *devs = x->devs;
	double *weights = x->weights;
	double *lo_bounds = x->lo_bounds;
	double *hi_bounds = x->hi_bounds;
	
	double randval, mean, dev, lo_bound, hi_bound;
	long test, i;
	long num_params = x->num_params;
	
	while (vec_size--) 
	{
		test = *in++;
		
		if (test >= 1)
		{
			if (test >= 2)
			{
				// Summed windowed gaussians random distribution
				
				// Choose a mean and dev pair based on weighting
				
				randval = rand_double_n(gen, weights[num_params - 1]);
				
				for (i = 0; i < num_params - 1; i++)
					if (randval < weights[i])
						break;
				
				// Generate a windowed gaussian number (between 0 and 1) using a fast implementation
				
				mean = means[i];
				dev = devs[i];
				lo_bound = lo_bounds[i];
				hi_bound = hi_bounds[i];
				
				randval =  ltqnorm(0.5 + 0.5 * rand_double_range(gen, lo_bound, hi_bound)) * dev + mean;
				if (randval > 1.)
					randval = 1.;
				if (randval < 0.)
					randval = 0.;
			}
			else 
			{
				// Generate a flat distribution random number between 0 and 1
				
				randval = rand_double(gen);
			}
		}
		else
		{
			// Output zeros
			
			randval = 0;
		}
		
		*out++ = randval;
	}
	
	return w + 5;
}


void randomvals_dsp(t_randomvals *x, t_signal **sp, short *count)
{				
	dsp_add(randomvals_perform, 4, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n, x);
}


void randomvals_perform64 (t_randomvals *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
	// Set pointers
	
	double *in = ins[0];
	double *out = outs[0];	
	
	t_rand_gen *gen = &x->gen;
	
	double *means = x->means;
	double *devs = x->devs;
	double *weights = x->weights;
	double *lo_bounds = x->lo_bounds;
	double *hi_bounds = x->hi_bounds;
	
	double randval, mean, dev, lo_bound, hi_bound;
	long test, i;
	long num_params = x->num_params;
	
	while (vec_size--) 
	{
		test = *in++;
		
		if (test >= 1)
		{
			if (test >= 2)
			{
				// Summed windowed gaussians random distribution
				
				// Choose a mean and dev pair based on weighting
				
				randval = rand_double_n(gen, weights[num_params - 1]);
				
				for (i = 0; i < num_params - 1; i++)
					if (randval < weights[i])
						break;
				
				// Generate a windowed gaussian number (between 0 and 1) using a fast implementation
				
				mean = means[i];
				dev = devs[i];
				lo_bound = lo_bounds[i];
				hi_bound = hi_bounds[i];
				
				randval =  ltqnorm(0.5 + 0.5 * rand_double_range(gen, lo_bound, hi_bound)) * dev + mean;
				if (randval > 1.)
					randval = 1.;
				if (randval < 0.)
					randval = 0.;
			}
			else 
			{
				// Generate a flat distribution random number between 0 and 1
				
				randval = rand_double(gen);
			}
		}
		else
		{
			// Output zeros
			
			randval = 0;
		}
		
		*out++ = randval;
	}
}


void randomvals_dsp64(t_randomvals *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{				
	object_method(dsp64, gensym("dsp_add64"), x, randomvals_perform64, 0, NULL);
}

#else

void randomvals_int (t_randomvals *x, long in)
{		
	t_rand_gen *gen = &x->gen;
	
	double *means = x->means;
	double *devs = x->devs;
	double *weights = x->weights;
	double *lo_bounds = x->lo_bounds;
	double *hi_bounds = x->hi_bounds;
	
	double randval, mean, dev, lo_bound, hi_bound;
	long i;
	long num_params = x->num_params;
	
	if (in >= 2)
	{
		// Summed windowed gaussians random distribution
		
		// Choose a mean and dev pair based on weighting
		
		randval = rand_double_n(gen, weights[num_params - 1]);
		
		for (i = 0; i < num_params - 1; i++)
			if (randval < weights[i])
				break;
		
		// Generate a windowed gaussian number (between 0 and 1) using a fast implementation
		
		mean = means[i];
		dev = devs[i];
		lo_bound = lo_bounds[i];
		hi_bound = hi_bounds[i];
		
		randval =  ltqnorm(0.5 + 0.5 * rand_double_range(gen, lo_bound, hi_bound)) * dev + mean;
		if (randval > 1.)
			randval = 1.;
		if (randval < 0.)
			randval = 0.;
	}
	else 
	{
		// Generate a flat distribution random number between 0 and 1
		
		randval = rand_double(gen);
	}
	
	outlet_float(x->the_outlet, randval);	
}

#endif


void randomvals_assist(t_randomvals *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_OUTLET)
    {
		sprintf(s, "Random Vals Out");
	}
    else 
	{
		sprintf(s, "Triggers / Parameters");
	}
	
}



