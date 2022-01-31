
/*
 *  rdcblock~ (resetable dc blocking filter)
 *
 *  rdcblock~ is a  dc blocking filter in which the memory can be reset on a sample-accurate basis.
 *
 *  There are two reset modes, one that zeros the memory, another that resets to the current input value.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Denormals.h>


// Globals and Object Structure

t_class *this_class;

struct t_rdcblock
{
    t_pxobject x_obj;
    
    double x1;
    double y1;
    
    t_atom_long mode;
};

// Function Prototypes

void *rdcblock_new(t_atom_long mode);
void rdcblock_free(t_rdcblock *x);
void rdcblock_assist(t_rdcblock *x, void *b, long m, long a, char *s);

t_int *rdcblock_perform(t_int *w);
t_int *rdcblock_perform_inval(t_int *w);

void rdcblock_perform64(t_rdcblock *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void rdcblock_perform_inval64(t_rdcblock *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

void rdcblock_dsp(t_rdcblock *x, t_signal **sp, short *count);
void rdcblock_dsp64(t_rdcblock *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

// Main

int C74_EXPORT main()
{
    this_class = class_new("rdcblock~",
                           (method) rdcblock_new,
                           (method) rdcblock_free,
                           sizeof(t_rdcblock),
                           (method) nullptr,
                           A_DEFLONG,
                           0);
    
    class_addmethod(this_class, (method) rdcblock_assist, "assist", A_CANT, 0);
    class_addmethod(this_class, (method) rdcblock_dsp, "dsp", A_CANT, 0);
    class_addmethod(this_class, (method) rdcblock_dsp64, "dsp64", A_CANT, 0);
    
    class_dspinit(this_class);
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / Free

void *rdcblock_new(t_atom_long mode)
{
    t_rdcblock *x = (t_rdcblock *) object_alloc(this_class);
    
    dsp_setup((t_pxobject *) x, 2);
    outlet_new((t_object *) x, "signal");
    
    x->x1 = x->y1 = 0.0;
    x->mode = mode;
    
    return x;
}

void rdcblock_free(t_rdcblock *x)
{
    dsp_free(&x->x_obj);
}

// Perform

t_int *rdcblock_perform(t_int *w)
{
    float *in1 = (float *) w[2];
    float *in2 = (float *) w[3];
    float *out = (float *) w[4];
    long vec_size = (long) w[5];
    t_rdcblock *x = (t_rdcblock *) w[6];
    
    double x0, y;
    
    // Recall memory
    
    double x1 = x->x1;
    double y1 = x->y1;
    
    float yf;
    
    while (vec_size--)
    {
        x0 = *in1++;
        
        // Sample accurate reset
        
        if (*in2++)
            x1 = y1 = 0.0;
        
        // Filter
        
        y = x0 - x1 + (0.99 * y1);
        FIX_DENORM_DOUBLE(y);
        
        // Shift memories
        
        x1 = x0;
        y1 = y;
        
        yf = (float) y;
        FIX_DENORM_FLOAT(yf);
        *out++ = yf;
    }
    
    // Store memory
    
    x->x1 = x1;
    x->y1 = y1;
    
    return w + 7;
}

t_int *rdcblock_perform_inval(t_int *w)
{
    float *in1 = (float *) w[2];
    float *in2 = (float *) w[3];
    float *out = (float *) w[4];
    long vec_size = (long) w[5];
    t_rdcblock *x = (t_rdcblock *) w[6];
    
    double x0, y;
    
    // Recall memory
    
    double x1 = x->x1;
    double y1 = x->y1;
    
    float yf;
    
    while (vec_size--)
    {
        x0 = *in1++;
        
        // Sample accurate reset
        
        if (*in2++)
        {
            x1 = x0;
            y1 = 0.0;
        }
        
        // Filter
        
        y = x0 - x1 + (0.99 * y1);
        FIX_DENORM_DOUBLE(y);
        
        // Shift memories
        
        x1 = x0;
        y1 = y;
        
        yf = (float) y;
        FIX_DENORM_FLOAT(yf);
        *out++ = yf;
    }
    
    // Store memory
    
    x->x1 = x1;
    x->y1 = y1;
    
    return w + 7;
}

void rdcblock_perform64(t_rdcblock *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    double *in1 = ins[0];
    double *in2 = ins[1];
    double *out = outs[0];
    
    // Recall memory
    
    double x1 = x->x1;
    double y1 = x->y1;
    double x0, y;
    
    while (vec_size--)
    {
        x0 = *in1++;
        
        // Sample accurate reset
        
        if (*in2++)
            x1 = y1 = 0.0;
        
        // Filter
        
        y = x0 - x1 + (0.99 * y1);
        FIX_DENORM_DOUBLE(y);
        
        // Shift memories
        
        x1 = x0;
        y1 = y;
        
        *out++ = y;
    }
    
    // Store memory
    
    x->x1 = x1;
    x->y1 = y1;
}

void rdcblock_perform_inval64(t_rdcblock *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    double *in1 = ins[0];
    double *in2 = ins[1];
    double *out = outs[0];
    
    // Recall memory
    
    double x1 = x->x1;
    double y1 = x->y1;
    double x0, y;
    
    while (vec_size--)
    {
        x0 = *in1++;
        
        // Sample accurate reset
        
        if (*in2++)
        {
            x1 = x0;
            y1 = 0.0;
        }
        
        // Filter and shift memory
        
        y = x0 - x1 + (0.99 * y1);
        FIX_DENORM_DOUBLE(y);
        
        // Shift memories
        
        x1 = x0;
        y1 = y;
        
        *out++ = y;
    }
    
    // Store memory
    
    x->x1 = x1;
    x->y1 = y;
}

// DSP

void rdcblock_dsp(t_rdcblock *x, t_signal **sp, short *count)
{
    if (x->mode)
        dsp_add(denormals_perform, 6, rdcblock_perform_inval, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n, x);
    else
        dsp_add(denormals_perform, 6, rdcblock_perform, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n, x);
}

void rdcblock_dsp64(t_rdcblock *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    if (x->mode)
        object_method(dsp64, gensym("dsp_add64"), x, rdcblock_perform_inval64, 0, nullptr);
    else
        object_method(dsp64, gensym("dsp_add64"), x, rdcblock_perform64, 0, nullptr);
}

// Assist

void rdcblock_assist(t_rdcblock *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
    {
        switch (a)
        {
            case 0:
                sprintf(s,"(signal) Input");
                break;
                
            case 1:
                sprintf(s,"(signal) Reset Trigger");
                break;
        }
    }
    else
        sprintf(s,"(signal) Output");
}
