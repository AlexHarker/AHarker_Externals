
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


// Class and object structure

t_class *this_class;

struct t_dynamic_request
{
    t_pxobject x_obj;
    
    long num_sig_ins;
    void **sig_ins;
    
    t_atom_long inlet_num;
    bool valid;
    
    double prev;
};

// Function prototypes

void *dynamic_request_new(t_atom_long inlet_num);
void dynamic_request_free(t_dynamic_request *x);
void dynamic_request_assist(t_dynamic_request *x, void *b, long m, long a, char *s);

void dynamic_request_int(t_dynamic_request *x, t_atom_long inlet_num);

t_int *dynamic_request_perform(t_int *w);
t_int *dynamic_request_perform_small(t_int *w);
void dynamic_request_perform64(t_dynamic_request *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void dynamic_request_perform_small64(t_dynamic_request *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

void dynamic_request_dsp(t_dynamic_request *x, t_signal **sp, short *count);
void dynamic_request_dsp64(t_dynamic_request *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

// Main

int C74_EXPORT main()
{
    this_class = class_new("dynamic.request~",
                           (method) dynamic_request_new,
                           (method) dynamic_request_free,
                           sizeof(t_dynamic_request),
                           nullptr,
                           A_DEFLONG,
                           0);
    
    class_addmethod(this_class, (method) dynamic_request_dsp, "dsp", A_CANT, 0);
    class_addmethod(this_class, (method) dynamic_request_dsp64, "dsp64", A_CANT, 0);
    class_addmethod(this_class, (method) dynamic_request_assist, "assist", A_CANT, 0);
    class_addmethod(this_class, (method) dynamic_request_int, "int", A_LONG, 0);
    
    class_dspinit(this_class);
    
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / free / assist

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
    
    dsp_setup((t_pxobject *)x, 1);
    outlet_new((t_object *)x, "signal");
    
    dynamic_request_int(x, inlet_num);
    
    return x;
}

void dynamic_request_free(t_dynamic_request *x)
{
    dsp_free(&x->x_obj);
}

void dynamic_request_assist(t_dynamic_request *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
        sprintf(s,"(signal) Requested Input %ld of Patcher", (long) x->inlet_num);
    else
        sprintf(s,"(signal) Request Value / (int) Inlet Number");
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

t_int *dynamic_request_perform(t_int *w)
{
    float *in = (float *)(w[1]);
    float *out = (float *)(w[2]);
    long vec_size = w[3];
    t_dynamic_request *x = (t_dynamic_request *)(w[4]);
    
    float *from;
    float prev = x->prev;
    
    from = (float *) x->sig_ins[x->inlet_num - 1];
    
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
    
    return w + 5;
}

// Non loop unrolled version for small vector sizes

t_int *dynamic_request_perform_small(t_int *w)
{
    float *in = (float *)(w[1]);
    float *out = (float *)(w[2]);
    long vec_size = w[3];
    t_dynamic_request *x = (t_dynamic_request *)(w[4]);
    
    float *from;
    float prev = x->prev;
    
    from = (float *) x->sig_ins[x->inlet_num - 1];
    
    for (long i = 0; i < vec_size; i++, from++)
    {
        if (*in++)
            prev = *from;
        *out++ = prev;
    }
    
    x->prev = prev;
    
    return w + 5;
}

// 64 Bit Manually loop unrolled for speed - if the vs is above 4 then we use this version

void dynamic_request_perform64(t_dynamic_request *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    double *in = ins[0];
    double *out = outs[0];
    double *from;
    
    double prev = x->prev;
    
    from = (double *) x->sig_ins[x->inlet_num - 1];
    
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

// 64 Bit Non loop unrolled version for small vector sizes

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

void dynamic_request_dsp(t_dynamic_request *x, t_signal **sp, short *count)
{
    if (x->valid)
    {
        if (sp[0]->s_n > 4)
            dsp_add(dynamic_request_perform, 4, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n, x);
        else
            dsp_add(dynamic_request_perform_small, 4, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n, x);
    }
}

void dynamic_request_dsp64(t_dynamic_request *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    if (x->valid)
    {
        if (maxvectorsize > 4)
            object_method(dsp64, gensym("dsp_add64"), x, dynamic_request_perform64, 0, nullptr);
        else
            object_method(dsp64, gensym("dsp_add64"), x, dynamic_request_perform_small64, 0, nullptr);
    }
}

