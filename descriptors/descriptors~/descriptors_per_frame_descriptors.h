
/*
 *  descriptors_per_frame_descriptors.h
 *
 *	A header file for the per frame descriptors routines in the descriptors~ and descriptorsrt~ objects.
 *	These routines calculate the raw descriptors given the fft data or raw input frame.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#ifndef _DESCRIPTORS_PFDESCRIPTORS_
#define _DESCRIPTORS_PFDESCRIPTORS_

#include <HISSTools_FFT/HISSTools_FFT.h>
#include <AH_VectorOps.h>

#include "descriptors_object_structure.h"
#include "descriptors_constants.h"
#include "descriptors_medianfilter.h"
#include "descriptors_spectralpeaks.h"


// Energy and basic spectral derivatives

//double get_energy (double *cumulate_sq_amps, long min_bin, long max_bin, double energy_compensation);
double get_energy_ratio (double *cumulate_sq_amps, long num_bins, long min_bin, long max_bin);
double get_spectral_rolloff (double *cumulate_sq_amps, long num_bins, double bin_freq, double threshold);
//double get_spectral_crest (float *vals_ptr, double *cumulate_ptr, long min_bin, long max_bin);

// Spectral difference measures

double get_flux (float *frame1, float *frame2, double *cumulate_ptr1, double *cumulate_ptr2, long min_bin, long max_bin, char norm_flag, char square_flag, char forward_only);
double get_mkl (float *log_frame1, float *log_frame2, float *frame2, double *cumulate_ptr1, double *cumulate_ptr2, long min_bin, long max_bin, char norm_flag, char forward_only, char weight_frame2, double log_thresh);
double get_foote (float *frame1, float *frame2, long min_bin, long max_bin, char forward_only);

// Amplitude measures

//double get_average_amp_abs (float *raw_frame, long num_samps);
//double get_average_amp_rms (float *raw_frame, long num_samps);
//double get_amp_peak (float *raw_frame, long num_samps);

// Loudness

//double get_loudness (float *sq_amplitudes, double *loudness_curve, long num_bins, double energy_compensation);

// Shape descriptors (centroid / spread / skewness / kurtosis - in lin and log forms)

//double get_shape_lin (t_descriptors *x, float *vals_ptr, double *cumulate_ptr, long min_bin, long max_bin, enum PFDescriptorType shape_type);
double get_shape_log (t_descriptors *x, float *vals_ptr, double *cumulate_ptr, double *log_freq, long min_bin, long max_bin, enum PFDescriptorType shape_type);

// Brightness lin and log (dependent on centroid and pitch)

double get_brightness_lin (t_descriptors *x, float *raw_frame, float *ac_coefficients, float * vals_ptr, double *cumulate_ptr, long num_bins, long min_bin, long max_bin, double bin_freq, double threshold);
double get_brightness_log (t_descriptors *x, float *raw_frame, float *ac_coefficients, float * vals_ptr, double *cumulate_ptr, double *log_freq, long num_bins, long min_bin, long max_bin, double threshold);

// Noisiness and inhormanicity

//double get_sfm (float *log_amplitudes, double *cumulate_ptr, long min_bin, long max_bin);
double get_noise_ratio (t_descriptors *x, long *median_indices, float *median_amplitudes, float *amplitudes, float *sq_amplitudes, double *cumulate_ptr, double *log_freq, long num_bins, long median_span);
double get_inharmonicity (t_descriptors *x, long *median_indices, float *median_amplitudes, float *amplitudes, float *log_amplitudes, float *raw_frame, float *ac_coefficients, float *freqs, float *amps, char *mask, long num_bins, long N, long median_span, double bin_freq, double threshold);

// AC pitch detection

void calc_ac_coefficients (t_descriptors *x, float *raw_frame);
void complex_mult_conjugate (FFT_SPLIT_COMPLEX_F in1, FFT_SPLIT_COMPLEX_F in2, long length, float scale);
double get_pitch (t_descriptors *x, float *raw_frame, float *ac_coeff, long length, double threshold, double *confidence_ret);

// Roughness

double get_roughness (float *freqs, float *amps, long num_sines);


#endif /* _DESCRIPTORS_PFDESCRIPTORS_ */
