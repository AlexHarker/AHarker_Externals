
/*
 *  dynamic.request~
 *
 *  dynamic.request~ acts somewhat like a combination of dynamic.in~ and sah~ within a dynamic~ host object.
 *
 *  A non-zero signal input cause dynamic.request~ fetches a value from the associated signal inlet.
 *  The value is then held until a new trigger is received.
 *  This allows efficient parameter updating for sample accurate processing.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <dynamic~.hpp>


// Globals and Object Structure

t_class *this_class;

struct t_dynamic_request
{
    t_pxobject x_obj;
    
    long num_sig_ins;
    double **sig_ins;
    
    t_atom_long inlet_num;
    bool valid;
    
    double prev;
};

// Function prototypes

void *dynamic_request_new(t_atom_long inlet_num);
void dynamic_request_free(t_dynamic_request *x);
void dynamic_request_assist(t_dynamic_request *x, void *b, long m, long a, char *s);

void dynamic_request_int(t_dynamic_request *x, t_atom_long inlet_num);

void dynamic_request_perform64(t_dynamic_request *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void dynamic_request_perform_small64(t_dynamic_request *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

void dynamic_request_dsp64(t_dynamic_request *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags);

// Main

int C74_EXPORT main()
{
    this_class = class_new("dynamic.request~",
                           (method) dynamic_request_new,
                           (method) dynamic_request_free,
                           sizeof(t_dynamic_request),
                           (method) nullptr,
                           A_DEFLONG,
                           0);
    
    class_addmethod(this_class, (method) dynamic_request_int, "int", A_LONG, 0);
    
    class_addmethod(this_class, (method) dynamic_request_dsp64, "dsp64", A_CANT, 0);
    class_addmethod(this_class, (method) dynamic_request_assist, "assist", A_CANT, 0);
    
    class_dspinit(this_class);
    
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / Free / Assist

void *dynamic_request_new(t_atom_long inlet_num)
{
    t_dynamic_request *x = (t_dynamic_request *) object_alloc(this_class);
    void *dynamic_parent = dynamic_get_parent();
    long num_sig_ins = dynamic_get_num_sig_ins(dynamic_parent);
    
    x->sig_ins = dynamic_get_sig_in_ptrs(dynamic_parent);
    x->num_sig_ins = num_sig_ins;
    x->inlet_num = -1;
    x->valid = false;
    x->prev = 0;
    
    dsp_setup((t_pxobject *) x, 1);
    outlet_new((t_object *) x, "signal");
    
    dynamic_request_int(x, inlet_num);
    
    return x;
}

void dynamic_request_free(t_dynamic_request *x)
{
    dsp_free(&x->x_obj);
}

void dynamic_request_assist(t_dynamic_request *x, void *b, long m, long a, char *s)
{
    static constexpr int maxAssist = 256;

    if (m == ASSIST_OUTLET)
        snprintf(s, maxAssist, "(signal) Requested Input %ld of Patcher", (long) x->inlet_num);
    else
        snprintf(s, maxAssist, "(signal) Request Value / (int) Inlet Number");
}

// Int Handler

// Unlike dynamic.in~ the request object is either valid when dsp goes on or not.
// It can only be set to valid inputs (this speeds things up).

void dynamic_request_int(t_dynamic_request *x, t_atom_long inlet_num)
{
    if (inlet_num >= 1 && inlet_num <= x->num_sig_ins)
    {
        x->inlet_num = inlet_num;
        x->valid = true;
    }
}

// Perform

// Manually loop unrolled for speed - if the vs is above 4 then we use this version

void dynamic_request_perform64(t_dynamic_request *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    double *in = ins[0];
    double *out = outs[0];
    double *from;
    
    double prev = x->prev;
    
    from = x->sig_ins[x->inlet_num - 1];
    
    for (long i = 0; i < vec_size >> 1; i++)
    {
        if (*in++)
            prev = *from;
        *out++ = prev;
        from++;
        if (*in++)
            prev = *from;
        *out++ = prev;
        from++;
    }
    
    x->prev = prev;
}

// Non loop unrolled version for small vector sizes

void dynamic_request_perform_small64(t_dynamic_request *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    double *in = ins[0];
    double *out = outs[0];
    double *from;
    
    double prev = x->prev;
    
    from = (double *) x->sig_ins[x->inlet_num - 1];
    
    for (long i = 0; i < vec_size; i++, from++)
    {
        if (*in++)
            prev = *from;
        *out++ = prev;
    }
    
    x->prev = prev;
}

// DSP

void dynamic_request_dsp64(t_dynamic_request *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags)
{
    if (x->valid)
    {
        if (max_vec > 4)
            object_method(dsp64, gensym("dsp_add64"), x, dynamic_request_perform64, 0, nullptr);
        else
            object_method(dsp64, gensym("dsp_add64"), x, dynamic_request_perform_small64, 0, nullptr);
    }
}
