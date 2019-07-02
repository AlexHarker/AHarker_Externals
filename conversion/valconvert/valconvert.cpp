
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

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

#include <SIMDSupport.hpp>
#include <SIMDExtended.hpp>


#ifdef MSP_VERSION
using max_object_base = t_pxobject;
const char *object_name = "valconvert~";
method free_routine = (method) dsp_free;
#else
using max_object_base = t_object;
const char *object_name = "valconvert";
method free_routine = 0L;
#endif


t_class *this_class;


enum t_conversion_mode {
	
	CONVERT_NONE = 0,
	CONVERT_LINEAR = 1,
	CONVERT_LOG_IN = 2,
	CONVERT_EXP_IN = 3,
	
};


struct t_valconvert
{
	max_object_base a_obj;
	
	t_conversion_mode mode;
		
    double mul;
    double sub;
    double lo;
    double hi;
	
	void *the_outlet;
	
};

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

#ifdef MSP_VERSION

t_int *valconvert_perform(t_int *w);
t_int *valconvert_perform_SIMD(t_int *w);
void valconvert_dsp(t_valconvert *x, t_signal **sp, short *count);

void valconvert_perform64(t_valconvert *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void valconvert_perform_SIMD64(t_valconvert *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void valconvert_dsp64(t_valconvert *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

#else

double valconvert_scale(t_valconvert *x, double input);
void valconvert_int(t_valconvert *x, t_atom_long i_in);
void valconvert_float(t_valconvert *x, double f_in);
void valconvert_list(t_valconvert *x, t_symbol *msg, long argc, t_atom *argv);

#endif

void valconvert_anything(t_valconvert *x, t_symbol *msg, long argc, t_atom *argv);
void valconvert_assist(t_valconvert *x, void *b, long m, long a, char *s);


t_symbol *symbol_to_lowercase(t_symbol *sym)
{
    std::string str(sym->s_name);
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
    
    return gensym(str.c_str());
}


int C74_EXPORT main()
{
    this_class = class_new(object_name,
                          (method) valconvert_new,
                          free_routine,
                          sizeof(t_valconvert),
                          NULL,
                          A_GIMME,
                          0);
	
#ifdef MSP_VERSION
	class_addmethod(this_class, (method)valconvert_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)valconvert_dsp64, "dsp64", A_CANT, 0);
	class_dspinit(this_class);
#else
	class_addmethod(this_class, (method)valconvert_int, "int", A_LONG, 0);
	class_addmethod(this_class, (method)valconvert_float, "float", A_FLOAT, 0);
	class_addmethod(this_class, (method)valconvert_list, "list", A_GIMME, 0);
#endif
	
	class_addmethod(this_class, (method)valconvert_anything, "anything", A_GIMME, 0);
	class_addmethod(this_class, (method)valconvert_assist, "assist", A_CANT, 0);
	
	class_register(CLASS_BOX, this_class);
	
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

void *valconvert_new(t_symbol *msg, long argc, t_atom *argv)
{
    t_valconvert *x = (t_valconvert *) object_alloc(this_class);
		
	x->mode = CONVERT_NONE;
	x->lo = 0.0;
	x->hi = 1.0;
	x->mul = 1.0;
	x->sub = 0.0;
	
	if (argc)
	{
		if (argc < 5 && symbol_to_lowercase(atom_getsym(argv)) != ps_none)
		{
			object_error((t_object *) x, "not enough arguments for parameter set up");
		}
		else
		{
			if (atom_gettype(argv) == A_SYM)
				valconvert_anything (x, atom_getsym(argv), argc - 1, argv + 1);
		}
	}
    
#ifdef MSP_VERSION
    dsp_setup((t_pxobject *)x, 1);
    outlet_new((t_object *)x, "signal");
#else
    x->the_outlet = outlet_new(x, 0);
#endif
	
    return x;
}

// Clip Helper

double clip(double x, double lo, double hi)
{
    x = (x > hi ? hi : x);
    return (x < lo ? lo : x);
}

// Log Helper

double safe_log(double x)
{
    return log(x < 0 ? 0 : x);
}

#ifdef MSP_VERSION

// Templates

template <class T>
void valconvert_perform_scalar(t_valconvert *x, const T* in, T* out, long vec_size)
{
    double mul = x->mul;
    double sub = x->sub;
    double lo = x->lo;
    double hi = x->hi;
    
    switch (x->mode)
    {
        case CONVERT_NONE:
            while (vec_size--)
                *out++ = *in++;
            break;
            
        case CONVERT_LINEAR:
            while (vec_size--)
                *out++ = static_cast<T>(clip((*in++ * mul) - sub, lo, hi));
            break;
            
        case CONVERT_LOG_IN:
            while (vec_size--)
                *out++ = static_cast<T>(clip(exp((*in++ * mul) - sub), lo, hi));
            break;
            
        case CONVERT_EXP_IN:
            while (vec_size--)
                *out++ = static_cast<T>(clip((safe_log(*in++) * mul) - sub, lo, hi));
            break;
    }
}

template <class T>
void valconvert_perform_simd_base(t_valconvert *x, const T* in, T* out, long vec_size)
{
    using VecType = SIMDType<T, SIMDLimits<T>::max_size>;
    
    const VecType *in_vec = reinterpret_cast<const VecType *>(in);
    VecType *out_vec = reinterpret_cast<VecType *>(out);
    
    long num_vecs = vec_size / SIMDLimits<T>::max_size;
    
    VecType mul(x->mul);
    VecType sub(x->sub);
    VecType lo(x->lo);
    VecType hi(x->hi);
    
    switch (x->mode)
    {
        case CONVERT_NONE:
            while (num_vecs--)
                *out_vec++ = *in_vec++;
            break;
            
        case CONVERT_LINEAR:
            while (num_vecs--)
                *out_vec++ = min(max(lo, (*in_vec++ * mul) - sub), hi);
            break;
            
        case CONVERT_LOG_IN:
            for (long i = 0; i < num_vecs; i++)
                out_vec[i] = (in_vec[i] * mul) - sub;
            exp_array(out, out, vec_size);
            for (long i = 0; i < num_vecs; i++)
                out_vec[i] = min(max(lo, out_vec[i]), hi);
            break;
            
        case CONVERT_EXP_IN:
            for (long i = 0; i < num_vecs; i++)
                out_vec[i] = max(in_vec[i], T(0));
            log_array(out, out, vec_size);
            for (long i = 0; i < num_vecs; i++)
                out_vec[i] = min(max(lo, (out_vec[i] * mul - sub)), hi);
            break;
    }
}

// 32 Bit MSP 

t_int *valconvert_perform(t_int *w)
{
	float *in = (float *) w[1];
	float *out = (float *) w[2];
	long vec_size = w[3];
    t_valconvert *x = (t_valconvert *) w[4];
	
    valconvert_perform_scalar(x, in, out, vec_size);
	
	return w + 5;
}

t_int *valconvert_perform_SIMD(t_int *w)
{
    float *in = (float *) w[1];
    float *out = (float *) w[2];
	long vec_size = (long) w[3];
	t_valconvert *x = (t_valconvert *) w[4];
			
    valconvert_perform_simd_base(x, in, out, vec_size);
	
	return w + 5;
}

void valconvert_dsp(t_valconvert *x, t_signal **sp, short *count)
{
	if (sp[0]->s_n >= SIMDLimits<float>::max_size)
		dsp_add(valconvert_perform_SIMD, 4, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n, x);
	else
		dsp_add(valconvert_perform, 4, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n, x);
}

// 64 bit MSP

void valconvert_perform64(t_valconvert *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    valconvert_perform_scalar(x, ins[0], outs[0], vec_size);
}

void valconvert_perform_SIMD64(t_valconvert *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    valconvert_perform_simd_base(x, ins[0], outs[0], vec_size);
}

void valconvert_dsp64(t_valconvert *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	if (maxvectorsize >= SIMDLimits<double>::max_size)
		object_method(dsp64, gensym("dsp_add64"), x, valconvert_perform_SIMD64, 0, NULL);				
	else	
		object_method(dsp64, gensym("dsp_add64"), x, valconvert_perform64, 0, NULL);
}

void valconvert_assist(t_valconvert *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
        sprintf(s, "(signal) Scaled Output");
    else
        sprintf(s, "(signal) Input / (message) Parameters");
}

#else

double valconvert_scale(t_valconvert *x, double input)
{
	switch (x->mode)
	{
		case CONVERT_NONE:      return input;
        case CONVERT_LINEAR:    return clip((input * x->mul) - x->sub, x->lo, x->hi);
        case CONVERT_LOG_IN:    return clip(exp((input * x->mul) - x->sub), x->lo, x->hi);
		case CONVERT_EXP_IN:    return clip((safe_log(input) * x->mul) - x->sub, x->lo, x->hi);
	}
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
    std::vector<t_atom> list(argc);
    
    for (long i = 0; i < argc; i++)
		atom_setfloat(list.data() + i ,valconvert_scale(x, atom_getfloat(argv++)));
	
	outlet_list(x->the_outlet, ps_list, argc, list.data());
}

void valconvert_assist(t_valconvert *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
        sprintf(s, "Scaled Output");
    else
        sprintf(s, "Input / Parameters");
}

#endif

void convert_power(double &a, double&b, double base, double divisor)
{
    a = pow(base, a / divisor);
    b = pow(base, b / divisor);
}

void convert_log(double &a, double&b)
{
    a = safe_log(a);
    b = safe_log(b);
}

void valconvert_anything(t_valconvert *x, t_symbol *msg, long argc, t_atom *argv)
{
    // Here we convert captialised symbols, so as to allow capitalisation

    msg = symbol_to_lowercase(msg);

	if (msg == ps_none)
	{
		x->mode = CONVERT_NONE;
		return;
	}
	
	if (argc < 4)
	{
		object_error((t_object *)x, "not enough values for parameter change");
		return;
	}
	
	double min_in = atom_getfloat(argv++);
	double max_in = atom_getfloat(argv++);
	double min_out = atom_getfloat(argv++);
	double max_out = atom_getfloat(argv++);

	if (msg == ps_log || msg == ps_amp || msg == ps_pitch)
		x->mode = CONVERT_LOG_IN;
	else if (msg == ps_exp  || msg == ps_iamp || msg == ps_ipitch)
		x->mode = CONVERT_EXP_IN;
    else
    {
        if (msg != ps_scale)
            object_error ((t_object *) x, "unknown conversion type - defaulting to scale");
        x->mode = CONVERT_LINEAR;
    }
    
	if (msg == ps_amp)
        convert_power(min_out, max_out, 10.0, 20.0);
	else if (msg == ps_pitch)
        convert_power(min_out, max_out, 2.0, 12.0);
	else if (msg == ps_iamp)
        convert_power(min_in, max_in, 10.0, 20.0);
	else if (msg == ps_ipitch)
        convert_power(min_in, max_in, 2.0, 12.0);

	x->lo = min_out;
	x->hi = max_out;
    
    if (x->mode == CONVERT_LOG_IN)
        convert_log(min_out, max_out);
    else if (x->mode == CONVERT_EXP_IN)
        convert_log(min_in, max_in);
    
    x->mul = (min_in == max_in) ? 0.0 : (max_out - min_out) / (max_in - min_in);
	x->sub = (min_in * x->mul) - min_out;
}
