
/*
 *  dynamic.out~
 *
 *	dynamic.out~ acts like out~ but for patchers loaded inside a dynamicdsp~ object.
 *	
 *	Unlike out~ the user can change the signal outlet which the object refers to by sending an int to the object.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <z_dsp.h>
#include <ext_obex.h>

#include <AH_VectorOps.h>
#include <dynamicdsp~.h>


t_class *this_class;


typedef struct _dynamic_out
{
    t_pxobject x_obj;
	
	void ***out_handle;
	long num_sig_outs;
	
	t_atom_long outlet_num;
	
} t_dynamic_out;


void dynamic_out_free(t_dynamic_out *x);
void *dynamic_out_new(t_atom_long out_num);
void dynamic_out_assist(t_dynamic_out *x, void *b, long m, long a, char *s);

void dynamic_out_dsp(t_dynamic_out *x, t_signal **sp, short *count);
void dynamic_out_dsp64 (t_dynamic_out *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

void dynamic_out_int(t_dynamic_out *x, t_atom_long intin);
t_int *dynamic_out_perform_scalar(t_int *w);
t_int *dynamic_out_perform(t_int *w);
void dynamic_out_perform_scalar64 (t_dynamic_out *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
#ifdef VECTOR_F64_128BIT
void dynamic_out_perform64 (t_dynamic_out *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
#endif


int C74_EXPORT main(void)
{
	this_class = class_new("dynamic.out~",
						   (method) dynamic_out_new, 
						   (method)dynamic_out_free, 
						   sizeof(t_dynamic_out), 
						   NULL, 
						   A_DEFLONG, 
						   0);
    
	class_addmethod(this_class, (method)dynamic_out_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)dynamic_out_dsp64, "dsp64", A_CANT, 0);
    class_addmethod(this_class, (method)dynamic_out_assist, "assist", A_CANT, 0);
	class_addmethod(this_class, (method)dynamic_out_int, "int", A_LONG, 0);
    
	class_dspinit(this_class);
	
	class_register(CLASS_BOX, this_class);
	
	return 0;
}

void dynamic_out_free(t_dynamic_out *x)
{
	dsp_free(&x->x_obj);
}

void *dynamic_out_new(t_atom_long outlet_num)
{
    t_dynamic_out *x = (t_dynamic_out *)object_alloc(this_class);
	void *dynamicdsp_parent = Get_Dynamic_Object();
	
	x->out_handle = Dynamic_Get_Sig_Out_Handle(dynamicdsp_parent, Get_Dynamic_Patch_Index(dynamicdsp_parent));
	x->num_sig_outs = Dynamic_Get_Num_Sig_Outs(dynamicdsp_parent);
	
	x->outlet_num = outlet_num;
		
     dsp_setup((t_pxobject *)x, 1);
    
    return (x);
}

void dynamic_out_assist(t_dynamic_out *x, void *b, long m, long a, char *s)
{
    sprintf(s,"(signal) Signal Output %ld of Patcher / (int) Outlet Number",(long)  x->outlet_num);
}

void dynamic_out_dsp(t_dynamic_out *x, t_signal **sp, short *count)
{
	if (((t_ptr_size) sp[0]->s_vec) % 16 || sp[0]->s_n < 8 || !SSE2_check())
		dsp_add(dynamic_out_perform_scalar, 3, sp[0]->s_vec, sp[0]->s_n, x);		// misaligned vector (not 16-byte aligned) or small vector sizes
	else
		dsp_add(dynamic_out_perform, 3, sp[0]->s_vec, sp[0]->s_n, x);				// aligned vector (should be the case from max 5.1.3 onwards
}

void dynamic_out_dsp64(t_dynamic_out *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
#ifdef VECTOR_F64_128BIT
	if (maxvectorsize >= 8 && SSE2_check())
		object_method(dsp64, gensym("dsp_add64"), x, dynamic_out_perform64, 0, NULL);				// aligned vector 
    else
#endif
		object_method(dsp64, gensym("dsp_add64"), x, dynamic_out_perform_scalar64, 0, NULL);		// scalar routine
}

void dynamic_out_int (t_dynamic_out *x, t_atom_long intin)
{
	x->outlet_num = intin;
}

// Perform Routine for misaligned vectors or small vector sizes (done in scalar code)

t_int *dynamic_out_perform_scalar(t_int *w)										
{	
    float *in1 = (float *)(w[1]);
    long vec_size = w[2];
    t_dynamic_out *x = (t_dynamic_out *)(w[3]);
	
	float ***out_handle = (float ***) x->out_handle;
    float *io_pointer;
	
	long outlet_num = x->outlet_num;
	long num_sig_outs = x->num_sig_outs;
	
    if (!out_handle || !*out_handle)
        return w + 4;

    if (outlet_num > 0 && outlet_num <= num_sig_outs)
    {
        if ((io_pointer = (*out_handle)[outlet_num - 1]))
		{
			for (long i = 0; i < vec_size; i++)
				*io_pointer++ += *in1++;
		}
	}
	
    return w + 4;
}

// Aligned SIMD Perform Routine

t_int *dynamic_out_perform(t_int *w)												
{	
    vFloat *in1 = (vFloat *)(w[1]);
    long vec_size = w[2];
    t_dynamic_out *x = (t_dynamic_out *)(w[3]);
	
	float ***out_handle = (float ***) x->out_handle;
    vFloat *io_pointer;
	
	long num_sig_outs = x->num_sig_outs;
	long outlet_num = x->outlet_num;
	
    if (!out_handle || !*out_handle)
         return w + 4;
    
    if (outlet_num > 0 && outlet_num <= num_sig_outs)
    {
        if ((io_pointer = (vFloat *) (*out_handle)[outlet_num - 1]))
        {
            for (long i = 0; i < vec_size >> 2; i++)
            {
                *io_pointer = F32_VEC_ADD_OP(*io_pointer, *in1++);
                io_pointer++;
            }
        }
    }
	
    return w + 4;
}

// Perform Routine for misaligned vectors or small vector sizes (done in scalar code) 64 Bit

void dynamic_out_perform_scalar64(t_dynamic_out *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
    double *in1 = ins[0];	
	double *io_pointer;
	double ***out_handle = (double ***) x->out_handle;
	
	t_atom_long outlet_num = x->outlet_num;
	long num_sig_outs = x->num_sig_outs;
	
    if (!out_handle || !*out_handle)
        return;
    
    if (outlet_num > 0 && outlet_num <= num_sig_outs)
    {
        if ((io_pointer = (*out_handle)[outlet_num - 1]))
        {
			for (long i = 0; i < vec_size; i++)
				*io_pointer++ += *in1++;
				io_pointer++;
		}
	}
}

#ifdef VECTOR_F64_128BIT

// Aligned SIMD Perform Routine 64 Bit

void dynamic_out_perform64(t_dynamic_out *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
    vDouble *in1 = (vDouble *) ins[0];	
	vDouble *io_pointer;
	
	double ***out_handle = (double ***) x->out_handle;
	
	long num_sig_outs = x->num_sig_outs;
	t_atom_long outlet_num = x->outlet_num;
	
	if (!out_handle || !*out_handle)
        return;
    
    if (outlet_num > 0 && outlet_num <= num_sig_outs)
    {
        if ((io_pointer = (vDouble *) (*out_handle)[outlet_num - 1]))
        {
            for (long i = 0; i < vec_size >> 1; i++)
            {
                *io_pointer = F64_VEC_ADD_OP(*io_pointer, *in1++);
                io_pointer++;
            }
        }
    }
}

#endif

