
/*
 *  descriptors_object.h
 *
 *	A header file for descriptors_object.c.
 *	This code deals with common functionality for the two varieties of the descriptors object.
 *
 *	Note that some of the routines with protoypes here are found in descriptors_rt.c and descriptors_non_rt.c
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#ifndef _DESCRIPTORS_OBJECT_
#define _DESCRIPTORS_OBJECT_

#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <HISSTools_FFT/HISSTools_FFT.h>
//#include <AH_Win_Math.h>
//#include <AH_VectorOpsExtended.h>

#include "descriptors_object_structure.h"
#include "descriptors_conversion_helpers.h"
#include "descriptors_per_frame_descriptors.h"
#include "descriptors_medianfilter.h"
#include "descriptors_spectralpeaks.h"
#include "descriptors_statistics.h"
#include "descriptors_set_descriptors.h"
	

// Prototypes for basic object functionality

void descriptors_main_common ();
void descriptors_new_common (t_descriptors *x, long max_fft_size_log2, long desciptor_feedback);
void *descriptors_new (t_symbol *s, short argc, t_atom *argv);
void descriptors_free (t_descriptors *x);
void descriptors_assist (t_descriptors *x, void *b, long m, long a, char *s);

// FFT parameter and window related routines

long descriptors_max_fft_size (t_descriptors *x, long max_fft_size);
void descriptors_fft_params (t_descriptors *x, t_symbol *s, short argc, t_atom *argv);
void descriptors_fft_params_internal (t_descriptors *x, long fft_size, long hop_size, long window_size, t_symbol *window_type);
void descriptors_generate_window (t_descriptors *x, float *window, long window_size, long fft_size, enum WindowType window_select);

// Routine for zero ring buffers

void descriptors_zero_ring_buffers (t_descriptors *x, long fft_size);

// Routines for setting the descriptors, energy threshold and analysing buffers

void descriptors_descriptors (t_descriptors *x, t_symbol *msg, short argc, t_atom *argv);
void descriptors_energy_thresh (t_descriptors *x, t_symbol *msg, short argc, t_atom *argv);
void descriptors_analyse(t_descriptors *x, t_symbol *msg, short argc, t_atom *argv);

// Descriptor calculation routines 

void calc_descriptors_rt (t_descriptors *x, float *samples);
void calc_descriptors_non_rt (t_descriptors *x);

double calc_pf_descriptor (t_descriptors *x, float *raw_frame, float *windowed_Frame, FFT_SPLIT_COMPLEX_F Raw_FFT_Frame, long frame_pointer, long num_samps, long fft_size, double **Params);

// Real-time dsp functions

void output_rt (t_descriptors *x);

t_int *descriptors_perform(t_int *w);
void descriptors_dsp (t_descriptors *x, t_signal **sp, short *count);

void descriptors_perform64 (t_descriptors *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void descriptors_dsp64 (t_descriptors *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

// Useful curves (pre-calculated for efficiency)

void calc_curves (t_descriptors *x);
void calc_log_freq_curve (double *log_freq, long num_bins, double bin_freq);
void calc_loudness_curve (double *loudness_curve, long num_bins, double bin_freq);


#endif /* _DESCRIPTORS_OBJECT_ */
