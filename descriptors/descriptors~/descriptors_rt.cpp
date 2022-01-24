
/*
 *  descriptorsrt~
 *
 *	descriptorsrt~ can be used to calculate a large number of audio descriptors (or features) in real-time from an incoming signal and output the results as a list.
 *	descriptorsrt~ is intended as a comprehensive analysis solution, being suitable for a wide range of tracking / following / detection applications.
 *	It is the real-time counterpart to the descriptors~ object - the two objects are very similar in terms of features and usage.
 *
 *	The object only calculates and outputs the descriptors that the user requests (these can be changed in realtime).
 *	The object is designed to be as efficient as possible, avoiding unnecessary calculations and re-calculations wherever possible and making extensive use of SIMD operations.
 *
 *	For in-depth details on usage of the descriptorsrt~ object see the helpfile documentation.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include "descriptors_object.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// Object Basics (main / new / free) /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int C74_EXPORT main()
{
	this_class = class_new("descriptorsrt~",
                           (method) descriptors_new,
                           (method) descriptors_free,
                           sizeof(t_descriptors),
                           0L,
                           A_GIMME,
                           0);
	
	class_addmethod(this_class, (method) descriptors_descriptors_rt, "descriptors", A_GIMME, 0);
	
	descriptors_main_common();
	
	class_register(CLASS_BOX, this_class);
	
	return 0;
}


void *descriptors_new (t_symbol *s, short argc, t_atom *argv)
{
    t_descriptors *x = (t_descriptors *) object_alloc(this_class);
	
	long descriptor_feedback = 0;
	long max_fft_size_log2;
	long max_fft_size_in = 0;
	long max_fft_size;
	
	void *allocated_memory;

	if (!x)
		return 0;

	// Get arguments 
	
	if (argc) 
		max_fft_size_in = atom_getlong(argv++);
	if (argc > 1) 
		descriptor_feedback = atom_getlong(argv++);
	
	// Set maximum fft size
	
	max_fft_size_log2 = descriptors_max_fft_size(x, max_fft_size_in);
	x->max_fft_size_log2 = max_fft_size_log2;
	x->max_fft_size = max_fft_size = 1 << (max_fft_size_log2);
	
	dsp_setup((t_pxobject *) x, 1);
		
	// Allocate memory
	
	 allocated_memory = ALIGNED_MALLOC(
		(max_fft_size * 2 * sizeof(float)) + (max_fft_size * sizeof(char)) + (max_fft_size * sizeof(float)) + ((max_fft_size * 3) * sizeof(float)) + ((max_fft_size >> 1) * 3 * RING_BUFFER_SIZE * sizeof(float)) 
		+ (max_fft_size * 3 * sizeof(float)) + (max_fft_size * (sizeof(float) + sizeof(long))) + (max_fft_size * RING_BUFFER_SIZE * sizeof(double)) + ((max_fft_size >> 1) * sizeof(double)) 
	 + ((max_fft_size >> 1) * sizeof(double)) + (MAX_OUTPUT * sizeof(t_atom)));
	
	if (!allocated_memory)
	{
		post ("descriptors(rt)~: couldn't allocate memory");
		return (0);
	}
	
	// Assign pointers
	// N.B. x->window must be the first allocation - this is the pointer that is freed.
	
	x->window = (float *) allocated_memory;
	allocated_memory = (void *) ((float *) allocated_memory + max_fft_size);	
	
	x->fft_memory = allocated_memory;
	allocated_memory = (void *) ((float *) allocated_memory + (max_fft_size * 3));
	
	x->amps_buffer = (float *) allocated_memory;
	allocated_memory = (void *) ((float *) allocated_memory + ((max_fft_size >> 1) * 3 * RING_BUFFER_SIZE));
	
	x->ac_memory = allocated_memory;
	allocated_memory = (void *) ((float *) allocated_memory + (max_fft_size * 3));

	x->n_data = allocated_memory;
	allocated_memory = (void *) ((char *) ((float *) allocated_memory + max_fft_size) + (max_fft_size >> 1));
	
	x->median_memory = allocated_memory;
	allocated_memory = (void *) ((long *) ((float *) allocated_memory + max_fft_size) + max_fft_size);
	
	x->cumulate = (double *) allocated_memory;
	allocated_memory = (void *) ((double *) allocated_memory + (max_fft_size * RING_BUFFER_SIZE));
	
	x->loudness_curve =  (double *) allocated_memory;
	allocated_memory = (void *) ((double *) allocated_memory + (max_fft_size >> 1));
	
	x->log_freq =  (double *) allocated_memory;
	allocated_memory = (void *) ((double *) allocated_memory + (max_fft_size >> 1));
	
	x->output_list = (t_atom *) allocated_memory;
	x->summed_amplitudes = 0;

	// Allocate a clock and call the common new routine
	
	x->output_rt_clock = clock_new(x, (method) output_rt);
	
	descriptors_new_common (x, max_fft_size_log2, descriptor_feedback);
	
	return x;
}


void descriptors_free(t_descriptors *x)
{
	dsp_free(&x->x_obj);
	ALIGNED_FREE (x->window);
	ALIGNED_FREE (x->rt_buffer);
	hisstools_destroy_setup(x->fft_setup_real);
	if (x->output_rt_clock) 
		freeobject((t_object *) x->output_rt_clock);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////// Handle RT Descriptor Calculation /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void calc_descriptors_rt (t_descriptors *x, float *samples)
{
	t_atom *output_list = x->output_list;
	
	// FFT Variables
	
	char frame_pointer = x->frame_pointer;
	
	long fft_size = x->fft_size;
	long fft_size_halved = fft_size >> 1;
	long fft_size_log2 = x->fft_size_log2;
	long window_size = x->window_size;
	
	FFT_SETUP_F fft_setup_real = x->fft_setup_real;
	
	float *raw_frame = (float *) x->fft_memory;
	float *windowed_frame = raw_frame + fft_size;
	
	FFT_SPLIT_COMPLEX_F raw_fft_frame;
	
	float *window = x->window;
	float *this_frame = x->amps_buffer + (3 * fft_size_halved * frame_pointer);
	float *sq_amplitudes = this_frame;
	float *amplitudes = sq_amplitudes + fft_size_halved;
	float *log_amplitudes = amplitudes + fft_size_halved;
	float *freqs = (float *) x->n_data;
	float *amps = freqs + fft_size_halved;
	
	vFloat *v_window = (vFloat *) window;
	vFloat *v_sq_amplitudes = (vFloat *) sq_amplitudes;
#if (defined F32_VEC_LOG_OP || defined F32_VEC_LOG_ARRAY)
	vFloat *v_log_amplitudes = (vFloat *) log_amplitudes;
#endif
	vFloat *v_raw_frame = (vFloat *) raw_frame;
	vFloat *v_windowed_frame = (vFloat *) windowed_frame;
	vFloat *real_data;
	vFloat *imag_data;
	
#if (defined F32_VEC_LOG_OP)
	vFloat v_half = {0.5, 0.5, 0.5, 0.5};
#endif
#if (defined F32_VEC_LOG_OP || defined F32_VEC_LOG_ARRAY)
	vFloat v_pow_min = {POW_MIN, POW_MIN, POW_MIN, POW_MIN};
#endif
	
	// Descriptor Variables
	
	long num_pf_descriptors = x->num_pf_descriptors;
	double *pf_params = x->pf_params;
	long n_count = 0;
	long i, j;
	
	double *cumulate_amps = x->cumulate + (fft_size * frame_pointer);
	double *cumulate_sq_amps = cumulate_amps + fft_size_halved;
	double sum;

	x->frame_pointer = (frame_pointer + 1) % RING_BUFFER_SIZE;
	
	raw_fft_frame.realp = windowed_frame + fft_size;
	raw_fft_frame.imagp = raw_fft_frame.realp + fft_size_halved;
	
	real_data = (vFloat *) raw_fft_frame.realp;
	imag_data = (vFloat *) raw_fft_frame.imagp;

	if (!num_pf_descriptors) 
		return;
	
	// Reset flags

	x->ac_flag = 0;
	x->median_flag = 0;
	x->centroid_lin_flag = 0;
	x->centroid_log_flag = 0;
	x->last_pf_spectralpeaks_n = 0;
	x->last_pf_spectralpeaks_med_size = 0;
	x->last_threshold = DBL_MAX;
	
    // FIX - zero-padding may have been previously broken
    
	// Copy samples to raw frame
	
	for (i = 0; i < window_size; i++)
		raw_frame[i] = samples[i];
	
	// Zero Pad to fft_size
	
	for (i = window_size; i < fft_size; i++)
		raw_frame[i] = 0;
	
	// Apply window
	
	for (i = 0; i < window_size >> 2; i++)
		v_windowed_frame[i] = F32_VEC_MUL_OP(v_raw_frame[i], v_window[i]);
	for (i <<= 2; i < window_size; i++)
		windowed_frame[i] = raw_frame[i] * window[i];
	
	// Do fft straight into position (with zero-padding)
	
    hisstools_rfft(fft_setup_real, windowed_frame, &raw_fft_frame, window_size, fft_size_log2);
    
	// Discard The nyquist bin (if necessary add this back later)
	
	raw_fft_frame.imagp[0] = 0.;
	
	// Calulate Square amplitudes (leaving the raw data also in case phase is needed for some descriptor added later)
	
	for (i = 0; i < fft_size_halved >> 2; i++)
		v_sq_amplitudes[i] = F32_VEC_ADD_OP(F32_VEC_MUL_OP(real_data[i], real_data[i]), F32_VEC_MUL_OP(imag_data[i], imag_data[i]));
		
	// Calculate log amplitudes
	
#ifdef F32_VEC_LOG_OP
	for (i = 0; i < fft_size_halved >> 2; i++)
		v_log_amplitudes[i] = F32_VEC_MUL_OP(F32_VEC_LOG_OP(F32_VEC_MAX_OP(v_sq_amplitudes[i], v_pow_min)), v_half);
#else
#ifdef F32_VEC_LOG_ARRAY
	for (i = 0; i < fft_size_halved >> 2; i++)
		v_log_amplitudes[i] = F32_VEC_SQRT_OP(F32_VEC_MAX_OP(v_sq_amplitudes[i], v_pow_min));
	F32_VEC_LOG_ARRAY(log_amplitudes, log_amplitudes, fft_size_halved);
#else
	for (i = 0; i < fft_size_halved; i++)
		log_amplitudes[i] = log((sq_amplitudes[i] < POW_MIN) ? POW_MIN : sq_amplitudes[i]) * 0.5;
#endif
#endif
	
	// Now sqrt amplitudes
	
	for (i = 0; i < fft_size_halved; i++)
		amplitudes[i] = sqrt(sq_amplitudes[i]);
		
	// Cumulate amps and square amps
		
	for (sum = 0., i = 0; i < fft_size_halved; i++)
	{
		sum += amplitudes[i];
		cumulate_amps[i] = sum;
	}
		
	for (sum = 0., i = 0; i < fft_size_halved; i++)
	{
		sum += sq_amplitudes[i];
		cumulate_sq_amps[i] = sum;
	}
		
	////////////////////////////////////////////// Calculate And Store Per Frame descriptors //////////////////////////////////////////
	
	if (!x->use_energy_thresh || cumulate_sq_amps[fft_size_halved - 1] > x->energy_thresh)
	{
		for (i = 0; i < num_pf_descriptors; i++)
		{
			double *current_param_ptr = pf_params;
			double descriptor = calc_pf_descriptor(x, raw_frame, windowed_frame, raw_fft_frame, frame_pointer, window_size, fft_size, &pf_params);
			
			if ((long) current_param_ptr[0] != DESCRIPTOR_PF_SPECTRAL_PEAKS)
				atom_setfloat(output_list + i + n_count, descriptor);
			else
			{
				long N = (long) current_param_ptr[1];
				
				for (j = 0; j < N; j ++)
				{
					atom_setfloat(output_list + i + n_count, freqs[j]); n_count++;
					atom_setfloat(output_list + i + n_count, amps[j]); n_count++;
				}
				
				// One descriptor is expected so we need to decrement by one here
				
				n_count--;												
			}
		}
	}
	else 
	{
		for (i = 0; i < x->output_length; i++)
			atom_setfloat(output_list + i, FLT_MAX);
	}
	
	// Output - call A clock To do this!
	
	clock_delay (x->output_rt_clock, 0);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// RT Output Perform and DSP Routines ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	


void output_rt (t_descriptors *x)
{
	outlet_list (x->the_list_outlet, nullptr, x->output_length, x->output_list);
}


t_int *descriptors_perform(t_int *w) 
{
	float *in1 = (float *) w[1];
	t_descriptors *x = (t_descriptors *) w[2];
	long vec_size = w[3];
	
	float *rt_buffer = x->rt_buffer;
	
	long bufffer_size = x->rt_memory_size;
	long window_size = x->window_size;
	long hop_count = x->hop_count;
	long hop_size = x->hop_size;
	long write_pointer = x->write_pointer;
	long block_pointer = write_pointer - window_size + (bufffer_size >> 1);
	long frametime = 0;
	long hop_to_do;
	long i;
	
	// FFT reset
	
	if (x->reset_fft)
	{
		write_pointer = 0;
		hop_count = hop_size;
		x->reset_fft = 0;
	}
	
	// Write input block into double circular buffer
	
	for (i = 0; i < vec_size; i++)
	{
		rt_buffer[write_pointer + (bufffer_size >> 1)] = rt_buffer[write_pointer] = in1[i];
		write_pointer = (write_pointer + 1) % (bufffer_size >> 1); 
	}
	
	// Check for fft due and perform calculations
	
	while (vec_size)
	{
		hop_to_do = (vec_size < hop_size) ? vec_size : hop_size;
		hop_count -= hop_to_do;
		vec_size -= hop_to_do;
		frametime += hop_to_do;
		
		if (hop_count <= 0)
		{
			calc_descriptors_rt (x, rt_buffer + ((block_pointer + frametime + hop_count) % (bufffer_size >> 1)));
			hop_count += hop_size;
		}
	}
		
	// Store variables
	
	x->write_pointer = write_pointer;
	x->hop_count = hop_count;
	
	return w + 4;
}


void descriptors_dsp (t_descriptors *x, t_signal **sp, short *count)
{
	float *samples;
	
	long rt_memory_size = x->rt_memory_size;
	long num_samps = x->max_fft_size;
	long mem_required = 2 * (num_samps + sp[0]->s_n);
	long i;
	
	// Allocate memory if none (or not enough) has been allocated
	
	if (rt_memory_size < mem_required)
	{
		ALIGNED_FREE (x->rt_buffer);
		x->rt_buffer = (float *) ALIGNED_MALLOC (mem_required * sizeof (float));
		x->rt_memory_size = mem_required;
	}
		
	// Set variables
	
	x->write_pointer = 0;
	x->sr = sp[0]->s_sr;

	// If memory is correctly allocated then zero the sample buffer and add the dp routine
	
	if (x->rt_buffer) 
	{
		samples = x->rt_buffer;
	
		for (i = 0; i < mem_required; i++)
			samples[i] = 0.0;
	
		dsp_add ((t_perfroutine)descriptors_perform, 3, sp[0]->s_vec, x, sp[0]->s_n);
	}
	else 
		x->rt_memory_size = 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////// RT 64bit Perform and DSP Routines ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	


// It is extravagant to calculate desciptors at 64bit, so for now we simply discard the extra bits at the front end (later this may change)


void descriptors_perform64 (t_descriptors *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
	double *in1 = ins[0];
	
	float *rt_buffer = x->rt_buffer;
	
	long bufffer_size = x->rt_memory_size;
	long window_size = x->window_size;
	long hop_count = x->hop_count;
	long hop_size = x->hop_size;
	long write_pointer = x->write_pointer;
	long block_pointer = write_pointer - window_size + (bufffer_size >> 1);
	long frametime = 0;
	long hop_to_do;
	long i;
	
	// FFT reset
	
	if (x->reset_fft)
	{
		write_pointer = 0;
		hop_count = hop_size;
		x->reset_fft = 0;
	}
	
	// Write input block into double circular buffer
	
	for (i = 0; i < vec_size; i++)
	{
		rt_buffer[write_pointer + (bufffer_size >> 1)] = rt_buffer[write_pointer] = in1[i];
		write_pointer = (write_pointer + 1) % (bufffer_size >> 1); 
	}
	
	// Check for fft due and perform calculations
	
	while (vec_size)
	{
		hop_to_do = (vec_size < hop_size) ? vec_size : hop_size;
		hop_count -= hop_to_do;
		vec_size -= hop_to_do;
		frametime += hop_to_do;
		
		if (hop_count <= 0)
		{
			calc_descriptors_rt (x, rt_buffer + ((block_pointer + frametime + hop_count) % (bufffer_size >> 1)));
			hop_count += hop_size;
		}
	}
	
	// Store variables
	
	x->write_pointer = write_pointer;
	x->hop_count = hop_count;
}


void descriptors_dsp64 (t_descriptors *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	float *samples;
	
	long rt_memory_size = x->rt_memory_size;
	long num_samps = x->max_fft_size;
	long mem_required = 2 * (num_samps + maxvectorsize);
	long i;
	
	// Allocate memory if none (or not enough) has been allocated
	
	if (rt_memory_size < mem_required)
	{
		ALIGNED_FREE (x->rt_buffer);
		x->rt_buffer = (float *) ALIGNED_MALLOC (mem_required * sizeof (float));
		x->rt_memory_size = mem_required;
	}
	
	// Set variables
	
	x->write_pointer = 0;
	x->sr = samplerate;
	
	// If memory is correctly allocated then zero the sample buffer and add the dp routine
	
	if (x->rt_buffer) 
	{
		samples = x->rt_buffer;
		
		for (i = 0; i < mem_required; i++)
			samples[i] = 0.0;
		
		object_method(dsp64, gensym("dsp_add64"), x, descriptors_perform64, 0, NULL);
	}
	else 
		x->rt_memory_size = 0;
}

