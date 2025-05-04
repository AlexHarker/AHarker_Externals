
/*
 *  ahnoise~
 *
 *  ahnoise~ is a high quality noise generator with random time seeding.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Lifecycle.hpp>
#include <RandomGenerator.hpp>


// Globals and Object Structure

t_class *this_class;

struct t_ahnoise
{
    t_pxobject a_obj;
    
    random_generator<> gen;
};

// Function Prototypes

void *ahnoise_new();
void ahnoise_free(t_ahnoise *x);
void ahnoise_assist(t_ahnoise *x, void *b, long m, long a, char *s);

void ahnoise_perform64(t_ahnoise *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void ahnoise_dsp64(t_ahnoise *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags);

// Main

int C74_EXPORT main()
{
    this_class = class_new("ahnoise~",
                           (method) ahnoise_new,
                           (method) ahnoise_free,
                           sizeof(t_ahnoise),
                           (method) nullptr,
                           0);
    
    class_addmethod(this_class, (method) ahnoise_dsp64, "dsp64", A_CANT, 0);
    class_addmethod(this_class, (method) ahnoise_assist, "assist", A_CANT, 0);
    
    class_dspinit(this_class);
    
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / Free

void *ahnoise_new()
{
    t_ahnoise *x = (t_ahnoise *) object_alloc(this_class);

    dsp_setup((t_pxobject *) x, 0);
    outlet_new((t_object *) x, "signal");

    create_object(x->gen);
    
    return x;
}

void ahnoise_free(t_ahnoise *x)
{
    dsp_free(&x->a_obj);
    destroy_object(x->gen);
}

// Perform

void ahnoise_perform64(t_ahnoise *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    double *out = outs[0];
    
    // Get random values and convert from [0 to 1] to [-1.0 to 1.0]

    while (vec_size--)
        *out++ = (x->gen.rand_double() * 2.0) - 1.0;
}

// DSP

void ahnoise_dsp64(t_ahnoise *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags)
{
    object_method(dsp64, gensym("dsp_add64"), x, ahnoise_perform64, 0, nullptr);
}

// Assist

void ahnoise_assist(t_ahnoise *x, void *b, long m, long a, char *s)
{
    static constexpr int maxAssist = 256;

    if (m == ASSIST_OUTLET)
    {
        snprintf(s, maxAssist, "(signal) Noise");
    }
    else
    {
        snprintf(s, maxAssist, "(signal) Dummy");
    }
}
