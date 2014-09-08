
/*
 *  voicemanager~
 *
 *	voicemanager~ is a (sub)sample-accurate voice manager for use with the voicedrive~ object.
 *
 *	Typically voicemanager~ is located outside of a poly~ (or dynamicdsp~) object within which the voicedrive~ objects are hosted (in each voice).
 *	The trigger input causes a voice to be assigned with the given length and subsample offset (which must be known before the voice starts).
 *	The object keeps track of which voices are free at any one time, assuming the incoming data is passed on correctly to each voice.
 *	Outgoing triggers are then numbered according to voice, and can be tested using an audio rate comparison to derive triggers for individual voices.
 *	See documentation for more info on limitations / implementation.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>


void *this_class;


typedef struct _voicemanager
{
    t_pxobject x_obj;
	
	double *free_times;
	
	double current_time;
	double sr_val;
	
	long max_voices;
	long active_voices;
	long active_connected;
	long reset;
	
} t_voicemanager;


void *voicemanager_new (t_atom_long max_voices);
void voicemanager_free (t_voicemanager *x);
void voicemanager_assist (t_voicemanager *x, void *b, long m, long a, char *s);

void voicemanager_reset (t_voicemanager *x);
void voicemanager_active (t_voicemanager *x, t_symbol *msg, long argc, t_atom *argv);

void voicemanager_dsp (t_voicemanager *x, t_signal **sp, short *count);
t_int *voicemanager_perform (t_int *w);

void voicemanager_perform64 (t_voicemanager *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void voicemanager_dsp64 (t_voicemanager *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

int C74_EXPORT main (void)
{	
	this_class = class_new("voicemanager~",
						   (method)voicemanager_new,
						   (method)voicemanager_free,
						   sizeof(t_voicemanager), 
						   NULL, 
						   A_DEFLONG, 
						   0);
	
	class_addmethod(this_class, (method)voicemanager_reset, "reset", 0);
	class_addmethod(this_class, (method)voicemanager_reset, "active", A_GIMME,0);
	class_addmethod(this_class, (method)voicemanager_assist, "assist", A_CANT, 0);
	class_addmethod(this_class, (method)voicemanager_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)voicemanager_dsp64, "dsp64", A_CANT, 0);
	
	class_dspinit(this_class);
	class_register(CLASS_BOX, this_class);
	
	return 0;
}


void *voicemanager_new(t_atom_long max_voices)
{
    t_voicemanager *x = (t_voicemanager *)object_alloc(this_class);
    
    dsp_setup((t_pxobject *)x, 4);
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");
	
	if (max_voices < 1) 
		max_voices = 1;
    if (max_voices > 1000000000)
		max_voices = 1000000000;
	
	x->free_times = malloc (sizeof(double) * max_voices);
	x->max_voices = max_voices;
	x->active_voices = max_voices;
	x->reset = 1;
	
	return (x);
}


void voicemanager_free(t_voicemanager *x)
{
	dsp_free(&x->x_obj);
	free (x->free_times);
}


void voicemanager_reset (t_voicemanager *x)
{
	x->reset = 1;
}


void voicemanager_active (t_voicemanager *x, t_symbol *msg, long argc, t_atom *argv)
{
	t_atom_long active  = x->max_voices;
	
	if (argc)
		active = atom_getlong(argv);
		
	if (active > x->max_voices)
		active = x->max_voices;
	
	x->active_voices = active;
}


t_int *voicemanager_perform (t_int *w)
{	
	// Set pointers
	
	float *in1 = (float *) w[1];
	float *in2 = (float *) w[2];
	float *in3 = (float *) w[3];
	float *in4 = (float *) w[4];
	float *out1 = (float *) w[5];
	float *out2 = (float *) w[6];
	float *out3 = (float *) w[7];
	long vec_size = w[8];
    t_voicemanager *x = (t_voicemanager *) w[9];
	
	long max_voices = x->max_voices;
	long active_voices = x->active_voices;
	long active_connected = x->active_connected;
	long voice_number;
	long i;
	
	double *free_times = x->free_times;
		
	double length;
	double subsample_offset;
	double current_time = x->current_time;
	double sr_val = x->sr_val;
	
	// Reset the data for all voices
	
	if (x->reset)
	{
		for (i = 0; i < max_voices; i++)
			free_times[i] = current_time;
		
		current_time = 0;
		x->reset = 0;
	}
	
	// Sample loop
	
	while (vec_size--) 
	{
		// Reset variables
		
		voice_number = 0;
		length = 0.;
		subsample_offset = 0.;
		
		active_voices = active_connected ? (long) *in4 : active_voices;
		active_voices  = active_voices > max_voices ? max_voices : active_voices;
		
		if (*in1 && *in3  < 1.)				
		{
			// Search for a free and active voice
			
			for (voice_number = 1; free_times[voice_number - 1] > current_time && voice_number <= active_voices; voice_number++);
			
			if (voice_number <= active_voices)
			{
				// Get data for trigger
				
				length = *in2;
				subsample_offset = *in3;
				
				if (subsample_offset < 0.)
					subsample_offset = 0;
				
				free_times[voice_number - 1] = current_time + (sr_val * length) - subsample_offset;
			}
			else 
				voice_number = 0;
		}
		
		// Advance input pointers 
		
		in1++;
		in2++;
		in3++;
		in4++;
		
		// Output parameters
		
		*out1++ = (float) voice_number;
		*out2++ = (float) length;
		*out3++ = (float) subsample_offset;
		
		current_time += 1.;
	}
	
	x->current_time =current_time;
	
	return w + 10;
}


void voicemanager_dsp(t_voicemanager *x, t_signal **sp, short *count)
{			
	x->sr_val = sp[0]->s_sr / 1000.;
	x->active_connected = count[3];
	
	dsp_add(voicemanager_perform, 9, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec, sp[0]->s_n, x);
}


void voicemanager_perform64 (t_voicemanager *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
	// Set pointers
	
	double *in1 = ins[0];
	double *in2 = ins[1];
	double *in3 = ins[2];
	double *in4 = ins[3];
	double *out1 = outs[0];
	double *out2 = outs[1];
	double *out3 = outs[2];
	
	long max_voices = x->max_voices;
	long active_voices = x->active_voices;
	long active_connected = x->active_connected;
	long voice_number;
	long i;
	
	double *free_times = x->free_times;
		
	double length;
	double subsample_offset;
	double current_time = x->current_time;
	double sr_val = x->sr_val;
	
	// Reset the data for all voices
	
	if (x->reset)
	{
		for (i = 0; i < max_voices; i++)
			free_times[i] = current_time;
		
		current_time = 0;
		x->reset = 0;
	}
	
	// Sample loop
	
	while (vec_size--) 
	{
		// Reset variables
		
		voice_number = 0;
		length = 0.;
		subsample_offset = 0.;
		
		active_voices = active_connected ? (long) *in4 : active_voices;
		active_voices  = active_voices > max_voices ? max_voices : active_voices;
		
		if (*in1 && *in3  < 1.)				
		{
			// Search for a free and active voice
			
			for (voice_number = 1; free_times[voice_number - 1] > current_time && voice_number <= active_voices; voice_number++);
			
			if (voice_number <= active_voices)
			{
				// Get data for trigger
				
				length = *in2;
				subsample_offset = *in3;
				
				if (subsample_offset < 0.)
					subsample_offset = 0;
				
				free_times[voice_number - 1] = current_time + (sr_val * length) - subsample_offset;
			}
			else 
				voice_number = 0;
		}
		
		// Advance input pointers 
		
		in1++;
		in2++;
		in3++;
		in4++;
		
		// Output parameters
		
		*out1++ = (float) voice_number;
		*out2++ = (float) length;
		*out3++ = (float) subsample_offset;
		
		current_time += 1.;
	}
	
	x->current_time =current_time;
}


void voicemanager_dsp64 (t_voicemanager *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{			
	x->sr_val = samplerate / 1000.;
	x->active_connected = count[3];
	
	object_method(dsp64, gensym("dsp_add64"), x, voicemanager_perform64, 0, NULL);
}


void voicemanager_assist(t_voicemanager *x, void *b, long m, long a, char *s)
{
	
	switch (a)
	{
		case 0:
			sprintf(s,"(signal) Triggers");
			break;
			
		case 1:
			sprintf(s,"(signal) Lengths");
			break;
			
		case 2:
			sprintf(s,"(signal) Subsample Offset");
			break;
			
		case 3:
			sprintf(s,"(signal) Number Active Voices");
			break;
	}
}


