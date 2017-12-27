
/*
 *  randfloats
 *
 *	randfloats is an object for generating high quality random numbers (either 0-1 or 0-n) using either flat or windowed gaussian distributions.
 *	Three gaussian functions may also be used simultaneously to create a more complex distribution, with weighting values used to randomly determine which is used.
 *
 *	This object is now somewhat superseeded by randomvals, although it may be simpler in some situations.
 *	The weighting used here is different to (and more complex than) that used in the randomvals(~) objects. 
 *	It is an updated version of the previous randfloats object submitted as part of my MPhil thesis at the University of Birmingham.
 *	This version uses much higher quality RNG and the code is simplified here, because the detail is now relocated in AH_Random.h
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Random.h>

#ifndef __APPLE__
#include <erf.h>
#endif

void *this_class;


typedef struct _randfloats
{
	t_object a_obj;
	
	t_rand_gen gen;
	double params[12];
	
	long f_inletNumber;
	void *f_proxies[2];	
	
	void *the_outlet;
	
} t_randfloats;


void *randfloats_new ();
void randfloats_free (t_randfloats *x);
void randfloats_assist (t_randfloats *x, void *b, long m, long a, char *s);

void randfloats_int (t_randfloats *x, t_atom_long value);
void randfloats_float (t_randfloats *x, double value);
void randfloats_bang (t_randfloats *x);
void randfloats_list (t_randfloats *x, t_symbol *msg, long argc, t_atom *argv);

double triplegaussrand (t_rand_gen *gen, double *params);
double erfweighting (float mean, float dev);


int C74_EXPORT main()
{	
	this_class = class_new("randfloats",
						   (method)randfloats_new,
						   (method)randfloats_free,
						   sizeof(t_randfloats), 
						   NULL, 
						   0);
	
	class_addmethod(this_class, (method)randfloats_bang, "bang", 0);
	class_addmethod(this_class, (method)randfloats_int, "int", A_LONG, 0);
	class_addmethod(this_class, (method)randfloats_float, "float", A_FLOAT, 0);
	class_addmethod(this_class, (method)randfloats_list, "list", A_GIMME, 0);
	class_addmethod(this_class, (method)randfloats_assist, "assist", A_CANT, 0);
	
	
	class_register(CLASS_BOX, this_class);
	
	return 0;
}


void *randfloats_new()
{
	long i;
	
	t_randfloats *x = (t_randfloats *)object_alloc(this_class);

	double *params = x->params;

	// Make Proxies
	
    for (i = 2; i > 0; i--)												
        x->f_proxies[i - 1] = proxy_new(x, i ,&x->f_inletNumber);
	
	x->the_outlet = floatout(x);
	
	rand_seed(&x->gen);

	// Default parameters
	
	for (i = 0; i < 3; i++)
	{
		params[4 * i] = 0.5;
		params[4 * i + 1] = 1.0;
		params[4 * i + 2] = 1.;
		params[4 * i + 3] = params[4 * i + 2] * params[4 * i + 1];
	}
	
	params[11] = (params[11] + params[7] + params[3]);
	params[7] += params[3];
	
    return (x);
}


void randfloats_free(t_randfloats *x)
{
	long i;
	
    for (i = 0; i < 2; i++)
        freeobject((t_object *)x->f_proxies[i]);
}


void randfloats_assist(t_randfloats *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_OUTLET)
    {
		sprintf(s,"Random Float Out");
	}
    else {
        switch (a) {
            case 0:
                sprintf(s,"Flat Random / Parameters");
                break;
			case 1:
                sprintf(s,"Gaussian Random");
                break;
			case 2:
                sprintf(s,"Three Gauss. Random");
                break;
        }
    }	
}


void randfloats_int (t_randfloats *x, t_atom_long value)
{
	randfloats_float (x, (double) value);
}


void randfloats_float (t_randfloats *x, double value)
{
	double randval = 0;

	switch (proxy_getinlet((t_object *)x))
	{
		case 0:
			randval = rand_double(&x->gen);
			break;
		case 1:
			randval = rand_windgauss(&x->gen, x->params[1], x->params[0]);
			break;
		case 2:
			randval = triplegaussrand(&x->gen, x->params);
			break;
	}
	
	randval *= value;
	
	outlet_float (x->the_outlet, randval);
}


void randfloats_bang (t_randfloats *x)
{
    double randval = 0;
	
	switch (proxy_getinlet((t_object *)x))
	{
		case 0:
			randval = rand_double(&x->gen);
			break;
		case 1:
			randval = rand_windgauss(&x->gen, x->params[0], x->params[1]);
			break;
		case 2:
			randval = triplegaussrand(&x->gen, x->params);
			break;
	}
	
	outlet_float (x->the_outlet, randval);
}


void randfloats_list (t_randfloats *x, t_symbol *msg, long argc, t_atom *argv)
{
	double *params = x->params;
	double temp;
	long i;
	
	if (argc >= 9)
	{
		// If there are enough arguments to set all three gaussian paramters
		
		for (i = 0; i < 3; i++)
		{
			// Get mean and check it is within bounds

			temp = atom_getfloat(argv + (3 * i));

			if (temp >= 0 && temp <= 1)
				params[4 * i] = temp;
			else
				params[4 * i] = 0.5;
			
			// Get dev and check it is within bounds
			
			temp = atom_getfloat(argv + (3 * i + 1));

			if (temp > 0 && temp <= 1000)
				params[4 * i + 1] = temp;
			else
				params[4 * i + 1] = 1.0;
			
			
			temp = atom_getfloat(argv + (3 * i + 2));
			
			// Get weight
			
			if (temp >= 0)
				params[4 * i + 2] = temp;
			else
				params[4 * i + 2] = 1.0;
			
			// Adjusted weight
			
			params[4 * i + 3] = params[4 * i + 2] * params[4 * i + 1] * erfweighting (params[4 * i], params[4 * i + 1]);
		}
		
		// Sum weights
		
		params[11] = (params[11] + params[7] + params[3]);
		params[7] += params[3];
	}
	else
	{
		// Set first gaussian parameters only
		
		// Mean - as above
		
		temp = atom_getfloat(argv);
		
		if (temp >= 0 && temp <= 1)
			params[0] = temp;
		else
			params[0] = 0.5;
		
		// Dev - as above
		
		temp = atom_getfloat(argv + 1);
		
		if (temp > 0 && temp <= 10)
			params[1] = temp;
		else
			params[1] = 1.0;
		
		// Adjusted weighting (use previous weighting)
		
		params[3] = params[2] * params[1] * erfweighting (params[0], params[1]);

		// Sum weights
		
		params[11] = (params[11] + params[7] + params[3]);
		params[7] += params[3];
	}
}


double triplegaussrand (t_rand_gen *gen, double *params)
{
	double mean, dev;
	double R = rand_double_n(gen, params[11]);
	
	// Choose which of the three sets of parameters to use
	
	if (R < params[3])
	{
		mean = params[0];
		dev = params[1];
	}
	else
	{
		if (R < params[7])
		{
			mean = params[4];
			dev = params[5];
		}
		else
		{
			mean = params[8];
			dev = params[9];
		}
	}
	
	// Get a random windowed gaussian number according to the retrived mean and dev
	
	return rand_windgauss (gen, mean, dev);
}


double erfweighting (float mean, float dev)
{
	// Weighting functions to ensure probability curves have equal max points regardless of dev
	
	double dev_recip = 1.0 / dev;
	double Z = -mean * dev_recip;
	double Y = (1.0 - mean) * dev_recip; 
	
	return erf(Y) - erf(Z);
}




