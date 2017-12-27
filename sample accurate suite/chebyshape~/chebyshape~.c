
/*
 *  chebyshape~
 *
 *	chebyshape~ is an object for waveshaping an audio input using chebyshev polynomials of the first kind.
 *
 *	There is a special mode for using chebyshape~ inside a dynamicdsp~ in which the coefficients may be updated directly from the inputs to the dynamicdsp~ object at specified samples.
 *	This is an efficient alternative to the normal mode, which updates coefficients every sample. 
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Denormals.h>
#include <dynamicdsp~.h>

void *this_class;


typedef struct _chebyshape
{
    t_pxobject x_obj;
	
	long num_coeff;
	
	void *coeff_ins[32];
	double coeff[32];
	
	long offset;
	long num_sig_ins;
	
	void **sig_ins;
	
} t_chebyshape;


void *chebyshape_new (t_atom_long num_coeff, t_atom_long offset);
void chebyshape_free (t_chebyshape *x);
void chebyshape_assist (t_chebyshape *x, void *b, long m, long a, char *s);

void chebyshape_dsp (t_chebyshape *x, t_signal **sp, short *count);
void chebyshape_dsp64(t_chebyshape *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

t_int *chebyshape_perform_dynamicdsp (t_int *w);
t_int *chebyshape_perform (t_int *w);

void chebyshape_perform_dynamicdsp64 (t_chebyshape *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void chebyshape_perform64 (t_chebyshape *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);


int C74_EXPORT main()
{	
	this_class = class_new("chebyshape~",
				(method)chebyshape_new,
				(method)chebyshape_free,
				sizeof(t_chebyshape), 
				NULL, 
				A_DEFLONG, 
				A_DEFLONG,
				0);
	
	class_addmethod(this_class, (method)chebyshape_assist, "assist", A_CANT, 0);
	class_addmethod(this_class, (method)chebyshape_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)chebyshape_dsp64, "dsp64", A_CANT, 0);
	   
	class_dspinit(this_class);
	class_register(CLASS_BOX, this_class);
		
	return 0;
}


void *chebyshape_new(t_atom_long num_coeff, t_atom_long offset)
{
    t_chebyshape *x = (t_chebyshape *)object_alloc(this_class);
    
	void *dynamicdsp_parent;
	long num_sig_ins;

    // Get dynamicdsp~ host (if it exists)
	
	dynamicdsp_parent = Get_Dynamic_Object();
    
	x->sig_ins = Dynamic_Get_Sig_In_Ptrs(dynamicdsp_parent);
    x->num_sig_ins = num_sig_ins = Dynamic_Get_Num_Sig_Ins(dynamicdsp_parent);
    
	// Number of coefficients
	
	if (num_coeff < 2) 
		num_coeff = 2;
	if (num_coeff > 32)
		num_coeff = 32;
	
	x->num_coeff = num_coeff;
	
	// Mode
	
	if (offset < 0)
		offset = 0;
    if (offset > num_sig_ins)
		offset = num_sig_ins;
	
	if (!offset)
    {
		dsp_setup((t_pxobject *)x, num_coeff + 1);
		outlet_new((t_object *)x, "signal");
		x->offset = 0;
	}
	else 
	{	
		dsp_setup((t_pxobject *)x, 2);
		outlet_new((t_object *)x, "signal");
		x->offset = (long) offset;
	}
		
	return (x);
}


void chebyshape_free(t_chebyshape *x)
{
	dsp_free(&x->x_obj);
}


t_int *chebyshape_perform_dynamicdsp (t_int *w)
{	
	// Set pointers
	
	float *in = (float *) w[1];
	float *trigger = (float *) w[2];
	float **coeff_ins = (float **) w[3];
	double *coeff = (double *) w[4];
	float *out = (float *) w[5];
	long vec_size = (long) w[6];
	long num_coeff = (long) w[7];
	
	double in_val;
	double cheby_mem1;
	double cheby_mem2;
	double current_cheby;
	double out_val = 0;
	
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
		// Hopefully fixing the feedback should be enough to quash denormals quickly enough  (on mac/SSE the macros below do nothing)
		
		for (i = 1;  i + 3 < max_cheby; i += 4)
		{
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff[i] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff[i+1] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff[i+2] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff[i+3] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
		}
		for (;  i < max_cheby; i++)
		{
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff[i] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
		}
		
		*out++ = AH_FIX_DENORM_FLOAT(out_val);
		j++;
	}
		
	return w + 8;
}

t_int *chebyshape_perform (t_int *w)
{	
	// Set pointers
	
	// N.B. perform routine for denormal handling
	
	float *in = (float *) w[2];
	float **coeff_ins = (float **) w[3];
	float *out = (float *) w[4];
	long vec_size = (long) w[5];
	long num_coeff = (long) w[6];
	
	double in_val;
	double cheby_mem1;
	double cheby_mem2;
	double current_cheby;
	double out_val = 0;
	
	long i, j = 0;
	
	while (vec_size--)
	{
		// Get input sample
		
		in_val = *in++;
		cheby_mem2 = 1.;
		cheby_mem1 = in_val;
		out_val = in_val * coeff_ins[0][j];
		
		// Do waveshaping iteratively (loop unrolled)
		// Hopefully fixing the feedback should be enough to quash denormals quickly enough  (on mac/SSE the macros below do nothing)
		
		for (i = 1;  i + 3 < num_coeff; i += 4)
		{
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff_ins[i][j] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff_ins[i+1][j] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff_ins[i+2][j] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff_ins[i+3][j] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
		}
		for (;  i < num_coeff;  i++)
		{
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff_ins[i][j] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
		}
		
		*out++ = AH_FIX_DENORM_FLOAT(out_val);
		j++;
	}
				
	return w + 7;
}


void chebyshape_perform_dynamicdsp64 (t_chebyshape *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
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
		// Hopefully fixing the feedback should be enough to quash denormals quickly enough  (on mac/SSE the macros below do nothing)
		
		for (i = 1;  i + 3 < max_cheby; i += 4)
		{
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff[i] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff[i+1] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff[i+2] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff[i+3] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
		}
		for (;  i < max_cheby; i++)
		{
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff[i] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
		}
		
		*out++ = AH_FIX_DENORM_FLOAT(out_val);
		j++;
	}
}


void chebyshape_perform64 (t_chebyshape *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
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
		// Hopefully fixing the feedback should be enough to quash denormals quickly enough  (on mac/SSE the macros below do nothing)
		
		for (i = 1;  i + 3 < num_coeff; i += 4)
		{
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff_ins[i][j] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff_ins[i+1][j] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff_ins[i+2][j] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff_ins[i+3][j] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
		}
		for (;  i < num_coeff;  i++)
		{
			current_cheby = (2 * in_val * cheby_mem1) - cheby_mem2;
			out_val += coeff_ins[i][j] * current_cheby;
			cheby_mem2 = cheby_mem1;
			cheby_mem1 = AH_FIX_DENORM_DOUBLE(current_cheby);
		}
		
		*out++ = AH_FIX_DENORM_FLOAT(out_val);
		j++;
	}
}


void chebyshape_dsp(t_chebyshape *x, t_signal **sp, short *count)
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
	{
		for (i = 0; i < num_coeff; i++)
			coeff_ins[i] = sp[i + 1]->s_vec;
		
		dsp_add(denormals_perform, 6, chebyshape_perform, sp[0]->s_vec, coeff_ins, sp[num_coeff + 1]->s_vec, sp[0]->s_n, num_coeff);
	}
	else 
	{
		if (offset + num_coeff - 1 <= num_sig_ins)
		{
			for (i = 0; i < num_coeff; i++)
			{
				coeff_ins[i] = sig_ins[i + offset - 1];
				coeff[i] = 0.;
			}
			dsp_add(chebyshape_perform_dynamicdsp, 7, sp[0]->s_vec, sp[1]->s_vec, coeff_ins, coeff, sp[2]->s_vec, sp[0]->s_n, num_coeff);
		}
	}
}


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
		object_method(dsp64, gensym("dsp_add64"), x, chebyshape_perform64, 0, NULL);
	else 
	{
		if (offset + num_coeff - 1 <= num_sig_ins)
		{
			for (i = 0; i < num_coeff; i++)
			{
				coeff_ins[i] = sig_ins[i + offset - 1];
				coeff[i] = 0.;
			}
			object_method(dsp64, gensym("dsp_add64"), x, chebyshape_perform_dynamicdsp64, 0, NULL);
		}
	}
}



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



