
/*
 *  ibufplayer~
 *
 *	ibufplayer~ is an efficient playback object for ibuffer~ and standard buffer~ objects.
 *
 *	ibufplayer~ features SIMD optimisation, individual channel level controls, reduced CPU usage for integer playback speeds and fixed speed, or varispeed (with a base speed) playback.
 *	It is also features three different kinds of cubic interpolation which can be requested as desired.
 *	It should at least (roughly) as fast as groove~ for worst case scenarios, and faster in various other situations.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Denormals.h>
#include <AH_Win_Math.h>
#include <ibuffer_access.hpp>

t_class *this_class;


typedef enum {
    
    FLAG_NONE,
    FLAG_PLAY,
    FLAG_STOP,
    
} t_transport_flag;


typedef struct _ibufplayer
{
    t_pxobject x_obj;
	
	t_symbol *buffer_name;
	
	double pos, speed, start_samp, min_samp, max_samp, sr_div;
	float vol1, vol2, vol3, vol4;

    t_transport_flag transport_flag;
    
	bool sig_control, input_connected, playing;
	long obj_n_chans;
	
    InterpType interp_type;
	
	void *done_clock;
	void *bang_outlet;
	
} t_ibufplayer;


void *ibufplayer_new(t_atom_long obj_n_chans);
void ibufplayer_free(t_ibufplayer *x);
void ibufplayer_assist(t_ibufplayer *x, void *b, long m, long a, char *s);

void ibufplayer_set(t_ibufplayer *x, t_symbol *msg, long argc, t_atom *argv);
void ibufplayer_set_internal(t_ibufplayer *x, t_symbol *s);
void ibufplayer_vols(t_ibufplayer *x, t_symbol *s, long argc, t_atom *argv);
void ibufplayer_interp(t_ibufplayer *x, t_symbol *s, long argc, t_atom *argv);
void ibufplayer_play(t_ibufplayer *x, t_symbol *s, long argc, t_atom *argv);
void ibufplayer_stop(t_ibufplayer *x);

void ibufplayer_done_bang(t_ibufplayer *x);

t_int *ibufplayer_perform(t_int *w);
void ibufplayer_dsp(t_ibufplayer *x, t_signal **sp, short *count);

void ibufplayer_perform64(t_ibufplayer *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void ibufplayer_dsp64(t_ibufplayer *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

int C74_EXPORT main(void)
{
	this_class = class_new ("ibufplayer~",
							(method) ibufplayer_new, 
							(method) ibufplayer_free, 
							sizeof (t_ibufplayer), 
							NULL, 
							A_DEFLONG,
							0);
	
	class_addmethod(this_class, (method)ibufplayer_set, "set", A_GIMME, 0);	
	class_addmethod(this_class, (method)ibufplayer_vols, "vols", A_GIMME, 0);
	class_addmethod(this_class, (method)ibufplayer_interp, "interp", A_GIMME, 0);
	class_addmethod(this_class, (method)ibufplayer_play, "play", A_GIMME, 0);
	class_addmethod(this_class, (method)ibufplayer_stop, "stop", 0);
	class_addmethod(this_class, (method)ibufplayer_assist, "assist", A_CANT, 0);
	class_addmethod(this_class, (method)ibufplayer_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)ibufplayer_dsp64, "dsp64", A_CANT, 0);
	
	class_dspinit(this_class);
	class_register(CLASS_BOX, this_class);
	
	ibuffer_init();
	  
	return 0;
}

void *ibufplayer_new(t_atom_long obj_n_chans)
{
	t_ibufplayer *x = (t_ibufplayer *)object_alloc(this_class);
    
    dsp_setup((t_pxobject *)x, 1);
	x->x_obj.z_misc = Z_NO_INPLACE;	
	
	x->bang_outlet = bangout(x);
	
	// Determine number of channels
	
	if (obj_n_chans < 1)
		obj_n_chans = 1;
	if (obj_n_chans > 4)
		obj_n_chans = 4;
	
	// Create signal outlets
	
    for (long i = 0; i < obj_n_chans + 1; i++)
		outlet_new((t_object *)x, "signal");
	
	// Default variables
	
	x->pos = 0;
	x->speed = 1;
	x->sig_control = false;
	x->playing = false;
	x->input_connected = false;
    x->transport_flag = FLAG_NONE;
	x->obj_n_chans = obj_n_chans;
	
	x->buffer_name = NULL;
	
	x->vol1 = 1.f;
	x->vol2 = 1.f;
	x->vol3 = 1.f;
	x->vol4 = 1.f;
	
	x->interp_type = kInterpCubicHermite;
	
	x->done_clock = clock_new(x, (method) ibufplayer_done_bang);

	return (x);
}

void ibufplayer_free(t_ibufplayer *x)
{
	dsp_free(&x->x_obj);
	freeobject(static_cast<t_object *>(x->done_clock));
}

void ibufplayer_assist(t_ibufplayer *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
    {
		if (a < x->obj_n_chans)
        {
            sprintf(s,"(signal) Output %ld", a + 1);
        }
		else 
		{
			switch (a - x->obj_n_chans)
			{
				case 0:
					sprintf(s,"(signal) Position Output (0-1)");
					break;
				case 1:
					sprintf(s,"(bang) Done Playing");
					break;
			}
		}
	}
	else 
	{
        sprintf(s,"(signal) Speed Input");
    }
}

void ibufplayer_set(t_ibufplayer *x, t_symbol *msg, long argc, t_atom *argv)
{
	ibufplayer_set_internal(x, argc ? atom_getsym(argv) : 0);
}

void ibufplayer_set_internal(t_ibufplayer *x, t_symbol *s)
{
	void *b = ibuffer_get_ptr(s);
	
	if (b) 
	{
        // FIX!
		//if (b != x->buffer_pointer)
			x->pos = -1.0;
		x->buffer_name = s;
		
	}
	else 
	{
		x->buffer_name = NULL;
		if (s)
			error("ibufplayer~: no buffer %s", s->s_name);
	}
    
    ibuffer_release_ptr(b);
}

void ibufplayer_vols(t_ibufplayer *x,  t_symbol *s, long argc, t_atom *argv)
{
	x->vol1 = atom_getfloat(argv++);
	
	if (argc > 1) 
		x->vol2 = atom_getfloat(argv++);
	
	if (argc > 2) 
		x->vol3 = atom_getfloat(argv++);
	
	if (argc > 3) 
		x->vol4 = atom_getfloat(argv);
}

void ibufplayer_interp(t_ibufplayer *x, t_symbol *s, long argc, t_atom *argv)
{
	t_symbol *mode = argc ? atom_getsym(argv) : ps_hermite;
	
	if (mode == ps_bspline)
		x->interp_type = kInterpCubicBSpline;
	if (mode == ps_hermite)
		x->interp_type = kInterpCubicHermite;
	if (mode == ps_lagrange)
		x->interp_type = kInterpCubicLagrange;
	
	if (mode != ps_bspline &&  mode != ps_hermite && mode != ps_lagrange)
		error ("ibufplayer~: no interpolation mode %s", mode->s_name);
}

void ibufplayer_stop(t_ibufplayer *x)
{	
	x->transport_flag = FLAG_STOP;
}

// FIX - temp

double ibuffer_sample_rate(t_symbol *name)
{
    void *buffer_pointer = ibuffer_get_ptr(name);
    const ibuffer_data data = ibuffer_info(buffer_pointer);
    
    double sample_rate = data.sample_rate;
    
    ibuffer_release_ptr(buffer_pointer);
    
    return sample_rate;
}
void ibufplayer_play(t_ibufplayer *x, t_symbol *s, long argc, t_atom *argv)
{
	double sr_ms = 44.1;
	double start_samp;
	double end_samp;
	double start = 0; 
	double end = -1; 
	double speed = 1;
	t_atom_long sig_control = 0;
	
	// Set buffer if there is one named
	
	if (argc && atom_gettype(argv) == A_SYM) 
	{
		ibufplayer_set_internal(x, atom_getsym(argv++));
		argc--;
	}
	
	// Get the following parameters (if given)
	
	if (argc >= 1) 
		speed = atom_getfloat(argv++);
	if (argc >= 2) 
		start = atom_getfloat(argv++);
	if (argc >= 3) 
		end = atom_getfloat(argv++); 
	if (argc >= 4) 
		sig_control = atom_getlong(argv++);
	
	// If we can't play (no buffer or zero speed) then we report done, so as to free resources depending on this
	
	if (!x->buffer_name || !speed)
	{
		ibufplayer_done_bang(x); 
		return;
	}
	
	// Calculate parameters and set the play flag to be picked up in the perform routine
	
	sr_ms = 0.001 * ibuffer_sample_rate(x->buffer_name);
	if (start < 0) 
		start = 0;
	if (end < 0) 
		end = DBL_MAX;
	
	speed = fabs(speed);
	start_samp = start * sr_ms;
	end_samp = end * sr_ms;
	x->start_samp = start_samp;
	
	// Consider playback direction
	
	if (start < end)
	{
		x->speed = speed;
		x->min_samp = start_samp;
		x->max_samp = end_samp;
	}
	else
	{
		x->speed = -speed;
		x->min_samp = end_samp;
		x->max_samp = start_samp;
	}
	
    x->sig_control = sig_control ? true : false;
	x->transport_flag = FLAG_PLAY;
}

void ibufplayer_done_bang(t_ibufplayer *x)
{
	outlet_bang(x->bang_outlet);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////// Core Perform Routines ///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
void ibufplayer_update_phase(double *&positions, T *&phases, double &pos, const double &speed, const double start_samp, const double length_recip)
{
    *positions++ = pos;
    *phases++ = (pos - start_samp) * length_recip;
    pos += speed;
}

template<class T>
long ibufplayer_phase(double *positions, T *phases, T *in, long vec_size, double speed, double start_samp, double min_samp, double max_samp, double length_recip, bool sig_control, double &pos_store)
{
    double pos = pos_store;
    long todo, i;
    
    if (sig_control)
    {
        for (i = 0; i < vec_size; i++)
        {
            if (pos > max_samp || pos < min_samp)
                break;
            
            ibufplayer_update_phase(positions, phases, pos, *in++ * speed, start_samp, length_recip);
        }
        todo = i;
    }
    else
    {
        if (speed > 0)
            todo = ((vec_size * speed) + pos > max_samp) ? (max_samp - pos) / speed : vec_size;
        else
            todo = ((vec_size * speed) + pos < min_samp) ? (min_samp - pos) / speed : vec_size;
        
        for (i = 0; i < todo; i++)
            ibufplayer_update_phase(positions, phases, pos, speed, start_samp, length_recip);
    }
    
    pos_store = todo < vec_size ? -1.0 : pos;
    
    return todo;
}

template <class T>
void perform_core(t_ibufplayer *x, T *in, T **outs, T *phase_out, double *positions, long vec_size)
{
    // Object and local variables
    
    float vols[4];
    
    long obj_n_chans = x->obj_n_chans;
    //bool input_connected = x->input_connected;
    long todo = 0;
    
    vols[0] = x->vol1;
    vols[1] = x->vol2;
    vols[2] = x->vol3;
    vols[3] = x->vol4;
    
    // Zero outputs and set default position output
    
    for (long i = 0; i < obj_n_chans; i++)
        memset(outs[i], 0, vec_size * sizeof(T));
    
    for (long i = 0; i < vec_size; i++)
        phase_out[i] = 1.0;
    
    void *buffer_pointer = ibuffer_get_ptr(x->buffer_name);
    const ibuffer_data data = ibuffer_info(buffer_pointer);
    
    // Check on playback state / new play instruction and decide whether to output
    
    if (buffer_pointer)
    {
        double speed = x->speed * data.sample_rate * x->sr_div;
        double start_samp = x->start_samp;
        double min_samp = x->min_samp;
        double max_samp = x->max_samp;
        
        if (max_samp > data.length)
            max_samp = data.length;
        
        double length_recip = (speed < 0) ? -1.0 / (max_samp - min_samp) : 1.0 / (max_samp - min_samp);
        
        if (x->transport_flag == FLAG_PLAY)
            x->pos = start_samp;
        
        if (x->transport_flag == FLAG_STOP)
            x->pos = -1.0;
        
        if (x->pos <= max_samp && x->pos >= min_samp)
        {
            bool sig_control = x->sig_control;
            
            x->playing = true;

            // Calculate the phasor block
            
            todo = ibufplayer_phase(positions, phase_out, in, vec_size, speed, start_samp, min_samp, max_samp, length_recip, sig_control, x->pos);
            
            long ispeed = speed;;
            InterpType interp_type = (!(speed - ispeed) && !sig_control) ? kInterpNone : x->interp_type;
            
            // Now get samples interpolating as relevant
            
            for (long i = 0; i < obj_n_chans; i++)
                if (vols[i] && data.num_chans > i)
                    ibuffer_read(data, outs[i], positions, todo, i, vols[i], interp_type);
            
            if (positions == reinterpret_cast<double *>(outs[obj_n_chans - 1]) && obj_n_chans > data.num_chans)
                memset(positions, 0, todo * sizeof(double));
        }
    }
    
    ibuffer_release_ptr(buffer_pointer);
    
    bool playing = !(todo < vec_size);
    if (!playing && x->playing)
        clock_delay (x->done_clock, 0);
    x->playing = playing;
    x->transport_flag = FLAG_NONE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// Perform and DSP for 32-bit signals ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

t_int *ibufplayer_perform(t_int *w)
{		
	// Ignore the copy of this function pointer (due to denormal fixer)
	
	// Set pointers
	
	float *in = (float *) w[2];
	
	float *outs[5];	
	long vec_size = w[8];
    t_ibufplayer *x = (t_ibufplayer *) w[9];
	
	float *phase_out;

	outs[0] = (float *)(w[3]);
	outs[1] = (float *)(w[4]);
	outs[2] = (float *)(w[5]);
	outs[3] = (float *)(w[6]);
	outs[4] = (float *)(w[7]);
	
	phase_out = outs[4];

    if (!x->x_obj.z_disabled)
        perform_core(x, in, outs, phase_out, NULL, vec_size);
    
	return w + 10;
}

void ibufplayer_dsp(t_ibufplayer *x, t_signal **sp, short *count)
{
	x->sr_div = 1.0 / sp[0]->s_sr;
	
	// Set buffer again in case it is no longer valid / extant
	
	ibufplayer_set_internal(x, x->buffer_name);

	// Check if input is connected
	
	x->input_connected = count[0];
	
	switch (x->obj_n_chans)
    {
        case 1:
            dsp_add(denormals_perform, 9, ibufplayer_perform, sp[0]->s_vec, sp[1]->s_vec, 0, 0, 0, sp[2]->s_vec, sp[0]->s_n, x);
            break;
        case 2:
            dsp_add(denormals_perform, 9, ibufplayer_perform, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, 0, 0, sp[3]->s_vec, sp[0]->s_n, x);
            break;
        case 3:
            dsp_add(denormals_perform, 9, ibufplayer_perform, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, 0, sp[4]->s_vec, sp[0]->s_n, x);
            break;
        case 4:
            dsp_add(denormals_perform, 9, ibufplayer_perform, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[0]->s_n, x);
            break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// Perform and DSP for 64-bit signals ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

void ibufplayer_perform64(t_ibufplayer *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    perform_core(x, ins[0], outs, outs[numouts - 1], outs[numouts - 2], vec_size);
}

void ibufplayer_dsp64(t_ibufplayer *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    x->sr_div = 1.0 / samplerate;
    
    // Set buffer again in case it is no longer valid / extant
    
    ibufplayer_set_internal(x, x->buffer_name);
    
    // Check if input is connected
    
    x->input_connected = count[0];
    
    object_method(dsp64, gensym("dsp_add64"), x, ibufplayer_perform64);
}


/*

// SIMD phasors for varispeed / fixed speed - these can only be used under intel as there is no double precision floating point type under Altivec

static __inline long ibufplayer_phase(double *pos_store, double speed, double start_samp, double min_samp, double max_samp, double length_recip,
                                      bool sig_control, vFloat *in,  vFloat *fracts, AH_SIntPtr *l_offsets, float *f_phase_out, long vec_size)
{
    double phase_test[4];
    double vpos_init[2];
    double pos = *pos_store;
    
    float *f_fracts = (float *) fracts;
    long todo, i;
    
    vDouble vdoublespeed        = {speed + speed, speed + speed};
    vDouble vminsamp            = {min_samp, min_samp};
    vDouble vmaxsamp            = {max_samp, max_samp};
    vDouble vstartsamp            = {start_samp, start_samp};
    vDouble    vlength_recip        = {length_recip, length_recip};
    
    // N.B. Here it is assumed that AH_SIntPtr is 32 bits wide, which is safe, because this function is only ever called when Max 5 is running (in 32 bits)
    
    vDouble vpos, vfract1, vfract2, vphase1, vphase2, vmultspeed, vnextmultspeed;
    vSInt32 *offsets = (vSInt32 *) l_offsets;
    vFloat *phase_out = (vFloat *) f_phase_out;
    vFloat *temp_fracts = fracts;
    vFloat vspeedin, vnextspeedin;
    vFloat vspeed = float2vector(speed);
    vSInt32 vipos1, vipos2;
    
    if (sig_control)
    {
        for (i = 0; i < vec_size >> 2; i++)
            *temp_fracts++ = F32_VEC_MUL_OP(*in++, vspeed);
        
        vpos_init[0] = pos;
        vpos_init[1] = pos + *f_fracts;
        vpos = F64_VEC_ULOAD(vpos_init);
        
        vnextspeedin = *fracts;
        vnextmultspeed = F64_VEC_FROM_F32(vnextspeedin);
        
        for (i = 0; i < vec_size >> 2; i++)
        {
            // Note - this reads over by one vector but we own the subsequent memory - last vpos calc is incorrect and is discarded
            
            vspeedin =  vnextspeedin;
            vnextspeedin = *(fracts + 1);
            
            vmultspeed = vnextmultspeed;
            vnextmultspeed = F64_VEC_FROM_F32(F32_VEC_MOVE_HI(vspeedin, vspeedin));
            
            vmultspeed = F64_VEC_ADD_OP(F64_VEC_SHUFFLE(vmultspeed, vnextmultspeed, 0x1), vmultspeed);
            vipos1 = I32_VEC_FROM_F64_TRUNC(vpos);
            vfract1 = F64_VEC_SUB_OP(vpos, F64_VEC_FROM_I32(vipos1));
            vphase1 = F64_VEC_MUL_OP(F64_VEC_SUB_OP(vpos, vstartsamp), vlength_recip);
            
            F64_VEC_USTORE(phase_test, F64_VEC_OR_OP(F64_VEC_GT_OP(vpos, vmaxsamp), F64_VEC_LT_OP(vpos, vminsamp)));
            vpos = F64_VEC_ADD_OP(vpos, vmultspeed);
            
            vmultspeed = vnextmultspeed;
            vnextmultspeed = F64_VEC_FROM_F32(vnextspeedin);
            
            vmultspeed = F64_VEC_ADD_OP(F64_VEC_SHUFFLE(vmultspeed, vnextmultspeed, 0x1), vmultspeed);
            vipos2 = I32_VEC_FROM_F64_TRUNC(vpos);
            vfract2 = F64_VEC_SUB_OP(vpos, F64_VEC_FROM_I32(vipos2));
            vphase2 = F64_VEC_MUL_OP(F64_VEC_SUB_OP(vpos, vstartsamp), vlength_recip);
            
            F64_VEC_USTORE(phase_test + 2 , F64_VEC_OR_OP(F64_VEC_GT_OP(vpos, vmaxsamp), F64_VEC_LT_OP(vpos, vminsamp)));
            vpos = F64_VEC_ADD_OP(vpos, vmultspeed);
            
            *offsets++ = I32_VEC_OR_OP(vipos1, I32_VEC_SHUFFLE(vipos2, 0x4E));
            *fracts++ = F32_VEC_MOVE_LO(F32_VEC_FROM_F64(vfract1), F32_VEC_FROM_F64(vfract2));
            *phase_out++ = F32_VEC_MOVE_LO(F32_VEC_FROM_F64(vphase1), F32_VEC_FROM_F64(vphase2));
            
            if (phase_test[0] || phase_test[1] || phase_test[2] || phase_test[3])
                break;
        }
        
        F64_VEC_STORE_LO(pos_store, vpos);
        todo =  i << 2;
        
        if (todo < vec_size)
        {
            if (!phase_test[0])
            {
                todo++;
                if (!phase_test[1])
                {
                    todo++;
                    if (!phase_test[2])
                        todo++;
                }
            }
        }
    }
    else
    {
        if (speed > 0)
            todo = ((vec_size * speed) + pos > max_samp) ? (max_samp - pos) / speed : vec_size;
        else
            todo = ((vec_size * speed) + pos < min_samp) ? (min_samp - pos) / speed : vec_size;
        
        vpos_init[0] = pos;
        vpos_init[1] = pos + speed;
        
        vpos = F64_VEC_ULOAD(vpos_init);
        
        for (i = 0; i < (todo + 3) >> 2; i++)
        {
            vipos1 = I32_VEC_FROM_F64_TRUNC(vpos);
            vfract1 = F64_VEC_SUB_OP(vpos, F64_VEC_FROM_I32(vipos1));
            vphase1 = F64_VEC_MUL_OP(F64_VEC_SUB_OP(vpos, vstartsamp), vlength_recip);
            
            vpos = F64_VEC_ADD_OP(vpos, vdoublespeed);
            
            vipos2 = I32_VEC_FROM_F64_TRUNC(vpos);
            vfract2 = F64_VEC_SUB_OP(vpos, F64_VEC_FROM_I32(vipos2));
            vphase2 = F64_VEC_MUL_OP(F64_VEC_SUB_OP(vpos, vstartsamp), vlength_recip);
            
            vpos = F64_VEC_ADD_OP(vpos, vdoublespeed);
            
            *offsets++ = I32_VEC_OR_OP(vipos1, I32_VEC_SHUFFLE(vipos2, 0x4E));
            *fracts++ = F32_VEC_MOVE_LO(F32_VEC_FROM_F64(vfract1), F32_VEC_FROM_F64(vfract2));
            *phase_out++ = F32_VEC_MOVE_LO(F32_VEC_FROM_F64(vphase1), F32_VEC_FROM_F64(vphase2));
        }
        
        F64_VEC_STORE_LO(pos_store, vpos);
    }
    
    for (i = todo; i < (((todo + 3) >> 2) << 2); i++)
        f_phase_out[i] = 1.f;
    
    return todo;
}


// Scalar phasors for use under ppc and for smaller vector sizes

static __inline long ibufplayer_phase_scalar(double *pos_store, double speed, double start_samp, double min_samp, double max_samp, double length_recip,
                                             bool sig_control, float *f_in,  float *f_fracts, AH_SIntPtr *offsets, float *phase_out, long vec_size)
{
    AH_SIntPtr ipos;
    double pos = *pos_store;
    long todo, i;
    float fract;
    
    if (sig_control)
    {
        for (i = 0; i < vec_size; i++)
        {
            float multspeed = *f_in++ * speed;
            *offsets++ = ipos = (AH_SIntPtr) pos;
            fract = pos - (double) ipos;
            *phase_out++ = (pos - start_samp) * length_recip;
            pos += multspeed;
            *f_fracts++ = fract;
            if (pos > max_samp || pos < min_samp)
                break;
        }
        todo = i;
    }
    else
    {
        if (speed > 0)
            todo = ((vec_size * speed) + pos > max_samp) ? (max_samp - pos) / speed : vec_size;
        else
            todo = ((vec_size * speed) + pos < min_samp) ? (min_samp - pos) / speed : vec_size;
        
        for (i = 0; i < todo; i++)
        {
            *offsets++ = ipos = (AH_SIntPtr) pos;
            *f_fracts++ = pos - (double) ipos;
            *phase_out++ = (pos - start_samp) * length_recip;
            pos += speed;
        }
    }
    
    *pos_store = pos;
    return todo;
}

static __inline long ibufplayer_phase64(double *pos_store, double speed, double start_samp, double min_samp, double max_samp, double length_recip,
                                         bool sig_control, double *d_in,  double *d_fracts, AH_SIntPtr *l_offsets, double *d_phase_out, long vec_size)
{
    double phase_test[2];
    double vpos_init[2];
    double pos = *pos_store;
    
    long todo, i;
    
    vDouble vdoublespeed		= {speed + speed, speed + speed};
    vDouble vminsamp			= {min_samp, min_samp};
    vDouble vmaxsamp			= {max_samp, max_samp};
    vDouble vstartsamp			= {start_samp, start_samp};
    vDouble	vlength_recip		= {length_recip, length_recip};
    
    vSInt64 *offsets = (vSInt64 *) l_offsets;
    
    vDouble *in = (vDouble *) d_in;
    vDouble *phase_out = (vDouble *) d_phase_out;
    vDouble *fracts = (vDouble *) d_fracts;
    
    vDouble vpos, vmultspeed, vnextmultspeed;
    vDouble vspeed = double2vector(speed);
    
    if (sig_control)
    {
        vpos_init[0] = pos;
        vpos_init[1] = pos + (*d_in * speed);
        vpos = F64_VEC_ULOAD(vpos_init);
        
        vnextmultspeed = F64_VEC_MUL_OP(*in++, vspeed);
        
        for (i = 0; i < (vec_size >> 1); i++)
        {
            // Note - this reads over by one vector but we own the subsequent memory - last vpos calc is incorrect and is discarded
            
            vmultspeed = vnextmultspeed;
            vnextmultspeed = F64_VEC_MUL_OP(*in++, vspeed);
            vmultspeed = F64_VEC_ADD_OP(F64_VEC_SHUFFLE(vmultspeed, vnextmultspeed, 0x1), vmultspeed);
            
            F64_VEC_SPLIT_I64_F64(vpos, offsets++, fracts++);
            *phase_out++ = F64_VEC_MUL_OP(F64_VEC_SUB_OP(vpos, vstartsamp), vlength_recip);
            
            F64_VEC_USTORE(phase_test, F64_VEC_OR_OP(F64_VEC_GT_OP(vpos, vmaxsamp), F64_VEC_LT_OP(vpos, vminsamp)));
            vpos = F64_VEC_ADD_OP(vpos, vmultspeed);
            
            if (phase_test[0] || phase_test[1])
                break;
        }
        
        F64_VEC_STORE_LO(pos_store, vpos);
        todo =  i << 1;
        
        // FIX - phase values could do with fixing here
        // FIX - why do values seem bigger when using SIMD?
        if (todo < vec_size)
        {
            if (!phase_test[0])
            {
                todo++;
                if (!phase_test[1])
                    todo++;
            }
        }
    }
    else
    {
        if (speed > 0)
            todo = ((vec_size * speed) + pos > max_samp) ? (max_samp - pos) / speed : vec_size;
        else
            todo = ((vec_size * speed) + pos < min_samp) ? (min_samp - pos) / speed : vec_size;
        
        vpos_init[0] = pos;
        vpos_init[1] = pos + speed;
        
        vpos = F64_VEC_ULOAD(vpos_init);
        
        for (i = 0; i < ((todo + 1) >> 1); i++)
        {
            F64_VEC_SPLIT_I64_F64(vpos, offsets++, fracts++);
            *phase_out++ = F64_VEC_MUL_OP(F64_VEC_SUB_OP(vpos, vstartsamp), vlength_recip);
            vpos = F64_VEC_ADD_OP(vpos, vdoublespeed);
        }
        
        F64_VEC_STORE_LO(pos_store, vpos);
    }
    
    for (i = todo; i < (((todo + 1) >> 1) << 1); i++)
        d_phase_out[i] = 1.0;
    
    return todo;
}

static __inline long ibufplayer_phase64(double *pos_store, double speed, double start_samp, double min_samp, double max_samp, double length_recip,
								bool sig_control, double *d_in,  double *d_fracts, AH_SIntPtr *l_offsets, double *d_phase_out, long vec_size)
{
	double phase_test[4];
	double vpos_init[2];
	double pos = *pos_store;
	
	long todo, i;
	
	vDouble vdoublespeed		= {speed + speed, speed + speed};
	vDouble vminsamp			= {min_samp, min_samp};
	vDouble vmaxsamp			= {max_samp, max_samp};
	vDouble vstartsamp			= {start_samp, start_samp};
	vDouble	vlength_recip		= {length_recip, length_recip};
	
	vSInt32 *offsets = (vSInt32 *) l_offsets;
	
	vDouble *in = (vDouble *) d_in;
	vDouble *phase_out = (vDouble *) d_phase_out;
	vDouble *fracts = (vDouble *) d_fracts;

	vDouble vpos, vmultspeed, vnextmultspeed;
	vDouble vspeed = double2vector (speed);
	vSInt32 vipos1, vipos2;
	
    return ibufplayer_phase_scalar64(pos_store, speed, start_samp, min_samp, max_samp, length_recip, sig_control, d_in, d_fracts, l_offsets, d_phase_out, vec_size);
    
	if (sig_control)
	{		
		vpos_init[0] = pos;
		vpos_init[1] = pos + (*d_in * speed);
		vpos = F64_VEC_ULOAD (vpos_init);
		
		vnextmultspeed = F64_VEC_MUL_OP(*in++, vspeed);
		
		for (i = 0; i < (vec_size >> 2); i++)
		{
			// Note - this reads over by one vector but we own the subsequent memory - last vpos calc is incorrect and is discarded
			
			vmultspeed = vnextmultspeed;
			vnextmultspeed = F64_VEC_MUL_OP(*in++, vspeed);
			vmultspeed = F64_VEC_ADD_OP(F64_VEC_SHUFFLE(vmultspeed, vnextmultspeed, 0x1), vmultspeed);

			vipos1 = I32_VEC_FROM_F64_TRUNC(vpos);
			*fracts++ = F64_VEC_SUB_OP(vpos, F64_VEC_FROM_I32(vipos1));
			*phase_out++ = F64_VEC_MUL_OP(F64_VEC_SUB_OP(vpos, vstartsamp), vlength_recip);
			
			F64_VEC_USTORE(phase_test, F64_VEC_OR_OP(F64_VEC_GT_OP(vpos, vmaxsamp), F64_VEC_LT_OP(vpos, vminsamp)));
			vpos = F64_VEC_ADD_OP(vpos, vmultspeed);
			
			vmultspeed = vnextmultspeed;
			vnextmultspeed = F64_VEC_MUL_OP(*in++, vspeed);
			vmultspeed = F64_VEC_ADD_OP(F64_VEC_SHUFFLE(vmultspeed, vnextmultspeed, 0x1), vmultspeed);
			
			vipos2 = I32_VEC_FROM_F64_TRUNC(vpos);
			*fracts++ = F64_VEC_SUB_OP(vpos, F64_VEC_FROM_I32(vipos2));
			*phase_out++ = F64_VEC_MUL_OP(F64_VEC_SUB_OP(vpos, vstartsamp), vlength_recip);
			
			F64_VEC_USTORE(phase_test + 2, F64_VEC_OR_OP(F64_VEC_GT_OP(vpos, vmaxsamp), F64_VEC_LT_OP(vpos, vminsamp)));
			vpos = F64_VEC_ADD_OP(vpos, vmultspeed);
			
			*offsets++ = I32_VEC_OR_OP(vipos1, I32_VEC_SHUFFLE(vipos2, 0x4E));
			
			if (phase_test[0] || phase_test[1] || phase_test[2] || phase_test[3])
				break;
		}
		
		F64_VEC_STORE_LO(pos_store, vpos);
		todo =  i << 2;
		
		// FIX - phase values could do with fixing here
		// FIX - why do values seem bigger when using SIMD?
		if (todo < vec_size)
		{
			if (!phase_test[0])
			{
				todo++;
				if (!phase_test[1])
				{		
					todo++;
					if (!phase_test[2])	
						todo++;
				}
			}
		}
	}
	else
	{
		if (speed > 0)
			todo = ((vec_size * speed) + pos > max_samp) ? (max_samp - pos) / speed : vec_size;
		else
			todo = ((vec_size * speed) + pos < min_samp) ? (min_samp - pos) / speed : vec_size;
		
		vpos_init[0] = pos;
		vpos_init[1] = pos + speed;
		
		vpos = F64_VEC_ULOAD (vpos_init);
		
        for (i = 0; i < ((todo + 3) >> 2); i++)
        {
            vipos1 = I32_VEC_FROM_F64_TRUNC(vpos);
            *fracts++ = F64_VEC_SUB_OP(vpos, F64_VEC_FROM_I32(vipos1));
            *phase_out++ = F64_VEC_MUL_OP(F64_VEC_SUB_OP(vpos, vstartsamp), vlength_recip);
            vpos = F64_VEC_ADD_OP(vpos, vdoublespeed);
            
            vipos2 = I32_VEC_FROM_F64_TRUNC(vpos);
            *fracts++ = F64_VEC_SUB_OP(vpos, F64_VEC_FROM_I32(vipos2));
            *phase_out++ = F64_VEC_MUL_OP(F64_VEC_SUB_OP(vpos, vstartsamp), vlength_recip);
            vpos = F64_VEC_ADD_OP(vpos, vdoublespeed);
            
            *offsets++ = I32_VEC_OR_OP(vipos1, I32_VEC_SHUFFLE(vipos2, 0x4E));
        }
        
		F64_VEC_STORE_LO(pos_store, vpos);
	}
	
	for (i = todo; i < (((todo + 3) >> 2) << 2); i++)
		d_phase_out[i] = 1.0;
	
	return todo;
}
*/




