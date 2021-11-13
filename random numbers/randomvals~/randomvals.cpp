
/*
 *  randomvals and randomvals~
 *
 *  randomvals / randomvals~ generate pseudorandom numbers of a flat or gaussian-based distribution between 0 and 1.
 *
 *  The audio rate version requires triggering to cause output.
 *  It is intended for sample-accurate control, rather than noise generation and this conserves CPU.
 *  The gaussian mode allows the user to combine multiple curves, each with its own parameters for mean (0-1), deviation and weight.
 *  The RNG implementation is expected be of high quality, with a reasonably small state (only a handful of bytes), and a long cycle.
 *
 *  Copyright 2010-21 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Lifecycle.hpp>
#include <RandomGenerator.hpp>

#include <algorithm>
#include <cmath>


// Globals and Object Structure

t_class *this_class;

struct t_randomvals
{
#ifdef MSP_VERSION
    t_pxobject a_obj;
#else
    t_object a_obj;
#endif
    
    random_generator<> gen;
    
    double means[64];
    double devs[64];
    double weights[64];
    double lo_bounds[64];
    double hi_bounds[64];
    
    long num_params;
    
    void *the_outlet;
};

// Function Prototypes

void *randomvals_new ();
void randomvals_free (t_randomvals *x);
void randomvals_assist (t_randomvals *x, void *b, long m, long a, char *s);
void randomvals_list (t_randomvals *x, t_symbol *msg, long argc, t_atom *argv);

double randomvals_generate(random_generator<>& gen, double *means, double *devs, double *weights, double *lo, double *hi, long num_params, bool gauss);

#ifdef MSP_VERSION
void randomvals_perform64 (t_randomvals *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void randomvals_dsp64(t_randomvals *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
#else
void randomvals_int (t_randomvals *x, t_atom_long value);
#endif

// Main

int C74_EXPORT main()
{
#ifdef MSP_VERSION
    this_class = class_new("randomvals~",
                           (method)randomvals_new,
                           (method)randomvals_free,
                           sizeof(t_randomvals),
                           nullptr,
                           A_DEFLONG,
                           0);
    
    class_addmethod(this_class, (method)randomvals_dsp64, "dsp64", A_CANT, 0);
    
    class_dspinit(this_class);
#else
    this_class = class_new("randomvals",
                           (method)randomvals_new,
                           (method)randomvals_free,
                           sizeof(t_randomvals),
                           nullptr,
                           A_DEFLONG,
                           0);
    
    class_addmethod(this_class, (method)randomvals_int, "int", A_LONG, 0);
#endif
    
    class_addmethod(this_class, (method)randomvals_list, "list", A_GIMME, 0);
    class_addmethod(this_class, (method)randomvals_assist, "assist", A_CANT, 0);
    
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / Free

void *randomvals_new()
{
    t_randomvals *x = (t_randomvals *)object_alloc(this_class);

    create_object(x->gen);

#ifdef MSP_VERSION
    dsp_setup((t_pxobject *)x, 1);
    outlet_new((t_object *)x, "signal");
#else
    x->the_outlet = floatout(x);
#endif
        
    t_atom list_atoms[3];

    atom_setfloat(list_atoms + 0, 0.5);
    atom_setfloat(list_atoms + 1, 1.0);
    atom_setfloat(list_atoms + 2, 1.0);
    
    randomvals_list(x, gensym("list"), 3, list_atoms);
    
    return x;
}

void randomvals_free(t_randomvals *x)
{
#ifdef MSP_VERSION
    dsp_free(&x->a_obj);
#endif
    destroy_object(x->gen);
}

// List method (parameter setting)

void randomvals_list(t_randomvals *x, t_symbol *msg, long argc, t_atom *argv)
{
    double weight_val = 0.0;
    double inf = HUGE_VAL;
    
    long num_params = argc / 3;
    
    num_params = std::min(num_params, 64L);
    
    if (!num_params)
        return;
    
    for (int i = 0; i < num_params; i++)
    {
        double mean, dev;
        
        // Calculate parameters to store
        
        x->means[i] = mean = std::max(0.0, std::min(1.0, atom_getfloat(argv++)));
        x->devs[i] = dev = std::max(0.0, atom_getfloat(argv++));;
        
        double weight = std::max(0.0, atom_getfloat(argv++));
        
        double lo_bound = erf(-mean / (dev * sqrt(2.0)));
        double hi_bound = erf((1.0 - mean) / (dev * sqrt(2.0)));
        
        // N.B. inf is fine as an input, but nan is not...
        
        x->lo_bounds[i] = std::isnan(lo_bound) ? erf(-inf) : lo_bound;;
        x->hi_bounds[i] = std::isnan(hi_bound) ? erf( inf) : hi_bound;
        
        weight_val += weight;
        x->weights[i] = weight_val;
    }
    
    x->num_params = num_params;
}

// Core generation routine

double randomvals_generate(random_generator<>& gen, double *means, double *devs, double *weights, double *lo, double *hi, long num_params, bool gauss)
{
    if (gauss)
    {
        // Summed windowed gaussians random distribution
                
        const double a = gen.rand_double(weights[num_params - 1]);
        
        // Choose a mean and dev pair based on weighting

        long i = 0;
        
        for (; i < num_params - 1; i++)
            if (a < weights[i])
                break;
        
        // Generate a windowed gaussian number (between 0 and 1) using a fast implementation
        
        const double r = ltqnorm(0.5 + 0.5 * gen.rand_double(lo[i], hi[i])) * devs[i] + means[i];
        return std::max(0.0, std::min(1.0, r));
    }
    
    // Generate a flat distribution random number between 0 and 1
        
    return gen.rand_double();
}

#ifdef MSP_VERSION

// Perform (MSP version)

template <typename T>
void perform_core(const T* in, T *out, random_generator<>& gen, double *means, double *devs, double *weights, double *lo, double *hi, long num_params, long vec_size)
{
    while (vec_size--)
    {
        T t = *in++;
        double r = T(0);
        
        if (t >= 1)
        {
            const bool gauss = t >= 2;
            r = static_cast<T>(randomvals_generate(gen, means, devs, weights, lo, hi, num_params, gauss));
        }
        
        *out++ = r;
    }
}

void randomvals_perform64(t_randomvals *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    perform_core(ins[0], outs[0], x->gen, x->means, x->devs, x->weights, x->lo_bounds, x->hi_bounds, x->num_params, vec_size);
}

// DSP

void randomvals_dsp64(t_randomvals *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    object_method(dsp64, gensym("dsp_add64"), x, randomvals_perform64, 0, nullptr);
}

#else

// Int operation (Max version)

void randomvals_int(t_randomvals *x, t_atom_long value)
{
    const bool gauss = value >= 2;
    const double r = randomvals_generate(x->gen, x->means, x->devs, x->weights, x->lo_bounds, x->hi_bounds, x->num_params, gauss);
    
    outlet_float(x->the_outlet, r);
}

#endif

// Assist

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
