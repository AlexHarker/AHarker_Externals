
/*
 *  chebyshape~
 *
 *  chebyshape~ is an object for waveshaping using chebyshev polynomials of the first kind.
 *
 *  There is a special mode for using chebyshape~ inside a dynamic~ host object.
 *  In this mode the coefficients may be updated sample-accurately directly from the inputs to the host object.
 *  This is an efficient alternative to the normal mode, which updates coefficients every sample.
 *
 *  Copyright 2010-21 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <algorithm>

#include <dynamic~.hpp>

// Globals and Object Structure

t_class *this_class;

constexpr long max_coeff = 32;

struct t_chebyshape
{
    t_pxobject x_obj;
    
    long num_coeff;
    
    void *coeff_ins[max_coeff];
    double coeff[max_coeff];
    
    long offset;
    long num_sig_ins;
    
    void **sig_ins;
};

// Function Protoypes

void *chebyshape_new(t_atom_long num_coeff, t_atom_long offset);
void chebyshape_free(t_chebyshape *x);
void chebyshape_assist(t_chebyshape *x, void *b, long m, long a, char *s);

void chebyshape_perform_dynamic64 (t_chebyshape *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void chebyshape_perform64 (t_chebyshape *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

void chebyshape_dsp64(t_chebyshape *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

// Main

int C74_EXPORT main()
{
    this_class = class_new("chebyshape~",
                           (method) chebyshape_new,
                           (method) chebyshape_free,
                           sizeof(t_chebyshape),
                           nullptr,
                           A_DEFLONG,
                           A_DEFLONG,
                           0);
    
    class_addmethod(this_class, (method) chebyshape_assist, "assist", A_CANT, 0);
    class_addmethod(this_class, (method) chebyshape_dsp64, "dsp64", A_CANT, 0);
    
    class_dspinit(this_class);
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / Free

void *chebyshape_new(t_atom_long num_coeff, t_atom_long offset)
{
    t_chebyshape *x = (t_chebyshape *) object_alloc(this_class);
    
    // Get dynamicdsp~ host (if it exists)
    
    void *dynamic_parent = dynamic_get_parent();
    
    x->sig_ins = dynamic_get_sig_in_ptrs(dynamic_parent);
    x->num_sig_ins = dynamic_get_num_sig_ins(dynamic_parent);
    
    // Clip Number of coefficients / Mode
    
    x->num_coeff = std::max(2L, std::min(static_cast<long>(num_coeff), max_coeff));
    x->offset = std::max(0L, std::min(static_cast<long>(offset), x->num_sig_ins));
    
    if (!x->offset)
        dsp_setup((t_pxobject *)x, num_coeff + 1);
    else
        dsp_setup((t_pxobject *)x, 2);
    
    outlet_new((t_object *)x, "signal");
    
    return x;
}

void chebyshape_free(t_chebyshape *x)
{
    dsp_free(&x->x_obj);
}

// Perform

// Perform (within a dynamic~ host object)

void chebyshape_perform_dynamic64(t_chebyshape *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    // Set pointers
    
    double *in = ins[0];
    double *trigger = ins[1];
    double **coeff_ins = ((double **) (x->sig_ins)) + x->offset - 1;
    double *coeff = x->coeff;
    double *out = outs[0];
    
    double in_val;
    double cheby_mem1;
    double cheby_mem2;
    double current_cheby;
    double out_val = 0;
    
    long num_coeff = x->num_coeff;
    long max_cheby;
    long i, j = 0;
    
    // Recall coefficients
    
    for (i = 0, max_cheby = 0; i < num_coeff; i++)
        if (coeff[i])
            max_cheby = i + 1;
    
    while (vec_size--)
    {
        // Get input sample
        
        in_val = *in++;
        cheby_mem2 = 1.;
        cheby_mem1 = in_val;
        out_val = in_val * coeff[0];
        
        // Update coeffients (loop unrolled)
        
        if (*trigger++)
        {
            for (i = 0; i + 3 < num_coeff; i += 4)
            {
                coeff[i+0] = coeff_ins[i+0][j];
                coeff[i+1] = coeff_ins[i+1][j];
                coeff[i+2] = coeff_ins[i+2][j];
                coeff[i+3] = coeff_ins[i+3][j];
            }
            for (; i < num_coeff; i++)
                coeff[i] = coeff_ins[i][j];
            
            for (i = 0, max_cheby = 0; i < num_coeff; i++)
                if (coeff[i])
                    max_cheby = i + 1;
        }
        
        // Do waveshaping iteratively (loop unrolled)
        
        for (i = 1;  i + 3 < max_cheby; i += 4)
        {
            current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
            out_val += coeff[i] * current_cheby;
            cheby_mem2 = cheby_mem1;
            cheby_mem1 = FIX_DENORM_DOUBLE(current_cheby);
            current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
            out_val += coeff[i+1] * current_cheby;
            cheby_mem2 = cheby_mem1;
            cheby_mem1 = FIX_DENORM_DOUBLE(current_cheby);
            current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
            out_val += coeff[i+2] * current_cheby;
            cheby_mem2 = cheby_mem1;
            cheby_mem1 = FIX_DENORM_DOUBLE(current_cheby);
            current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
            out_val += coeff[i+3] * current_cheby;
            cheby_mem2 = cheby_mem1;
            cheby_mem1 = FIX_DENORM_DOUBLE(current_cheby);
        }
        for (;  i < max_cheby; i++)
        {
            current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
            out_val += coeff[i] * current_cheby;
            cheby_mem2 = cheby_mem1;
            cheby_mem1 = FIX_DENORM_DOUBLE(current_cheby);
        }
        
        *out++ = FIX_DENORM_DOUBLE(out_val);
        j++;
    }
}

// Perform (standard)

void chebyshape_perform64(t_chebyshape *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    // Set pointers
    
    double *in = ins[0];
    double **coeff_ins = ins + 1;
    double *out = outs[0];
    
    double in_val;
    double cheby_mem1;
    double cheby_mem2;
    double current_cheby;
    double out_val = 0;
    
    long num_coeff = x->num_coeff;
    long i, j = 0;
    
    while (vec_size--)
    {
        // Get input sample
        
        in_val = *in++;
        cheby_mem2 = 1.;
        cheby_mem1 = in_val;
        out_val = in_val * coeff_ins[0][j];
        
        // Do waveshaping iteratively (loop unrolled)
        
        for (i = 1;  i + 3 < num_coeff; i += 4)
        {
            current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
            out_val += coeff_ins[i][j] * current_cheby;
            cheby_mem2 = cheby_mem1;
            cheby_mem1 = FIX_DENORM_DOUBLE(current_cheby);
            current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
            out_val += coeff_ins[i+1][j] * current_cheby;
            cheby_mem2 = cheby_mem1;
            cheby_mem1 = FIX_DENORM_DOUBLE(current_cheby);
            current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
            out_val += coeff_ins[i+2][j] * current_cheby;
            cheby_mem2 = cheby_mem1;
            cheby_mem1 = FIX_DENORM_DOUBLE(current_cheby);
            current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
            out_val += coeff_ins[i+3][j] * current_cheby;
            cheby_mem2 = cheby_mem1;
            cheby_mem1 = FIX_DENORM_DOUBLE(current_cheby);
        }
        for (;  i < num_coeff;  i++)
        {
            current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
            out_val += coeff_ins[i][j] * current_cheby;
            cheby_mem2 = cheby_mem1;
            cheby_mem1 = FIX_DENORM_DOUBLE(current_cheby);
        }
        
        *out++ = FIX_DENORM_DOUBLE(out_val);
        j++;
    }
}

// DSP

void chebyshape_dsp64(t_chebyshape *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    void **sig_ins = x->sig_ins;
    void **coeff_ins = x->coeff_ins;
    
    double *coeff = x->coeff;
    
    long num_sig_ins = x->num_sig_ins;
    long num_coeff = x->num_coeff;
    long offset = x->offset;
    long i;
    
    // Add perform routine according to mode
    
    if (!offset)
        object_method(dsp64, gensym("dsp_add64"), x, chebyshape_perform64, 0, nullptr);
    else
    {
        if (offset + num_coeff - 1 <= num_sig_ins)
        {
            for (i = 0; i < num_coeff; i++)
            {
                coeff_ins[i] = sig_ins[i + offset - 1];
                coeff[i] = 0.;
            }
            object_method(dsp64, gensym("dsp_add64"), x, chebyshape_perform_dynamic64, 0, nullptr);
        }
    }
}

// Assist

void chebyshape_assist(t_chebyshape *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
    {
        sprintf(s,"(signal) Waveshaped Output");
    }
    else
    {
        switch (a)
        {
            case 0:
                sprintf(s,"(signal) Input");
                break;
                
            default:
                if (x->offset)
                    sprintf(s,"(signal) Triggers");
                else
                    sprintf(s,"(signal) Coefficient %ld", a - 1);
        }
    }
}

