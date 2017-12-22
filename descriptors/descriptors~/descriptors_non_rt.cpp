
/*
 *  descriptors~
 *
 *	The descriptors~ object can be used to calculate a large number of audio descriptors (or features) from a sample stored in an MSP buffer~ (or an ibuffer~ object).
 *	Various statistical calculations may be performed on the resultant descriptors (mean / median / standard deviation / max n values etc.) to produce useful information about the sample.
 *	descriptors~ is intended as a comprehensive analysis solution, being suitable for a wide range of analysis applications.
 *	It is the non real-time counterpart to the descriptorsrt~ object - the two objects are very similar in terms of features and usage.
 *
 *	The object only calculates and outputs the descriptors that the user requests, and can also operate on subsections of a buffer~ / ibuffer~.
 *	The object is designed to be as efficient as possible, avoiding unnecessary calculations and re-calculations wherever possible and making extensive use of SIMD operations.
 *
 *	For in-depth details on usage of of the descriptors~ object see the helpfile documentation.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include "descriptors_object.h"

#include <ibuffer_access.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// Object Basics (main / new / free) /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int C74_EXPORT main (void)
{	
	this_class = class_new ("descriptors~",
							(method) descriptors_new, 
							(method) descriptors_free, 
							sizeof(t_descriptors), 
							0L, 
							A_GIMME, 
							0);
	
	class_addmethod (this_class, (method)descriptors_analyse, "analyse", A_GIMME, 0L);
	class_addmethod (this_class, (method)descriptors_analyse, "analyze", A_GIMME, 0L);
	class_addmethod (this_class, (method)descriptors_descriptors_non_rt, "descriptors", A_GIMME, 0L);
	
	descriptors_main_common();
	
	class_register(CLASS_BOX, this_class);
	
    ibuffer_init();

	return 0;
}


void *descriptors_new (t_symbol *s, short argc, t_atom *argv)
{
    t_descriptors *x = (t_descriptors *)object_alloc(this_class);
	
	long max_fft_size_log2;
	long max_fft_size_in = 0;
	long max_fft_size;
	long descriptor_data_size = 0;
	long descriptor_feedback = 0;
	
	long mask_max_size = MAX_N_SEARCH;
	long n_search_memory_size = MAX_N_SEARCH * 22;

	void *allocated_memory;

	if (!x)
		return 0;

	// Get arguments 

	if (argc) 
		max_fft_size_in = atom_getlong(argv++);
	if (argc > 1) 
		descriptor_data_size = atom_getlong(argv++);
	if (argc > 2)
		descriptor_feedback = atom_getlong(argv++);
	
	// Set maximum fft size

	max_fft_size_log2 = descriptors_max_fft_size(x, max_fft_size_in);
	x->max_fft_size_log2 = max_fft_size_log2;
	x->max_fft_size = max_fft_size = 1 << (max_fft_size_log2);

	// This below sorts out the z_compile crash
	
	dsp_setup((t_pxobject *)x, 0);					
	
	// Allocate 4Mb of memory for the descriptors as a default
	
	if(!descriptor_data_size) descriptor_data_size = 4194304;			
	
	if (n_search_memory_size < (2 * (max_fft_size >> 1))) n_search_memory_size = (2 * (max_fft_size >> 1));
	if (mask_max_size < (max_fft_size >> 1)) mask_max_size = max_fft_size >> 1;
	
	// Allocate memory

	 allocated_memory = ALIGNED_MALLOC(
		descriptor_data_size + (n_search_memory_size * 22 * sizeof(double)) + (mask_max_size * sizeof(char)) + (max_fft_size * sizeof(float)) + + ((max_fft_size * 3) * sizeof(float)) + ((max_fft_size >> 1) * 3 * RING_BUFFER_SIZE * sizeof(float))
		+ (max_fft_size * 3 * sizeof(float)) + (max_fft_size * (sizeof(double) + sizeof(long))) + ((max_fft_size >> 1) * sizeof(double)) + (max_fft_size * RING_BUFFER_SIZE * sizeof(double)) + ((max_fft_size >> 1) * sizeof(double)) 
		+ ((max_fft_size >> 1) * sizeof(double)) + (MAX_OUTPUT * sizeof(t_atom)));
	
	if (!allocated_memory)
	{
		error ("descriptors(rt)~: couldn't allocate memory");
		return 0;
	}
	
	// Assign pointers (aligned pointers first)
	// N.B. x->window must be the first allocation - this is the pointer that is freed.

	x->window = (float *) allocated_memory;
	allocated_memory = (void *) ((float *) allocated_memory + max_fft_size);	
	
	x->fft_memory = allocated_memory;
	allocated_memory = (void *) ((float *) allocated_memory + (max_fft_size * 3));
	
	x->amps_buffer = (float *) allocated_memory;
	allocated_memory = (void *) ((float *) allocated_memory + ((max_fft_size >> 1) * 3 * RING_BUFFER_SIZE));
	
	x->ac_memory = allocated_memory;
	allocated_memory = (void *) ((float *) allocated_memory + (max_fft_size * 3));

	x->descriptor_data = (double *) allocated_memory;
	x->descriptor_data_size = descriptor_data_size; 	
	allocated_memory = (void *) ((char *) allocated_memory + descriptor_data_size);			
	
	x->n_data = allocated_memory;
	allocated_memory = (void *) ((char *) ((double *) allocated_memory + n_search_memory_size) + mask_max_size);
	
	x->median_memory = allocated_memory;
	allocated_memory = (void *) ((long *) ((double *) allocated_memory + max_fft_size) + max_fft_size);
	
	x->summed_amplitudes = (double *) allocated_memory;
	allocated_memory = (void *) ((double *) allocated_memory + (max_fft_size >> 1));
	
	x->cumulate = (double *) allocated_memory;
	allocated_memory = (void *) ((double *) allocated_memory + (max_fft_size * RING_BUFFER_SIZE));
	
	x->loudness_curve = (double *) allocated_memory;
	allocated_memory = (void *) ((double *) allocated_memory + (max_fft_size >> 1));
	
	x->log_freq = (double *) allocated_memory;
	allocated_memory = (void *) ((double *) allocated_memory + (max_fft_size >> 1));
	
	x->output_list = (t_atom *) allocated_memory;
	
	x->output_rt_clock = 0;
	
	descriptors_new_common (x, max_fft_size_log2, descriptor_feedback);

	return x;
}


void descriptors_free(t_descriptors *x)
{
	ALIGNED_FREE (x->window);
	hisstools_destroy_setup_f(x->fft_setup_real);
	if (x->output_rt_clock) 
		freeobject((t_object *)x->output_rt_clock);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////// User Routines For Performing Analysis //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void descriptors_analyse (t_descriptors *x, t_symbol *msg, short argc, t_atom *argv)
{
	t_symbol *buffer_name = 0;
	void *b;
	
	double sr;
	double mstosamps_val;
	double start_point_ms = 0.;
	double end_point_ms = 0.;
	
	long buffer_chan = 1;
	
	if (!x->descriptor_data_size) return;
	
	if (argc && atom_gettype(argv) != A_SYM)
	{
		error ("descriptors(rt)~: no buffer name given for analysis");
		return;
	}
	
	buffer_name = atom_getsym(argv);
	
	// Get arguments
	
	if (argc > 1) 
		buffer_chan = atom_getlong (argv + 1);
	if (argc > 2) 
		start_point_ms = atom_getfloat (argv + 2);
	if (argc > 3) 
		end_point_ms = atom_getfloat (argv + 3);		
	if (argc > 4) 
		error ("descriptors(rt)~: too many arguments to analyse function");
	
	// Check the buffer
	
	b = ibuffer_get_ptr (buffer_name);
	
	if (!b)
	{
		error ("descriptors(rt)~: buffer not found");
		return;
	}
	
	x->buffer_pointer = b;
	x->buffer_name = buffer_name;
	
	// Calculate lengths
	
	sr = x->sr = ibuffer_sample_rate (b);
	mstosamps_val = sr / 1000.;
	
	if (start_point_ms < 0) 
		start_point_ms = 0;
	if (end_point_ms < 0) 
		return;
	
	if (buffer_chan < 1) 
		buffer_chan = 1;
	
	// Store variables
	
	x->start_point = start_point_ms * mstosamps_val;
	x->end_point = end_point_ms * mstosamps_val;
	x->buffer_chan = buffer_chan - 1;
	
	calc_descriptors_non_rt (x);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////// Handle Non RT descriptor Calculation ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void calc_descriptors_non_rt (t_descriptors *x)
{
	t_atom *output_list = x->output_list;
	
	double energy_thresh = x->energy_thresh;
	
	long use_energy_thresh = x->use_energy_thresh;
	
	// FFT variables
	
	long fft_size = x->fft_size;
	long fft_size_halved = fft_size >> 1;
	long fft_size_log2 = x->fft_size_log2;
	long window_size = x->window_size;
	long hop_size = x->hop_size;
	
	FFT_SETUP_F fft_setup_real = x->fft_setup_real;
	FFT_SPLIT_COMPLEX_F raw_fft_frame;

	double *summed_amplitudes = x->summed_amplitudes;
	
	float *window = x->window;	
	float *raw_frame = (float *) x->fft_memory;
	float *windowed_frame = raw_frame + fft_size;

	vFloat *v_window = (vFloat *) window;	
	vFloat *v_windowed_frame = (vFloat *) windowed_frame;
	vFloat *real_data;
	vFloat *imag_data;

#if (defined F32_VEC_LOG_OP)
	vFloat v_half = {0.5, 0.5, 0.5, 0.5};
#endif
#if (defined F32_VEC_LOG_OP || defined F32_VEC_LOG_ARRAY)
	vFloat v_pow_min = {POW_MIN, POW_MIN, POW_MIN, POW_MIN};
#endif
	
	// descriptor variables
	
	double *descriptor_data = x->descriptor_data;
	double *current_data;
	double *pf_params = x->pf_params;
	double *pf_calc_params = x->pf_calc_params;
	double *pf_params_temp;
	double *pb_params = x->pb_params;
	
	long *pf_output_params = x->pf_output_params;
	long *pb_pos = x->pb_pos;
	
	long num_pb_descriptors = x->num_pb_descriptors;
	long num_pf_descriptors = x->num_pf_descriptors;
	long do_sum_amps = x->do_sum_amps;
	long output_length = x->output_length;
	long descriptor_data_size = x->descriptor_data_size;
	long from_pf_descriptor; 
	long to_pf_descriptor;
	long num_pf_descriptors_per_loop;
	
	// Memory pointers for n_min and n_max finding etc.
	
	double *n_min = (double *) x->n_data;
	double *n_min_pos = n_min + MAX_N_SEARCH;
	
	double *n_max = n_min + (2 * MAX_N_SEARCH);
	double *n_max_pos = n_min + (3 * MAX_N_SEARCH);
	
	double *n_peaks = n_min + (4 * MAX_N_SEARCH);
	double *n_peak_pos = n_min + (5 * MAX_N_SEARCH);
	
	double *n_troughs = n_min + (6 * MAX_N_SEARCH);
	double *n_trough_pos = n_min + (7 * MAX_N_SEARCH);
	
	double *nc_peak = n_min + (8 * MAX_N_SEARCH);
	double *nc_peak_pos = n_min + (9 * MAX_N_SEARCH); 
	double *nc_above_pos1 = n_min + (10 * MAX_N_SEARCH); 
	double *nc_above_pos2 = n_min + (11 * MAX_N_SEARCH);
	
	double *nc_trough = n_min + (12 * MAX_N_SEARCH);
	double *nc_trough_pos = n_min + (13 * MAX_N_SEARCH); 
	double *nc_below_pos1 = n_min + (14 * MAX_N_SEARCH); 
	double *nc_below_pos2 = n_min + (15 * MAX_N_SEARCH);
	
	double *nlc_above = n_min + (16 * MAX_N_SEARCH);
	double *nlc_above_pos1 = n_min + (17 * MAX_N_SEARCH); 
	double *nlc_above_pos2 = n_min + (18 * MAX_N_SEARCH);
	
	double *nlc_below = n_min + (19 * MAX_N_SEARCH);
	double *nlc_below_pos1 = n_min + (20 * MAX_N_SEARCH); 
	double *nlc_below_pos2 = n_min + (21 * MAX_N_SEARCH);
	
	char *mask = (char *) ((double *) n_min + (22 * MAX_N_SEARCH));

	// Buffer variables
	
	void *b = x->buffer_pointer;
	void *buffer_samples_ptr;
	
	AH_SIntPtr file_length;
	long num_of_chans;
	long int_size;
	
	// Buffer access variables
	
	double num_frames_recip;
	
	long buffer_pos = 0;
	long buffer_chan = x->buffer_chan;
	long start_point = x->start_point;
	long end_point = x->end_point;
	long num_frames;
	long num_samps;
	
	// Loop iterators
	
	long i, j, k, l, m;

	enum StatisticsType stats_type;
	long output_pos;
	long store_n;

	double sr;
	double samps_to_ms_val;
	double frame_to_ms_val;
	double ms_to_frame_val;
	double bin_freq;

	enum PBDescriptorType descriptor_type;
	
	double descriptor;
	
	long num_bins = fft_size_halved;
	long median_span;
	long N;
	
	double *median_amplitudes = (double *) x->median_memory;
	double *freqs = (double *) x->n_data;
	double *amps = freqs + fft_size_halved;
	
	long *median_indices = (long *) ((double *) median_amplitudes + num_bins);

	long spectral_peak_check;
	
	raw_fft_frame.realp = windowed_frame + fft_size;
	raw_fft_frame.imagp = raw_fft_frame.realp + fft_size_halved;
	
	real_data = (vFloat *) raw_fft_frame.realp;
	imag_data = (vFloat *) raw_fft_frame.imagp;

	// Check that some descriptors have been set
	
	if (num_pf_descriptors + num_pb_descriptors <= 0)
	{
		error("descriptors(rt)~: no descriptors set");
		return;
	}
	
	// Access buffer and increment pointer
	
	if (!ibuffer_info (b, &buffer_samples_ptr, &file_length, &num_of_chans, &int_size))
	{
		error ("descriptors(rt)~: could not access buffer");
		return;
	}
	
	ibuffer_increment_inuse(b);		
	
	sr = ibuffer_sample_rate (b);
	samps_to_ms_val = 1000. / sr;
	frame_to_ms_val = hop_size * 1000. / sr;
	ms_to_frame_val = sr / (hop_size * 1000.);
	bin_freq = sr / (double) fft_size;

	// If the sample rate is different from the last one used, recalculate curves for the new sample rates
	
	if (sr != x->sr) 
		calc_curves(x);
	x->sr = sr;
	
	// Range check buffer access variables and calculate numer of frames
	
	if (num_of_chans < buffer_chan + 1)
		buffer_chan = buffer_chan % num_of_chans;
				
	if (end_point && end_point < file_length)
		file_length = end_point;
	file_length -= start_point;
	
	num_frames = (long) ceil ((double) file_length / (double) hop_size);
	
	if (num_frames < 1)
	{
		error("descriptors(rt)~: zero length file or segment");
		ibuffer_decrement_inuse(b);
		return;
	}
	
	num_frames_recip = 1. / num_frames;
	
	// See how many descriptors we can calculate per loop (hopefully all of them, but this allows more conservative memory allocation)
	
	num_pf_descriptors_per_loop = descriptor_data_size / (num_frames * sizeof(double));
	
	if (!num_pf_descriptors_per_loop)
	{
		error("descriptors(rt)~: not enough memory - file is too long!");
		ibuffer_decrement_inuse(b);
		return;
	}	

	// Zero summed amplitudes if necessary
	
	if (do_sum_amps)
	{
		for (i = 0; i < fft_size_halved; i++) 
			summed_amplitudes[i] = 0.;
	}
	
	// Calculation loop (note that we need to run this to do spectral peaks)

	spectral_peak_check = 0;
	
	if (do_sum_amps && num_pf_descriptors < 1)
		spectral_peak_check = 1;
	
	for (i = 0; i < num_pf_descriptors + spectral_peak_check; i += num_pf_descriptors_per_loop)
	{
		char frame_pointer = 0;
		
		from_pf_descriptor  = i;
		to_pf_descriptor = i + num_pf_descriptors_per_loop;
		if (to_pf_descriptor > num_pf_descriptors) 
			to_pf_descriptor = num_pf_descriptors;

		descriptors_zero_ring_buffers(x, fft_size);
		
		for (k = 0; k < num_frames; k++)
		{
			double *cumulate_amps = x->cumulate + (fft_size * frame_pointer);
			double *cumulate_sq_amps = cumulate_amps + fft_size_halved;
			double sum;
			
			float *this_frame = x->amps_buffer + (3 * fft_size_halved * frame_pointer);
			float *sq_amplitudes = this_frame;
			float *amplitudes = sq_amplitudes + fft_size_halved;
			float *log_amplitudes = amplitudes + fft_size_halved;
			
			vFloat *v_sq_amplitudes = (vFloat *) sq_amplitudes;
#if (defined F32_VEC_LOG_OP || defined F32_VEC_LOG_ARRAY)
			vFloat *v_log_amplitudes = (vFloat *) log_amplitudes;	
#endif
			vFloat *v_raw_frame = (vFloat *) raw_frame;
			
			x->ac_flag = 0;
			x->median_flag = 0;
			x->centroid_lin_flag = 0;
			x->centroid_log_flag = 0;
			x->last_pf_spectralpeaks_n = 0;
			x->last_pf_spectralpeaks_med_size = 0;
			x->last_threshold = DBL_MAX;
			
			// Get a window of samples
			
			buffer_pos = k * hop_size;
					
			if (file_length >= (buffer_pos + window_size)) num_samps = window_size;
			else num_samps = file_length - buffer_pos;
			
			ibuffer_get_samps (buffer_samples_ptr, raw_frame, start_point + buffer_pos, num_samps, num_of_chans, buffer_chan, int_size);
			
			// Zero pad to fft size
			
			for (j = num_samps; j < fft_size; j++)
				raw_frame[j] = 0.;
			
			// Apply window
			
			for (j = 0; j < window_size >> 2; j++)
				v_windowed_frame[j] = F32_VEC_MUL_OP(v_raw_frame[j], v_window[j]);
			for (j <<= 2; j < window_size; j++)
				windowed_frame[j] = raw_frame[j] * window[j];
			
			// Do FFT straight into position
			
			hisstools_unzip_f(windowed_frame, &raw_fft_frame, fft_size_log2);
			hisstools_rfft_f(fft_setup_real, &raw_fft_frame, fft_size_log2);
			
			// Discard the nyquist bin (if necessary add this back later)
			
			raw_fft_frame.imagp[0] = 0.;
			
			// Calulate square amplitudes / amplitudes / log_amplitudes (leaving the raw data also in case phase is needed for some descriptor added later)
			
			for (j = 0; j < fft_size_halved >> 2; j++)
				v_sq_amplitudes[j] = F32_VEC_ADD_OP(F32_VEC_MUL_OP(real_data[j], real_data[j]), F32_VEC_MUL_OP(imag_data[j], imag_data[j]));

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
				log_amplitudes[i] = log((sq_amplitudes[i] < POW_MIN) ? POW_MIN : sq_amplitudes[i]) * 0.5;;
#endif
#endif
			
			for (j = 0; j < fft_size_halved; j++)
				amplitudes[j] = sqrt(sq_amplitudes[j]);
				
			// Sum amplitudes if needed (double precision)
			
			if (do_sum_amps)
			{
				for (j = 0; j < fft_size_halved;j++)
					summed_amplitudes[j] += amplitudes[j] * num_frames_recip;
			}
			
			// Cumulate amps and square amps
			
			cumulate_amps = x->cumulate + (fft_size * frame_pointer);
			cumulate_sq_amps = cumulate_amps + fft_size_halved;
			

			for (sum = 0., j = 0; j < fft_size_halved; j++)
			{
				sum += amplitudes[j];
				cumulate_amps[j] = sum;
			}
						
			for (sum = 0., j = 0; j < fft_size_halved; j++)
			{
				sum += sq_amplitudes[j];
				cumulate_sq_amps[j] = sum;
			}
				
			pf_params_temp = pf_params;
			
			////////////////////////////////////////////// Calculate Per Frame descriptors //////////////////////////////////////////
			
			if (!use_energy_thresh || cumulate_sq_amps[fft_size_halved - 1] > energy_thresh)
			{
				for (j = from_pf_descriptor; j < to_pf_descriptor; j++)
					descriptor_data[(j * num_frames) + k] = calc_pf_descriptor(x, raw_frame, windowed_frame, raw_fft_frame, frame_pointer, window_size, fft_size, &pf_params_temp);
			}
			else
			{	
				for (j = from_pf_descriptor; j < to_pf_descriptor; j++)
					descriptor_data[(j * num_frames) + k] = DBL_MAX;
			}
			
			frame_pointer = (frame_pointer + 1) % RING_BUFFER_SIZE;
		}

		//////////////////////////////////////// Derive outputs from the raw per frame data ////////////////////////////////////////

		for (j = from_pf_descriptor; j <  to_pf_descriptor; j++)
		{
			// N.B. Only calculate what we need of the below

			double time_centroid = DBL_MAX;
			double range = DBL_MAX;
			double mean = DBL_MAX;
			double standard_deviation = DBL_MAX;
			double median = DBL_MAX;
			double threshold_ratio_above = DBL_MAX;
			double threshold_ratio_below = DBL_MAX;
		
			// Get info on what to calculate
			
			double *pf_calc_params_current = pf_calc_params + (j * 12);
			long PF_NStore = *pf_output_params++;
			 
			long descriptor_flags = (long) pf_calc_params_current[0];	
			double mask_time = pf_calc_params_current[1];
			long do_n_max = (long) pf_calc_params_current[2];
			long do_n_min = (long) pf_calc_params_current[3];
			long do_n_peak = (long) pf_calc_params_current[4];
			long do_n_trough = (long) pf_calc_params_current[5];
			long do_cross_above = (long) pf_calc_params_current[6];
			long do_cross_below = (long) pf_calc_params_current[7];
			long do_longest_cross_above = (long) pf_calc_params_current[8];
			long do_longest_cross_below = (long) pf_calc_params_current[9];
			double threshold = pf_calc_params_current[10];
			enum ThresholdType thresh_type = (enum ThresholdType) pf_calc_params_current[11];

			long mask_size = mask_time * ms_to_frame_val;

			current_data = descriptor_data + (j * num_frames);

			if (descriptor_flags & DO_MEAN) 
				mean = calc_mean_and_time_centroid (current_data, num_frames, &time_centroid, frame_to_ms_val);
			if (descriptor_flags & DO_STDD) 
				standard_deviation = calc_standard_deviation(current_data, num_frames, mean);

			// N searchs
			
			if (do_n_max) 
				calc_n_max (current_data, num_frames, mask_size, mask, do_n_max, n_max, n_max_pos, frame_to_ms_val);
			if (do_n_min) 
				calc_n_min (current_data, num_frames, mask_size, mask, do_n_min, n_min, n_min_pos, frame_to_ms_val);
			if (do_n_peak) 
				calc_n_peak (current_data, num_frames, mask_size, mask, do_n_peak, n_peaks, n_peak_pos, frame_to_ms_val);
			if (do_n_trough) 
				calc_n_trough (current_data, num_frames, mask_size, mask, do_n_trough, n_troughs, n_trough_pos, frame_to_ms_val);
			
			// Find threshold based on stats
			
			switch (thresh_type)
			{
				case THRESH_ABS:
					break;
				case THRESH_MEAN_MUL:
					threshold *= mean;
					break;
				case THRESH_MEAN_DB:
					threshold = dbtoa(threshold) * mean;
					break;
				case THRESH_MEAN_ADD:
					threshold += mean;
					break;
				case THRESH_PEAK_MUL:
					threshold *= n_max[0];
					break;
				case THRESH_PEAK_DB:
					threshold = dbtoa(threshold) * n_max[0];
					break;
				case THRESH_PEAK_ADD:
					threshold += n_max[0];
					break;
			}
			
			// Threshold searches
			
			if (do_cross_above) 
				calc_n_thresh_cross_above (current_data, num_frames, mask_size, mask, do_cross_above, nc_peak, nc_peak_pos, nc_above_pos1, nc_above_pos2, threshold, frame_to_ms_val);
			if (do_cross_below) 
				calc_n_thresh_cross_below (current_data, num_frames, mask_size, mask, do_cross_below, nc_trough, nc_trough_pos, nc_below_pos1, nc_below_pos2, threshold, frame_to_ms_val);
			if (do_longest_cross_above) 
				calc_longest_crossing_points_above (current_data, num_frames, mask_size, do_longest_cross_above, nlc_above, nlc_above_pos1, nlc_above_pos2, threshold, frame_to_ms_val);
			if (do_longest_cross_below) 
				calc_longest_crossing_points_below (current_data, num_frames, mask_size, do_longest_cross_below, nlc_below, nlc_below_pos1, nlc_below_pos2, threshold, frame_to_ms_val);
			
			if (descriptor_flags & DO_RATIO_ABOVE) 
				threshold_ratio_above = calc_threshold_ratio (current_data, num_frames, 1, threshold);
			if (descriptor_flags & DO_RATIO_BELOW) 
				threshold_ratio_below = calc_threshold_ratio (current_data, num_frames, 0, threshold);
		
			if (descriptor_flags & DO_RANGE) 
				range = n_max[0] - n_min[0];

			if (descriptor_flags & DO_MEDIAN) 
				median = calc_median (current_data, num_frames);

			// Store all output into position
			
			for (l = 0; l < PF_NStore; l++)
			{
				stats_type = (StatisticsType) *pf_output_params++;
				output_pos = *pf_output_params++;
				
				switch (stats_type)
				{
					case STATS_MEAN:
						atom_setfloat(output_list + output_pos, mean);
						break;
						
					case STATS_MEDIAN:
						atom_setfloat(output_list + output_pos, median);
						break;
						
					case STATS_TIME_CENTROID:
						atom_setfloat(output_list + output_pos, time_centroid);
						break;
						
					case STATS_STANDARD_DEV:
						atom_setfloat(output_list + output_pos, standard_deviation);
						break;
						
					case STATS_RANGE:
						atom_setfloat(output_list + output_pos, range);
						break;
						
					case STATS_MAX:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
							atom_setfloat(output_list + output_pos + m, n_max[m]);
						break;
						
					case STATS_MIN:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
							atom_setfloat(output_list + output_pos + m, n_min[m]);
						break;
						
					case STATS_PEAK:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
							atom_setfloat(output_list + output_pos + m, n_peaks[m]);
						break;
						
					case STATS_TROUGH:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
							atom_setfloat(output_list + output_pos + m, n_troughs[m]);
						break;
						
					case STATS_MAX_POS:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
							atom_setfloat(output_list + output_pos + m, n_max_pos[m]);
						break;
						
					case STATS_MIN_POS:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
							atom_setfloat(output_list + output_pos + m, n_min_pos[m]);
						break;
						
					case STATS_PEAK_POS:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
							atom_setfloat(output_list + output_pos + m, n_peak_pos[m]);
						break;
						
					case STATS_TROUGH_POS:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
							atom_setfloat(output_list + output_pos + m, n_trough_pos[m]);
						break;
						
					case STATS_CROSSING_PEAK:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
							atom_setfloat(output_list + output_pos + m, nc_peak[m]);
						break;
						
					case STATS_CROSSING_PEAK_POS:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
							atom_setfloat(output_list + output_pos + m, nc_peak_pos[m]);
						break;
						
					case STATS_CROSSING_ABOVE:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
							atom_setfloat(output_list + output_pos + m, nc_above_pos1[m]);
						break;
						
					case STATS_CROSSINGS_ABOVE:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
						{
							atom_setfloat(output_list + output_pos + (2 * m), nc_above_pos1[m]);
							atom_setfloat(output_list + output_pos + (2 * m) + 1, nc_above_pos2[m]);
						}
						break;
						
					case STATS_CROSSING_TROUGH:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
							atom_setfloat(output_list + output_pos + m, nc_trough[m]);
						break;
						
					case STATS_CROSSING_TROUGH_POS:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
							atom_setfloat(output_list + output_pos + m, nc_trough_pos[m]);
						break;
						
					case STATS_CROSSING_BELOW:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
							atom_setfloat(output_list + output_pos + m, nc_below_pos1[m]);
						break;
						
					case STATS_CROSSINGS_BELOW:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
						{
							atom_setfloat(output_list + output_pos + (2 * m), nc_below_pos1[m]);
							atom_setfloat(output_list + output_pos + (2 * m) + 1, nc_below_pos2[m]);
						}
						break;
						
					case STATS_LONGEST_CROSS_ABOVE:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
							atom_setfloat(output_list + output_pos + m, nlc_above[m]);
						break;

					case STATS_LONGEST_CROSSINGS_ABOVE:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
						{
							atom_setfloat(output_list + output_pos + (2 * m), nlc_above_pos1[m]);
							atom_setfloat(output_list + output_pos + (2 * m) + 1, nlc_above_pos2[m]);
						}
						break;
					
					case STATS_LONGEST_CROSS_BELOW:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
							atom_setfloat(output_list + output_pos + m, nlc_below[m]);
						break;

					case STATS_LONGEST_CROSSINGS_BELOW:
						store_n = *pf_output_params++;
						for (m = 0; m < store_n; m++)
						{
							atom_setfloat(output_list + output_pos + (2 * m), nlc_below_pos1[m]);
							atom_setfloat(output_list + output_pos + (2 * m) + 1, nlc_below_pos2[m]);
						}
						break;
	
					case STATS_RATIO_ABOVE:
						atom_setfloat(output_list + output_pos, threshold_ratio_above);
						break;
						
					case STATS_RATIO_BELOW:
						atom_setfloat(output_list + output_pos, threshold_ratio_below);
						break;
						
					case STATS_NONE:
						break;
				}
			}
		}
	}

	// Do per block descriptors
	
	for (i = 0; i < num_pb_descriptors; i++)
	{				
		// Get descriptor type, calculate and store into position
		
		descriptor_type = (enum PBDescriptorType) pb_params[0];
		
		switch (descriptor_type)
		{
			case DESCRIPTOR_PB_DURATION:
			
				descriptor = file_length * samps_to_ms_val;
				atom_setfloat (output_list + pb_pos[i], descriptor);
				
				pb_params += 1;
				break;
				
			////////////////////////////////////////////////////////

			case DESCRIPTOR_PB_SPECTRAL_PEAKS:
		
				N = (long) pb_params[1];
				median_span = (long) pb_params[2];
				
				if (median_span)
				{	
					medianfilter_double (median_indices, median_amplitudes, summed_amplitudes, num_bins, median_span);
					spectralpeaks_double (summed_amplitudes, median_amplitudes, num_bins, freqs, amps, bin_freq, dbtoa(-180.));
				}
				else spectralpeaks_double (summed_amplitudes, 0, num_bins, freqs, amps, bin_freq, dbtoa(-180.));
			
				for (j = 0; j < N; j++)
				{
					atom_setfloat(output_list + pb_pos[i] + (2 * j), freqs[j]);
					atom_setfloat(output_list + pb_pos[i] + (2 * j) + 1, amps[j]);
				}
				
				pb_params += 3;
				break;
				
				case DESCRIPTOR_PB_NONE:
					break;
		}
	}
	
	// Decrement buffer pointer
	
	ibuffer_decrement_inuse(b);

	// Output
	
	outlet_list (x->the_list_outlet, 0L, output_length, output_list);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////// Dummy DSP Routine ////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	


void descriptors_dsp (t_descriptors *x, t_signal **sp, short *count)
{
}


void descriptors_dsp64 (t_descriptors *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
}
