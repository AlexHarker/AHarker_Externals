
/*
 *  ibuftable~
 *
 *	ibuftable~ is an efficient object designed for table lookup (for window functions etc.).
 *
 *	ibuftable~ features SIMD optimisation and four types of interpolation (linear interpolation and three different kinds of cubic interpolation which can be requested as desired.)
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Denormals.h>
#include <ibuffer_access.hpp>


t_class *this_class;


typedef struct _ibuftable
{
    t_pxobject x_obj;
		
	t_symbol *buffer_name;
	
	t_atom_long start_samp;
	t_atom_long end_samp;
	t_atom_long chan;
    
    InterpType interp_type;
	
} t_ibuftable;


void *ibuftable_new(t_symbol *s, long argc, t_atom *argv);
void ibuftable_free(t_ibuftable *x);
void ibuftable_assist(t_ibuftable *x, void *b, long m, long a, char *s);

void ibuftable_set(t_ibuftable *x, t_symbol *s, long argc, t_atom *argv);
void ibuftable_set_internal(t_ibuftable *x, t_symbol *s);
void ibuftable_startsamp(t_ibuftable *x, t_atom_long startsamp);
void ibuftable_endsamp(t_ibuftable *x, t_atom_long endsamp);
void ibuftable_chan(t_ibuftable *x, t_atom_long chan);

t_int *ibuftable_perform(t_int *w);
void ibuftable_dsp(t_ibuftable *x, t_signal **sp, short *count);

void ibuftable_perform64(t_ibuftable *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void ibuftable_dsp64(t_ibuftable *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

int C74_EXPORT main()
{	
	this_class = class_new("ibuftable~",
						   (method)ibuftable_new,
						   (method)ibuftable_free,
						   sizeof(t_ibuftable), 
						   NULL, 
						   A_GIMME,
						   0);
	
	class_addmethod(this_class, (method)ibuftable_set, "set", A_GIMME, 0);
	class_addmethod(this_class, (method)ibuftable_assist, "assist", A_CANT, 0);
	class_addmethod(this_class, (method)ibuftable_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)ibuftable_dsp64, "dsp64", A_CANT, 0);
	
    // Add Attributes
    
    add_ibuffer_interp_attribute<t_ibuftable, kInterpLinear>(this_class);
    
    CLASS_ATTR_LONG(this_class, "startsamp", 0L, t_ibuftable, start_samp);
    CLASS_ATTR_FILTER_MIN(this_class, "startsamp", 0);
    CLASS_ATTR_LABEL(this_class, "startsamp", 0L, "Start Sample");
    
    CLASS_ATTR_LONG(this_class, "endsamp", 0L, t_ibuftable, end_samp);
    CLASS_ATTR_FILTER_MIN(this_class, "endsamp", 0);
    CLASS_ATTR_LABEL(this_class, "endsamp", 0L, "End Sample");
    
    CLASS_ATTR_LONG(this_class, "chan", 0L, t_ibuftable, chan);
    CLASS_ATTR_FILTER_MIN(this_class, "chan", 1);
    CLASS_ATTR_LABEL(this_class, "chan", 0L, "Buffer Channel");
    
	class_dspinit(this_class);
	class_register(CLASS_BOX, this_class);
		
	return 0;
}

void *ibuftable_new(t_symbol *s, long argc, t_atom *argv)
{
    t_ibuftable *x = (t_ibuftable *)object_alloc(this_class);
    
    dsp_setup((t_pxobject *)x, 1);
	outlet_new((t_object *)x, "signal");
	
	// Default variables
	
    t_symbol *buffer_name = NULL;
    t_atom_long start_samp = 0;
    t_atom_long end_samp = 512;
    t_atom_long chan = 1;
    
	// Arguments
	
    long non_attr_argc = attr_args_offset(argc, argv);
    
    x->buffer_name = non_attr_argc > 0 ? atom_getsym(argv + 0) : buffer_name;
    x->start_samp = non_attr_argc > 1 ? atom_getlong(argv + 1) : std::max(start_samp, static_cast<t_atom_long>(0));
    x->end_samp = non_attr_argc > 2 ? atom_getlong(argv + 2) : std::max(end_samp, static_cast<t_atom_long>(0));
	x->chan = non_attr_argc > 3 ? atom_getlong(argv + 3) : std::max(chan, static_cast<t_atom_long>(1));
    
	x->interp_type = kInterpLinear;
	
    // Set attributes from arguments
    
    attr_args_process(x, argc, argv);
    
	return x;
}

void ibuftable_free(t_ibuftable *x)
{
	dsp_free(&x->x_obj);
}

void ibuftable_assist(t_ibuftable *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
		sprintf(s,"(signal) Output");
	else 
        sprintf(s,"(signal) Position Input (0-1)");
}

void ibuftable_set(t_ibuftable *x, t_symbol *msg, long argc, t_atom *argv)
{	
	ibuftable_set_internal(x, argc ? atom_getsym(argv) : 0);
}

void ibuftable_set_internal(t_ibuftable *x, t_symbol *s)
{
    ibuffer_data buffer(s);
    
    x->buffer_name = s;
    
    if (buffer.get_type() == kBufferNone && s)
        object_error((t_object *) x, "ibuftable~: no buffer %s", s->s_name);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////// Core Perform Routines ///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

template <class T, int N>
void perform_positions(SIMDType<T, N> *positions, SIMDType<T, N> *in, long num_samps, const double start_samp, const double end_samp)
{
    const SIMDType<T, N> mul(end_samp - start_samp);
    const SIMDType<T, N> add(start_samp);
    const SIMDType<T, N> zero(static_cast<T>(0));
    const SIMDType<T, N> one(static_cast<T>(1));
    
    long num_vecs = num_samps / N;
    
    for (long i = 0; i < num_vecs; i++)
        positions[i] = (mul * min(one, max(zero, *in++))) + add;
}

template <class T>
void perform_positions(T *positions, T *in, long vec_size, double start_samp, double end_samp)
{
    const int size = SIMDLimits<T>::max_size;
    long vec_count = (vec_size / size) * size;
    
    SIMDType<T, size> *v_positions = reinterpret_cast<SIMDType<T, size> *>(positions);
    SIMDType<T, size> *v_in = reinterpret_cast<SIMDType<T, size> *>(in);
    
    SIMDType<T, 1> *s_positions = reinterpret_cast<SIMDType<T, 1> *>(positions + vec_count);
    SIMDType<T, 1> *s_in = reinterpret_cast<SIMDType<T, 1> *>(in + vec_count);
    
    perform_positions(v_positions, v_in, vec_count, start_samp, end_samp);
    perform_positions(s_positions, s_in, (vec_size - vec_count), start_samp, end_samp);
}

long clip(const long in, const long max)
{
    return std::min(max, std::max(0L, in));
}

template <class T>
void perform_core(t_ibuftable *x, T *in, T *out, long vec_size)
{
    // Check if the buffer is set / valid and get the length information
    
    ibuffer_data buffer(x->buffer_name);
    
    long start_samp = clip(x->start_samp, buffer.get_length() - 1);
    long end_samp = clip(x->end_samp, buffer.get_length() - 1);
    long chan = clip(x->chan - 1, buffer.get_num_chans() - 1);
    
    // Calculate output

    if (buffer.get_length() >= 1)
    {
        perform_positions(out, in, vec_size, start_samp, end_samp);
        ibuffer_read(buffer, out, out, vec_size, chan, 1.f, x->interp_type);
    }
    else
    {
        for (long i = 0; i < vec_size; i++)
            *out++ = 0.f;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// Perform and DSP for 32-bit signals ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

t_int *ibuftable_perform(t_int *w)
{	
	// Ignore the copy of this function pointer (due to denormal fixer)
	
	// Set pointers
	
	float *in = (float *) w[2];
	float *out = (float *)(w[3]);
	long vec_size = w[4];
    t_ibuftable *x = (t_ibuftable *) w[5];
		
	if (!x->x_obj.z_disabled)
        perform_core(x, in, out, vec_size);

	return w + 6;
}

void ibuftable_dsp(t_ibuftable *x, t_signal **sp, short *count)
{
	// Set buffer again in case it is no longer valid / extant

	ibuftable_set_internal(x, x->buffer_name);
	
	if (count[0] && count[1])
        dsp_add(denormals_perform, 5, ibuftable_perform, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n, x);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// Perform and DSP for 64-bit signals ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ibuftable_perform64(t_ibuftable *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
	perform_core(x, ins[0], outs[0], vec_size);
}

void ibuftable_dsp64(t_ibuftable *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	// Set buffer again in case it is no longer valid / extant
	
	ibuftable_set_internal(x, x->buffer_name);
	
	if (count[0] && count[1])
        object_method(dsp64, gensym("dsp_add64"), x, ibuftable_perform64);
}
