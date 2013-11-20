
/*
 *  descriptors_spectralpeaks.h
 *
 *	A header file for the spectral peak-finding routines in the descriptors~ and descriptorsrt~ objects (both single and double precision).
 *	This code deals with identifying and resolving spectral peaks (which are assumed to be sinusoidal components), both for output, and also to drive various per frame descriptor calculations.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#ifndef _DESCRIPTORS_SPECTRALPEAKS_
#define _DESCRIPTORS_SPECTRALPEAKS_


#include "descriptors_object_structure.h"
#include "descriptors_conversion_helpers.h"
#include "descriptors_medianfilter.h"
#include "descriptors_combsort.h"

#include <AH_VectorOpsExtended.h>

// Double precision spectral peak finding

void spectralpeaks_double (double *amplitudes, double *median_amplitudes, long num_bins, double *freqs, double *amps, double bin_freq, double log_thresh);
double peakcorrection_double (double *amplitudes, long bin, long num_bins, double *better_amp);

// Single precision spectral peak finding

void spectralpeaks_medianmask_float (t_descriptors *x, long * median_indices, float *median_amplitudes, float *amplitudes, float *log_amplitudes, long median_span, long num_bins, char *mask, long N, float *freqs, float *amps, float bin_freq, float log_thresh);
void spectralpeaks_float (float *amplitudes, float *log_amplitudes, float *median_amplitudes, long num_bins, float *freqs, float *amps, float bin_freq, float log_thresh);
float peakcorrection_float (float *amplitudes, long bin, long num_bins, float *better_amp);


#endif /* _DESCRIPTORS_SPECTRALPEAKS_ */
