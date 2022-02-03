
/*
 *  descriptors_object.c
 *
 *	This code deals with common functionality for the two varieties of the descriptors object.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "descriptors_object.h"
#include "Statistics.hpp"

// Common Basics (main / new / assist)

t_class *this_class;

void descriptors_main_common ()
{
	// Setup class basics
	
	class_addmethod(this_class, (method) descriptors_fft_params, "fftparams", A_GIMME, 0);
	class_addmethod(this_class, (method) descriptors_energy_thresh, "energythresh", A_GIMME, 0);
	class_addmethod(this_class, (method) descriptors_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method) descriptors_dsp64, "dsp64", A_CANT, 0);
	class_addmethod(this_class, (method) descriptors_assist, "assist", A_CANT, 0);

	class_dspinit(this_class);
	
	// Initialise symbols
	
	ps_energy = gensym("energy");
	ps_energy_ratio = gensym("energy_ratio");
	ps_rolloff = gensym("rolloff");
	ps_spectral_crest = gensym("spectral_crest");
	ps_flux = gensym("flux");
	ps_mkl = gensym("mkl");
	ps_foote = gensym("foote");
	ps_abs = gensym("abs");
	ps_rms = gensym("rms");
	ps_peakamp = gensym("peakamp");
	ps_loudness = gensym("loudness");
	ps_lin_centroid = gensym("lin_centroid");
	ps_lin_spread = gensym("lin_spread");
	ps_lin_skewness = gensym("lin_skewness");
	ps_lin_kurtosis = gensym("lin_kurtosis");
	ps_log_centroid = gensym("log_centroid");
	ps_log_spread = gensym("log_spread");
	ps_log_skewness = gensym("log_skewness");
	ps_log_kurtosis = gensym("log_kurtosis");
	ps_lin_brightness = gensym("lin_brightness");
	ps_log_brightness = gensym("log_brightness");
	ps_sfm = gensym("sfm");
	ps_noise_ratio = gensym("noise_ratio");
	ps_harmonic_ratio = gensym("harmonic_ratio");
	ps_pitch = gensym("pitch");
	ps_confidence = gensym("confidence");
	ps_inharmonicity = gensym("inharmonicity");
	ps_roughness = gensym("roughness");
	ps_spectral_peaks = gensym("spectral_peaks");
	ps_duration = gensym("duration");

	ps_mean = gensym("mean");
	ps_median = gensym("median");
	ps_time_centroid = gensym("time_centroid");
	ps_standard_dev = gensym("standard_dev");
	ps_range = gensym("range");

	ps_max = gensym("max");
	ps_max_pos = gensym("max_pos");
	ps_min = gensym("min");
	ps_min_pos = gensym("min_pos");
	ps_peak = gensym("peak");
	ps_peak_pos = gensym("peak_pos");
	ps_trough = gensym("trough");
	ps_trough_pos = gensym("trough_pos");

	ps_crossing_peak = gensym("crossing_peak");
	ps_crossing_peak_pos = gensym("crossing_peak_pos");
	ps_cross_above = gensym("cross_above");
	ps_crossings_above = gensym("crossings_above");

	ps_crossing_trough = gensym("crossing_trough");
	ps_crossing_trough_pos = gensym("crossing_trough_pos");
	ps_cross_below = gensym("cross_below");
	ps_crossings_below = gensym("crossings_below");

	ps_longest_cross_above  = gensym("longest_cross_above");
	ps_longest_crossings_above = gensym("longest_crossings_above");
	ps_longest_cross_below  = gensym("longest_cross_below");
	ps_longest_crossings_below = gensym("longest_crossings_below");
	
	ps_ratio_above = gensym("ratio_above");
	ps_ratio_below = gensym("ratio_below");

	ps_masktime = gensym("masktime");
	ps_threshold = gensym("threshold");

	ps_abs = gensym("abs");
	ps_peak_mul = gensym("peak_mul");
	ps_peak_add = gensym("peak_add");
	ps_peak_db = gensym("peak_db");
	ps_mean_mul = gensym("mean_mul");
	ps_mean_add = gensym("mean_add");
	ps_mean_db = gensym("mean_db");
	
	ps_masktime = gensym("masktime");

	ps_rectangle = gensym("rectangle");
	ps_hann = gensym("hann");
	ps_hamming = gensym("hamming");
	ps_kaiser = gensym("kaiser");
	ps_triangle = gensym("triangle");
	ps_blackman = gensym("blackman");
	ps_blackman62 = gensym("blackman62");
	ps_blackman70 = gensym("blackman70");
	ps_blackman74 = gensym("blackman74");
	ps_blackman92 = gensym("blackman92");
	ps_blackman_harris = gensym("blackman-harris");
	ps_flattop = gensym("flattop");
	
	ps_nullsym = gensym("");
		
	// Constants
	
	POW_MIN = pow(10, DB_EPSILON / 10.);
	LOG_MIN = logf(POW_MIN) * 0.5;
	MKL_EQUALISE_MAX_LOG = log(pow(10, DB_MAX_MKL_EQUAL) * 20.);
}


void descriptors_new_common (t_descriptors *x, long max_fft_size_log2, long desciptor_feedback)
{
	// Initialise variables
	
    hisstools_create_setup(&x->fft_setup_real, max_fft_size_log2);
	x->frame_pointer = 0;
	
	// Multiply amplitude by 2 before conversion to get power (note this is never used as is - but set it to some sensible value anyway)
	
	x->energy_thresh = dbtoa(-180. * 2.);		
	x->use_energy_thresh = 0;
	
	x->do_sum_amps = 0;
	x->num_pf_descriptors = 0;	
	x->num_pb_descriptors = 0;
	x->output_length = 0;
	
	x->buffer_chan = 0;
	x->start_point = 0;
	x->end_point = 0; 
	
	x->buffer_name = 0;
	
	x->sr = 44100.0;
	
	x->rt_buffer = 0;	
	x->rt_memory_size = 0;
	
	x->write_pointer = 0;
	x->the_list_outlet = listout(x);
	
	x->descriptor_feedback = desciptor_feedback;
	
	if (1 << max_fft_size_log2 < 4096)
		descriptors_fft_params_internal (x, 1 << max_fft_size_log2, 0, 0, ps_nullsym);	

	else
		descriptors_fft_params_internal (x, 4096, 0, 0, ps_nullsym);	
}


void descriptors_assist(t_descriptors *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
    {
		sprintf(s,"descriptors Out");
	}
	else 
	{
        sprintf(s,"(signal / messages) All Messages / Input");
    }
}


// FFT params and Window Generation



long int_log2 (long in, long *inexact)
{
	// Helper to calculate fft sizes log 2
	
	long temp = in;
	long out = 0;
	
	if (in <= 0) 
		return - 1;
	
	while (temp)
	{
		temp >>= 1;
		out++;		
	}
	
	if (in == 1 << (out - 1)) 
	{
		out--;
		*inexact = 0;
	}
	else
		*inexact = 1;
	
	return out;
}


long descriptors_max_fft_size (t_descriptors *x, long max_fft_size)
{
	// Returns the max fft size log 2 after checking range and value
	
	long inexact = 0;
	long max_fft_size_log2 = int_log2 (max_fft_size, &inexact);
	
	if (max_fft_size_log2 < 0)
		max_fft_size_log2 = DEFAULT_MAX_FFT_SIZE_LOG2; 

	if (max_fft_size_log2 > MAX_FFT_SIZE_LOG2)
	{
		error ("descriptors(rt)~: maximum fft size too large - using %ld", 1 << MAX_FFT_SIZE_LOG2);
		max_fft_size_log2 = MAX_FFT_SIZE_LOG2;
	}
	
	if (max_fft_size_log2 && max_fft_size_log2 < MIN_FFT_SIZE_LOG2)
	{
		error ("descriptors(rt)~: maximum fft size too small - using %ld", 1 << MIN_FFT_SIZE_LOG2);
		max_fft_size_log2 = MIN_FFT_SIZE_LOG2;
	}

	if (inexact)
		error ("descriptors(rt)~: maximum fft size must be power of two - using %ld", 1 << max_fft_size_log2);

	return max_fft_size_log2;
}


void descriptors_fft_params (t_descriptors *x, t_symbol *msg, short argc, t_atom *argv)
{
	// Load in args as relevant
	
	long fft_size = (argc > 0) ? atom_getlong(argv + 0) : 0;
	long hop_size = (argc > 1) ? atom_getlong(argv + 1) : 0;
	long window_size = (argc > 2) ? atom_getlong(argv + 2) : 0;
	t_symbol *window_type = (argc > 3) ? atom_getsym(argv + 3) : ps_nullsym;
	
		
	// Ignore blank argument set (keep current values)
	
	if (argc < 0)
		return;
	
	descriptors_fft_params_internal(x, fft_size, hop_size, window_size, window_type);
}
	
void descriptors_fft_params_internal (t_descriptors *x, long fft_size, long hop_size, long window_size, t_symbol *window_type)
{
	enum WindowType window_select = WIND_HANN;
	
	long inexact = 0;
	long fft_size_log2 = int_log2 (fft_size, &inexact);

	// Check for valid fft size
	
	if (fft_size_log2 < MIN_FFT_SIZE_LOG2)
	{
		fft_size_log2 = MIN_FFT_SIZE_LOG2;
		error ("descriptors(rt)~: fft_size is too small - using %ld", 1 << fft_size_log2);
	}
	
	if (fft_size_log2 > x->max_fft_size_log2)
	{
		x->fft_size_log2 = fft_size_log2 = x->max_fft_size_log2;
		error ("descriptors(rt)~: fft_size too large - using %ld", 1 << fft_size_log2);
	}
		
	if (inexact)
		error ("descriptors(rt)~: fft_size must be power of two - using %ld", 1 << fft_size_log2);
	
	x->fft_size_log2 = fft_size_log2;
	x->fft_size = fft_size = 1 << fft_size_log2;

	// Set hop size (default to overlap of two)
	
	if (hop_size > 0) 
		x->hop_size = hop_size;
	if (hop_size <= 0) 
		x->hop_size = x->fft_size >> 1;											
	x->hop_count = x->hop_size;
	
	// Set window size (default to fft size)
	
	if (window_size < 0) 
		window_size = x->window_size;
	if (window_size <= 0) 
		window_size = x->fft_size;
	if (window_size > x->fft_size) 
		window_size = x->fft_size;
	
	// Set window type (defulat to vonn hann window
	
	if (window_type == ps_rectangle) window_select = WIND_RECT;
	if (window_type == ps_hamming) window_select = WIND_HAMMING;
	if (window_type == ps_kaiser) window_select = WIND_KAISER;
	if (window_type == ps_triangle) window_select = WIND_TRIANGLE;
	if (window_type == ps_blackman) window_select = WIND_BLACKMAN;
	if (window_type == ps_blackman62) window_select = WIND_BLACKMAN_62;
	if (window_type == ps_blackman70) window_select = WIND_BLACKMAN_70;
	if (window_type == ps_blackman74) window_select = WIND_BLACKMAN_74;
	if (window_type == ps_blackman92) window_select = WIND_BLACKMAN_92;
	if (window_type == ps_blackman_harris) window_select = WIND_BLACKMAN_HARRIS;
	if (window_type == ps_flattop) window_select = WIND_FLAT_TOP;
	
	if (!window_select && window_type != ps_hann && window_type != ps_nullsym) 
		error ("descriptors: window type not recognised - using hann window");
	
	// Generate the window, zero ring buffers and calculate the curves for this fft size
	
	descriptors_generate_window(x, x->window, window_size, x->fft_size, window_select);
	descriptors_zero_ring_buffers(x, x->fft_size);
    calc_curves(x);
	
	x->window_size = window_size;
	x->reset_fft = 1;
	
}


void descriptors_generate_window(t_descriptors *x, float *window, long window_size, long fft_size, enum WindowType window_select)
{
	FFT_SETUP_F fft_setup_real = x->fft_setup_real;
	
	FFT_SPLIT_COMPLEX_F raw_fft_frame;
	
	float *raw_frame = (float *) x->fft_memory;
	float *amplitudes = (float *) x->amps_buffer;

	long fft_size_halved = fft_size >> 1;
	long fft_size_log2 = x->fft_size_log2;
		
	vFloat *v_amplitudes = (vFloat *) amplitudes;
	vFloat *real_data;
	vFloat *imag_data;
	
	float scale;
	
	double gain = 0.;
	double alpha_bessel_recip;
	double new_term;
	double x_sq;
	double b_func;
	double temp;
	
	long i, j;
		
	raw_fft_frame.realp = raw_frame + fft_size;
	raw_fft_frame.imagp = raw_fft_frame.realp + fft_size_halved;
	
	real_data = (vFloat *) raw_fft_frame.realp;
	imag_data = (vFloat *) raw_fft_frame.imagp;

	switch (window_select)
	{
		case WIND_RECT:
			for (i = 0; i < window_size; i++)
				window[i] = 1.f;
			break;

		case WIND_HANN:
			for (i = 0; i < window_size; i++)
				window[i] = 0.5 - (0.5 * cos(FFTW_TWOPI * ((double) i / (double) window_size)));
			break;

		case WIND_HAMMING:
			for (i = 0; i < window_size; i++)
				window[i] = 0.54347826 - (0.45652174 * cos(FFTW_TWOPI * ((double) i / (double) window_size)));
			break;

		case WIND_KAISER:
			
			// First find bessel function of alpha
			
			x_sq = 46.24;
			new_term = 0.25 * x_sq;
			b_func = 1.0;
			j = 2;
			
			// Gives maximum accuracy
			
			while (new_term)	
			{
				b_func += new_term;
				alpha_bessel_recip = (1.0 / (4.0 * (double) j * (double) j));
				new_term = new_term * x_sq * (1.0 / (4.0 * (double) j * (double) j));
				j++;
			}
				
				alpha_bessel_recip = 1 / b_func;
			
			// Now create kaiser window
			
			for (i = 0; i < window_size; i++)
			{
				temp = ((2.0 * (double) i) - ((double) window_size - 1.0));
				temp = temp / window_size;
				temp *= temp;
				x_sq = (1 - temp) * 46.24;
				new_term = 0.25 * x_sq;
				b_func = 1;
				j = 2;
				
				// Gives maximum accuracy
				
				while (new_term)	
				{
					b_func += new_term;
					new_term = new_term * x_sq * (1.0 / (4.0 * (double) j * (double) j));
					j++;
				}
				window[i] = b_func * alpha_bessel_recip;
			}
			break;

		case WIND_TRIANGLE:
			for (i = 0; i < (window_size >> 1); i++)
				window[i] = (double) i / (double) (window_size >> 1);
			for (; i < window_size; i++)
				window[i] = (double) (((double) window_size - 1) - (double) i) / (double) (window_size >> 1);
			break;

		case WIND_BLACKMAN:
			for (i = 0; i < window_size; i++)
				window[i] = 0.42659071 - (0.49656062 * cos(FFTW_TWOPI * ((double) i / (double) window_size))) + (0.07684867 * cos(FFTW_FOURPI * ((double) i / (double) window_size)));
			break;

		case WIND_BLACKMAN_62:
			for (i = 0; i < window_size; i++)
				window[i] = (0.44859f - 0.49364f * cos(FFTW_TWOPI * ((double) i / (double) window_size)) + 0.05677f * cos(FFTW_FOURPI * ((double) i / (double) window_size)));
			break;

		case WIND_BLACKMAN_70:
			for (i = 0; i < window_size; i++)
				window[i] = (0.42323f - 0.49755f * cos(FFTW_TWOPI * ((double) i / (double) window_size)) + 0.07922f * cos(FFTW_FOURPI * ((double) i / (double) window_size)));
			break;

		case WIND_BLACKMAN_74:
			for (i = 0; i < window_size; i++)
				window[i] = (0.402217f - 0.49703f * cos(FFTW_TWOPI * ((double) i / (double) window_size)) + 0.09892f * cos(FFTW_FOURPI * ((double) i / (double) window_size)) - 0.00188 * cos(FFTW_THREEPI * ((double) i / (double) window_size)));
		break;

		case WIND_BLACKMAN_92:
			for (i = 0; i < window_size; i++)
				window[i] = (0.35875f - 0.48829f * cos(FFTW_TWOPI * ((double) i / (double) window_size)) + 0.14128f * cos(FFTW_FOURPI * ((double) i / (double) window_size)) - 0.01168 * cos(FFTW_THREEPI * ((double) i / (double) window_size)));
			break;

		case WIND_BLACKMAN_HARRIS:
			for (i = 0; i < window_size; i++)
				window[i] = 0.35875 - (0.48829 * cos(FFTW_TWOPI * ((double) i / (double) window_size))) + (0.14128 * cos(FFTW_FOURPI * ((double) i / (double) window_size))) - (0.01168 * cos(FFTW_SIXPI * ((double) i / (double) window_size)));
			break;

		case WIND_FLAT_TOP:
			for (i = 0; i < window_size; i++)
				window[i] = 0.2810639 - (0.5208972 * cos(FFTW_TWOPI * ((double) i / (double) window_size))) + (0.1980399 * cos(FFTW_FOURPI * ((double) i / (double) window_size)));
			break;
	}
	
	// Calculate the gain of the window
	
	for (i = 0, gain = 0.0; i < window_size; i++)
		gain += (double) window[i];

	scale = (float) (1.0 / gain);
	
	for (i = 0; i < window_size; i++)
		window[i] *= scale;
		
	// Calculate the energy gain of the window

	// Copy window 

	for (i = 0; i < window_size; i++)
		raw_frame[i] = window[i] * sin((8 * FFTW_TWOPI * (double) i) / (double) fft_size);
		
	// Do fft with zero padding
		
	hisstools_rfft(fft_setup_real, raw_frame, &raw_fft_frame, window_size, fft_size_log2);
    
	// Zero nyquist bin (have to match what is done elsewhere)
	
	raw_fft_frame.imagp[0] = 0.f;
	
	// Calulate square amplitudes
		
	for (i = 0; i < fft_size_halved >> 2; i++)
		v_amplitudes[i] = F32_VEC_ADD_OP(F32_VEC_MUL_OP(real_data[i], real_data[i]), F32_VEC_MUL_OP(imag_data[i], imag_data[i]));
		
	// Now sum 

	for (i = 0, gain = 0.0; i < fft_size_halved; i++)
		gain += amplitudes[i];
		
	// N.B. - the window has been scaled for amplitude gain
	
	x->energy_compensation = 1.0 / gain;
}



// Zero Ring Buffers



void descriptors_zero_ring_buffers (t_descriptors *x, long fft_size)
{
	double *cumulate_buffer = x->cumulate;
	float *ring_buffer = x->amps_buffer;
	long i, j;
	
	for (j = 0; j < RING_BUFFER_SIZE; j++)
	{
		for (i = 0; i < fft_size; i++)
			*ring_buffer++ = 0.f;
		for (i = 0; i < fft_size >> 1; i++)
			*ring_buffer++ = (float) LOG_MIN;
	}
	
	for (i = 0; i < fft_size * RING_BUFFER_SIZE; i++)
		*cumulate_buffer++ = 0.;
}


// Energy Threshold


void descriptors_energy_thresh (t_descriptors *x, t_symbol *msg, short argc, t_atom *argv)
{
	if (argc)
	{
		x->energy_thresh = dbtoa(atom_getfloat(argv) * 2);
		x->use_energy_thresh = 1;
	}
	else 
	{
		x->use_energy_thresh = 0;
	}
	
	if (argc > 2)
		error("descriptors(rt)~: too many arguments to energythresh message");
}



// Calculate Raw Per Frame descriptors


double calc_pf_descriptor(t_descriptors *x, float *raw_frame, float *windowed_frame, FFT_SPLIT_COMPLEX_F raw_fft_frame, long frame_pointer, long num_samps, long fft_size, double **param_ptr)
{
	// This routine calls the specified individual per frame descriptor routine with the correct variables, according to the parameter data at param_ptr
	
	long num_bins = fft_size >> 1;
	long min_bin, max_bin;
	long median_span;
	long N;
	
	double *params = *param_ptr;
	double descriptor = DBL_MAX;
	double sr = x->sr;
	double bin_freq = sr / (double) (2 * num_bins);
	double one_over_bin_freq = 1. / bin_freq;
	double threshold;
	double confidence_ret;
	
	float *this_frame = x->amps_buffer + (3 * num_bins * frame_pointer);
	float *sq_amplitudes = this_frame;
	float *amplitudes = this_frame + num_bins;
	float *log_amplitudes = this_frame + (2 * num_bins);
	
	double *cumulateAmps = x->cumulate + (fft_size * frame_pointer);
	double *cumulate_sq_amps = cumulateAmps + num_bins;
	
	float *freqs = (float *) x->n_data;
	float *amps = freqs + num_bins;
	char *mask = (char *) ((float *) amps + fft_size);
	
	float *median_amplitudes = (float *) x->median_memory;
	long *median_indices = (long *) ((float *) x->median_memory + num_bins);
	
	float *ac_coefficients = (float *) x->ac_memory;
	double *loudness_curve = x->loudness_curve;
	double *log_freq = x->log_freq;
	
	float *vals_ptr;
	float *frame1, *frame2;
	float *log_frame1, *log_frame2;
	double *cumulate_ptr1, *cumulate_ptr2;
	
	long past_frame_pointer;
	
	char norm_flag;
	char square_flag;
	char forward_only;
	char weight_frame2;
	
    enum PFDescriptorType descriptor_type = (enum PFDescriptorType) params[0];
	
	switch (descriptor_type)
	{
		case DESCRIPTOR_PF_ENERGY:
		
			if (get_bin_range(&min_bin, &max_bin, params + 1, one_over_bin_freq, num_bins))
			{
				//descriptor = get_energy(cumulate_sq_amps, min_bin, max_bin, x->energy_compensation);
                descriptor = statSumSquares(amplitudes + min_bin, max_bin - min_bin) * x->energy_compensation;
				if (params[3]) descriptor = pow_to_db (descriptor);	
			}
			
			*param_ptr += 4;
			break;
    
		case DESCRIPTOR_PF_ENERGY_RATIO:
		
			if (get_bin_range(&min_bin, &max_bin, params + 1, one_over_bin_freq, num_bins))
                descriptor = statSumSquares(amplitudes + min_bin, max_bin - min_bin) / statSumSquares(amplitudes, num_bins);
                
                //descriptor = get_energy_ratio(cumulate_sq_amps, num_bins, min_bin, max_bin);
			
			*param_ptr += 3;
			break;
				
		case DESCRIPTOR_PF_SPECTRAL_ROLLOFF:
			
			threshold = params[1];
			descriptor = get_spectral_rolloff(cumulate_sq_amps, num_bins, bin_freq, threshold);			
			
			*param_ptr += 2;
			break;
				
		case DESCRIPTOR_PF_SPECTRAL_CREST:
		
            // FIX - this should be based on the amplitudes, not the energy
            
			if (get_bin_range(&min_bin, &max_bin, params + 1, one_over_bin_freq, num_bins))
                descriptor = statCrest(amplitudes + min_bin, max_bin - min_bin);
				//descriptor = get_spectral_crest(amplitudes, cumulate_amplitudes, min_bin, max_bin);
			
			if (params[3]) descriptor = pow_to_db (descriptor);
			
			*param_ptr += 4;
			break;
				
		case DESCRIPTOR_PF_FLUX:
		
			past_frame_pointer = (frame_pointer - (long) params[6]);
			frame1 = x->amps_buffer + (3 * num_bins * past_frame_pointer) + num_bins;
			frame2 = amplitudes;
			while (past_frame_pointer < 1)
				past_frame_pointer += RING_BUFFER_SIZE;
			past_frame_pointer %= RING_BUFFER_SIZE;
			cumulate_ptr1 = x->cumulate + (fft_size * past_frame_pointer);
			cumulate_ptr2 = cumulateAmps;
			forward_only = (char) params[3];
			square_flag = (char) params[4];
			norm_flag = (char) params[5];
			
			if (get_bin_range(&min_bin, &max_bin, params + 1, one_over_bin_freq, num_bins))
				descriptor = get_flux(frame1, frame2, cumulate_ptr1, cumulate_ptr2, min_bin, max_bin, norm_flag, square_flag, forward_only);
				
			*param_ptr += 7;
			break;
				
		case DESCRIPTOR_PF_MKL:
			
			past_frame_pointer = (frame_pointer - (long) params[7]);
			while (past_frame_pointer < 1)
				past_frame_pointer += RING_BUFFER_SIZE;
			past_frame_pointer %= RING_BUFFER_SIZE;
			log_frame1 = x->amps_buffer + (3 * num_bins * past_frame_pointer) + (2 * num_bins);
			log_frame2 = log_amplitudes;
			vals_ptr = amplitudes;
			cumulate_ptr1 = x->cumulate + (fft_size * past_frame_pointer);
			cumulate_ptr2 = cumulateAmps;
			threshold = params[3];
			forward_only = (char) params[4];
			weight_frame2 = (char) params[5];
			norm_flag = (char) params[6];
			
			if (get_bin_range(&min_bin, &max_bin, params + 1, one_over_bin_freq, num_bins))
				descriptor = get_mkl(log_frame1, log_frame2, vals_ptr, cumulate_ptr1, cumulate_ptr2, min_bin, max_bin, norm_flag, forward_only, weight_frame2, threshold);

			*param_ptr += 8;
			break;
				
		case DESCRIPTOR_PF_FOOTE:
			
			past_frame_pointer = (frame_pointer - (long) params[4]);
			while (past_frame_pointer < 1)
				past_frame_pointer += RING_BUFFER_SIZE;
			past_frame_pointer %= RING_BUFFER_SIZE;
			frame1 = x->amps_buffer + (3 * num_bins * past_frame_pointer) + num_bins;
			frame2 = amplitudes;
			forward_only = (char) params[3];
			
			if (get_bin_range(&min_bin, &max_bin, params + 1, one_over_bin_freq, num_bins))
				descriptor = get_foote(frame1, frame2, min_bin, max_bin, forward_only);

			*param_ptr += 5;
			break;
		
		case DESCRIPTOR_PF_AVERAGE_AMP_ABS:
		
            descriptor = statSumAbs(raw_frame, num_samps);
			//descriptor = get_average_amp_abs(raw_frame, num_samps);
			if (params[1]) 
				descriptor = atodb (descriptor);
			
			*param_ptr += 2;
			break;
		
		case DESCRIPTOR_PF_AVERAGE_AMP_RMS:
		
			descriptor = statRMS(raw_frame, num_samps);
            // descriptor = get_average_amp_rms(raw_frame, num_samps);
			if (params[1])
				descriptor = atodb (descriptor);
			
			*param_ptr += 2;
			break;
		
		case DESCRIPTOR_PF_PEAK_AMP:
		
            // descriptor = get_amp_peak(raw_frame, num_samps);
            descriptor = statMax(raw_frame, num_samps);
			if (params[1]) 
				descriptor = atodb (descriptor);
			
			*param_ptr += 2;
			break;
		
		case DESCRIPTOR_PF_LOUDNESS:
		
			//descriptor = get_loudness(sq_amplitudes, loudness_curve, num_bins, x->energy_compensation);
			descriptor = statWeightedSum(sq_amplitudes, loudness_curve, num_bins) * x->energy_compensation;
            
			if (params[1]) 
				descriptor = pow_to_db(descriptor);
			else 
				descriptor = sqrt(descriptor);
			
			*param_ptr += 2;
			break;
		
		case DESCRIPTOR_PF_CENTROID_LIN: 
		case DESCRIPTOR_PF_SPREAD_LIN:
		case DESCRIPTOR_PF_SKEWNESS_LIN:
		case DESCRIPTOR_PF_KURTOSIS_LIN:
					
			vals_ptr = amplitudes;
			cumulate_ptr1 = cumulateAmps;
		
			if (get_bin_range(&min_bin, &max_bin, params + 1, one_over_bin_freq, num_bins))
			{
                if (descriptor_type == DESCRIPTOR_PF_CENTROID_LIN)
                    descriptor = statCentroid(amplitudes + min_bin, max_bin - min_bin);
                else if (descriptor_type == DESCRIPTOR_PF_SPREAD_LIN)
                    descriptor = statSpread(amplitudes + min_bin, max_bin - min_bin);
                else if (descriptor_type == DESCRIPTOR_PF_SKEWNESS_LIN)
                    descriptor = statSkewness(amplitudes + min_bin, max_bin - min_bin);
                else
                    descriptor = statKurtosis(amplitudes + min_bin, max_bin - min_bin);
                
				//descriptor = get_shape_lin(x, vals_ptr, cumulate_ptr1, min_bin, max_bin, descriptor_type);
				if (descriptor_type == DESCRIPTOR_PF_CENTROID_LIN || descriptor_type == DESCRIPTOR_PF_SPREAD_LIN) descriptor *= bin_freq;			// Conversion to Linear Frequency								
			}
			
			*param_ptr += 3;
			break;
		
		case DESCRIPTOR_PF_CENTROID_LOG: 
		case DESCRIPTOR_PF_SPREAD_LOG:
		case DESCRIPTOR_PF_SKEWNESS_LOG:
		case DESCRIPTOR_PF_KURTOSIS_LOG:
				
			// Calculate the log centroid
			
			vals_ptr = amplitudes;
			cumulate_ptr1 = cumulateAmps;
			
			if (get_bin_range(&min_bin, &max_bin, params + 1, one_over_bin_freq, num_bins))
			{
				descriptor = get_shape_log(x, vals_ptr, cumulate_ptr1, log_freq, min_bin, max_bin, descriptor_type);
				if (descriptor_type == DESCRIPTOR_PF_CENTROID_LOG) descriptor = exp2(descriptor);
			}
							
			*param_ptr += 3;
			break;
		
		case DESCRIPTOR_PF_BRIGHTNESS_LIN: 
		
			vals_ptr = amplitudes;
			cumulate_ptr1 = cumulateAmps;
			
			threshold = params[3];
		
			if (get_bin_range(&min_bin, &max_bin, params + 1, one_over_bin_freq, num_bins))
				descriptor = get_brightness_lin(x, raw_frame, ac_coefficients, vals_ptr, cumulate_ptr1, num_bins, min_bin, max_bin, bin_freq, threshold);
			
			*param_ptr += 4;
			break;
		
		case DESCRIPTOR_PF_BRIGHTNESS_LOG: 
		
			vals_ptr = amplitudes;
			cumulate_ptr1 = cumulateAmps;
			
			threshold = params[3];
		
			if (get_bin_range(&min_bin, &max_bin, params + 1, one_over_bin_freq, num_bins))
				descriptor = get_brightness_log(x, raw_frame, ac_coefficients, vals_ptr, cumulate_ptr1, log_freq, num_bins, min_bin, max_bin, threshold);
			
			*param_ptr += 4;
			break;
	
		case DESCRIPTOR_PF_SPECTRAL_FLATNESS:
						
            // FIX - should be based on amplitudes not energy
			cumulate_ptr1 = cumulate_sq_amps;
			
			if (get_bin_range(&min_bin, &max_bin, params + 1, one_over_bin_freq, num_bins))
                descriptor = statFlatness(amplitudes + min_bin, max_bin - min_bin);
            //descriptor = get_sfm(log_amplitudes, cumulate_ptr1, min_bin, max_bin);
			
			*param_ptr += 3;
			break;
		
		case DESCRIPTOR_PF_NOISE_RATIO:
		case DESCRIPTOR_PF_NON_NOISE_RATIO:
	
			median_span = (long) params[1];
			cumulate_ptr1 = cumulate_sq_amps;
			
			descriptor =  get_noise_ratio(x, median_indices, median_amplitudes, amplitudes, sq_amplitudes, cumulate_ptr1, log_freq, num_bins, median_span);					
			if (descriptor_type == DESCRIPTOR_PF_NON_NOISE_RATIO && descriptor != DBL_MAX)
				descriptor = 1. - descriptor;
			
			*param_ptr += 2;
			break;
	
		case DESCRIPTOR_PF_PITCH:
		case DESCRIPTOR_PF_PITCH_CONFIDENCE:
	
			threshold = params[1];
		
			descriptor = sr / get_pitch (x, raw_frame, ac_coefficients, num_bins, threshold, &confidence_ret);
			if (descriptor_type == DESCRIPTOR_PF_PITCH_CONFIDENCE) descriptor = confidence_ret;
							
			*param_ptr += 2;
			break;
				
		case DESCRIPTOR_PF_INHARMONICITY:
					
			N = (long) params[1];
			median_span = (long) params[2];
			threshold = params[3];
							
			descriptor = get_inharmonicity(x, median_indices, median_amplitudes, amplitudes, log_amplitudes, raw_frame, ac_coefficients, freqs, amps, mask, num_bins, N, median_span, bin_freq, threshold);				
			
			*param_ptr += 4;
			break;
			
		case DESCRIPTOR_PF_ROUGHNESS:
	
			N = (long) params[1];
			median_span = (long) params[2];
		
			spectralpeaks_medianmask_float (x, median_indices, median_amplitudes, amplitudes, log_amplitudes, median_span, num_bins, mask, N, freqs, amps, bin_freq, (-90. / 20.) * log(10.));
			descriptor = get_roughness (freqs, amps, N);

			*param_ptr += 3;
			break;
				
		case DESCRIPTOR_PF_SPECTRAL_PEAKS:
	
			N = (long) params[1];
			median_span = (long) params[2];
			
			spectralpeaks_medianmask_float (x, median_indices, median_amplitudes, amplitudes, log_amplitudes, median_span, num_bins, mask, N, freqs, amps, bin_freq, (-90. / 20.) * log(10.));
		
			// NB - return DBL_MAX here - it's not a valid single descriptor
			
			*param_ptr += 3;
			break;
			
		case DESCRIPTOR_PF_NONE:
			break;
	}
		
	if (!std::isfinite(descriptor)) descriptor = DBL_MAX;
		return descriptor;
}



// Curves Dependent on sr and FFT SIze 



void calc_curves (t_descriptors *x)
{
	long fft_size_halved = x->fft_size >> 1;
	double bin_freq = x->sr / (double) x->fft_size;
	
	
	calc_log_freq_curve (x->log_freq, fft_size_halved, bin_freq);
	calc_loudness_curve (x->loudness_curve, fft_size_halved, bin_freq);
}


void calc_log_freq_curve (double *log_freq, long num_bins, double bin_freq)
{
	long i;
		
	log_freq[0] = log2(0.00001);
	
	for (i = 1; i < num_bins; i++)
		log_freq[i] = log2((double) i * bin_freq);
}


void calc_loudness_curve (double *loudness_curve, long num_bins, double bin_freq)
{
	long i;
	double f;
	double freq_mult = bin_freq / 1000.;
	
	for (i = 0; i < num_bins; i++)
	{
		f = i * freq_mult;
		loudness_curve[i] = pow(10., (-3.64 * pow(f, -0.8) + 6.5 * exp(-0.6 * (f - 3.3) * (f - 3.3)) - (0.001 * (f * f * f * f))) / 20.);
	}
	
	// Now square it to get a power curve (so we can do a straight multiply later)
	
	for (i = 0; i < num_bins; i++)
		loudness_curve[i] *= loudness_curve[i];
}

