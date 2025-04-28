
/*
 *  triggerlimit~
 *
 *  triggerlimit~ limits the rate of non-zero samples to a maximum rate specified in samples.
 *
 *  The initial intention of this object was to reduce rapid triggering of grains (and hence high CPU usage).
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>


// Globals and Object Structure

t_class *this_class;

struct t_triggerlimit
{
    t_pxobject x_obj;
    
    t_atom_long limit;
    t_atom_long count;
};

// Function  Protoypes

void *triggerlimit_new(t_atom_long limit);
void triggerlimit_int(t_triggerlimit *x, t_atom_long limit);
void triggerlimit_free(t_triggerlimit *x);
void triggerlimit_assist(t_triggerlimit *x, void *b, long m, long a, char *s);

void triggerlimit_perform64(t_triggerlimit *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void triggerlimit_dsp64(t_triggerlimit *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags);

// Main

int C74_EXPORT main()
{
    this_class = class_new("triggerlimit~",
                           (method) triggerlimit_new,
                           (method) triggerlimit_free,
                           sizeof(t_triggerlimit),
                           (method) nullptr,
                           A_DEFLONG,
                           0);
    
    class_addmethod(this_class, (method) triggerlimit_int, "int", A_LONG, 0);
    class_addmethod(this_class, (method) triggerlimit_assist, "assist", A_CANT, 0);
    class_addmethod(this_class, (method) triggerlimit_dsp64, "dsp64", A_CANT, 0);
    
    class_dspinit(this_class);
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / Free

void *triggerlimit_new(t_atom_long limit)
{
    t_triggerlimit *x = (t_triggerlimit *) object_alloc(this_class);
    
    dsp_setup((t_pxobject *) x, 1);
    outlet_new((t_object *) x, "signal");
    
    if (limit < 1)
        limit = 1;
    x->limit = limit;
    x->count = 0;
    
    return x;
}

void triggerlimit_free(t_triggerlimit *x)
{
    dsp_free(&x->x_obj);
}

// Int Handler

void triggerlimit_int(t_triggerlimit *x, t_atom_long limit)
{
    if (limit < 1)
        limit = 1;
    x->limit = limit;
    x->count = 0;
}

// Perform

void triggerlimit_perform64(t_triggerlimit *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    double *in = ins[0];
    double *out = outs[0];
    
    t_atom_long limit = x->limit;
    t_atom_long count = x->count;
    long i;
    
    double in_val;
    double output;
    
    for (i = 0; i < vec_size; i++)
    {
        output = 0.f;
        in_val = *in++;
        
        if (in_val && !count)
        {
            output = in_val;
            count = limit;
        }
        
        if (count)
            count--;
        
        *out++ = output;
    }
    
    x->count = count;
}

// DSP

void triggerlimit_dsp64(t_triggerlimit *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags)
{
    x->count = 0;
    object_method(dsp64, gensym("dsp_add64"), x, triggerlimit_perform64, 0, nullptr);
}

// Assist

void triggerlimit_assist(t_triggerlimit *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"(signal) Triggers In ");
    else
        sprintf(s,"(signal) Triggers Out");
}
