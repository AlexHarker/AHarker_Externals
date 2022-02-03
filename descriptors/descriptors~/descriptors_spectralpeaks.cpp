
/*
 *  descriptors_spectralpeaks.h
 *
 *	Spectral peak-finding routines for the descriptors~ and descriptorsrt~ objects (both single and double precision).
 *	This code deals with identifying and resolving spectral peaks (which are assumed to be sinusoidal components), both for output, and also to drive various per frame descriptor calculations.
 *
 *	As noted in the median filter source code, the median filtering used here does not currently seem that necessary / effective, but has been left in for now.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */



#include "descriptors_spectralpeaks.h"



// Double precision spectral peak finding 



void spectralpeaks_double (double *amplitudes, double *median_amplitudes, long num_bins, double *freqs, double *amps, double bin_freq, double log_thresh)
{	
	double next_val = amplitudes[0];
	double current_val = 0.;
	double prev_val = 0.f;
	double amp_correction;
	long i;
	long j = 0;
	
	if (median_amplitudes)
	{
		// Find peaks with median filter to avoid noise (this could probably be made a lot more useful if there were the ability to scale the output of the median filter)

		for (i = 0; i < num_bins - 1; i++)
		{
			prev_val = current_val;
			current_val = next_val;
			next_val = amplitudes[i + 1];
			if (current_val > prev_val && current_val > next_val && current_val > median_amplitudes[i])
				freqs[j++] = i;
		}

		prev_val = current_val;
		current_val = amplitudes[i];
			
		if (current_val > prev_val && current_val > median_amplitudes[i])
			freqs[j++] = i;
	}
	else
	{
		// No median filter - find peaks

		for (i = 0; i < num_bins - 1; i++)
		{
			prev_val = current_val;
			current_val = next_val;
			next_val = amplitudes[i + 1];
			if (current_val > prev_val && current_val > next_val)
				freqs[j++] = i;
		}

		prev_val = current_val;
		current_val = amplitudes[i];
			
		if (current_val > prev_val)
			freqs[j++] = i;
	}
		
	// Peak correction

	for (i = 0; i < j; i++)
	{
		freqs[i] = (bin_freq * peakcorrection_double(amplitudes, (long) freqs[i], num_bins, &amp_correction));
		amps[i] = amp_correction;
	}
	
	// Sort peaks into oreder of descending amplitude

	combsort_peaks_double(amps, freqs, j);
	
	// Convert to linear amplitudes (note the peak correction performs a log scaling)
		
	for (i = 0; i < j; i++)
		amps[i] = exp(amps[i]);
	
	// Convert to db (later maybe make this an option for this descriptor)
	
	for (i = 0; i < j; i++)
		amps[i] = atodb(amps[i]);
	
	// Store zeros if we've run out of peaks
		
	for (i = j; i < num_bins; i++)
	{
		freqs[i] = 0.;
		amps[i] = 0.;
	}
}	


// Use parabolic interpolation to provide a more accurate estimation of the peak frequency and amplitude (double precision)

double peakcorrection_double (double *amplitudes, long bin, long num_bins, double *better_amp)
{
	double a, b, c;
	double divisor, correction;
	double better_bin;

	// Get ampllitudes
	
	if (bin) 
		a = amplitudes[bin - 1];
	else 
		a = amplitudes[1];
	
	b = amplitudes[bin];
	
	if (bin != num_bins - 1) 
		c = amplitudes[bin + 1];
	else
		c = amplitudes[num_bins - 2];
	
	// Take log values (avoiding values that are too low)
	
	a = log(a);								
	b = log(b);
	c = log(c);
	
	if (a < LOG_MIN) 
		a = LOG_MIN;								
	if (b < LOG_MIN) 
		b = LOG_MIN;
	if (c < LOG_MIN) 
		c = LOG_MIN;			
	
	// Parabolic interpolation
	
	divisor = a + c - (2.0 * b);
	
	if (divisor == 0) 
		correction = 0;
	else 
		correction = (0.5 * (a - c)) / divisor;
	
	// N.B - Leave amplitude in a log format
	
	better_bin = (double) bin + correction;
	*better_amp = b  - (0.25 * (a - c) * correction);
	
	return better_bin;
	
}



// Single precision spectral peak finding (with mdeian filtering) 



// Routine to handle single precision spectral peak finding with or without a median filter to avoid noise

void spectralpeaks_medianmask_float (t_descriptors *x, long * median_indices, float *median_amplitudes, float *amplitudes, float *log_amplitudes, long median_span, long num_bins, char *mask, long N, float *freqs, float *amps, float bin_freq, float log_thresh)
{	
	if (!median_span) 
		median_amplitudes = 0;
	else 
	{
		if (!x->last_pf_spectralpeaks_n || x->last_pf_spectralpeaks_med_size != median_span) 
			medianfilter_float (x, median_indices, median_amplitudes, amplitudes, num_bins, median_span);
	}
	
	if (!x->last_pf_spectralpeaks_n) 
		spectralpeaks_float (amplitudes, log_amplitudes, median_amplitudes, num_bins, freqs, amps, bin_freq, log_thresh);
	
	x->last_pf_spectralpeaks_med_size = median_span;
	x->last_pf_spectralpeaks_n = 1;
}


void spectralpeaks_float (float *amplitudes, float *log_amplitudes, float *median_amplitudes, long num_bins, float *freqs, float *amps, float bin_freq, float log_thresh)
{
	float next_val = amplitudes[0];
	float current_val = 0.;
	float prev_val = 0.f;
	float amp_correction;
	long i;
	long j = 0;
	
#ifdef F32_VEC_EXP_OP
	vFloat *vamps;
#endif
	
	if (median_amplitudes)
	{
		// Find peaks with median filter to avoid noise (this could probably be made a lot more useful if there were the ability to scale the output of the median filter)
		
		for (i = 0; i < num_bins - 1; i++)
		{
			prev_val = current_val;
			current_val = next_val;
			next_val = amplitudes[i + 1];
			if (current_val > prev_val && current_val > next_val && current_val > median_amplitudes[i])
				freqs[j++] = i;
		}

		prev_val = current_val;
		current_val = amplitudes[i];
			
		if (current_val > prev_val && current_val > median_amplitudes[i])
			freqs[j++] = i;
	}
	else
	{
		// No median filter - find peaks
		
		for (i = 0; i < num_bins - 1; i++)
		{
			prev_val = current_val;
			current_val = next_val;
			next_val = amplitudes[i + 1];
			if (current_val > prev_val && current_val > next_val)
				freqs[j++] = i;
		}

		prev_val = current_val;
		current_val = amplitudes[i];
			
		if (current_val > prev_val)
			freqs[j++] = i;
	}
		
	// Peak correction
	
	for (i = 0; i < j; i++)
	{
		freqs[i] = (bin_freq * peakcorrection_float(log_amplitudes, (long) freqs[i], num_bins, &amp_correction));
		amps[i] = amp_correction;
	}
	
	// Sort peaks into oreder of descending amplitude
	
	combsort_peaks_float(amps, freqs, j);
	
	log_thresh += amps[0];
	
	for (i = 0; i < j; i++)
		if (amps[i] < log_thresh) 
			break;
	
	j = i;
	
	// Convert to linear amplitudes
		
#ifdef F32_VEC_EXP_OP
	vamps = (vFloat *) amps;

	for (i = 0; i < j >> 2; i++)
		vamps[i] = F32_VEC_EXP_OP(vamps[i]);
	for (i <<= 2; i < j; i++)
		amps[i] = expf(amps[i]);
#else
#ifdef F32_VEC_EXP_ARRAY
	F32_VEC_EXP_ARRAY(amps, amps, j);
#else
	for (i = 0; i < j; i++)
		amps[i] = expf(amps[i]);
#endif
#endif

	// Store zeros if we've run out of peaks
		
	for (i = j; i < num_bins; i++)
	{
		freqs[i] = 0.;
		amps[i] = 0.;
	}
}	


// Routine for parabolic interpolation to provide a more accurate estimation of the peak frequency and amplitude (single precision)

float peakcorrection_float (float *log_amplitudes, long bin, long num_bins, float *better_amp)
{
	float a, b, c;
	float divisor;
	float correction;
	float better_bin;

	// Get log amplitudes
	
	if (bin) 
		a = log_amplitudes[bin - 1];
	else 
		a = log_amplitudes[1];
	
	b = log_amplitudes[bin];
	
	if (bin != num_bins - 1) 
		c = log_amplitudes[bin + 1];
	else 
		c = log_amplitudes[num_bins - 2];
			
	// Parabolic interpolation
	
	divisor = a + c - (2.0 * b);
	
	if (divisor == 0) 
		correction = 0;
	else 
		correction = (0.5 * (a - c)) / divisor;
		
	// N.B - Leave amplitude in a log format
	
	better_bin = (float) bin + correction;
	*better_amp = b - (0.25 * (a - c) * correction);					
	 
	return better_bin;
}									
