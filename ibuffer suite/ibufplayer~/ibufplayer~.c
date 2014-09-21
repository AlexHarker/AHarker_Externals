
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
#include <dynamicdsp~.h>
#include <ibuffer_access.h>

void *this_class;


typedef enum {
    
    FLAG_NONE,
    FLAG_PLAY,
    FLAG_STOP,
    
} t_transport_flag;


typedef struct _ibufplayer
{
    t_pxobject x_obj;
	
	t_object *buffer_pointer;
	t_symbol *buffer_name;
	
	double pos, speed, start_samp, min_samp, max_samp, sr_div;
	float vol1, vol2, vol3, vol4;

    t_transport_flag transport_flag;
    
	char sig_control, input_connected, playing;
	long interp_mode, obj_n_chans;
	
	void **temp_mem;
	void *default_mem;
	
	void *dynamicdsp_parent;
	long dynamicdsp_index;
	
	void *done_clock;
	void *bang_outlet;
	
} t_ibufplayer;




void *ibufplayer_new (long obj_n_chans);
void ibufplayer_free (t_ibufplayer *x);
void ibufplayer_assist (t_ibufplayer *x, void *b, long m, long a, char *s);

void ibufplayer_set(t_ibufplayer *x, t_symbol *msg, short argc, t_atom *argv);
void ibufplayer_set_internal(t_ibufplayer *x, t_symbol *s);
void ibufplayer_vols (t_ibufplayer *x, t_symbol *s, short argc, t_atom *argv);
void ibufplayer_interp (t_ibufplayer *x, t_symbol *s, short argc, t_atom *argv);
void ibufplayer_play (t_ibufplayer *x, t_symbol *s, short argc, t_atom *argv);
void ibufplayer_stop (t_ibufplayer *x);

void ibufplayer_done_bang (t_ibufplayer *x);

__inline long ibufplayer_phase (double *pos_store, double speed, double start_samp, double min_samp, double max_samp, double length_recip, 
							  char sig_control, vFloat *in,  vFloat *fracts, long *offsets, float *phase_out, long vec_size);

__inline long ibufplayer_phase_scalar (double *pos_store, double speed, double start_samp, double min_samp, double max_samp, double length_recip, 
									 char sig_control, float *f_in,  float *f_fracts, long *offsets, float *phase_out, long vec_size);

t_int *ibufplayer_perform (t_int *w);
t_int *ibufplayer_perform_small (t_int *w);
void ibufplayer_dsp (t_ibufplayer *x, t_signal **sp, short *count);

__inline long ibufplayer_phase64 (double *pos_store, double speed, double start_samp, double min_samp, double max_samp, double length_recip, 
								char sig_control, double *in,  double *fracts, long *l_offsets, double *d_phase_out, long vec_size);

__inline long ibufplayer_phase_scalar64 (double *pos_store, double speed, double start_samp, double min_samp, double max_samp, double length_recip, 
										 char sig_control, double *f_in,  double *f_fracts, long *offsets, double *phase_out, long vec_size);

void ibufplayer_perform64 (t_ibufplayer *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void ibufplayer_perform_small64 (t_ibufplayer *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void ibufplayer_dsp64 (t_ibufplayer *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

int main(void)
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
	
	ibuffer_init ();
	
	return 0;
}


void *ibufplayer_new (long obj_n_chans)
{
    long i;
	
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
	
    for (i = 0; i < obj_n_chans + 1; i++)
		outlet_new((t_object *)x, "signal");
	
	// Default variables
	
	x->pos = 0;
	x->speed = 1;
	x->sig_control = 0;
	x->playing = 0;
	x->transport_flag = FLAG_NONE;
	x->obj_n_chans = obj_n_chans;
	x->input_connected = 0;
	
	x->buffer_pointer = 0;
	x->buffer_name = 0;
	
	x->default_mem = 0;
	x->temp_mem = 0;
	
	x->vol1 = 1.f;
	x->vol2 = 1.f;
	x->vol3 = 1.f;
	x->vol4 = 1.f;
	
	x->interp_mode = INTERP_TYPE_CUBIC_BSPLINE;
	
	x->done_clock = clock_new (x, (method) ibufplayer_done_bang);
	
	// Get dynamicdsp~ host if it exists (for temporary memory)
	
	x->dynamicdsp_parent = Get_Dynamic_Object();
	x->dynamicdsp_index = Get_Dynamic_Patch_Index(x->dynamicdsp_parent);
	
	return (x);
}


void ibufplayer_free (t_ibufplayer *x)
{
	dsp_free(&x->x_obj);
	ALIGNED_FREE(x->default_mem);
	freeobject(x->done_clock);
}


void ibufplayer_assist (t_ibufplayer *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
    {
		if (a < x->obj_n_chans)
		{
			switch (a)
			{
				case 0:
					sprintf(s,"(signal) Output 1");
					break;
				case 1:
					sprintf(s,"(signal) Output 2");
					break;
				case 2:
					sprintf(s,"(signal) Output 3");
					break;
				case 3:
					sprintf(s,"(signal) Output 4");
					break;
			}	
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


void ibufplayer_set(t_ibufplayer *x, t_symbol *msg, short argc, t_atom *argv)
{
	ibufplayer_set_internal(x, argc ? atom_getsym(argv) : 0);
}


void ibufplayer_set_internal (t_ibufplayer *x, t_symbol *s)
{
	void *b = ibuffer_get_ptr (s);
	
	if (b) 
	{	
		if (b != x->buffer_pointer)
			x->pos = -1.0;
		x->buffer_pointer = b;
		x->buffer_name = s;
		
	}
	else 
	{
		x->buffer_pointer = 0;
		x->buffer_name = 0;
		if (s)
			error ("ibufplayer~: no buffer %s", s->s_name);
	}
}


void ibufplayer_vols (t_ibufplayer *x,  t_symbol *s, short argc, t_atom *argv)
{
	x->vol1 = atom_getfloat(argv++);
	
	if (argc > 1) 
		x->vol2 = atom_getfloat(argv++);
	
	if (argc > 2) 
		x->vol3 = atom_getfloat(argv++);
	
	if (argc > 3) 
		x->vol4 = atom_getfloat(argv);
}


void ibufplayer_interp (t_ibufplayer *x, t_symbol *s, short argc, t_atom *argv)
{
	t_symbol *mode = argc ? atom_getsym(argv) : ps_bspline;
	
	if (mode == ps_bspline)
		x->interp_mode = INTERP_TYPE_CUBIC_BSPLINE;
	if (mode == ps_hermite)
		x->interp_mode = INTERP_TYPE_CUBIC_HERMITE;
	if (mode == ps_lagrange)
		x->interp_mode = INTERP_TYPE_CUBIC_LAGRANGE;
	
	if (mode != ps_bspline &&  mode != ps_hermite && mode != ps_lagrange)
		error ("ibufplayer~: no interpolation mode %s", mode->s_name);
}


void ibufplayer_stop (t_ibufplayer *x)
{	
	x->transport_flag = FLAG_STOP;
}


void ibufplayer_play (t_ibufplayer *x, t_symbol *s, short argc, t_atom *argv)
{
	double sr_ms = 44.1;
	double start_samp;
	double end_samp;
	double start = 0; 
	double end = -1; 
	double speed = 1;
	long sig_control = 0; 
	
	// Set buffer if there is one named
	
	if (argc && argv[0].a_type == A_SYM) 
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
	
	if (!x->buffer_pointer || !speed)
	{
		ibufplayer_done_bang(x); 
		return;
	}
	
	// Calculate parameters and set the play flag to be picked up in the perform routine
	
	sr_ms = 0.001 * ibuffer_sample_rate (x->buffer_pointer);
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
	
	x->sig_control = sig_control;
	x->transport_flag = FLAG_PLAY;
}


void ibufplayer_done_bang (t_ibufplayer *x)
{
	outlet_bang(x->bang_outlet);
}		

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// Perform and DSP for 32-bit signals ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

#ifdef VECTOR_F64_128BIT

// SIMD phasors for varispeed / fixed speed - these can only be used under intel as there is no double precision floating point type under Altivec

__inline long ibufplayer_phase (double *pos_store, double speed, double start_samp, double min_samp, double max_samp, double length_recip, 
							  char sig_control, vFloat *in,  vFloat *fracts, long *l_offsets, float *f_phase_out, long vec_size)
{
	double phase_test[4];
	double vpos_init[2];
	double pos = *pos_store;
	
	float *f_fracts = (float *) fracts;
	long todo, i;
	
	vDouble vdoublespeed		= {speed + speed, speed + speed};
	vDouble vminsamp			= {min_samp, min_samp};
	vDouble vmaxsamp			= {max_samp, max_samp};
	vDouble vstartsamp			= {start_samp, start_samp};
	vDouble	vlength_recip		= {length_recip, length_recip};
	
	vDouble vpos, vfract1, vfract2, vphase1, vphase2, vmultspeed, vnextmultspeed;
	vSInt32 *offsets = (vSInt32 *) l_offsets;
	vFloat *phase_out = (vFloat *) f_phase_out;
	vFloat *temp_fracts = fracts;
	vFloat vspeedin, vnextspeedin;
	vFloat vspeed = float2vector (speed);
	vSInt32 vipos1, vipos2;
	
	if (sig_control)
	{		
		for (i = 0; i < vec_size >> 2; i++)
			*temp_fracts++ = F32_VEC_MUL_OP(*in++, vspeed);
		
		vpos_init[0] = pos;
		vpos_init[1] = pos + *f_fracts;
		vpos = F64_VEC_ULOAD (vpos_init);
		
		vnextspeedin = *fracts;
		vnextmultspeed = F64_VEC_FROM_F32 (vnextspeedin);
		
		for (i = 0; i < vec_size >> 2; i++)
		{
			// Note - this reads over by one vector but we own the subsequent memory - last vpos calc is incorrect and is discarded
			
			vspeedin =  vnextspeedin;
			vnextspeedin = *(fracts + 1);				
			
			vmultspeed = vnextmultspeed;
			vnextmultspeed = F64_VEC_FROM_F32 (F32_VEC_MOVE_HI (vspeedin, vspeedin));
			
			vmultspeed = F64_VEC_ADD_OP (F64_VEC_SHUFFLE (vmultspeed, vnextmultspeed, 0x1), vmultspeed);
			vipos1 = I32_VEC_FROM_F64_TRUNC (vpos);
			vfract1 = F64_VEC_SUB_OP (vpos, F64_VEC_FROM_I32 (vipos1));
			vphase1 = F64_VEC_MUL_OP (F64_VEC_SUB_OP (vpos, vstartsamp), vlength_recip);
			
			F64_VEC_USTORE (phase_test, F64_VEC_OR_OP (F64_VEC_GT_OP (vpos, vmaxsamp), F64_VEC_LT_OP (vpos, vminsamp)));
			vpos = F64_VEC_ADD_OP (vpos, vmultspeed);			
			
			vmultspeed = vnextmultspeed;
			vnextmultspeed = F64_VEC_FROM_F32 (vnextspeedin);
			
			vmultspeed = F64_VEC_ADD_OP (F64_VEC_SHUFFLE (vmultspeed, vnextmultspeed, 0x1), vmultspeed);
			vipos2 = I32_VEC_FROM_F64_TRUNC (vpos);
			vfract2 = F64_VEC_SUB_OP (vpos, F64_VEC_FROM_I32 (vipos2));
			vphase2 = F64_VEC_MUL_OP (F64_VEC_SUB_OP (vpos, vstartsamp), vlength_recip);
			
			F64_VEC_USTORE (phase_test + 2 , F64_VEC_OR_OP (F64_VEC_GT_OP (vpos, vmaxsamp), F64_VEC_LT_OP (vpos, vminsamp)));
			vpos = F64_VEC_ADD_OP (vpos, vmultspeed);
			
			*offsets++ = I32_VEC_OR_OP (vipos1, I32_VEC_SHUFFLE_OP (vipos2, 0x4E));
			*fracts++ = F32_VEC_MOVE_LO (F32_VEC_FROM_F64 (vfract1), F32_VEC_FROM_F64 (vfract2));
			*phase_out++ = F32_VEC_MOVE_LO (F32_VEC_FROM_F64 (vphase1), F32_VEC_FROM_F64 (vphase2)); 
			
			if (phase_test[0] || phase_test[1] || phase_test[2] || phase_test[3])
				break;
		}
		
		F64_VEC_STORE_LO (pos_store, vpos);
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
		
		vpos = F64_VEC_ULOAD (vpos_init);
		
		for (i = 0; i < (todo + 3) >> 2; i++)
		{
			vipos1 = I32_VEC_FROM_F64_TRUNC (vpos);
			vfract1 = F64_VEC_SUB_OP (vpos, F64_VEC_FROM_I32 (vipos1));
			vphase1 = F64_VEC_MUL_OP (F64_VEC_SUB_OP (vpos, vstartsamp), vlength_recip);
			
			vpos = F64_VEC_ADD_OP (vpos, vdoublespeed);
			
			vipos2 = I32_VEC_FROM_F64_TRUNC (vpos);
			vfract2 = F64_VEC_SUB_OP (vpos, F64_VEC_FROM_I32 (vipos2));
			vphase2 = F64_VEC_MUL_OP (F64_VEC_SUB_OP (vpos, vstartsamp), vlength_recip);
			
			vpos = F64_VEC_ADD_OP (vpos, vdoublespeed);
			
			*offsets++ = I32_VEC_OR_OP (vipos1, I32_VEC_SHUFFLE_OP (vipos2, 0x4E));
			*fracts++ = F32_VEC_MOVE_LO (F32_VEC_FROM_F64 (vfract1), F32_VEC_FROM_F64 (vfract2));
			*phase_out++ = F32_VEC_MOVE_LO (F32_VEC_FROM_F64 (vphase1), F32_VEC_FROM_F64 (vphase2));  
		}
		
		F64_VEC_STORE_LO (pos_store, vpos);
	}
	
	for (i = todo; i < (((todo + 3) >> 2) << 2); i++)
		f_phase_out[i] = 1.f;
		
	return todo;
}	

#else

__inline long ibufplayer_phase (double *pos_store, double speed, double start_samp, double min_samp, double max_samp, double length_recip, 
							  char sig_control, vFloat *in,  vFloat *fracts, long *l_offsets, float *f_phase_out, long vec_size)
{
	return ibufplayer_phase_scalar (pos_store, speed, start_samp, min_samp, max_samp, length_recip, sig_control, (float *) in,  (float *) fracts, l_offsets, f_phase_out, vec_size);
}

#endif


// Scalar phasors for use under ppc and for smaller vector sizes

__inline long ibufplayer_phase_scalar (double *pos_store, double speed, double start_samp, double min_samp, double max_samp, double length_recip, 
									 char sig_control, float *f_in,  float *f_fracts, long *offsets, float *phase_out, long vec_size)
{
	double pos = *pos_store;
	long todo, ipos, i;
	float fract;
	
	if (sig_control)
	{
		for (i = 0; i < vec_size; i++)
		{
			float multspeed = *f_in++ * speed;
			*offsets++ = ipos = pos;
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
			*offsets++ = ipos = pos;
			*f_fracts++ = pos - (double) ipos;
			*phase_out++ = (pos - start_samp) * length_recip;
			pos += speed;
		}
	}
	
	*pos_store = pos;
	return todo;
}	


t_int *ibufplayer_perform (t_int *w)
{		
	// Ignore the copy of this function pointer (due to denormal fixer)
	
	// Set pointers
	
	vFloat *in = (vFloat *) w[2];
	
	float *outs[5];	
	long vec_size = w[8];
    t_ibufplayer *x = (t_ibufplayer *) w[9];
	
	long *offsets = *x->temp_mem;
	float *phase_out;
	vFloat *fracts = (vFloat *) (offsets + vec_size);
	
	void *temp[4];

	// Standard ibuffer variables
	
	void *buffer_pointer = x->buffer_pointer;
	void *samps = 0;
	long length = 0;
	long n_chans = 0;
	long format = -1;
	
	// Object and local variables
	
	double pos = x->pos;
	double speed = x->speed;
	double start_samp = x->start_samp;
	double min_samp = x->min_samp;
	double max_samp = x->max_samp;
	double length_recip = 1.0;	
	double sr_div = x->sr_div;
	
	float vols[4];
	
	long obj_n_chans = x->obj_n_chans;
	long interp_mode = x->interp_mode;
	char sig_control = x->sig_control;
	char input_connected = x->input_connected;
	long ispeed = speed;
	long todo = 0;
	long on = 0;
	long i;

	char playing;

	outs[0] = (float *)(w[3]);
	outs[1] = (float *)(w[4]);
	outs[2] = (float *)(w[5]);
	outs[3] = (float *)(w[6]);
	outs[4] = (float *)(w[7]);

	temp[0] = outs[0];
	temp[1] = (void *) (offsets + (2 * vec_size));
	temp[2] = (void *) (offsets + (3 * vec_size));
	temp[3] = (void *) (offsets + (4 * vec_size));
	
	vols[0] = x->vol1;
	vols[1] = x->vol2;
	vols[2] = x->vol3;
	vols[3] = x->vol4;

	phase_out = outs[4];

	// Zero outputs and set default position output
	
	for (i = 0; i < obj_n_chans; i++)									
		memset (outs[i], 0, vec_size * sizeof(float));
	
	for (i = 0; i < vec_size; i++)									
		outs[4][i] = 1.f;
	
	if (x->x_obj.z_disabled || !x->temp_mem)
		return w + 10;		
	
	// Check on playback state / new play instruction and decide whether to output
	
	if (buffer_pointer && ibuffer_info (buffer_pointer, &samps, &length, &n_chans, &format))
	{
		speed *= ibuffer_sample_rate (buffer_pointer) * sr_div;
		ispeed = speed;
		
		if (!max_samp || max_samp > length) 
			max_samp = length; 
		
		length_recip = (speed < 0) ? -1.0 / (max_samp - min_samp) : 1.0 / (max_samp - min_samp);
		
		if (x->transport_flag == FLAG_PLAY)
			pos = start_samp;
		
		if (x->transport_flag == FLAG_STOP)
			x->pos = pos = -1.0;
		
		if (pos <= max_samp && pos >= min_samp)
		{
			on = 1;
			x->playing = 1;
		}
	}
	
	x->transport_flag = FLAG_NONE;
	
	// Calculate output
	
	if (on)
	{
		ibuffer_increment_inuse (buffer_pointer);
		
		// Calculate phasor block and preprocess for use with the routines below
		
		todo = ibufplayer_phase (&pos, speed, start_samp, min_samp, max_samp, length_recip, sig_control && input_connected, in, fracts, offsets, phase_out, vec_size);
		ibuffer_preprocess_offsets (offsets, vec_size, n_chans, format);
		x->pos = todo < vec_size ? -1.0 : pos;
		
		// Now get samples interpolate as relevant
		
		if (speed - ispeed || sig_control)
		{			
			switch (interp_mode)
			{
				case INTERP_TYPE_CUBIC_BSPLINE:
					for (i = 0; i < obj_n_chans; i++)	
					{																		
						if (vols[i] && n_chans > i) 
						{																				
							temp[0] = outs[i];																							
							ibuffer_float_samps_simd_cubic_bspline(samps, (vFloat *) outs[i], offsets, fracts, temp, todo, n_chans, i, format, vols[i]);				
						}
					}
					break;
					
				case INTERP_TYPE_CUBIC_HERMITE:
					for (i = 0; i < obj_n_chans; i++)	
					{																		
						if (vols[i] && n_chans > i) 
						{																				
							temp[0] = outs[i];																							
							ibuffer_float_samps_simd_cubic_hermite(samps, (vFloat *) outs[i], offsets, fracts, temp, todo, n_chans, i, format, vols[i]);				
						}
					}
					break;
					
				case INTERP_TYPE_CUBIC_LAGRANGE:
					for (i = 0; i < obj_n_chans; i++)	
					{																		
						if (vols[i] && n_chans > i) 
						{																				
							temp[0] = outs[i];																							
							ibuffer_float_samps_simd_cubic_lagrange(samps, (vFloat *) outs[i], offsets, fracts, temp, todo, n_chans, i, format, vols[i]);				
						}
					}
					break;
			}
		}
		else 
		{
			for (i = 0; i < obj_n_chans; i++)
			{
				if (vols[i] && n_chans > i)													
					ibuffer_float_samps_simd_nointerp (samps, (vFloat *) outs[i], offsets, todo, n_chans, i, format, vols[i]);
			}
		}
		
		ibuffer_decrement_inuse (buffer_pointer);
	}
	
	playing = !(todo < vec_size);  
	if (!playing && x->playing) 
		clock_delay (x->done_clock, 0);
	x->playing = playing;
	
	return w + 10;
}


t_int *ibufplayer_perform_small (t_int *w)
{		
	// Ignore the copy of this function pointer (due to denormal fixer)
	
	// Set pointers
	
	float *in = (float *) w[2];
	
	float *outs[5];
	
	long vec_size = w[8];
    t_ibufplayer *x = (t_ibufplayer *) w[9];
	
	long *offsets = *x->temp_mem;
	float *phase_out;
	float *fracts = (float *) (offsets + vec_size);	
	
	// Standard ibuffer variables
	
	void *buffer_pointer = x->buffer_pointer;
	void *samps = 0;
	long length = 0;
	long n_chans = 0;
	long format = -1;
	
	// Object and local variables
	
	double pos = x->pos;
	double speed = x->speed;
	double start_samp = x->start_samp;
	double min_samp = x->min_samp;
	double max_samp = x->max_samp;
	double length_recip = 1.0;	
	double sr_div = x->sr_div;
	
	float vols[4];
	
	long obj_n_chans = x->obj_n_chans;
	long interp_mode = x->interp_mode;
	char sig_control = x->sig_control;
	char input_connected = x->input_connected;
	long ispeed = speed;
	long todo = 0;
	long on = 0;
	long i, j;
	
	char playing;

	vols[0] = x->vol1;
	vols[1] = x->vol2;
	vols[2] = x->vol3;
	vols[3] = x->vol4;
	
	outs[0] = (float *)(w[3]);
	outs[1] = (float *)(w[4]);
	outs[2] = (float *)(w[5]);
	outs[3] = (float *)(w[6]);
	outs[4] = (float *)(w[7]);

	phase_out = outs[4];

	// Zero outputs and set default position output
	
	for (i = 0; i < obj_n_chans; i++)									
		memset (outs[i], 0, vec_size * sizeof(float));
	
	for (i = 0; i < vec_size; i++)									
		outs[4][i] = 1.f;
	
	if (x->x_obj.z_disabled || !x->temp_mem)
		return w + 10;		
	
	// Check on playback state / new play instruction and decide whether to output
	
	if (buffer_pointer && ibuffer_info (buffer_pointer, &samps, &length, &n_chans, &format))
	{
		speed *= ibuffer_sample_rate (buffer_pointer) * sr_div;
		ispeed = speed;
		
		if (max_samp > length) 
			max_samp = length; 
		
		length_recip = (speed < 0) ? -1.0 / (max_samp - min_samp) : 1.0 / (max_samp - min_samp);
		
		if (x->transport_flag == FLAG_PLAY)
			pos = start_samp;
		
		if (x->transport_flag == FLAG_STOP)
			x->pos = pos = -1.0;
		
		if (pos <= max_samp && pos >= min_samp)
		{
			on = 1;
			x->playing = 1;
		}
	}
	
	x->transport_flag = FLAG_NONE;
	
	// Calculate output
	
	if (on)
	{
		ibuffer_increment_inuse (buffer_pointer);
		
		// Calculate the phasor block
		
		todo = ibufplayer_phase_scalar (&pos, speed, start_samp, min_samp, max_samp, length_recip, sig_control && input_connected, in, fracts, offsets, phase_out, vec_size);
		x->pos = todo < vec_size ? -1.0 : pos;
		
		// Now get samples interpolate as relevant
		
		if (speed - ispeed || sig_control)
		{			
			switch (interp_mode)
			{
				case INTERP_TYPE_CUBIC_BSPLINE:
					
					for (j = 0; j < obj_n_chans; j++)	
					{
						if (vols[j] && n_chans > j) 
							ibuffer_float_samps_scalar_cubic_bspline(samps, outs[j], offsets, fracts, todo, n_chans, j, format, vols[j]);
					}
					break;
					
				case INTERP_TYPE_CUBIC_HERMITE:
					
					for (j = 0; j < obj_n_chans; j++)	
					{
						if (vols[j] && n_chans > j) 
							ibuffer_float_samps_scalar_cubic_hermite(samps, outs[j], offsets, fracts, todo, n_chans, j, format, vols[j]);
					}
					break;
					
				case INTERP_TYPE_CUBIC_LAGRANGE:
					
					for (j = 0; j < obj_n_chans; j++)	
					{
						if (vols[j] && n_chans > j) 
							ibuffer_float_samps_scalar_cubic_lagrange(samps, outs[j], offsets, fracts, todo, n_chans, j, format, vols[j]);
					}
					break;					
			}
		}
		else 
		{
			for (j = 0; j < obj_n_chans; j++)	
			{
				if (vols[j] && n_chans > j) 
					ibuffer_float_samps_scalar_nointerp(samps, outs[j], offsets, todo, n_chans, j, format, vols[j]);
			}
		}
		
		ibuffer_decrement_inuse (buffer_pointer);
	}
	
	playing = !(todo < vec_size);  
	if (!playing && x->playing) 
		clock_delay (x->done_clock, 0);
	x->playing = playing;
	
	return w + 10;
}


void ibufplayer_dsp (t_ibufplayer *x, t_signal **sp, short *count)
{
	long mem_size = sp[0]->s_n * (4 * sizeof(float) + sizeof(long));
	x->sr_div = 1.0 / sp[0]->s_sr;
	
	// Try to access dynamicdsp~ host tempory memory
	
	ALIGNED_FREE(x->default_mem);
	
	if (!Dynamic_Temp_Mem_Resize(x->dynamicdsp_parent, x->dynamicdsp_index, mem_size)) 
		x->default_mem = ALIGNED_MALLOC (mem_size);

	x->temp_mem = Dynamic_Get_TempMem (x->dynamicdsp_parent, x->dynamicdsp_index, &x->default_mem);
	
	// Set buffer again in case it is no longer valid / extant
	
	ibufplayer_set_internal(x, x->buffer_name);

	// Check if input is connected
	
	x->input_connected = count[0];
	
	if (sp[0]->s_n >= 4 && SSE2_check())
	{
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
	else 
	{
		switch (x->obj_n_chans)
		{
			case 1:
				dsp_add(denormals_perform, 9, ibufplayer_perform_small, sp[0]->s_vec, sp[1]->s_vec, 0, 0, 0, sp[2]->s_vec, sp[0]->s_n, x);
				break;
			case 2:
				dsp_add(denormals_perform, 9, ibufplayer_perform_small, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, 0, 0, sp[3]->s_vec, sp[0]->s_n, x);
				break;
			case 3:
				dsp_add(denormals_perform, 9, ibufplayer_perform_small, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, 0, sp[4]->s_vec, sp[0]->s_n, x);
				break;
			case 4:
				dsp_add(denormals_perform, 9, ibufplayer_perform_small, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[0]->s_n, x);
				break;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// Perform and DSP for 64-bit signals ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

#ifdef VECTOR_F64_128BIT

// SIMD phasors for varispeed / fixed speed - these can only be used under intel as there is no double precision floating point type under Altivec

__inline long ibufplayer_phase64 (double *pos_store, double speed, double start_samp, double min_samp, double max_samp, double length_recip, 
								char sig_control, double *d_in,  double *d_fracts, long *l_offsets, double *d_phase_out, long vec_size)
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
	
	if (sig_control)
	{		
		vpos_init[0] = pos;
		vpos_init[1] = pos + (*d_in * speed);
		vpos = F64_VEC_ULOAD (vpos_init);
		
		vnextmultspeed = F64_VEC_MUL_OP (*in++, vspeed);
		
		for (i = 0; i < vec_size >> 2; i++)
		{
			// Note - this reads over by one vector but we own the subsequent memory - last vpos calc is incorrect and is discarded
			
			vmultspeed = vnextmultspeed;
			vnextmultspeed = F64_VEC_MUL_OP (*in++, vspeed);				
			vmultspeed = F64_VEC_ADD_OP (F64_VEC_SHUFFLE (vmultspeed, vnextmultspeed, 0x1), vmultspeed);

			vipos1 = I32_VEC_FROM_F64_TRUNC (vpos);
			*fracts++ = F64_VEC_SUB_OP (vpos, F64_VEC_FROM_I32 (vipos1));			
			*phase_out++ = F64_VEC_MUL_OP (F64_VEC_SUB_OP (vpos, vstartsamp), vlength_recip);
			
			F64_VEC_USTORE (phase_test, F64_VEC_OR_OP (F64_VEC_GT_OP (vpos, vmaxsamp), F64_VEC_LT_OP (vpos, vminsamp)));
			vpos = F64_VEC_ADD_OP (vpos, vmultspeed);	
			
			vmultspeed = vnextmultspeed;
			vnextmultspeed = F64_VEC_MUL_OP (*in++, vspeed);
			vmultspeed = F64_VEC_ADD_OP (F64_VEC_SHUFFLE (vmultspeed, vnextmultspeed, 0x1), vmultspeed);
			
			vipos2 = I32_VEC_FROM_F64_TRUNC (vpos);
			*fracts++ = F64_VEC_SUB_OP (vpos, F64_VEC_FROM_I32 (vipos2));			
			*phase_out++ = F64_VEC_MUL_OP (F64_VEC_SUB_OP (vpos, vstartsamp), vlength_recip);
			
			F64_VEC_USTORE (phase_test + 2, F64_VEC_OR_OP (F64_VEC_GT_OP (vpos, vmaxsamp), F64_VEC_LT_OP (vpos, vminsamp)));
			vpos = F64_VEC_ADD_OP (vpos, vmultspeed);	
			
			*offsets++ = I32_VEC_OR_OP (vipos1, I32_VEC_SHUFFLE_OP (vipos2, 0x4E));
			
			if (phase_test[0] || phase_test[1] || phase_test[2] || phase_test[3])
				break;
		}
		
		F64_VEC_STORE_LO (pos_store, vpos);
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
		
		for (i = 0; i < (todo + 3) >> 2; i++)
		{
			vipos1 = I32_VEC_FROM_F64_TRUNC (vpos);
			*fracts++ = F64_VEC_SUB_OP (vpos, F64_VEC_FROM_I32 (vipos1));
			*phase_out++ = F64_VEC_MUL_OP (F64_VEC_SUB_OP (vpos, vstartsamp), vlength_recip);
			vpos = F64_VEC_ADD_OP (vpos, vdoublespeed);
			
			vipos2 = I32_VEC_FROM_F64_TRUNC (vpos);
			*fracts++ = F64_VEC_SUB_OP (vpos, F64_VEC_FROM_I32 (vipos2));
			*phase_out++ = F64_VEC_MUL_OP (F64_VEC_SUB_OP (vpos, vstartsamp), vlength_recip);
			vpos = F64_VEC_ADD_OP (vpos, vdoublespeed);
			
			*offsets++ = I32_VEC_OR_OP (vipos1, I32_VEC_SHUFFLE_OP (vipos2, 0x4E));
		}
		
		F64_VEC_STORE_LO (pos_store, vpos);
	}
	
	for (i = todo; i < (((todo + 3) >> 2) << 2); i++)
		d_phase_out[i] = 1.0;
	
	return todo;
}	

#else

__inline long ibufplayer_phase64 (double *pos_store, double speed, double start_samp, double min_samp, double max_samp, double length_recip, 
							  char sig_control, double *in,  double *fracts, long *l_offsets, double *d_phase_out, long vec_size)
{
	return ibufplayer_phase_scalar64 (pos_store, speed, start_samp, min_samp, max_samp, length_recip, sig_control, in, fracts, l_offsets, d_phase_out, vec_size);
}

#endif


// Scalar phasors for use under ppc and for smaller vector sizes

__inline long ibufplayer_phase_scalar64 (double *pos_store, double speed, double start_samp, double min_samp, double max_samp, double length_recip, 
									   char sig_control, double *f_in,  double *f_fracts, long *offsets, double *phase_out, long vec_size)
{
	double pos = *pos_store;
	long todo, ipos, i;
	double fract;
	
	if (sig_control)
	{
		for (i = 0; i < vec_size; i++)
		{
			double multspeed = *f_in++ * speed;
			*offsets++ = ipos = pos;
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
			*offsets++ = ipos = pos;
			*f_fracts++ = pos - (double) ipos;
			*phase_out++ = (pos - start_samp) * length_recip;
			pos += speed;
		}
	}
	
	*pos_store = pos;
	return todo;
}	

#ifdef VECTOR_F64_128BIT

void ibufplayer_perform64 (t_ibufplayer *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{			
	// Set pointers
	
	double *in = (double *) ins[0];
	double *fracts = (double *) *x->temp_mem;

	long *offsets = (long *) (fracts + vec_size);
	double *phase_out;
	
	void *temp[4];
	
	// Standard ibuffer variables
	
	void *buffer_pointer = x->buffer_pointer;
	void *samps = 0;
	long length = 0;
	long n_chans = 0;
	long format = -1;
	
	// Object and local variables
	
	double pos = x->pos;
	double speed = x->speed;
	double start_samp = x->start_samp;
	double min_samp = x->min_samp;
	double max_samp = x->max_samp;
	double length_recip = 1.0;	
	double sr_div = x->sr_div;
	
	float vols[4];
	
	long obj_n_chans = x->obj_n_chans;
	long interp_mode = x->interp_mode;
	char sig_control = x->sig_control;
	char input_connected = x->input_connected;
	long ispeed = speed;
	long todo = 0;
	long on = 0;
	long i;
	
	char playing;
	    
	temp[0] = NULL;
	temp[1] = (void *) (offsets + vec_size);
	temp[2] = (void *) ((float *) temp[1] + vec_size);
	temp[3] = (void *) ((float *) temp[2] + vec_size);
		
	vols[0] = x->vol1;
	vols[1] = x->vol2;
	vols[2] = x->vol3;
	vols[3] = x->vol4;
	
	phase_out = outs[numouts - 1];
	
	// Zero outputs and set default position output
	
	for (i = 0; i < obj_n_chans; i++)									
		memset (outs[i], 0, vec_size * sizeof(double));
	
	for (i = 0; i < vec_size; i++)									
		phase_out[i] = 1.0;
	
	if (x->x_obj.z_disabled || !x->temp_mem)
		return;		
	
	// Check on playback state / new play instruction and decide whether to output
	
	if (buffer_pointer && ibuffer_info (buffer_pointer, &samps, &length, &n_chans, &format))
	{
		speed *= ibuffer_sample_rate (buffer_pointer) * sr_div;
		ispeed = speed;
		
		if (!max_samp || max_samp > length) 
			max_samp = length; 
		
		length_recip = (speed < 0) ? -1.0 / (max_samp - min_samp) : 1.0 / (max_samp - min_samp);
		
		if (x->transport_flag == FLAG_PLAY)
			pos = start_samp;
		
		if (x->transport_flag == FLAG_STOP)
			x->pos = pos = -1.0;
		
		if (pos <= max_samp && pos >= min_samp)
		{
			on = 1;
			x->playing = 1;
		}
	}
	
    x->transport_flag = FLAG_NONE;
	
	// Calculate output
	
	if (on)
	{
		ibuffer_increment_inuse (buffer_pointer);
		
		// Calculate phasor block and preprocess for use with the routines below
		
		todo = ibufplayer_phase64 (&pos, speed, start_samp, min_samp, max_samp, length_recip, sig_control && input_connected, in, fracts, offsets, phase_out, vec_size);
		ibuffer_preprocess_offsets (offsets, vec_size, n_chans, format);
		x->pos = todo < vec_size ? -1.0 : pos;
		
		// Now get samples interpolate as relevant
				
		if (speed - ispeed || sig_control)
		{			
			switch (interp_mode)
			{
				case INTERP_TYPE_CUBIC_BSPLINE:
					for (i = 0; i < obj_n_chans; i++)	
					{																		
						if (vols[i] && n_chans > i) 
						{																				
							temp[0] = outs[i] + (((todo + 3) >> 2) << 1);																							
							ibuffer_double_samps_simd_cubic_bspline(samps, (vDouble *) outs[i], offsets, (vDouble *) fracts, temp, todo, n_chans, i, format, vols[i]);				
						}
					}
					break;
					
				case INTERP_TYPE_CUBIC_HERMITE:
					for (i = 0; i < obj_n_chans; i++)	
					{																		
						if (vols[i] && n_chans > i) 
						{																				
							temp[0] = outs[i] + (((todo + 3) >> 2) << 1);																							
							ibuffer_double_samps_simd_cubic_hermite(samps, (vDouble *) outs[i], offsets, (vDouble *) fracts, temp, todo, n_chans, i, format, vols[i]);				
						}
					}
					break;
					
				case INTERP_TYPE_CUBIC_LAGRANGE:
					for (i = 0; i < obj_n_chans; i++)	
					{																		
						if (vols[i] && n_chans > i) 
						{																				
							temp[0] = outs[i] + (((todo + 3) >> 2) << 1);																							
							ibuffer_double_samps_simd_cubic_lagrange(samps, (vDouble *) outs[i], offsets, (vDouble *) fracts, temp, todo, n_chans, i, format, vols[i]);				
						}
					}
					break;
			}
		}
		else 
		{
			for (i = 0; i < obj_n_chans; i++)
			{
				if (vols[i] && n_chans > i)													
					ibuffer_double_samps_simd_nointerp (samps, (vDouble *) outs[i], offsets, todo, n_chans, i, format, vols[i]);
			}
		}
		
		ibuffer_decrement_inuse (buffer_pointer);
	}
	
	playing = !(todo < vec_size);  
	if (!playing && x->playing) 
		clock_delay (x->done_clock, 0);
	x->playing = playing;
}

#else

void ibufplayer_perform64 (t_ibufplayer *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
	ibufplayer_perform_small64(x, dsp64, ins, numins, outs, numouts, vec_size, flags, userparam);
}

#endif

void ibufplayer_perform_small64 (t_ibufplayer *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{			
	// Set pointers
	
	double *in = ins[0];
			
	long *offsets = *x->temp_mem;
	double *phase_out;
	double *fracts = (double *) (offsets + vec_size);	
	
	// Standard ibuffer variables
	
	void *buffer_pointer = x->buffer_pointer;
	void *samps = 0;
	long length = 0;
	long n_chans = 0;
	long format = -1;
	
	// Object and local variables
	
	double pos = x->pos;
	double speed = x->speed;
	double start_samp = x->start_samp;
	double min_samp = x->min_samp;
	double max_samp = x->max_samp;
	double length_recip = 1.0;	
	double sr_div = x->sr_div;
	
	float vols[4];
	
	long obj_n_chans = x->obj_n_chans;
	long interp_mode = x->interp_mode;
	char sig_control = x->sig_control;
	char input_connected = x->input_connected;
	long ispeed = speed;
	long todo = 0;
	long on = 0;
	long i, j;
	
	char playing;
	
	vols[0] = x->vol1;
	vols[1] = x->vol2;
	vols[2] = x->vol3;
	vols[3] = x->vol4;
	
	phase_out = outs[numouts - 1];
	
	// Zero outputs and set default position output
	
	for (i = 0; i < obj_n_chans; i++)									
		memset (outs[i], 0, vec_size * sizeof(double));
	
	for (i = 0; i < vec_size; i++)									
		phase_out[i] = 1.0;
	
	if (x->x_obj.z_disabled || !x->temp_mem)
		return;		
	
	// Check on playback state / new play instruction and decide whether to output
	
	if (buffer_pointer && ibuffer_info (buffer_pointer, &samps, &length, &n_chans, &format))
	{
		speed *= ibuffer_sample_rate (buffer_pointer) * sr_div;
		ispeed = speed;
		
		if (max_samp > length) 
			max_samp = length; 
		
		length_recip = (speed < 0) ? -1.0 / (max_samp - min_samp) : 1.0 / (max_samp - min_samp);
		
		if (x->transport_flag == FLAG_PLAY)
			pos = start_samp;
		
		if (x->transport_flag == FLAG_STOP)
			x->pos = pos = -1.0;
		
		if (pos <= max_samp && pos >= min_samp)
		{
			on = 1;
			x->playing = 1;
		}
	}
	
    x->transport_flag = FLAG_NONE;
	
	// Calculate output
	
	if (on)
	{
		ibuffer_increment_inuse (buffer_pointer);
		
		// Calculate the phasor block
		
		todo = ibufplayer_phase_scalar64 (&pos, speed, start_samp, min_samp, max_samp, length_recip, sig_control && input_connected, in, fracts, offsets, phase_out, vec_size);
		x->pos = todo < vec_size ? -1.0 : pos;
		
		// Now get samples interpolate as relevant
		
		if (speed - ispeed || sig_control)
		{			
			switch (interp_mode)
			{
				case INTERP_TYPE_CUBIC_BSPLINE:
					
					for (j = 0; j < obj_n_chans; j++)	
					{
						if (vols[j] && n_chans > j) 
							ibuffer_double_samps_scalar_cubic_bspline (samps, outs[j], offsets, fracts, todo, n_chans, j, format, vols[j]);
					}
					break;
					
				case INTERP_TYPE_CUBIC_HERMITE:
					
					for (j = 0; j < obj_n_chans; j++)	
					{
						if (vols[j] && n_chans > j) 
							ibuffer_double_samps_scalar_cubic_hermite (samps, outs[j], offsets, fracts, todo, n_chans, j, format, vols[j]);
					}
					break;
					
				case INTERP_TYPE_CUBIC_LAGRANGE:
					
					for (j = 0; j < obj_n_chans; j++)	
					{
						if (vols[j] && n_chans > j) 
							ibuffer_double_samps_scalar_cubic_lagrange (samps, outs[j], offsets, fracts, todo, n_chans, j, format, vols[j]);
					}
					break;					
			}
		}
		else 
		{
			for (j = 0; j < obj_n_chans; j++)	
			{
				if (vols[j] && n_chans > j) 
					ibuffer_double_samps_scalar_nointerp(samps, outs[j], offsets, todo, n_chans, j, format, vols[j]);
			}
		}
		
		ibuffer_decrement_inuse (buffer_pointer);
	}
	
	playing = !(todo < vec_size);  
	if (!playing && x->playing) 
		clock_delay (x->done_clock, 0);
	x->playing = playing;
}


void ibufplayer_dsp64 (t_ibufplayer *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	long mem_size = maxvectorsize * (3 * sizeof(float) + sizeof (double) + sizeof(long));
	x->sr_div = 1.0 / samplerate;
	
	// Try to access dynamicdsp~ host tempory memory
	
	ALIGNED_FREE(x->default_mem);
	
	if (!Dynamic_Temp_Mem_Resize(x->dynamicdsp_parent, x->dynamicdsp_index, mem_size)) 
		x->default_mem = ALIGNED_MALLOC (mem_size);
	
	x->temp_mem = Dynamic_Get_TempMem (x->dynamicdsp_parent, x->dynamicdsp_index, &x->default_mem);
	
	// Set buffer again in case it is no longer valid / extant
	
	ibufplayer_set_internal(x, x->buffer_name);
	
	// Check if input is connected
	
	x->input_connected = count[0];
	
	if (maxvectorsize >= 4 && SSE2_check())
		object_method(dsp64, gensym("dsp_add64"), x, ibufplayer_perform64);
	else 
		object_method(dsp64, gensym("dsp_add64"), x, ibufplayer_perform_small64);
}

