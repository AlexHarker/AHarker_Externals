
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

#include <AH_Lifecycle.hpp>
#include <RandomGenerator.hpp>

#include <algorithm>


t_class *this_class;

struct t_randfloats
{
	t_object a_obj;
	
	random_generator<> gen;
    
	double params[12];
	
	long f_inletNumber;
	void *f_proxies[2];	
	
	void *the_outlet;
};

void *randfloats_new();
void randfloats_free(t_randfloats *x);
void randfloats_assist(t_randfloats *x, void *b, long m, long a, char *s);

double randfloats_input(t_randfloats *x);

void randfloats_int(t_randfloats *x, t_atom_long value);
void randfloats_float(t_randfloats *x, double value);
void randfloats_bang(t_randfloats *x);

double clip(double value, double min_val, double max_val);
void randfloats_list(t_randfloats *x, t_symbol *msg, long argc, t_atom *argv);

double triple_gauss_rand(random_generator<>& gen, double *params);
double erf_weighting(double mean, double dev);

int C74_EXPORT main()
{	
	this_class = class_new("randfloats",
						   (method)randfloats_new,
						   (method)randfloats_free,
						   sizeof(t_randfloats), 
                           nullptr,
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
	t_randfloats *x = (t_randfloats *)object_alloc(this_class);

    create_object(x->gen);

	// Make Proxies and outlet
	
    for (int i = 2; i > 0; i--)
        x->f_proxies[i - 1] = proxy_new(x, i ,&x->f_inletNumber);
	
	x->the_outlet = floatout(x);
	    
	// Default parameters
	
	for (int i = 0; i < 3; i++)
	{
        x->params[4 * i + 0] = 0.5;
        x->params[4 * i + 1] = 1.0;
        x->params[4 * i + 2] = 1.0;
        x->params[4 * i + 3] = x->params[4 * i + 2] * x->params[4 * i + 1];
	}
	
    x->params[11] = x->params[11] + x->params[7] + x->params[3];
    x->params[7] += x->params[3];
	
    return x;
}

void randfloats_free(t_randfloats *x)
{
    for (long i = 0; i < 2; i++)
        freeobject((t_object *)x->f_proxies[i]);
    
    destroy_object(x->gen);
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

double randfloats_input(t_randfloats *x)
{
    switch (proxy_getinlet((t_object *)x))
    {
        case 1:     return x->gen.rand_windowed_gaussian(x->params[0], x->params[1]);
        case 2:     return triple_gauss_rand(x->gen, x->params);
            
        default:
            return x->gen.rand_double();
    }
}

void randfloats_int(t_randfloats *x, t_atom_long value)
{
	randfloats_float(x, (double) value);
}

void randfloats_float(t_randfloats *x, double value)
{
	outlet_float(x->the_outlet, randfloats_input(x) * value);
}

void randfloats_bang(t_randfloats *x)
{
	outlet_float(x->the_outlet, randfloats_input(x));
}

double clip(double value, double min_val, double max_val)
{
    return std::max(min_val, std::min(max_val, value));
}

void randfloats_list(t_randfloats *x, t_symbol *msg, long argc, t_atom *argv)
{
	double *params = x->params;
    const static double max_dev = 1000000.0;
    
	if (argc >= 9)
	{
		// If there are enough arguments to set all three gaussian paramters
		
		for (int i = 0; i < 3; i++)
		{
			// Get mean/dev/weight check they are within bounds

			params[4 * i + 0] = clip(atom_getfloat(argv + (3 * i + 0)), 0.0, 1.0);
            params[4 * i + 1] = clip(atom_getfloat(argv + (3 * i + 1)), 0.0, max_dev);
            params[4 * i + 2] = std::max(0.0, static_cast<double>(atom_getfloat(argv + (3 * i + 2))));

			// Adjusted weight
			
			params[4 * i + 3] = params[4 * i + 2] * params[4 * i + 1] * erf_weighting(params[4 * i], params[4 * i + 1]);
		}
	}
	else
	{
        if (argc >= 2)
        {
            // Set first gaussian parameters only
		
            // Get mean/dev and check they are within bounds
            
            params[0] = clip(atom_getfloat(argv + 0), 0.0, 1.0);
            params[1] = clip(atom_getfloat(argv + 1), 0.0, max_dev);

            // Adjusted weighting (use previous weighting)
            
            params[3] = params[2] * params[1] * erf_weighting(params[0], params[1]);
        }
	}
    
    // Sum weights
    
    params[11] = (params[11] + params[7] + params[3]);
    params[7] += params[3];
}

double triple_gauss_rand(random_generator<>& gen, double *params)
{
    // Choose which of the three sets of parameters to use
    
	double R = gen.rand_double(params[11]);
	
    // Get a random windowed gaussian number according to the retrived mean and dev

	if (R < params[3])
        return gen.rand_windowed_gaussian(params[0], params[1]);
	else if (R < params[7])
        return gen.rand_windowed_gaussian(params[4], params[5]);
    else
        return gen.rand_windowed_gaussian(params[8], params[9]);
}

double erf_weighting(double mean, double dev)
{
	// Weighting functions to ensure probability curves have equal max points regardless of dev
	
	double dev_recip = 1.0 / dev;
	double Z = -mean * dev_recip;
	double Y = (1.0 - mean) * dev_recip; 
	
	return erf(Y) - erf(Z);
}
