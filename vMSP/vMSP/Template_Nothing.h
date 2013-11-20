

#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Denormals.h>

void *this_class;


// Object structure

typedef struct OBJNAME_SECOND(_)
{
    t_pxobject x_obj;
	
	double double_val;
		
} OBJNAME_SECOND(t_);


void *OBJNAME_FIRST(_new)(double float_val);
void OBJNAME_FIRST(_free)(OBJNAME_SECOND(t_) *x);

void OBJNAME_FIRST(_float )(OBJNAME_SECOND(t_) *x, double datain);
void OBJNAME_FIRST(_int )(OBJNAME_SECOND(t_) *x, long datain);

void OBJNAME_FIRST(_dsp)(OBJNAME_SECOND(t_) *x, t_signal **sp, short *count);
void OBJNAME_FIRST(_dsp64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);


t_int *OBJNAME_FIRST(_perform)(t_int *w);

void OBJNAME_FIRST(_perform_64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);



void OBJNAME_FIRST(_assist)(OBJNAME_SECOND(t_) *x, void *b, long m, long a, char *s);



// Main routine

int main(void)
{
	this_class = class_new (OBJNAME_STR, (method) OBJNAME_FIRST(_new), (method)OBJNAME_FIRST(_free), (short)sizeof(OBJNAME_SECOND(t_)), NULL, A_DEFFLOAT, 0);
	
	class_addmethod(this_class, (method)OBJNAME_FIRST(_float), "float", A_FLOAT, 0);
	class_addmethod(this_class, (method)OBJNAME_FIRST(_int), "int", A_LONG, 0);
	class_addmethod(this_class, (method)OBJNAME_FIRST(_dsp), "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)OBJNAME_FIRST(_dsp64), "dsp64", A_CANT, 0);
	class_addmethod(this_class, (method)OBJNAME_FIRST(_assist), "assist", A_CANT, 0);
    
	class_dspinit(this_class);
	class_register(CLASS_BOX, this_class);
	
	post ("%s - using vector version by Alex Harker", OBJNAME_STR);
	
	return 0;
}


// Free routine

void OBJNAME_FIRST(_free)(OBJNAME_SECOND(t_) *x)
{
	dsp_free(&x->x_obj);
}


// New routine

void *OBJNAME_FIRST(_new)(double double_val)
{
    OBJNAME_SECOND(t_) *x = (OBJNAME_SECOND(t_) *) object_alloc (this_class);
		
    dsp_setup((t_pxobject *)x, 2);
    outlet_new((t_object *)x,"signal");
    
	x->double_val = double_val;
	
    return (x);
}


// Float input routine

void OBJNAME_FIRST(_float)(OBJNAME_SECOND(t_) *x, double datain)
{
	x->double_val = datain;
}


// Int input routine

void OBJNAME_FIRST(_int)(OBJNAME_SECOND(t_) *x, long datain)
{
	x->double_val = (double) datain;
}


// Dsp routine

void OBJNAME_FIRST(_dsp)(OBJNAME_SECOND(t_) *x, t_signal **sp, short *count)
{
	dsp_add(denormals_perform, 6, OBJNAME_FIRST(_perform), sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[2]->s_n, x);
}


void OBJNAME_FIRST(_dsp64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	
	object_method(dsp64, gensym("dsp_add64"), x, OBJNAME_FIRST(_perform_64), 0, NULL);
}


t_int *OBJNAME_FIRST(_perform)(t_int *w)
{		
    float *in1 = (float *)(w[2]);
    float *in2 = (float *)(w[3]);
	float *out1 = (float *)(w[4]);
    long vec_size = w[5];

	in1 = (float *)(w[2]);
	in2 = (float *)(w[3]);
	out1 = (float *)(w[4]);
	vec_size = w[5];
	
    return w + 7;
}


void OBJNAME_FIRST(_perform_64)(OBJNAME_SECOND(t_) *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{		
    double *in1 = (double *)ins[0];
    double *in2 = (double *)ins[1];
	double *out1 = (double *)outs[0];
    long vec_size = sampleframes >> 1;
	
	in1 = (double *)ins[0];
	in2 = (double *)ins[1];
	out1 = (double *)outs[0];
	vec_size = sampleframes >> 1;
}


// Assist routine

void OBJNAME_FIRST(_assist)(OBJNAME_SECOND(t_) *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) {
        switch (a) {	
            case 0:
                sprintf(s,"(signal, float) In 1");
                break;
            case 1:
                sprintf(s,"(signal, float) In 2");
                break;
        }
    }
    else {
		sprintf(s,"(signal) Out");
	}
}
