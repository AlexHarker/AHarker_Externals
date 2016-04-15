
#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_VectorOps.h>

void *this_class;


typedef struct _vectests
{
	t_pxobject a_obj;
    long vector;
		
} t_vectests;


void *vectests_new ();
void vectests_free (t_vectests *x);
void vectests_assist (t_vectests *x, void *b, long m, long a, char *s);

t_int *vectests_perform (t_int *w);
void vectests_dsp (t_vectests *x, t_signal **sp, short *count);
void vectests_int (t_vectests *x, t_atom_long);


void vectests_perform64 (t_vectests *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void vectests_dsp64 (t_vectests *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);


int C74_EXPORT main (void)
{	
	this_class = class_new("vectests~",
						   (method)vectests_new,
						   (method)vectests_free,
						   sizeof(t_vectests), 
						   NULL, 
						   0);
	
    class_addmethod(this_class, (method)vectests_int, "int", A_LONG, 0);
	class_addmethod(this_class, (method)vectests_dsp64, "dsp64", A_CANT, 0);
	class_addmethod(this_class, (method)vectests_assist, "assist", A_CANT, 0);
	
	class_dspinit(this_class);
	
	class_register(CLASS_BOX, this_class);
	
	return 0;
}


void *vectests_new()
{
    t_vectests *x = (t_vectests *)object_alloc(this_class);

    dsp_setup((t_pxobject *)x, 1);
	outlet_new((t_object *)x, "signal");
	
    x->vector = 1;
    
	return (x);
}


void vectests_free(t_vectests *x)
{
	dsp_free(&x->a_obj);	
}

void vectests_int(t_vectests *x, t_atom_long in)
{
    x->vector = in;
}

void vectests_perform64 (t_vectests *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
	// Set pointers

    double *in = ins[0];
	double *out = outs[0];
    
    vDouble *vin = (vDouble *) in;
    vDouble *vout = (vDouble *) out;
    
    long long *lout = (long long *)out;
    
    if (abs(x->vector) == 8)
    {
        if (x->vector > 0)
        {
            vSInt64 temp;
            
            for (unsigned int i = 0; i < (vec_size >> 1); i ++)
                F64_VEC_SPLIT_I64_F64(vin[i], &temp, vout + i);
        }
        else
        {
            for (unsigned int i = 0; i < vec_size; i ++)
            {
                long long lval = (long long) in[i];
                out[i] = in[i] - (double) lval;
            }
        }
    }
    else
    {
        if (x->vector > 0)
        {
            for (unsigned int i = 0; i < (vec_size >> 1); i ++)
                vout[i] = I64_VEC_FROM_F64_TRUNC(vin[i]);
        }
        else
        {
            for (unsigned int i = 0; i < vec_size; i ++)
                lout[i] = (long long) in[i];
        }
        
        if (abs(x->vector) > 1)
        {
            for (unsigned int i = 0; i < vec_size; i ++)
                out[i] = (double) lout[i];
        }
    }
}


void vectests_dsp64 (t_vectests *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{				
	object_method(dsp64, gensym("dsp_add64"), x, vectests_perform64, 0, NULL);
}


void vectests_assist(t_vectests *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_OUTLET) 
	{
		sprintf(s,"(signal) Noise");
	}
    else 
	{
		sprintf(s,"(signal) Dummy");
    }
}

