
/*
 *  denormkiller~
 *
 *	denormkiller~ is an audio object that turns off denormals on the vector unit (which is also used for most scalar fp maths under OS X).
 *	
 *	This object can remedy problems with third party audio objects that are due to poor or no denormal handling. One object per audio thread is required.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>


#if defined( __i386__ ) || defined( __x86_64__ )
#define TARGET_INTEL
#endif


void *this_class;


typedef struct _denormkiller
{
    t_pxobject x_obj;
	
} t_denormkiller;


void *denormkiller_new ();
void denormkiller_assist (t_denormkiller *x, void *b, long m, long a, char *s);

t_int *denormkiller_perform (t_int *w);
void denormkiller_dsp (t_denormkiller *x, t_signal **sp, short *count);

void denormkiller_perform64 (t_denormkiller *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void denormkiller_dsp64 (t_denormkiller *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);


int C74_EXPORT main()
{
	this_class = class_new ("denormkiller~",
							(method)denormkiller_new,
							(method)dsp_free,
							sizeof(t_denormkiller),
							NULL, 
							0);	
	
	class_addmethod (this_class, (method)denormkiller_assist, "assist", A_CANT, 0);
	class_addmethod (this_class, (method)denormkiller_dsp, "dsp", A_CANT, 0);
	class_addmethod (this_class, (method)denormkiller_dsp64, "dsp64", A_CANT, 0);
	
    class_dspinit (this_class);
	
	class_register (CLASS_BOX, this_class);
	
	return 0;
}


void *denormkiller_new ()
{
    t_denormkiller *x = (t_denormkiller *) object_alloc (this_class);
	
    dsp_setup((t_pxobject *)x, 0);
	
    return x;
}

void denormkiller_assist (t_denormkiller *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) 
        sprintf(s, "Dummy");
}

t_int *denormkiller_perform(t_int *w)
{	
	// under intel replace the old MXCSR setting with the same, except set DAZ and FZ bits to flush denormals to zero
	
#ifdef TARGET_INTEL
	_mm_setcsr(_mm_getcsr() | 0x8040);					
#endif
	
	return w + 1;
}

void denormkiller_dsp (t_denormkiller *x, t_signal **sp, short *count)
{
    dsp_add(denormkiller_perform, 0);
}

void denormkiller_perform64 (t_denormkiller *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
	// under intel replace the old MXCSR setting with the same, except set DAZ and FZ bits to flush denormals to zero
	
#ifdef TARGET_INTEL
	_mm_setcsr(_mm_getcsr() | 0x8040);					
#endif
}

void denormkiller_dsp64 (t_denormkiller *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    object_method(dsp64, gensym("dsp_add64"), x, denormkiller_perform64, 0, NULL);
}
