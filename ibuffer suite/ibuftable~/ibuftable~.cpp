
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


void *ibuftable_new(t_symbol *the_buffer, t_atom_long start_samp, t_atom_long end_samp, t_atom_long chan);
void ibuftable_free(t_ibuftable *x);
void ibuftable_assist(t_ibuftable *x, void *b, long m, long a, char *s);

void ibuftable_interp(t_ibuftable *x, t_symbol *s, long argc, t_atom *argv);
void ibuftable_set(t_ibuftable *x, t_symbol *msg, long argc, t_atom *argv);
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
						   A_SYM, 
						   A_DEFLONG, 
						   A_DEFLONG, 
						   A_DEFLONG, 
						   0);
	
	class_addmethod(this_class, (method)ibuftable_interp, "interp", A_GIMME, 0);
	class_addmethod(this_class, (method)ibuftable_set, "set", A_GIMME, 0);	
	class_addmethod(this_class, (method)ibuftable_startsamp, "startsamp", A_LONG, 0);	
	class_addmethod(this_class, (method)ibuftable_endsamp, "endsamp", A_LONG, 0);	
	class_addmethod(this_class, (method)ibuftable_chan, "chan", A_LONG, 0);	
	class_addmethod(this_class, (method)ibuftable_assist, "assist", A_CANT, 0);
	class_addmethod(this_class, (method)ibuftable_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)ibuftable_dsp64, "dsp64", A_CANT, 0);
	
	class_dspinit(this_class);
	class_register(CLASS_BOX, this_class);
	
	ibuffer_init();
	
	return 0;
}

void *ibuftable_new(t_symbol *the_buffer, t_atom_long start_samp, t_atom_long end_samp, t_atom_long chan)
{
    t_ibuftable *x = (t_ibuftable *)object_alloc(this_class);
    
    dsp_setup((t_pxobject *)x, 1);
    x->x_obj.z_misc = Z_NO_INPLACE;
	outlet_new((t_object *)x, "signal");
	
	// Default variables
	
    x->buffer_name = the_buffer;
	
	// Arguments
	
    if (end_samp == 0)
        end_samp = 512;
    if (end_samp < start_samp)
        end_samp = start_samp;
    
    if (start_samp > end_samp)
        std::swap(start_samp, end_samp);
    
	x->start_samp = start_samp;
	x->end_samp = end_samp;
	x->chan = chan;
	x->interp_type = kInterpLinear;
	
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

void ibuftable_interp(t_ibuftable *x, t_symbol *s, long argc, t_atom *argv)
{
	t_symbol *mode = argc ? atom_getsym(argv) : ps_linear;
	
	if (mode == ps_linear)
		x->interp_type = kInterpLinear;
	if (mode == ps_bspline)
		x->interp_type = kInterpCubicBSpline;
	if (mode == ps_hermite)
		x->interp_type = kInterpCubicHermite;
	if (mode == ps_lagrange)
		x->interp_type = kInterpCubicLagrange;
	
	if (mode != ps_linear && mode != ps_bspline &&  mode != ps_hermite && mode != ps_lagrange)
		object_error ((t_object *) x, "ibuftable~: no interpolation mode %s", mode->s_name);
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

void ibuftable_startsamp(t_ibuftable *x, t_atom_long start_samp)
{
    x->start_samp = start_samp;
}

void ibuftable_endsamp(t_ibuftable *x, t_atom_long end_samp)
{
    x->end_samp = end_samp;
}

void ibuftable_chan(t_ibuftable *x, t_atom_long chan)
{
    x->chan = chan;
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
