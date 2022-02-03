
/*
 *  descriptors_object_structure.h
 *
 *	The object sturcture for the descriptors~ and descriptorsrt~ objects.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#ifndef _DESCRIPTORS_OBJECT_STRUCTURE_
#define _DESCRIPTORS_OBJECT_STRUCTURE_


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <HISSTools_FFT/HISSTools_FFT.h>

#include "descriptors_constants.h"

extern t_class *this_class;

typedef struct _descriptors
{
    t_pxobject x_obj;
	
	// The last output list	and its length
	
	t_atom *output_list;
	long output_length;
	
	// RT Stuff
	
	float *rt_buffer;	
	long rt_memory_size;
	
	long write_pointer;
	long hop_count;

	// Buffer Stuff
	
	t_symbol *buffer_name;

	long buffer_chan;
	long start_point;
	long end_point;

	// Relevant Sample Rate (buffer or real-time)

	double sr;
	
	// Descriptor Stuff
	
	long descriptor_data_size;
	double *descriptor_data;
	void *n_data;
	
	// FFT Stuff
	
    FFT_SETUP_F fft_setup_real;	

	void *fft_memory;
	float *window;
	float *amps_buffer;
	
	long max_fft_size_log2;
	long max_fft_size;
	long fft_size;
	long fft_size_log2;
	long window_size;
	long hop_size;
	
	double *cumulate;
	double *summed_amplitudes;
	
	double energy_compensation;
	
	char reset_fft;
	char frame_pointer;

	// Autocorrelation Stuff
	
	void *ac_memory;
	char ac_flag;
	double last_pitch;
	double last_confidence;
	double last_threshold;
	
	// Median Filter Stuff
	
	void *median_memory;
	char median_flag;
	long last_median_span;
	
	// Stored Curves Memory
	
	double *loudness_curve;
	double *log_freq;
	
	// Shape Saves
	
	char centroid_lin_flag;
	char spread_lin_flag;
	
	long last_min_bin_lin;
	long last_max_bin_lin;
	
	double last_centroid_lin;
	double last_spread_lin;
	
	char centroid_log_flag;
	char spread_log_flag;
	
	long last_min_bin_log;
	long last_max_bin_log;
	
	double last_centroid_log;
	double last_spread_log;
	
	// Spectral Peaks Saves
	
	long last_pf_spectralpeaks_n;
	long last_pf_spectralpeaks_med_size;

	// Outlets
	
	void *the_list_outlet;
	
	// Output Clock
	
	void *output_rt_clock;
	
	// Params 
	
	double energy_thresh;
	long use_energy_thresh;
	long descriptor_feedback;
	char do_sum_amps;
	
	// Per Frame Descriptors
	
	double pf_params[MAX_PF_PARAMS];
	double pf_calc_params[MAX_PF_CALC];
	long pf_output_params[MAX_PF_OUTPUT_PARAMS];
	long num_pf_descriptors;

	// Per Block Descriptors
	
	double pb_params[MAX_PB_PARAMS];			
	long pb_pos[MAX_PB_POS];	
	long num_pb_descriptors;

	
} t_descriptors;


#endif /* _DESCRIPTORS_OBJECT_STRUCTURE_ */
