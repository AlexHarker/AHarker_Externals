
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
 *  Copyright 2010-22 Alex Harker. All rights reserved.
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

using window_gauss_params = random_generator<>::windowed_gaussian_params;

struct t_randomvals
{
#ifdef MSP_VERSION
    t_pxobject a_obj;
#else
    t_object a_obj;
#endif
    
    random_generator<> gen;
    
    window_gauss_params params[64];
    double weights[64];
    
    long num_params;
    
    void *the_outlet;
};

// Function Prototypes

void *randomvals_new();
void randomvals_free(t_randomvals *x);
void randomvals_assist(t_randomvals *x, void *b, long m, long a, char *s);
void randomvals_list(t_randomvals *x, t_symbol *msg, long argc, t_atom *argv);

double randomvals_generate(random_generator<>& gen, window_gauss_params *params, double *weights, long num_params, bool gauss);

#ifdef MSP_VERSION
t_int *randomvals_perform(t_int *w);
void randomvals_perform64(t_randomvals *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void randomvals_dsp(t_randomvals *x, t_signal **sp, short *count);
void randomvals_dsp64(t_randomvals *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
#else
void randomvals_int(t_randomvals *x, t_atom_long value);
#endif

// Main

int C74_EXPORT main()
{
#ifdef MSP_VERSION
    this_class = class_new("randomvals~",
                           (method) randomvals_new,
                           (method) randomvals_free,
                           sizeof(t_randomvals),
                           (method) nullptr,
                           A_DEFLONG,
                           0);
    
    class_addmethod(this_class, (method) randomvals_dsp, "dsp", A_CANT, 0);
    class_addmethod(this_class, (method) randomvals_dsp64, "dsp64", A_CANT, 0);
    
    class_dspinit(this_class);
#else
    this_class = class_new("randomvals",
                           (method) randomvals_new,
                           (method) randomvals_free,
                           sizeof(t_randomvals),
                           (method) nullptr,
                           A_DEFLONG,
                           0);
    
    class_addmethod(this_class, (method) randomvals_int, "int", A_LONG, 0);
#endif
    
    class_addmethod(this_class, (method) randomvals_list, "list", A_GIMME, 0);
    class_addmethod(this_class, (method) randomvals_assist, "assist", A_CANT, 0);
    
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / Free

void *randomvals_new()
{
    t_randomvals *x = (t_randomvals *) object_alloc(this_class);

    create_object(x->gen);

#ifdef MSP_VERSION
    dsp_setup((t_pxobject *) x, 1);
    outlet_new((t_object *) x, "signal");
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
    
    long num_params = argc / 3;
    
    num_params = std::min(num_params, 64L);
    
    if (!num_params)
        return;
    
    for (int i = 0; i < num_params; i++)
    {
        // Calculate parameters to store
        
        const double mean = std::max(0.0, std::min(1.0, atom_getfloat(argv++)));
        const double dev = std::max(0.0, atom_getfloat(argv++));
        const double weight = std::max(0.0, atom_getfloat(argv++));
        
        x->params[i] = window_gauss_params(mean, dev);
        
        weight_val += weight;
        x->weights[i] = weight_val;
    }
    
    x->num_params = num_params;
}

// Core generation routine

double randomvals_generate(random_generator<>& gen, window_gauss_params *params, double *weights, long num_params, bool gauss)
{
    if (gauss)
    {
        // Summed windowed gaussians random distribution
                
        const double a = gen.rand_double(weights[num_params - 1]);
        
        // Choose a mean and dev pair based on weighting

        long i = 0;
        
        for (; i < num_params - 1; i++)
            if (a <= weights[i])
                break;
        
        // Generate a windowed gaussian number (between 0 and 1) using a fast implementation
        
        return gen.rand_windowed_gaussian(params[i]);
    }
    
    // Generate a flat distribution random number between 0 and 1
        
    return gen.rand_double();
}

#ifdef MSP_VERSION

// Perform (MSP version)

template <typename T>
void perform_core(const T* in, T *out, random_generator<>& gen, window_gauss_params *params, double *weights, long num_params, long vec_size)
{
    while (vec_size--)
    {
        T t = *in++;
        double r = T(0);
        
        if (t >= 1)
        {
            const bool gauss = t >= 2;
            r = static_cast<T>(randomvals_generate(gen, params, weights, num_params, gauss));
        }
        
        *out++ = static_cast<T>(r);
    }
}

t_int *randomvals_perform(t_int *w)
{
    // Set pointers
    
    float *in = reinterpret_cast<float *>(w[1]);
    float *out = reinterpret_cast<float *>(w[2]);
    long vec_size = static_cast<long>(w[3]);
    t_randomvals *x = reinterpret_cast<t_randomvals *>(w[4]);
    
    perform_core(in, out, x->gen, x->params, x->weights, x->num_params, vec_size);
    
    return w + 5;
}

void randomvals_perform64(t_randomvals *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    perform_core(ins[0], outs[0], x->gen, x->params, x->weights, x->num_params, vec_size);
}

// DSP

void randomvals_dsp(t_randomvals *x, t_signal **sp, short *count)
{
    dsp_add(randomvals_perform, 4, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n, x);
}

void randomvals_dsp64(t_randomvals *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    object_method(dsp64, gensym("dsp_add64"), x, randomvals_perform64, 0, nullptr);
}

#else

// Int operation (Max version)

void randomvals_int(t_randomvals *x, t_atom_long value)
{
    const bool gauss = value >= 2;

    outlet_float(x->the_outlet, randomvals_generate(x->gen, x->params, x->weights, x->num_params, gauss));
}

#endif

// Assist

// FIX - better labels?

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
