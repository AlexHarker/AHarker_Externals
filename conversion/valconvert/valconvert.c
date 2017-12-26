
/*
 *  valconvert and valconvert~
 *
 *	valconvert and its audio rate counterpart (valconvert~) are general pupose scaling objects for useful musical scalings (log / exp / pitch / amp / scale).
 *
 *	The max version scales ints (float output), floats and lists, the audio rate version scales signals.
 *
 *	Scaling is set by a parameter message. 
 *	Scaling type refers to interpretation of the *input* rather than the output, so the log setting treats the input as logarithmic and scales expoentatially (for frequency for example).
 *	See the documentation for more detail on other modes.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

void *this_class;


typedef enum {
	
	CONVERT_NONE = 0,
	CONVERT_LINEAR = 1,
	CONVERT_LOG_IN = 2,
	CONVERT_EXP_IN = 3,
	
} t_conversion_mode;


typedef struct valconvert {
    
#ifdef MSP_VERSION
	t_pxobject a_obj;
#else
	t_object a_obj;
#endif
	
	t_conversion_mode mode;
		
	double mult, subtract;
	double min, max;
	
	void *the_outlet;
	
} t_valconvert;


t_symbol *ps_Scale;
t_symbol *ps_Log;
t_symbol *ps_Amp;
t_symbol *ps_Pitch;
t_symbol *ps_Exp;
t_symbol *ps_None;
t_symbol *ps_iAmp;
t_symbol *ps_iPitch;

t_symbol *ps_scale;
t_symbol *ps_log;
t_symbol *ps_amp;
t_symbol *ps_pitch;
t_symbol *ps_exp;
t_symbol *ps_none;
t_symbol *ps_iamp;
t_symbol *ps_ipitch;

t_symbol *ps_list;


void *valconvert_new(t_symbol *msg, long argc, t_atom *argv);
void valconvert_free(t_valconvert *x);

#ifdef MSP_VERSION

t_int *valconvert_perform(t_int *w);
#if (defined F32_VEC_EXP_OP || defined F32_VEC_EXP_ARRAY) && (defined F32_VEC_LOG_OP || defined F32_VEC_LOG_ARRAY)
t_int *valconvert_perform_SIMD(t_int *w);
#endif
void valconvert_dsp(t_valconvert *x, t_signal **sp, short *count);

void valconvert_perform64(t_valconvert *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
#if (defined VECTOR_F64_128BIT) && (defined F64_VEC_EXP_OP || defined F64_VEC_EXP_ARRAY) && (defined F64_VEC_LOG_OP || defined F64_VEC_LOG_ARRAY)
void valconvert_perform_SIMD64(t_valconvert *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
#endif
void valconvert_dsp64(t_valconvert *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

#else

double valconvert_scale(t_valconvert *x, double input);
void valconvert_int(t_valconvert *x, t_atom_long i_in);
void valconvert_float(t_valconvert *x, double f_in);
void valconvert_list(t_valconvert *x, t_symbol *msg, long argc, t_atom *argv);

#endif

void valconvert_anything(t_valconvert *x, t_symbol *msg, long argc, t_atom *argv);

void valconvert_assist(t_valconvert *x, void *b, long m, long a, char *s);


int C74_EXPORT main(void)
{	
	
#ifdef MSP_VERSION
	
	this_class = class_new("valconvert~", 
						   (method) valconvert_new, 
						   (method) valconvert_free, 
						   sizeof(t_valconvert), 
						   NULL, 
						   A_GIMME,
						   0);
	
	class_addmethod (this_class, (method)valconvert_dsp, "dsp", A_CANT, 0);
	class_addmethod (this_class, (method)valconvert_dsp64, "dsp64", A_CANT, 0);
	
	class_dspinit(this_class);
	
#else
	
	this_class = class_new("valconvert", 
						   (method) valconvert_new, 
						   (method) valconvert_free, 
						   sizeof(t_valconvert), 
						   NULL, 
						   A_GIMME,
						   0);
	
	
	class_addmethod (this_class, (method)valconvert_int, "int", A_LONG, 0);
	class_addmethod (this_class, (method)valconvert_float, "float", A_FLOAT, 0);
	class_addmethod (this_class, (method)valconvert_list, "list", A_GIMME, 0);
	
#endif
	
	class_addmethod (this_class, (method)valconvert_anything, "anything", A_GIMME, 0);
	class_addmethod (this_class, (method)valconvert_assist, "assist", A_CANT, 0);
	
	class_register(CLASS_BOX, this_class);
	
	ps_Scale = gensym("Scale");
	ps_Log = gensym("Log");
	ps_Amp = gensym("Amp");
	ps_Pitch = gensym("Pitch");
	ps_Exp = gensym("Exp");
	ps_iAmp = gensym("iAmp");
	ps_iPitch = gensym("iPitch");
	ps_None = gensym("None");
	
	
	ps_scale = gensym("scale");
	ps_log = gensym("log");
	ps_amp = gensym("amp");
	ps_pitch = gensym("pitch");
	ps_exp = gensym("exp");
	ps_iamp = gensym("iamp");
	ps_ipitch = gensym("ipitch");
	ps_none = gensym("none");
	
	ps_list = gensym("list");

	return 0;
}


void valconvert_free(t_valconvert *x)
{
	
#ifdef MSP_VERSION	
	dsp_free(&x->a_obj);	
#endif
	
}

void *valconvert_new(t_symbol *msg, long argc, t_atom *argv)
{
    t_valconvert *x = (t_valconvert *) object_alloc (this_class);
	
#ifdef MSP_VERSION	
	dsp_setup((t_pxobject *)x, 1);
	outlet_new((t_object *)x, "signal");
#else
	x->the_outlet = outlet_new(x, 0);
#endif
	
	x->mode = CONVERT_NONE;
	x->min = 0;
	x->max = 1;
	x->mult = 1;
	x->subtract = 0;
	
	if (argc)
	{
		if (argc < 5 && atom_getsym(argv) != ps_none && atom_getsym(argv) != ps_None)
		{
			object_error((t_object *) x, "not enough arguments for parameter set up");
		}
		else
		{
			if (atom_gettype(argv) == A_SYM)
				valconvert_anything (x, atom_getsym(argv), argc - 1, argv + 1);
		}
	}
	
    return (x);
}

#ifdef MSP_VERSION

// 32 Bit MSP 

t_int *valconvert_perform(t_int *w)
{
	float *in = (float *) w[1];
	float *out = (float *) w[2];
	long vec_size = w[3];
    t_valconvert *x = (t_valconvert *) w[4];
	
	double scaled = 0.0;
	double mult = x->mult;
	double subtract = x->subtract;
	double max = x->max;
	double min = x->min;
	
	switch (x->mode)
	{
		case CONVERT_NONE:
			while (vec_size--)
				*out++ = *in++;
			break;
			
		case CONVERT_LINEAR:
			while (vec_size--)
			{
				scaled = (*in++ * mult) - subtract;
				scaled = (scaled > max) ? max: scaled;
				scaled = (scaled < min) ? min: scaled;
				*out++ = (float) scaled;
			}
			break;
			
		case CONVERT_LOG_IN:
			while (vec_size--)
			{
				scaled = exp((*in++ * mult) - subtract);
				scaled = (scaled > max) ? max: scaled;
				scaled = (scaled < min) ? min: scaled;
				*out++ = (float) scaled;
			}
			break;
			
		case CONVERT_EXP_IN:
			while (vec_size--)
			{
				scaled = (log(*in++) * mult) - subtract;
				scaled = (scaled > max) ? max: scaled;
				scaled = (scaled < min) ? min: scaled;
				*out++ = (float) scaled;
			}
			break;	
	}
	
	return w + 5;
}


#if (defined F32_VEC_EXP_OP || defined F32_VEC_EXP_ARRAY) && (defined F32_VEC_LOG_OP || defined F32_VEC_LOG_ARRAY)

t_int *valconvert_perform_SIMD(t_int *w)
{
	vFloat *in = (vFloat *) w[1];
	vFloat *out = (vFloat *) w[2];
	long vec_size_over_4 = w[3] >> 2;	
	t_valconvert *x = (t_valconvert *) w[4];
			
	vFloat scaled = float2vector(0.f);
	vFloat mult = float2vector((float) x->mult);
	vFloat subtract = float2vector((float) x->subtract);
	vFloat min = float2vector((float) x->min);
	vFloat max = float2vector((float) x->max);
	
#ifndef F32_VEC_EXP_OP
	long i;
#endif
		
	switch (x->mode)
	{
		case CONVERT_NONE:
			while (vec_size_over_4--)
				*out++ = *in++;
			break;
			
		case CONVERT_LINEAR:
			while (vec_size_over_4--)
			{
				scaled = F32_VEC_SUB_OP(F32_VEC_MUL_OP(*in++, mult), subtract);
				*out++ = F32_VEC_MIN_OP(F32_VEC_MAX_OP(min, scaled), max);
			}
			break;
			
		case CONVERT_LOG_IN:
			
#ifdef F32_VEC_EXP_OP
			while (vec_size_over_4--)
			{
				scaled = F32_VEC_SUB_OP(F32_VEC_MUL_OP(*in++, mult), subtract);
				scaled = F32_VEC_EXP_OP(scaled);
				*out++ = F32_VEC_MIN_OP(F32_VEC_MAX_OP(min, scaled), max);
			}
			break;
#else
			for (i = 0; i < vec_size_over_4; i++)
				out[i] = F32_VEC_SUB_OP(F32_VEC_MUL_OP(in[i], mult), subtract);
			F32_VEC_EXP_ARRAY((float *) out, (float *) out, (vec_size_over_4  << 2));
			for (i = 0; i < vec_size_over_4; i++)
				out[i] = F32_VEC_MIN_OP(F32_VEC_MAX_OP(min, out[i]), max);
			break;
#endif
			
		case CONVERT_EXP_IN:
			
#ifdef F32_VEC_LOG_OP
			while (vec_size_over_4--)
			{
				scaled = F32_VEC_LOG_OP(*in++);
				scaled = F32_VEC_SUB_OP(F32_VEC_MUL_OP(scaled, mult), subtract);
				*out++ = F32_VEC_MIN_OP(F32_VEC_MAX_OP(min, scaled), max);		
			}
			break;
#else
			F32_VEC_LOG_ARRAY ((float *) out, (float *) in, (vec_size_over_4  << 2));
			while (vec_size_over_4--)
			{
				scaled = F32_VEC_SUB_OP(F32_VEC_MUL_OP(*out, mult), subtract);
				*out++ = F32_VEC_MIN_OP(F32_VEC_MAX_OP(min, scaled), max);				
			}
			break;
#endif
	}
	
	return w + 5;
}

#endif


void valconvert_dsp(t_valconvert *x, t_signal **sp, short *count)
{				
#if (defined F32_VEC_EXP_OP || defined F32_VEC_EXP_ARRAY) && (defined F32_VEC_LOG_OP || defined F32_VEC_LOG_ARRAY)
	if (sp[0]->s_n >= 8 && SSE2_check())
		dsp_add(valconvert_perform_SIMD, 4, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n, x);
	else	
#endif
		dsp_add(valconvert_perform, 4, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n, x);
}


// 64 bit MSP

void valconvert_perform64(t_valconvert *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
	double *in = ins[0];
	double *out = outs[0];
	
	double scaled = 0.;
	double mult = x->mult;
	double subtract = x->subtract;
	double max = x->max;
	double min = x->min;
	
	switch (x->mode)
	{
		case CONVERT_NONE:
			while (vec_size--)
				*out++ = *in++;
			break;
			
		case CONVERT_LINEAR:
			while (vec_size--)
			{
				scaled = (*in++ * mult) - subtract;
				scaled = (scaled > max) ? max: scaled;
				*out++ = (scaled < min) ? min: scaled;
			}
			break;
			
		case CONVERT_LOG_IN:
			while (vec_size--)
			{
				scaled = exp((*in++ * mult) - subtract);
				scaled = (scaled > max) ? max: scaled;
				*out++ = (scaled < min) ? min: scaled;
			}
			break;
			
		case CONVERT_EXP_IN:
			while (vec_size--)
			{
				scaled = (log(*in++) * mult) - subtract;
				scaled = (scaled > max) ? max: scaled;
				*out++ = (scaled < min) ? min: scaled;
			}
			break;	
	}
}


#if (defined VECTOR_F64_128BIT) && (defined F64_VEC_EXP_OP || defined F64_VEC_EXP_ARRAY) && (defined F64_VEC_LOG_OP || defined F64_VEC_LOG_ARRAY)

void valconvert_perform_SIMD64(t_valconvert *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
	vDouble *in = (vDouble *) ins[0];
	vDouble *out = (vDouble *) outs[0];

	long vec_size_over_2 = vec_size >> 1;
	long i = 0;
	
	vDouble scaled = double2vector(0.);
	vDouble mult = double2vector(x->mult);
	vDouble subtract = double2vector(x->subtract);
	vDouble min = double2vector(x->min);
	vDouble max = double2vector(x->max);
		
	switch (x->mode)
	{
		case CONVERT_NONE:
			while (vec_size_over_2--)
				*out++ = *in++;
			break;
			
		case CONVERT_LINEAR:
			while (vec_size_over_2--)
			{
				scaled = F64_VEC_SUB_OP(F64_VEC_MUL_OP(*in++, mult), subtract);
				*out++ = F64_VEC_MIN_OP(F64_VEC_MAX_OP(min, scaled), max);
			}
			break;
			
		case CONVERT_LOG_IN:
#ifdef F64_VEC_EXP_OP
			while (vec_size_over_2--)
			{
				scaled = F64_VEC_SUB_OP(F64_VEC_MUL_OP(*in++, mult), subtract);
				scaled = F64_VEC_EXP_OP(scaled);
				*out++ = F64_VEC_MIN_OP(F64_VEC_MAX_OP(min, scaled), max);
			}
			break;
#else
			for (i = 0; i < vec_size_over_2; i++)
				out[i] = F64_VEC_SUB_OP(F64_VEC_MUL_OP(in[i], mult), subtract);
			F64_VEC_EXP_ARRAY(outs[0], outs[0], vec_size);
			for (i = 0; i < vec_size_over_2; i++)
				out[i] = F64_VEC_MIN_OP(F64_VEC_MAX_OP(min, out[i]), max);
			break;
#endif
			
		case CONVERT_EXP_IN:
			
#ifdef F64_VEC_LOG_OP
			while (vec_size_over_2--)
			{
				scaled = F64_VEC_LOG_OP(*in++);
				scaled = F64_VEC_SUB_OP(F64_VEC_MUL_OP(*out, mult), subtract);
				*out++ = F64_VEC_MIN_OP(F64_VEC_MAX_OP(min, scaled), max);		
			}
			break;
#else
			F64_VEC_LOG_ARRAY (outs[0], ins[0], vec_size);
			while (vec_size_over_2--)
			{
				scaled = F64_VEC_SUB_OP(F64_VEC_MUL_OP(*out, mult), subtract);
				*out++ = F64_VEC_MIN_OP(F64_VEC_MAX_OP(min, scaled), max);				
			}
			break;
#endif	
	}
}

#endif


void valconvert_dsp64(t_valconvert *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{				
#if (defined VECTOR_F64_128BIT) && (defined F64_VEC_EXP_OP || defined F64_VEC_EXP_ARRAY) && (defined F64_VEC_LOG_OP || defined F64_VEC_LOG_ARRAY)
	if (maxvectorsize >= 8 && SSE2_check())
		object_method(dsp64, gensym("dsp_add64"), x, valconvert_perform_SIMD64, 0, NULL);				
	else	
#endif
		object_method(dsp64, gensym("dsp_add64"), x, valconvert_perform64, 0, NULL);				
}


#else

double valconvert_scale(t_valconvert *x, double input)
{
	double scaled = 0.0;
	double mult = x->mult;
	double subtract = x->subtract;
	double max = x->max;
	double min = x->min;
	
	switch (x->mode)
	{
		case CONVERT_NONE:
			return input;
		case CONVERT_LINEAR:
			scaled = input * mult - subtract;
			break;
		case CONVERT_LOG_IN:
			scaled = exp(input * mult - subtract);
			break;
		case CONVERT_EXP_IN:
			scaled = (log(input) * mult) - subtract;
			break;
	}
	
	scaled = (scaled > max) ? max: scaled;
	scaled = (scaled < min) ? min: scaled;
	
	return scaled;
}


void valconvert_float(t_valconvert *x, double f_in)
{ 
	outlet_float(x->the_outlet, valconvert_scale(x, f_in));
} 


void valconvert_int(t_valconvert *x, t_atom_long l_in)
{ 	
	outlet_float(x->the_outlet, valconvert_scale(x, l_in));
} 


void valconvert_list(t_valconvert *x, t_symbol *msg, long argc, t_atom *argv)
{
    
    t_atom *list = (t_atom *) malloc(argc * sizeof (t_atom));
	t_atom *listptr = list;
	long i = argc;
	
    if (!list)
    {
        object_error((t_object *) x, "could not allocate memory for conversion");
        return;
    }

	while (i--) 
		atom_setfloat(listptr++ ,valconvert_scale (x, atom_getfloat(argv++)));
	
	outlet_list(x->the_outlet, ps_list, argc, list);
    
    free(list);
}


#endif


void valconvert_anything(t_valconvert *x, t_symbol *msg, long argc, t_atom *argv)
{
	t_conversion_mode mode = CONVERT_LINEAR;
	
	double min_in;
	double max_in;
	double min_out;
	double max_out;
	double mult;
	double subtract;
	double min;
	double max;

	if (msg == ps_none || msg == ps_None)
	{
		x->mode = CONVERT_NONE;
		return;
	}
	
	if (argc < 4)
	{
		object_error((t_object *)x, "not enough values for parameter change");
		return;
	}
	
	// Here we convert captialised symbols, so as to allow capitalisation (which is needed for backward compatibility)
	
	if (msg == ps_Scale) msg = ps_scale;
	if (msg == ps_Log) msg = ps_log;
	if (msg == ps_Exp) msg = ps_exp;
	if (msg == ps_Amp) msg = ps_amp;
	if (msg == ps_Pitch) msg = ps_pitch;
	if (msg == ps_iAmp) msg = ps_iamp;
	if (msg == ps_iPitch) msg = ps_ipitch;
	
	min_in = atom_getfloat(argv++);
	max_in = atom_getfloat(argv++);
	min_out = atom_getfloat(argv++);
	max_out = atom_getfloat(argv++);

	if (msg == ps_log || msg == ps_amp || msg == ps_pitch)
		mode = CONVERT_LOG_IN;
	
	
	if (msg == ps_exp  || msg == ps_iamp || msg == ps_ipitch)
		mode = CONVERT_EXP_IN;
	
	if (msg == ps_amp)
	{
		min_out = pow (10.0, min_out / 20.0);
		max_out = pow (10.0, max_out / 20.0);
	}
	
	if (msg == ps_pitch)
	{
		min_out = pow(2.0, min_out / 12.0);
		max_out = pow(2.0, max_out / 12.0);
	}	
	
	if (msg == ps_iamp)
	{
		min_in = pow (10.0, min_in / 20.0);
		max_in = pow (10.0, max_in / 20.0);
	}
	
	if (msg == ps_ipitch)
	{
		min_in = pow (2.0, min_in / 12.0);
		max_in = pow (2.0, max_in / 12.0);
	}	
	
	min = min_out;
	max = max_out;
	
	if (mode == CONVERT_LOG_IN)
	{
		min_out = log(min_out);
		max_out = log(max_out);
	}
	
	if (mode == CONVERT_EXP_IN)
	{
		min_in = log(min_in);
		max_in = log(max_in);
	}
	
	if (min_in == max_in)
		mult = 0.0;
	else
		mult = (max_out - min_out) / (max_in - min_in);
	
	subtract = (min_in * mult) - min_out;
	
	if (mode == CONVERT_LINEAR && msg != ps_scale)
		object_error ((t_object *) x, "unknown conversion type - defaulting to scale");
	
	x->mode = mode;
	x->mult = mult;
	x->min = min;
	x->max = max;
	x->subtract = subtract;
}


void valconvert_assist(t_valconvert *x, void *b, long m, long a, char *s)
{
	
#ifdef MSP_VERSION		
	if (m == ASSIST_OUTLET)
		sprintf(s, "(signal) Scaled Output");
    else 
		sprintf(s, "(signal) Input / (message) Parameters");
#else
	if (m == ASSIST_OUTLET)
		sprintf(s, "Scaled Output");
    else 
		sprintf(s, "Input / Parameters");
#endif
	
}
