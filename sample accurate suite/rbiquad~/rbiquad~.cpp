
/*
 *  rbiquad~ (resetable biquad)
 *
 *	rbiquad~ is a biquad filter that accepts only signal rate coefficients.
 *  The memory can be cleared on a sample accurate basis, using a dedicated reset input.
 *
 *  Copyright 2010-21 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>


// Globals and Object Structure

t_class *this_class;

struct t_rbiquad
{
    t_pxobject x_obj;
	
	double x1;
	double x2;
	double y1;
	double y2;
};

// Function Prototypes

void *rbiquad_new();
void rbiquad_free(t_rbiquad *x);
void rbiquad_assist(t_rbiquad *x, void *b, long m, long a, char *s);

void rbiquad_perform64(t_rbiquad *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void rbiquad_dsp64(t_rbiquad *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

// Main

int C74_EXPORT main()
{	
	this_class = class_new("rbiquad~",
				(method) rbiquad_new,
				(method) rbiquad_free,
				sizeof(t_rbiquad), 
				NULL, 
				0);
	
	class_addmethod(this_class, (method) rbiquad_assist, "assist", A_CANT, 0);
	class_addmethod(this_class, (method) rbiquad_dsp64, "dsp64", A_CANT, 0);
	
	class_dspinit(this_class);
	class_register(CLASS_BOX, this_class);
		
	return 0;
}

// New / Free

void *rbiquad_new()
{
    t_rbiquad *x = (t_rbiquad *) object_alloc(this_class);
    
    dsp_setup((t_pxobject *)x, 7);
	outlet_new((t_object *)x, "signal");
	
	x->x1 = x->x2 = x->y1 = x->y2 = 0.;
	
	return x;
}

void rbiquad_free(t_rbiquad *x)
{
	dsp_free(&x->x_obj);
}

// Perform

void rbiquad_perform64(t_rbiquad *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
    // Set Pointers

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
	
	double x1 = x->x1;
	double x2 = x->x2;
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
			x1 = x2 = y1 = y2 = 0.;
		
		// Filter 
		
		y = (a0 * in) + (a1 * x1) + (a2 * x2) - (b1 * y1) - (b2 * y2);

		out_val = FIX_DENORM_DOUBLE(y);
		
		*out++ = out_val;
		
		// Shift memory
		
		y2 = y1;
		y1 = out_val;
		x2 = x1;
		x1 = in;
	}
	
	// Store memory
	
	x->x1 = x1;
	x->x2 = x2;
	x->y1 = y1;
	x->y2 = y2;
}

// DSP

void rbiquad_dsp64(t_rbiquad *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{				
	object_method(dsp64, gensym("dsp_add64"), x, rbiquad_perform64, 0, NULL);
}

// Assist


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
        sprintf(s,"(signal) Output");
}
