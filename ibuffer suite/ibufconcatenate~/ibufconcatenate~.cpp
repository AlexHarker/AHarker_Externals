
/*
 *  ibufconcatenate~
 *
 *	The ibufconcatenate~ object is used to handle data to do with sections of a buffer. 
 *
 *	It can be used in one of two ways:
 *
 *		1 - entries - store start and end points for items in a buffer by entering them manually.
 *		2 - concatenation - concatenate multiple msp buffers into one buffer, storing start and end points.
 *
 *	The bounds of each chunk can be retrieved either as a message, or as a signal in a sample-accurate manner, according to the mode of the object.
 *	The ibufconcatedrive~ object can also be used to give sample-accurate playback of only one item at a time.
 *
 *	See the helpfile documentation for more on how this object can be used in practice.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <algorithm>

#include <ibuffer_access.hpp>
#include "ibufconcatenate_info~.h"


t_class *this_class;


#define GROW_SIZE 1000000


typedef struct _ibufconcatenate
{
    t_pxobject x_obj;
	
	t_ibufconcatenate_info *attachment;
	
	long max_mode;
	
	void *data_out;
	void *last_added_out;

} t_ibufconcatenate;


void *ibufconcatenate_new (t_symbol *buffer_name, long max_mode);
void ibufconcatenate_free (t_ibufconcatenate *x);
void ibufconcatenate_assist (t_ibufconcatenate *x, void *b, long m, long a, char *s);

void ibufconcatenate_set(t_ibufconcatenate *x, t_symbol *buffer_name);
void ibufconcatenate_clear (t_ibufconcatenate *x);
void ibufconcatenate_append (t_ibufconcatenate *x, t_symbol *source_name);
void ibufconcatenate_entry (t_ibufconcatenate *x, double start, double end);

static __inline long mstosamps (double ms, double sr);
void ibufconcatenate_int(t_ibufconcatenate *x, long in);

t_int *ibufconcatenate_perform (t_int *w);
void ibufconcatenate_perform64 (t_ibufconcatenate *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

void ibufconcatenate_dsp(t_ibufconcatenate *x, t_signal **sp, short *count);
void ibufconcatenate_dsp64(t_ibufconcatenate *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);


int C74_EXPORT main()
{
    this_class = class_new ("ibufconcatenate~",
							(method) ibufconcatenate_new, 
							(method)ibufconcatenate_free, 
							sizeof(t_ibufconcatenate), 
							0L, 
							A_SYM, 
							A_DEFLONG,
							0);
	
	class_addmethod (this_class, (method)ibufconcatenate_int, "int", A_LONG, 0L);
	class_addmethod (this_class, (method)ibufconcatenate_set, "set", A_SYM, 0L);
	class_addmethod (this_class, (method)ibufconcatenate_clear, "clear", 0L);
	class_addmethod (this_class, (method)ibufconcatenate_append, "append", A_SYM, 0L);
    class_addmethod (this_class, (method)ibufconcatenate_entry, "entry", A_DEFFLOAT, A_DEFFLOAT, 0L);
	class_addmethod (this_class, (method)ibufconcatenate_assist, "assist", A_CANT, 0L);
	class_addmethod(this_class, (method)ibufconcatenate_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)ibufconcatenate_dsp64, "dsp64", A_CANT, 0);
	
	class_register(CLASS_BOX, this_class);
	
	class_dspinit(this_class);
	
	ibuffer_init ();
		
	return 0;
}


void *ibufconcatenate_new (t_symbol *buffer_name, long max_mode)
{
    t_ibufconcatenate *x = (t_ibufconcatenate *)object_alloc (this_class);
	
	x->last_added_out = listout(x);

	if (!max_mode)
	{
		dsp_setup((t_pxobject *)x, 1);
		outlet_new((t_object *)x, "signal");
		outlet_new((t_object *)x, "signal");
		outlet_new((t_object *)x, "signal");
		outlet_new((t_object *)x, "signal");
	}
	else
	{
		x->data_out = listout(x);
	}
	
	x->attachment = new_ibufconcatenate_info(buffer_name);
	
	if (!x->attachment)
		return 0;
	
	x->max_mode = max_mode;
	
	return(x);
}


void ibufconcatenate_free(t_ibufconcatenate *x)
{
	if (!x->max_mode)
		dsp_free(&x->x_obj);
	detach_ibufconcatenate_info(x->attachment);
}


void ibufconcatenate_assist (t_ibufconcatenate *x, void *b, long m, long a, char *s)
{
	if (!x->max_mode)
	{
		if (m == ASSIST_OUTLET)
		{
			switch (a)
			{
				case 0:
					sprintf(s,"(signal) Start (ms)");
					break;
					
				case 1:
					sprintf(s,"(signal) Start High Resolution (ms)");
					break;
					
				case 2:
					sprintf(s,"(signal) End (ms)");
					break;
					
				case 3:
					sprintf(s,"(signal) End High Resolution (ms)");
					break;
					
				case 4:
					sprintf(s,"Last Item Added");
					break;
			}
		}
		else 
		{
			sprintf(s,"(signal) Item / Entries / Concatenation Instructions");
		}
	}
	else 
	{
		if (m == ASSIST_OUTLET)		
		{
			switch (a)
			{
				case 0:
					sprintf(s,"Item Info Out");
					break;
					
				case 1:
					sprintf(s,"Last Item Added");
					break;
			}
		}
		else 
			sprintf(s,"Entries / Concatenation Instructions / Item (int)");
	}
}


void ibufconcatenate_set(t_ibufconcatenate *x, t_symbol *buffer_name)
{
	detach_ibufconcatenate_info(x->attachment);
	x->attachment = new_ibufconcatenate_info(buffer_name);
}


void ibufconcatenate_clear (t_ibufconcatenate *x)
{	
	x->attachment->num_items = 0;
	x->attachment->samp_offset = 0;
}


void ibufconcatenate_append(t_ibufconcatenate *x, t_symbol *source_name)
{
    ibuffer_data target(x->attachment->buffer_name);
    ibuffer_data source(source_name);
	
	float *out_samps;
	
	double *starts = x->attachment->starts;
	double *ends = x->attachment->ends;
	
	long samp_offset = x->attachment->samp_offset;
	long num_items = x->attachment->num_items;
	long new_size;
	long old_size;
	
	double sr_const;
		
	t_atom last_added_list[3];

	if (target.get_type() == kBufferMaxBuffer)
	{
		// Check we have a mono buffer and that there is space for another item
		
		if (target.get_num_chans() > 1)
		{
			error ("ibufconcatenate: only supports writing to mono buffers at this time");
			return;
		}
		
		if (num_items >= MAX_ITEMS)
			return;

		// Check we have enough memory in the buffer
		
		if (samp_offset + source.get_length() + 1 > target.get_length())
		{
			// Calculate memory to allocate
			
			float *temp;

			new_size = samp_offset + source.get_length() + 1;
			old_size = target.get_length();
			
			if (old_size + GROW_SIZE > new_size)
				new_size = old_size + GROW_SIZE;
			
			// Allocate temporary memory
			
			temp = static_cast<float*>(malloc(sizeof(float) * target.get_length()));
			
			if (!temp)
			{
				error ("ibufconcatenate: no room left in buffer");
				return;
			}

			// Copy out
            
            out_samps = static_cast<float *>(target.get_samples());
            std::copy(out_samps, out_samps + old_size, temp);
			
			// Set buffer to new size
			
            target.set_size_in_samples(new_size);
			
            out_samps = static_cast<float *>(target.get_samples());
			if (target.get_length());
                std::copy(temp, temp + old_size, out_samps);
			
			// Free temporary memory and check the resize has worked 
			
			free(temp);

            if (samp_offset + source.get_length() + 1 > target.get_length())
			{
				error ("ibufconcatenate: no room left in buffer");
				return;
			}
		}
		
		out_samps = static_cast<float *>(target.get_samples());
		
		// Get samples from the source - this is constrained to only msp buffers do to unknown pointer alignment in the target (with the offset)
		
        ibuffer_get_samps(source, out_samps, samp_offset, source.get_length(), 0);
		
		// Add a silent sample
		
		*(out_samps + samp_offset + source.get_length()) = 0.f;
		
		// Store data
		
		sr_const = 1000.0 / target.get_sample_rate();
		starts[num_items] = (double) samp_offset * sr_const;
		samp_offset = samp_offset + source.get_length() + 1;
		ends[num_items] = (double) (samp_offset - 1) * sr_const; 
		
		x->attachment->num_items = num_items + 1;
		x->attachment->samp_offset = samp_offset;
		
		// Set the buffer as dirty
		
        target.set_dirty();
		
		// We are done with the buffers
        
        target.release();
        source.release();
        
		// Output Values
		
		atom_setlong(last_added_list, num_items + 1);
		atom_setfloat(last_added_list+1, starts[num_items]);
		atom_setfloat(last_added_list+2, ends[num_items]);
		
		outlet_list(x->last_added_out, 0, 3, last_added_list);
	}
}

double ibuffer_sample_rate(t_symbol *name)
{
    ibuffer_data buffer(name);
    
    if (buffer.get_sample_rate())
        return buffer.get_sample_rate();
    else
        return 44100.0;
}

void ibufconcatenate_entry(t_ibufconcatenate *x, double start, double end)
{
	// Default sr to something sensible
	
    double sr = ibuffer_sample_rate(x->attachment->buffer_name);
	
	double *starts = x->attachment->starts;
	double *ends = x->attachment->ends;
	
	long samp_offset = x->attachment->samp_offset;
	long num_items = x->attachment->num_items;
	
	t_atom last_added_list[3];

	if (num_items >= MAX_ITEMS)
		return;
    
	// Store values
	
	starts[num_items] = start;
	ends[num_items] = end; 
		
	if ((long) (end * sr / 1000.0) > samp_offset)
		samp_offset = end * sr / 1000.0;
		
	x->attachment->num_items = num_items + 1;
	x->attachment->samp_offset = samp_offset;
	
	// Output
	
	atom_setlong(last_added_list, num_items + 1);
	atom_setfloat(last_added_list+1, starts[num_items]);
	atom_setfloat(last_added_list+2, ends[num_items]);
	
	outlet_list(x->last_added_out, 0, 3, last_added_list);
}


static __inline long mstosamps (double ms, double sr)
{
#ifdef __APPLE__ 
	return (long) round((ms * sr) / 1000.);
#else
	return (long) (((ms * sr) / 1000.) + 0.5);
#endif
}


double ibuffer_full_length(t_symbol *name)
{
    ibuffer_data buffer(name);
    
    if (buffer.get_length())
        return (buffer.get_length() / buffer.get_sample_rate()) * 1000.0;
    else
        return 0.0;
}


void ibufconcatenate_int(t_ibufconcatenate *x, long item)
{
	double *starts = x->attachment->starts;
	double *ends = x->attachment->ends;
	
	double start;
	double end;
	double full_length = ibuffer_full_length(x->attachment->buffer_name);
	double sr = 0;
    
    long num_items = x->attachment->num_items;
    
	t_atom atom_out[3];

	if (!x->max_mode)
		return;

	if (item < 1 || item > num_items)
	{
		// Default Output Values
		
		start = 0.; 
		
		if (item != -1)
			end = 0.;
		else
			end = full_length;
	}
	else 
	{	
		// Chunk Output Values
		
		start = starts[item - 1];
		end = ends[item - 1];
	}	
	
	atom_setlong(atom_out+0, mstosamps(start, sr));
	atom_setlong(atom_out+1, mstosamps(end, sr));
	atom_setfloat(atom_out+2, sr);
	
	outlet_list(x->data_out, 0, 3, atom_out);
}


t_int *ibufconcatenate_perform(t_int *w)
{	
	// Set pointers
	
	float *in = (float *) w[1];
	float *start_lo_res = (float *) w[2];
	float *start_hi_res = (float *) w[3];
	float *end_lo_res = (float *) w[4];
	float *end_hi_res = (float *) w[5];
	long vec_size = (long) w[6];
	t_ibufconcatenate *x = (t_ibufconcatenate *) w[7];
		
	double *starts = x->attachment->starts;
	double *ends = x->attachment->ends;
    double full_length = ibuffer_full_length(x->attachment->buffer_name);
	double start;
	double end;
	
	float ftemp;
		
    long num_items = x->attachment->num_items;
    
	while (vec_size--)
	{
		// Get item
		
		long item = (long) *in++;
		
		if (item < 1 || item > num_items)
		{
			// Default Output Values
			
			*start_lo_res++ = 0.f;
			*start_hi_res++ = 0.f; 
			
			if (item != -1)
			{	
				*end_lo_res++ = 0.f;
				*end_hi_res++ = 0.f;
			}
			else
			{
				*end_lo_res++ = ftemp = full_length;
				*end_hi_res++ = full_length - ftemp;
			}
		}
		else 
		{	
			// Chunk Output Values
			
			start = starts[item - 1];
			end = ends[item - 1];
			*start_lo_res++ = ftemp = start;
			*start_hi_res++ = start - ftemp;
			*end_lo_res++ = ftemp = end;
			*end_hi_res++ = end - ftemp;
		}	
	}
	
	return w + 8;
}


void ibufconcatenate_perform64(t_ibufconcatenate *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
	// Set pointers
	
	double *in = ins[0];
	double *start_lo_res = outs[0];
	double *start_hi_res = outs[1];
	double *end_lo_res = outs[2];
	double *end_hi_res = outs[3];
		
	double *starts = x->attachment->starts;
	double *ends = x->attachment->ends;
    double full_length = ibuffer_full_length(x->attachment->buffer_name);
	double start;
	double end;
    
	long num_items = x->attachment->num_items;
	
	while (vec_size--)
	{
		// Get item
		
		long item = (long) *in++;
		
		if (item < 1 || item > num_items)
		{
			// Default Output Values
			
			*start_lo_res++ = 0.f;
			*start_hi_res++ = 0.f; 
			
			if (item != -1)
			{	
				*end_lo_res++ = 0.f;
				*end_hi_res++ = 0.f;
			}
			else
			{
				*end_lo_res++ = full_length;
				*end_hi_res++ = 0.;
			}
		}
		else 
		{	
			// Chunk Output Values
			
			start = starts[item - 1];
			end = ends[item - 1];
			*start_lo_res++ = start;
			*start_hi_res++ = 0.0;
			*end_lo_res++ = end;
			*end_hi_res++ = 0.0;
		}	
	}
}


void ibufconcatenate_dsp(t_ibufconcatenate *x, t_signal **sp, short *count)
{				
	if (!x->max_mode)
		dsp_add(ibufconcatenate_perform, 7, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[0]->s_n, x);
}


void ibufconcatenate_dsp64(t_ibufconcatenate *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	if (!x->max_mode)
		object_method(dsp64, gensym("dsp_add64"), x, ibufconcatenate_perform64, 0, NULL);
}

