
/*
 *  ibufconcatedrive~
 *
 *  The ibufconcatedrive~ object is high resolution drive object (accumulator) that is internally clipped according to the given items (or chunks) associated with a buffer.
 *  Typically this object forms part of a sample accurate sampler or granulator, and is used in conjunction with play~ or hr.play~ in older versions of max.
 *
 *  See the helpfile documentation for more on how this object can be used in practice.
 *
 *  Copyright 2010-21 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include "ibufconcatenate_info.hpp"

#include <algorithm>
#include <limits>


t_class *this_class;

struct t_ibufconcatedrive
{
    t_pxobject x_obj;
    
    double sr_const;
    double accum;
    double lo;
    double hi;
    
    t_ibufconcatenate_info *attachment;
};


void *ibufconcatedrive_new(t_symbol *buffernmae, double init_val);
void ibufconcatedrive_set(t_ibufconcatedrive *x, t_symbol *msg, short argc, t_atom *argv);
void ibufconcatedrive_free(t_ibufconcatedrive *x);
void ibufconcatedrive_assist(t_ibufconcatedrive *x, void *b, long m, long a, char *s);

void ibufconcatedrive_dsp64(t_ibufconcatedrive *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void ibufconcatedrive_perform64(t_ibufconcatedrive *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);


int C74_EXPORT main()
{
    this_class = class_new("ibufconcatedrive~",
                           (method)ibufconcatedrive_new,
                           (method)ibufconcatedrive_free,
                           sizeof(t_ibufconcatedrive),
                           nullptr,
                           A_SYM,
                           A_DEFFLOAT,
                           0);
    
    class_addmethod(this_class, (method)ibufconcatedrive_set, "set", A_GIMME, 0);
    class_addmethod(this_class, (method)ibufconcatedrive_assist, "assist", A_CANT, 0);
    class_addmethod(this_class, (method)ibufconcatedrive_dsp64, "dsp64", A_CANT, 0);
    
    class_dspinit(this_class);
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

void *ibufconcatedrive_new(t_symbol *buffer_name, double init_val)
{
    t_ibufconcatedrive *x = (t_ibufconcatedrive *)object_alloc(this_class);
    
    dsp_setup((t_pxobject *)x, 4);
    outlet_new((t_object *)x, "signal");
    outlet_new((t_object *)x, "signal");
    
    // Set default variables and initial output value
    
    x->accum = init_val;
    
    x->lo = 0;
    x->hi = 0;
    
    x->attachment = attach_ibufconcatenate_info(buffer_name);
    
    return x;
}

void ibufconcatedrive_set(t_ibufconcatedrive *x, t_symbol *msg, short argc, t_atom *argv)
{
    t_symbol *buffer_name = argc ? atom_getsym(argv) : 0;
    
    if (buffer_name)
    {
        detach_ibufconcatenate_info(x->attachment);
        x->attachment = attach_ibufconcatenate_info(buffer_name);
    }
}

void ibufconcatedrive_free(t_ibufconcatedrive *x)
{
    dsp_free(&x->x_obj);
    detach_ibufconcatenate_info(x->attachment);
}

void store(double *lo_res, double *hi_res, double output)
{
    *lo_res = output;
    *hi_res = 0.0;
}

template <class T>
void ibufconcatedrive_perform_core(t_ibufconcatedrive *x, T **ins, T **outs, long vec_size)
{
    // Set pointers
    
    T *in = ins[0];
    T *item_in = ins[1];
    T *reset_in = ins[2];
    T *reset_trigger = ins[3];
    T *out_lo_res = outs[0];
    T *out_hi_res = outs[1];
    
    if (!x->attachment)
        return;
    
    double lo = x->lo;
    double hi = x->hi;
    double sr_const = x->sr_const;
    double accum = x->accum;
    
    t_ibufconcatenate_info::read_access access(x->attachment);
    
    while (vec_size--)
    {
        // Get new buffer chunk data
        
        if (*reset_trigger++)
        {
            // Get item and reset to the given input value
            
            long item = (long) *item_in;
            accum = *reset_in;
            
            // Defaults
            
            lo = 0.;
            hi = std::numeric_limits<float>::max();
            
            access.get_item(item - 1, lo, hi);
        }
        else
        {
            // Accumulate (do drive)
            
            accum += *in++ * sr_const;
        }
        
        reset_in++;
        item_in++;
        
        // Clip the output to within the given bounds of the current chunk
        
        accum = std::min(hi, std::max(accum, lo));
        
        // Output
        
        store(out_lo_res++, out_hi_res++, accum);
    }
    
    x->accum = accum;
    x->lo = lo;
    x->hi = hi;
}

void ibufconcatedrive_perform64(t_ibufconcatedrive *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    ibufconcatedrive_perform_core(x, ins, outs, vec_size);
}

void ibufconcatedrive_dsp64(t_ibufconcatedrive *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    x->sr_const = 1000. / samplerate;
    object_method(dsp64, gensym("dsp_add64"), x, ibufconcatedrive_perform64, 0, nullptr);
    
}

void ibufconcatedrive_assist(t_ibufconcatedrive *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
    {
        switch (a)
        {
            case 0:
                sprintf(s,"(signal) Output (ms)");
                break;
                
            case 1:
                sprintf(s,"(signal) High Resolution Output (ms)");
                break;
        }
    }
    else
    {
        switch (a)
        {
            case 0:
                sprintf(s,"(signal) Speed");
                break;
                
            case 1:
                sprintf(s,"(signal) Item In");
                break;
                
            case 2:
                sprintf(s,"(signal) Reset Value (ms)");
                break;
                
            case 3:
                sprintf(s,"(signal) Reset Trigger");
                break;
        }
    }
}
