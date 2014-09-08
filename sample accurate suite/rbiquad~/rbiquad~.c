
/*
 *  rbiquad~ (reset - able biquad)
 *
 *	rbiquad~ is a biquad filter that accepts only signal rate coefficients. The memory can be cleared on a sample accurate basis, using a dedicated reset input.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Denormals.h>

void *this_class;


typedef struct _rbiquad
{
    t_pxobject x_obj;
	
	double in_mem1;
	double in_mem2;
	double y1;
	double y2;
	
} t_rbiquad;


void *rbiquad_new ();
void rbiquad_free (t_rbiquad *x);
void rbiquad_assist (t_rbiquad *x, void *b, long m, long a, char *s);

void rbiquad_dsp (t_rbiquad *x, t_signal **sp, short *count);
t_int *rbiquad_perform (t_int *w);

void rbiquad_perform64 (t_rbiquad *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void rbiquad_dsp64 (t_rbiquad *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);


int C74_EXPORT main (void)
{	
	this_class = class_new("rbiquad~",
				(method)rbiquad_new,
				(method)rbiquad_free,
				sizeof(t_rbiquad), 
				NULL, 
				0);
	
	class_addmethod(this_class, (method)rbiquad_assist, "assist", A_CANT, 0);
	class_addmethod(this_class, (method)rbiquad_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)rbiquad_dsp64, "dsp64", A_CANT, 0);
	
	class_dspinit(this_class);
	class_register(CLASS_BOX, this_class);
		
	return 0;
}


void *rbiquad_new()
{
    t_rbiquad *x = (t_rbiquad *)object_alloc(this_class);
    
    dsp_setup((t_pxobject *)x, 7);
	outlet_new((t_object *)x, "signal");
	
	x->in_mem1 = x->in_mem2 = x->y1 = x->y2 = 0.;
	
	return (x);
}


void rbiquad_free(t_rbiquad *x)
{
	dsp_free(&x->x_obj);
}


t_int *rbiquad_perform (t_int *w)
{	
	// set pointers
	
	float *in1 = (float *) w[2];
	float *in2 = (float *) w[3];
	float *in3 = (float *) w[4];
	float *in4 = (float *) w[5];
	float *in5 = (float *) w[6];
	float *in6 = (float *) w[7];
	float *in7 = (float *) w[8];
	float *out = (float *) w[9];
	long vec_size = w[10];
    t_rbiquad *x = (t_rbiquad *) w[11];
	
	double a0, a1, a2, b1, b2, in, y;
	
	// Recall memory
	
	double in_mem1 = x->in_mem1;
	double in_mem2 = x->in_mem2;
	double y1 = x->y1;
	double y2 = x->y2;
	
	float out_val;
	
	while (vec_size--) 
	{
		// Get input and coeffiecients
		
		in = *in1++;
		a0 = *in2++;
		a1 = *in3++;
		a2 = *in4++;
		b1 = *in5++;
		b2 = *in6++;
		
		// Sample accurate reset
		
		if (*in7++)						
			in_mem1 = in_mem2 = y1 = y2 = 0.;
		
		// Filter 
		
		y = (a0 * in) + (a1 * in_mem1) + (a2 * in_mem2) - (b1 * y1) - (b2 * y2);
		
		out_val = (float) y;
		
		*out++ = AH_FIX_DENORM_FLOAT(out_val);
		
		// Shift memory
		
		y2 = y1;
		y1 = AH_FIX_DENORM_DOUBLE(y);
		in_mem2 = in_mem1;
		in_mem1 = in;
	}
	
	// Store memory
	
	x->in_mem1 = in_mem1;
	x->in_mem2 = in_mem2;
	x->y1 = y1;
	x->y2 = y2;
	
	return w + 12;
}


void rbiquad_dsp(t_rbiquad *x, t_signal **sp, short *count)
{				
	dsp_add(denormals_perform, 11, rbiquad_perform, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec, sp[7]->s_vec, sp[0]->s_n, x);
}



void rbiquad_perform64 (t_rbiquad *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
	// set pointers
	
	double *in1 = ins[0];
	double *in2 = ins[1];
	double *in3 = ins[2];
	double *in4 = ins[3];
	double *in5 = ins[4];
	double *in6 = ins[5];
	double *in7 = ins[6];
	double *out = outs[0];
	
	double a0, a1, a2, b1, b2, in, y;
	
	// Recall memory
	
	double in_mem1 = x->in_mem1;
	double in_mem2 = x->in_mem2;
	double y1 = x->y1;
	double y2 = x->y2;
	double out_val;
	
	while (vec_size--) 
	{
		// Get input and coeffiecients
		
		in = *in1++;
		a0 = *in2++;
		a1 = *in3++;
		a2 = *in4++;
		b1 = *in5++;
		b2 = *in6++;
		
		// Sample accurate reset
		
		if (*in7++)						
			in_mem1 = in_mem2 = y1 = y2 = 0.;
		
		// Filter 
		
		y = (a0 * in) + (a1 * in_mem1) + (a2 * in_mem2) - (b1 * y1) - (b2 * y2);

		out_val = AH_FIX_DENORM_DOUBLE(y);
		
		*out++ = out_val;
		
		// Shift memory
		
		y2 = y1;
		y1 = out_val;
		in_mem2 = in_mem1;
		in_mem1 = in;
	}
	
	// Store memory
	
	x->in_mem1 = in_mem1;
	x->in_mem2 = in_mem2;
	x->y1 = y1;
	x->y2 = y2;
}


void rbiquad_dsp64 (t_rbiquad *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{				
	object_method(dsp64, gensym("dsp_add64"), x, rbiquad_perform64, 0, NULL);
}



void rbiquad_assist(t_rbiquad *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET)
	{
		switch (a)
		{
			case 0:
				sprintf(s,"(signal) Input");
				break;
				
			case 1:
				sprintf(s,"(signal) FF Coefficient 0");
				break;
				
			case 2:
				sprintf(s,"(signal) FF Coefficient 1");
				break;
				
			case 3:
				sprintf(s,"(signal) FF Coefficient 2");
				break;
				
			case 4:
				sprintf(s,"(signal) FB Coefficient 1");
				break;
				
			case 5:
				sprintf(s,"(signal) FB Coefficient 2");
				break;

			case 6:
				sprintf(s,"(signal) Reset Trigger");
				break;				
		}
	}
	else
		sprintf(s,"(signal) Output");}



