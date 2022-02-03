
/*
 *  descriptors_per_frame_descriptors.c
 *
 *	These routines calculate the raw descriptors given the fft data or raw input frame for the descriptors~ and descriptorsrt~ objects.
 *	The routines are commented to some extent, where it is helpful. However, in order to understand the dsp better the reader is refered to the following paper:
 *
 *	http://recherche.ircam.fr/equipes/analyse-synthese/peeters/ARTICLES/Peeters_2003_cuidadoaudiofeatures.pdf
 *
 *	Although the algorithms here are not all taken from this paper, it presents most of the audio feature algorithms utilised (as well as several additional ones) in mathematical form.
 *
 *  Copyright 2010 Alex Harker (except the roughness routine - see below and http://www.uni-graz.at/richard.parncutt/computerprograms.html ). All rights reserved.
 *
 */


#include "descriptors_per_frame_descriptors.h"



// Energy 



double get_energy(double *cumulate_ptr, long min_bin, long max_bin, double energy_compensation)
{ 
	if (min_bin) 
		return (cumulate_ptr[max_bin - 1] - cumulate_ptr[min_bin - 1]) * energy_compensation;
	
	return cumulate_ptr[max_bin - 1] * energy_compensation;
}



// Energy Ratio 



double get_energy_ratio(double *cumulate_ptr, long num_bins, long min_bin, long max_bin)
{
	double sum1 = get_energy(cumulate_ptr, min_bin, max_bin, 1.);		
	double sum2 = cumulate_ptr[num_bins - 1];
	
	if (sum2) 
		return (sum1 / sum2);
	else 
		return DBL_MAX;
}



// Rolloff 



double get_spectral_rolloff(double *cumulate_ptr, long num_bins, double bin_freq, double threshold)
{
	double sum = cumulate_ptr[num_bins - 1] * threshold;
	long i;	
	
	if (sum) 
	{
		for (i = 0; i < num_bins; i++)
			if (cumulate_ptr[i] > sum) 
				break;
		
		return i * bin_freq;
	}
	
	return DBL_MAX;
}



// Spectral Crest 



double get_spectral_crest(float *vals_ptr, double *cumulate_ptr, long min_bin, long max_bin)
{
	double sum = cumulate_ptr[max_bin - 1];
	double max = 0.;
	double current_val;
	long i;
	
	if (min_bin) 
		sum -= cumulate_ptr[min_bin - 1];
	
	for (i = min_bin; i < max_bin; i++)
	{
		current_val = vals_ptr[i];
		if (current_val > max) max = current_val;
	}
	
	if (sum) 
		return (max * (double) (max_bin - min_bin))  / sum;
	
	return DBL_MAX;
}



// Spectral Flux Distance Measure 



double get_flux(float *frame1, float *frame2, double *cumulate_ptr1, double *cumulate_ptr2, long min_bin, long max_bin, char norm_flag, char square_flag, char forward_only)
{
	double norm_factor1 = 1.;
	double norm_factor2 = 1.;
	double current_val;
	double sum = 0;
	long i;
	
	if (norm_flag)
	{
		norm_factor1 = cumulate_ptr1[max_bin - 1];
		if (min_bin) 
			norm_factor1 -= cumulate_ptr1[min_bin - 1];
		norm_factor2 = cumulate_ptr2[max_bin - 1];
		if (min_bin) 
			norm_factor2 -= cumulate_ptr2[min_bin - 1];
	}
	
	if (norm_factor1) 
		norm_factor1 = 1. / norm_factor1;
	else 
		norm_factor1 = 1.;
	
	if (norm_factor2) 
		norm_factor2 = 1. / norm_factor2;
	else 
		norm_factor2 = 1.;
	
	if (square_flag)																		
	{		
		if (forward_only)																	
		{
			// Forward changes only using square amplitudes
			
			for (i = min_bin; i < max_bin; i++)	
			{
				current_val = ((frame2[1] * norm_factor2) - (frame1[i] * norm_factor1));
				if (current_val < 0.) current_val = 0.;
				current_val *= current_val;
				sum += current_val;
			}
		}
		else
		{
			// Both changes using square amplitudes
			
			for (i = min_bin; i < max_bin; i++)	
			{
				current_val = ((frame2[1] * norm_factor2) - (frame1[i] * norm_factor1));
				current_val *= current_val;
				sum += current_val;
			}
		}
	}
	else
	{
		if (forward_only)
		{
			// Forward changes only using amplitudes
			
			for (i = min_bin; i < max_bin; i++)	
			{
				current_val = ((frame2[1] * norm_factor2) - (frame1[i] * norm_factor1));
				if (current_val < 0.) current_val = 0.;
				sum += current_val;	
			}
		}
		else
		{
			// Both changes using amplitudes
			
			for (i = min_bin; i < max_bin; i++)	
			{
				current_val = ((frame2[1] * norm_factor2) - (frame1[i] * norm_factor1));
				sum += current_val;
			}
		}
	}
	
	if (square_flag) 
		sum = sqrt(sum);
	
	return sum;
}



// Modified Kullback-Leibler Spectral Distance Measure 



double get_mkl(float *log_frame1, float *log_frame2, float *frame2, double *cumulate_ptr1, double *cumulate_ptr2, long min_bin, long max_bin, char norm_flag, char forward_only, char weight_frame2, double log_thresh)
{
	double norm_factor1 = 1.;
	double norm_factor2 = 1.;
	double current_val;
	double sum = 0;
	double log_norm_factor;
	long i;
	
	if (norm_flag)
	{
		norm_factor1 = cumulate_ptr1[max_bin - 1];
		if (min_bin) 
			norm_factor1 -= cumulate_ptr1[min_bin - 1];
		norm_factor2 = cumulate_ptr2[max_bin - 1];
		if (min_bin) 
			norm_factor2 -= cumulate_ptr2[min_bin - 1];
		if (!norm_factor1) norm_factor1 = sqrt(POW_MIN) * (double) (max_bin - min_bin);
	}
	
	log_norm_factor = log(norm_factor1 / norm_factor2);
	
	if (log_norm_factor < MKL_EQUALISE_MAX_LOG) 
		log_norm_factor = MKL_EQUALISE_MAX_LOG;
	
	if (norm_factor2) 
		norm_factor1 = 1. / norm_factor2;
	else 
		norm_factor2 = 1.;
	
	if (weight_frame2)
	{
		if (forward_only)
		{
			// Forward changes only weighting by the second frame
			
			for (i = min_bin; i < max_bin; i++)	
			{
				current_val = log_frame2[i] - log_frame1[i];
				current_val += log_norm_factor;
				if (current_val > 0 && log_frame2[i] >= log_thresh)
					sum += current_val * frame2[i] * norm_factor2;
			}
		}
		else
		{
			// Both changes weighting by the second frame
			
			for (i = min_bin; i < max_bin; i++)	
			{
				current_val = log_frame2[i] - log_frame1[i];
				current_val += log_norm_factor;
				if (log_frame2[i] >= log_thresh)
					sum += current_val * frame2[i] * norm_factor2;
			}
		}
	}
	else
	{
		if (forward_only)
		{
			// Forward changes only
			
			for (i = min_bin; i < max_bin; i++)	
			{
				current_val = log_frame2[i] - log_frame1[i];
				current_val += log_norm_factor;
				if (current_val > 0 && log_frame2[i] >= log_thresh)
					sum += current_val;
			}
		}
		else
		{
			// Both changes
			
			for (i = min_bin; i < max_bin; i++)	
			{
				current_val = log_frame2[i] - log_frame1[i];
				current_val += log_norm_factor;
				if (log_frame2[i] >= log_thresh)
					sum += current_val;
			}
		}
	}
	
	return sum / (double) (2 * (max_bin - min_bin));
}



// Foote Spectral Distance Measure 



double get_foote(float *frame1, float *frame2, long min_bin, long max_bin, char forward_only)
{
	double frame1_temp;
	double frame2_temp;
	double norm_sum1 = 0.;
	double norm_sum2 = 0.;
	double sum = 0;
	long i;
	
	if (forward_only)
	{
		// Forward changes only
		
		for (i = min_bin; i < max_bin; i++)
		{
			frame1_temp = frame1[i];
			frame2_temp = frame2[i];
			if (frame2_temp > frame1_temp)
			{
				sum +=frame1_temp * frame2_temp;
				norm_sum1 += frame1_temp * frame1_temp;
				norm_sum2 += frame2_temp * frame2_temp;
			}
		}
	}
	else
	{
		// Both changes
		
		for (i = min_bin; i < max_bin; i++)
		{
			frame1_temp = frame1[i];
			frame2_temp = frame2[i];
			sum +=frame1_temp * frame2_temp;
			norm_sum1 += frame1_temp * frame1_temp;
			norm_sum2 += frame2_temp * frame2_temp;
		}
	}
	
	if (norm_sum1 && norm_sum2)
		return 1. - (sum / sqrt(norm_sum1 * norm_sum2));
	if (norm_sum2) 
		return 1.;
	
	return DBL_MAX;
}



// Average Amplitude ABS 



double get_average_amp_abs(float *raw_frame, long num_samps)
{
	double sum = 0;
	long i;
	
	for (i = 0; i < num_samps; i++)
		sum += fabs(raw_frame[i]);
	
	return sum / (double) num_samps;
}



// Average Amplitude RMS 



double get_average_amp_rms(float *raw_frame, long num_samps)
{
	double sum = 0;
	long i;
	
	for (i = 0; i < num_samps; i++)
		sum += raw_frame[i] * raw_frame[i];
	
	return sqrt(sum / (double) num_samps);
}



// Peak Amplitude ABS 



double get_amp_peak(float *raw_frame, long num_samps)
{
	double current_val;
	double peak = 0.;
	long i;
	
	for (i = 0; i < num_samps; i++)
	{
		current_val = fabs(raw_frame[i]);
		if (current_val > peak)
			peak = current_val;
	}
	
	return peak;
}



// Loudness 



double get_loudness(float *sq_amplitudes, double *loudness_curve, long num_bins, double energy_compensation)	
{
	double sum = 0;
	long i;
	
	for (i = 0; i < num_bins; i++)
		sum += loudness_curve[i] * sq_amplitudes[i];
	
	return sum * energy_compensation;
}



// Shape Linear 



double get_shape_lin(t_descriptors *x, float *vals_ptr, double *cumulate_ptr, long min_bin, long max_bin, enum PFDescriptorType shape_type)
{
	double current_val;
	double sum1 = 0;
	double sum2 = 0;
	double mean;
	double spread;
	long i;
	
	// Calculate the lin centroid
	
	sum2 = cumulate_ptr[max_bin - 1];
	if (min_bin) 
		sum2 -= cumulate_ptr[min_bin - 1];
	
	if (!sum2) 
		return DBL_MAX;	
	
	if (!x->centroid_lin_flag || x->last_min_bin_lin != min_bin || x->last_max_bin_lin != max_bin)
	{
		for (i = min_bin; i < max_bin; i++)
			sum1 += vals_ptr[i] * (double) i;
		
		mean = (sum1 / sum2);
		sum1 = 0.;
		
		x->centroid_lin_flag = 1;
		x->last_centroid_lin = mean;
		x->last_min_bin_lin = min_bin;
		x->last_max_bin_lin = max_bin;
		x->spread_lin_flag = 0;
	}
	else 
	{
		mean = x->last_centroid_lin;
	}
	
	if (shape_type == DESCRIPTOR_PF_CENTROID_LIN) 
		return mean;											
	
	// Spread etc. (lin)
	
	if (!x->spread_lin_flag)
	{
		for (i = min_bin; i < max_bin; i++)
		{
			current_val = ((double) i - mean);
			sum1 += vals_ptr[i] * current_val * current_val;
		}
		
		spread = sqrt(sum1 / sum2);
		sum1 = 0.;
		
		x->spread_lin_flag = 1;
		x->last_spread_lin = spread;
	}
	else 
	{
		spread = x->last_spread_lin;
	}
	
	if (shape_type == DESCRIPTOR_PF_SPREAD_LIN) 
		return spread * spread;
	
	if (shape_type == DESCRIPTOR_PF_SKEWNESS_LIN)
	{
		for (i = min_bin; i < max_bin; i++)
		{
			current_val = ((double) i - mean);
			sum1 += vals_ptr[i] * current_val * current_val * current_val;
		}
		
		return sum1 / (spread * spread * spread * sum2);
	}
	
	if (shape_type == DESCRIPTOR_PF_KURTOSIS_LIN)
	{	
		for (i = min_bin; i < max_bin; i++)
		{
			current_val = ((double) i - mean);
			sum1 += vals_ptr[i] * current_val * current_val * current_val * current_val;
		}
		
		return sum1 / (spread * spread * spread * spread * sum2);
	}
	
	// This will happen iff the shape_type is incorrectly defined
	
	return DBL_MAX;				
}



// Shape Log 



double get_shape_log(t_descriptors *x, float *vals_ptr, double *cumulate_ptr, double *log_freq, long min_bin, long max_bin, enum PFDescriptorType shape_type)
{
	double current_val;
	double sum1 = 0;
	double sum2 = 0;
	double mean;
	double spread;
	long i;
	
	// Calculate the log centroid
	
	sum2 = cumulate_ptr[max_bin - 1];
	if (min_bin) 
		sum2 -= cumulate_ptr[min_bin - 1];
	
	if (!sum2) 
		return DBL_MAX;
	
	if (!x->centroid_log_flag || x->last_min_bin_log != min_bin || x->last_max_bin_log != max_bin)
	{
		for (i = min_bin; i < max_bin; i++)
			sum1 += vals_ptr[i] * log_freq[i];
		
		mean = (sum1 / sum2);
		sum1 = 0.;
		
		x->centroid_log_flag = 1;
		x->last_centroid_log = mean;
		x->last_min_bin_log = min_bin;
		x->last_max_bin_log = max_bin;
		x->spread_log_flag = 0;
	}
	else 
	{
		mean = x->last_centroid_log;
	}
	
	if (shape_type == DESCRIPTOR_PF_CENTROID_LOG) 
		return mean;					
	
	// Spread etc. (log)
	
	if (!x->spread_log_flag)
	{
		for (i = min_bin; i < max_bin; i++)
		{
			current_val = log_freq[i] - mean;
			sum1 += vals_ptr[i] * current_val * current_val;
		}
		
        // NOTE - spread should be the variance, whereas here we report the standard deviation or similar...
        
		spread = sqrt(sum1 / sum2);
		sum1 = 0.;
		
		x->spread_log_flag = 1;
		x->last_spread_log = spread;
	}
	else 
	{
		spread = x->last_spread_log;
	}
	
	if (shape_type == DESCRIPTOR_PF_SPREAD_LOG) 
		return spread;
	
	if (shape_type == DESCRIPTOR_PF_SKEWNESS_LOG)
	{
		for (i = min_bin; i < max_bin; i++)
		{
			current_val = ((double) log_freq[i] - mean);
			sum1 += vals_ptr[i] * current_val * current_val * current_val;
		}
		
        // NOTE - shouldn't we divide by sum2 here (see also above)?
        
		return sum1 / (spread * spread * spread);
	}
	
	if (shape_type == DESCRIPTOR_PF_KURTOSIS_LOG)
	{	
		for (i = min_bin; i < max_bin; i++)
		{
			current_val = ((double) log_freq[i] - mean);
			sum1 += vals_ptr[i] * current_val * current_val * current_val * current_val;
		}
		
        // NOTE - shouldn't we divide by sum2 here (see also above)?

		return sum1 / (spread * spread * spread * spread);
	}
	
	// This will happen iff the shape_type is incorrectly defined
	
	return DBL_MAX;
}



// Brightness Linear 



double get_brightness_lin(t_descriptors *x, float *raw_frame, float *ac_coefficients, float * vals_ptr, double *cumulate_ptr, long num_bins, long min_bin, long max_bin, double bin_freq, double threshold)
{
	double confidence_ret;
	double pitch = x->sr / get_pitch (x, raw_frame, ac_coefficients, num_bins, threshold, &confidence_ret);
	double centroid = get_shape_lin(x, vals_ptr, cumulate_ptr, min_bin, max_bin, DESCRIPTOR_PF_CENTROID_LIN) * bin_freq;
	if (pitch > 10. && pitch != DBL_MAX && centroid != DBL_MAX) 
		return centroid / pitch;
	
	return DBL_MAX;
}



// Brightness Log 



double get_brightness_log(t_descriptors *x, float *raw_frame, float *ac_coefficients, float * vals_ptr, double *cumulate_ptr, double *log_freq, long num_bins, long min_bin, long max_bin, double threshold)
{
	double confidence_ret;
	double pitch = x->sr / get_pitch (x, raw_frame, ac_coefficients, num_bins, threshold, &confidence_ret);
	double centroid = exp2(get_shape_log(x, vals_ptr, cumulate_ptr, log_freq, min_bin, max_bin, DESCRIPTOR_PF_CENTROID_LOG));
	if (pitch > 10. && pitch != DBL_MAX && centroid != DBL_MAX) 
		return centroid / pitch;
	
	return DBL_MAX;
}



// Spectral Flatness Measure 



double get_sfm(float *log_amplitudes, double *cumulate_ptr, long min_bin, long max_bin)
{
	// Using log summing here improves range issues massively
	
	double sum1 = 0;
	double sum2 = cumulate_ptr[max_bin - 1];
	long i;
	
	if (min_bin) 
		sum2 -= cumulate_ptr[min_bin - 1];	
	
	if (sum2)
	{
		for (i = min_bin; i < max_bin; i++)
			sum1 += log_amplitudes[i] * 2.;
		
		return exp(sum1  / (max_bin - min_bin)) / (sum2 / (max_bin - min_bin));
	}
	
	return DBL_MAX;
}



// Noise Ratio 



double get_noise_ratio(t_descriptors *x, long *median_indices, float *median_amplitudes, float *amplitudes, float *sq_amplitudes, double *cumulate_ptr, double *log_freq, long num_bins, long median_span)
{
	double noise_ratio;
	double sum1 = 0;
	double sum2 = 0;
	long i;
	
	sum2 = cumulate_ptr[num_bins = 1];
	
	if (sum2)
	{
		medianfilter_float (x, median_indices, median_amplitudes, amplitudes, num_bins, median_span);
		
		for (i = 0; i < num_bins; i++)
			sum1 += median_amplitudes[i] * median_amplitudes[i];
		
		noise_ratio =  (sum1 / sum2);
		if (noise_ratio > 1.) 
			noise_ratio = 1.;
		
		return noise_ratio;
	}
	
	return DBL_MAX;
}



// Inharmonicity 



double get_inharmonicity(t_descriptors *x, long *median_indices, float *median_amplitudes, float *amplitudes, float *log_amplitudes, float *raw_frame, float *ac_coefficients, float *freqs, float *amps, char *mask, long num_bins, long N, long median_span, double bin_freq, double threshold)
{
	double sum1 = 0;
	double sum2 = 0;
	double current_val;
	double divergence;
	double pitch;
	long i;
	
	// Calculate the spectral peaks and pitch
	
	spectralpeaks_medianmask_float (x, median_indices, median_amplitudes, amplitudes, log_amplitudes, median_span, num_bins, mask, N, freqs, amps, bin_freq, (-90. / 20.) * log(10.));
	pitch = x->sr / get_pitch (x, raw_frame, ac_coefficients, num_bins, threshold, &current_val);
	
	// If there is apitch vlue calculate the inharmonicity
	
	if (pitch != DBL_MAX)
	{
		for (i = 0; i < N; i++)
		{
			current_val = freqs[i];
			
			if (current_val)
			{
				if (pitch > current_val) 
					divergence = pitch / current_val;
				else 
					divergence = current_val / pitch;
				divergence -= floor(divergence);
				if (divergence > 0.5) divergence = 1. - divergence;
				divergence *= 2.;
			}
			else
			{
				divergence = 0.;
			}
			
			current_val = amps[i]; // * amps[i];
			sum1 += current_val * divergence;
			sum2 += current_val;
		}
	}
	
	if (sum2) 
		return sum1 / sum2;
	
	return DBL_MAX;
}



// Routines for Autocorrelation Pitch Detection 



void calc_ac_coefficients(t_descriptors *x, float *raw_frame)
{
	FFT_SETUP_F fft_setup_real = x->fft_setup_real;
	
	FFT_SPLIT_COMPLEX_F full_fft_frame;
	FFT_SPLIT_COMPLEX_F half_fft_frame;

	float *ac_coefficients = (float *) x->ac_memory;
	
	long fft_size = x->fft_size;
	long fft_size_halved = fft_size >> 1;
	long fft_size_log2 = x->fft_size_log2;
	long window_size = x->window_size;
		
	double norm_factor = 0.;
	long i;
	
	full_fft_frame.realp = ac_coefficients + fft_size;
	full_fft_frame.imagp = ac_coefficients + fft_size + fft_size_halved;
	half_fft_frame.realp = ac_coefficients + fft_size + (2 * fft_size_halved);
	half_fft_frame.imagp = ac_coefficients + fft_size + (3 * fft_size_halved);
	
	if (x->ac_flag) 
		return;
	
	x->ac_flag = 1;
	
	// Calculate normalisation factor
	
	for (i = 0; i < (window_size >> 1); i++)
		norm_factor += raw_frame[i] * raw_frame[i];
	
	norm_factor = 0.25 / ((double)  fft_size * norm_factor);
	
	// Do ffts straight into position with zero padding (one half the size of the other)
	
	hisstools_rfft(fft_setup_real, raw_frame, &full_fft_frame, window_size, fft_size_log2);
	hisstools_rfft(fft_setup_real, raw_frame, &half_fft_frame, (window_size >> 1), fft_size_log2);
	
	// Calculate ac coefficients
	
	complex_mult_conjugate(full_fft_frame, half_fft_frame, window_size >> 1, (float) norm_factor);
	
	// Inverse fft
	
    hisstools_rifft(fft_setup_real, &full_fft_frame, ac_coefficients, fft_size_log2);
}


void complex_mult_conjugate(FFT_SPLIT_COMPLEX_F in1, FFT_SPLIT_COMPLEX_F in2, long length, float scale)
{
	vFloat *in_real1 = (vFloat *) in1.realp;
	vFloat *in_imag1 = (vFloat *) in1.imagp;
	vFloat *in_real2 = (vFloat *) in2.realp;
	vFloat *in_imag2 = (vFloat *) in2.imagp;
	
	vFloat vscale = {scale, scale, scale, scale};
	vFloat in_real1_temp, in_real2_temp, in_imag1_temp, in_imag2_temp;
	
	long i;
	
	float nyquist1 = in1.imagp[0];
	float nyquist2 = in2.imagp[0];
		
	in1.imagp[0] = 0.f;
	in2.imagp[0] = 0.f;	
	
	for (i = 0; i < length >> 2; i++)
	{
		in_real1_temp = in_real1[i];
		in_real2_temp = in_real2[i];
		in_imag1_temp = in_imag1[i];
		in_imag2_temp = in_imag2[i];
		
		in_real1[i] = F32_VEC_MUL_OP(F32_VEC_ADD_OP(F32_VEC_MUL_OP(in_real1_temp, in_real2_temp), F32_VEC_MUL_OP(in_imag1_temp, in_imag2_temp)), vscale);
		in_imag1[i] = F32_VEC_MUL_OP(F32_VEC_SUB_OP(F32_VEC_MUL_OP(in_imag1_temp, in_real2_temp), F32_VEC_MUL_OP(in_real1_temp, in_imag2_temp)), vscale);
	}
	
	in1.imagp[0] = (nyquist1 * nyquist2) * scale;
}


double get_pitch (t_descriptors *x, float *raw_frame, float *ac_coeff, long length, double threshold, double *confidence_ret)
{
	long end_first = length;
	long no_pitch = 0;
	long bin_freq = 0;
	long from = 0;
	long to = 0;
	long i;
	
	double corrected_bin_freq = 0.;
	double confidence = 0.;
	float max_val = 0.;
	float zero_lag;
	float current_val = 0.;
	float compare_thresh;
	
	// Calculate the coefficients and then work from the zero lag value
	
	calc_ac_coefficients (x, raw_frame);
	
	if (x->last_threshold == threshold)
	{
		*confidence_ret = x->last_confidence;
		return x->last_pitch;
	}
	
	zero_lag = ac_coeff[0];
	compare_thresh = zero_lag * threshold;
	
	// Ignore first peak
	
	for (i = 1; i < length; i++)
		if (ac_coeff[i] < compare_thresh) 
			break;
	
	end_first = i;
	
	// Find start of second peak
	
	for (; i < length; i++)
		if (ac_coeff[i] > compare_thresh) 
			break;
	
	from = i;	
	
	// Find end of second peak
	
	for (; i < length; i++)
		if (ac_coeff[i] < compare_thresh) 
			break;
	
	if (i == length)
	{
		from = end_first;
		to = length;
		no_pitch = 1;
	}
	else
	{
		to = i;
	}
	
	for (i = from; i < to; i++)
	{
		current_val = ac_coeff[i];
		
		if (current_val > max_val)
		{                      
			max_val = current_val;
			bin_freq = i;
		}
	}
	
	confidence = max_val;									
	
	// Do the correction if we have found a pitch (using parabolic interpolation)
	
	if (!no_pitch)
	{
		double prev_val = ac_coeff[bin_freq - 1];
		double next_val = ac_coeff[bin_freq + 1] ;
		double beta = 0.5 * (next_val - prev_val);
		double gamma = (2.0  * max_val) - next_val - prev_val;
		corrected_bin_freq = bin_freq + (beta / gamma);
	}
	else 
	{
		corrected_bin_freq = DBL_MAX;
	}
	
	if (confidence > 1.) 
		confidence = 1.;
	*confidence_ret = confidence;
	
	x->last_pitch = corrected_bin_freq;
	x->last_confidence = confidence;
	x->last_threshold = threshold;
	
	return corrected_bin_freq;
}



// Routine for Calculating Roughness 



// This roughness calulator takes num_sines partials (freq and amplitude pairs) - ordering is unimportant

// This code is adapated from code by Richard Parncutt, formerly of Mcgill University, and currently based at the Univeristy of Graz
// Richard Parncutt's current webpage is: http://www.uni-graz.at/richard.parncutt/
// The code can be found at: http://www.uni-graz.at/richard.parncutt/computerprograms.html
// The original comments from the code are reproduced in an adapted form below.

// Implementation of Hutchinson & Knopoff (1978), here called "H&K", which is in turn based on Plomp & Levelt (1965), here called "P&L.
// March 1993 version, by Richard Parncutt.
// The "standard curve" of P&L is represented by the functiong(x) = [e * (x/a) * exp (-x/a)] ^ i, x < 1.2 = 0, x > 1.2
// where x is the interval between two partials expressed in critical bandwidths, and a is the interval for maximum roughness (about 0.25 CBs),
// and i is an index (power) of about 2.  (NB: I [ie. Richard Parncutt]invented this function; H&K used a look-up table.) 
// Critical bandwidth CBW is given by P&L's function, as cited by H&K.


double get_roughness(float *freqs, float *amps, long num_sines)
{
	double e; 																// base of natural logarithms
	double cb_int; 															// interval between two partials in critical bandwidths
	double cbw; 															// critical bandwidth according to P&L, H&K
	double index; 															// for standard curve of P&L
	double mean_freq; 														// mean frequency of two cpts
	double numerator, denominator; 											// for calculating H&K Eq. (3)
	double ratio; 															// temporary variable
	double standard_curve; 													// P&L Fig. 10, H&K Fig. 1
	double min_amp = 0;
	
	long i, j;
	
	// Parameters for analytic version of standard curve of P&L:
	
	double cb_int0 = 0.25; 													// interval for max roughness (P&L: ca. 0.25)
	double cb_int1 = 1.2; 													// interval beyond which roughness is negligible (P&L: 1.2)
	double cb_int0_recip;
		
	index = 2.; 															// the bigger index, the narrower the curve
	e = exp(1); 															// 2.7182818 - base of natural log
	cb_int0_recip = 1. / cb_int0;
	
	numerator = 0; 
	denominator = 0;
		
	for (i = 0; i < num_sines; i++) 										
	{
		if (amps[i])
		{
			for (j = i + 1; j < num_sines; j++)								
			{
				if (amps[j])
				{
					mean_freq = (freqs[i] + freqs[j]) / 2; 
					
					// The below is from H&K p. 5
					
					cbw = 1.72 * pow (mean_freq, 0.65); 									
					cb_int = (fabsf(freqs[j] - freqs[i])) / cbw; 
					
					// (Otherwise roughness is negligible) (save computing time)
					
					if (cb_int < cb_int1)												
					{
						ratio = cb_int * cb_int0_recip;

						// Below approximates P&L
						
						standard_curve = pow((e * ratio) * exp(-ratio), index);			
						numerator += amps[i] * amps[j] * standard_curve;
					}
					denominator += amps[i] * amps[i];
					if (amps[i] < min_amp) 
						min_amp = amps[i];
				}
			}	
		}
	}
	
	// This hack allows the algorithm to work without needing the partials in a particular order
	
	denominator -= min_amp * min_amp;
	
	if (denominator)
		return (numerator / denominator);
	else
		return DBL_MAX;
}	
