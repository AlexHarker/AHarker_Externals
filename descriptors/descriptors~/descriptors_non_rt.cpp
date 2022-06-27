
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



#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <algorithm>
#include <memory>
#include <vector>

#include <AH_Lifecycle.hpp>
#include <ibuffer_access.hpp>

#include "descriptors_graph.hpp"
#include "descriptors_summary_graph.hpp"

#include "modules_core.hpp"
#include "modules_change.hpp"
#include "modules_content.hpp"
#include "modules_level.hpp"
#include "modules_pitch.hpp"
#include "modules_spectral.hpp"
#include "modules_summary.hpp"

// Globals and Object Structure

t_class *this_class;

setup_list s_setups;

struct t_descriptors
{
    t_pxobject x_obj;
    
    // FFT / Scheduling Data
    
    long max_fft_size_log2;
    long max_fft_size;
    
    // FIX
    
    long hop_count;
    
    global_params params;
    
    bool reset;
    
    // General Parameters
    
    bool descriptors_feedback;
    
    // Descriptors
    
    std::unique_ptr<summary_graph> m_graph;
    
    // Output List
    
    std::vector<t_atom> output_list;
    
    // Outlet
    
    void *m_outlet;
    
    // Output Clock
    
    t_clock *output_clock;
};

#include "descriptors_temp.hpp"

// Function Prototypes

void *descriptors_new(t_symbol *s, short argc, t_atom *argv);
void descriptors_free(t_descriptors *x);
void descriptors_assist(t_descriptors *x, void *b, long m, long a, char *s);

void descriptors_descriptors(t_descriptors *x, t_symbol *msg, short argc, t_atom *argv);
void descriptors_energy_thresh(t_descriptors *x, t_symbol *msg, short argc, t_atom *argv);

void descriptors_analyse(t_descriptors *x, t_symbol *msg, short argc, t_atom *argv);

// Object Basics (main / new / free)

int C74_EXPORT main()
{	
    this_class = class_new("descriptors~",
                           (method) descriptors_new,
                           (method) descriptors_free,
                           sizeof(t_descriptors),
                           0L,
                           A_GIMME,
                           0);
	
	class_addmethod(this_class, (method) descriptors_analyse, "analyse", A_GIMME, 0);
	class_addmethod(this_class, (method) descriptors_analyse, "analyze", A_GIMME, 0);
	
    class_addmethod(this_class, (method) descriptors_descriptors, "descriptors", A_GIMME, 0);
    class_addmethod(this_class, (method) descriptors_fft_params<t_descriptors>, "fftparams", A_GIMME, 0);
    class_addmethod(this_class, (method) descriptors_energy_thresh, "energythresh", A_GIMME, 0);

	class_register(CLASS_BOX, this_class);
	
    // Per-frame Descriptors
    
    s_setups.add_module("abs", module_average_abs_amp::setup);
    s_setups.add_module("rms", module_average_rms_amp::setup);
    s_setups.add_module("peakamp", module_peak_amp::setup);
    s_setups.add_module("energy", module_energy::setup);
    s_setups.add_module("energy_ratio", module_energy_ratio::setup);
    s_setups.add_module("spectral_crest", module_spectral_crest::setup);
    s_setups.add_module("sfm", module_sfm::setup);
    s_setups.add_module("rolloff", module_rolloff::setup);
    s_setups.add_module("loudness", module_loudness::setup);
    s_setups.add_module("lin_centroid", module_lin_centroid::setup);
    s_setups.add_module("lin_spread", module_lin_spread::setup);
    s_setups.add_module("lin_skewness", module_lin_skewness::setup);
    s_setups.add_module("lin_kurtosis", module_lin_kurtosis::setup);
    s_setups.add_module("log_centroid", module_log_centroid::setup);
    s_setups.add_module("log_spread", module_log_spread::setup);
    s_setups.add_module("log_skewness", module_log_skewness::setup);
    s_setups.add_module("log_kurtosis", module_log_kurtosis::setup);
    s_setups.add_module("pitch", module_pitch::setup);
    s_setups.add_module("confidence", module_confidence::setup);
    s_setups.add_module("lin_brightness", module_lin_brightness::setup);
    s_setups.add_module("log_brightness", module_log_brightness::setup);
    s_setups.add_module("noise_ratio", module_noise_ratio::setup);
    s_setups.add_module("harmonic_ratio", module_harmonic_ratio::setup);
    s_setups.add_module("foote", module_foote::setup);
    s_setups.add_module("flux", module_flux::setup);
    s_setups.add_module("mkl", module_mkl::setup);
    s_setups.add_module("inharmonicity", module_inharmonicity::setup);
    s_setups.add_module("roughness", module_roughness::setup);

    // Stats
    
    s_setups.add_module("mean", stat_module_mean::setup);
    s_setups.add_module("standard_dev", stat_module_stddev::setup);
    s_setups.add_module("time_centroid", stat_module_centroid::setup);
    s_setups.add_module("median", stat_module_median::setup);
    s_setups.add_module("range", stat_module_range::setup);

    s_setups.add_module("min", stat_module_min::setup);
    s_setups.add_module("max", stat_module_max::setup);
    s_setups.add_module("min_pos", stat_module_min_pos::setup);
    s_setups.add_module("max_pos", stat_module_max_pos::setup);
    
    // Summaries
    
    s_setups.add_module("duration", summary_module_duration::setup);

	return 0;
}

void *descriptors_new(t_symbol *s, short argc, t_atom *argv)
{
    t_descriptors *x = (t_descriptors *) object_alloc(this_class);
	
	long max_fft_size_log2;
	long max_fft_size_in = 0;
	long max_fft_size;
	long descriptor_data_size = 0;
	long descriptor_feedback = 0;

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

    descriptors_fft_params_internal(x, x->max_fft_size, 0, 0, nullptr);

    x->m_outlet = listout(x);

    create_object(x->output_list);
    create_object(x->m_graph);
    
    return x;
}

void descriptors_free(t_descriptors *x)
{
    destroy_object(x->output_list);
    destroy_object(x->m_graph);
}

// Set Descriptors

void descriptors_descriptors(t_descriptors *x, t_symbol *msg, short argc, t_atom *argv)
{
    auto graph = new class summary_graph();
    
    graph->build(s_setups, x->params, argc, argv);
    x->output_list.resize(graph->size());
    x->m_graph.reset(graph);
}

// Energy Threshold

void descriptors_energy_thresh(t_descriptors *x, t_symbol *msg, short argc, t_atom *argv)
{
    if (argc)
        x->params.m_energy_threshold = dbtoa(atom_getfloat(argv));
    else
        x->params.m_energy_threshold = 0.0;
    
    if (argc > 2)
        object_error((t_object *) x, "too many arguments to energythresh message");
}

// User Routines For Performing Analysis

void descriptors_analyse(t_descriptors *x, t_symbol *msg, short argc, t_atom *argv)
{
    auto& graph = x->m_graph;

	t_symbol *buffer_name = nullptr;
	
    t_atom_long buffer_chan = 1;

    double start_point_ms = 0.0;
	double end_point_ms = 0.0;

    if (!graph)
    {
        object_error((t_object *) x, "no descriptors set");
        return;
    }
    
	if (argc && atom_gettype(argv) != A_SYM)
	{
        object_error((t_object *)x, "no buffer name given for analysis");
		return;
	}
	
	buffer_name = atom_getsym(argv);
	
	// Get arguments
	
	if (argc > 1) 
		buffer_chan = atom_getlong(argv + 1);
	if (argc > 2) 
		start_point_ms = atom_getfloat(argv + 2);
	if (argc > 3) 
		end_point_ms = atom_getfloat(argv + 3);
	if (argc > 4) 
		object_error((t_object *)x, "too many arguments to analyse function");
	
	// Check the buffer
	
	ibuffer_data buffer(buffer_name);
	
    if (buffer.get_type() == kBufferNone)
	{
        object_error((t_object *)x, "buffer not found");
		return;
	}
		
	// Calculate lengths
	
    double mstosamps_mul = buffer.get_sample_rate() / 1000.0;

    start_point_ms = std::max(0.0, start_point_ms);
    end_point_ms = std::max(0.0, end_point_ms);
    buffer_chan = std::max(t_atom_long(1), buffer_chan) - 1;
	
    if ((start_point_ms && end_point_ms <= start_point_ms) || end_point_ms < 0.0)
        return;
    
	// Store variables
	
	long start_point = start_point_ms * mstosamps_mul;
    long end_point = end_point_ms * mstosamps_mul;
	    
    // Access buffer and increment pointer
    
    if (!buffer.get_sample_rate())
    {
        object_error((t_object *) x, "could not access buffer");
        return;
    }
    
    // Range check buffer access variables and calculate numer of frames
    
    if (buffer.get_num_chans() < buffer_chan + 1)
        buffer_chan = buffer_chan % buffer.get_num_chans();
    
    auto signal_length = buffer.get_length();
    if (end_point && end_point < signal_length)
        signal_length = end_point;
    signal_length -= start_point;
    
    x->params.m_signal_length = signal_length;
    x->params.m_sr = buffer.get_sample_rate();
    
    if (x->params.num_frames() < 1)
    {
        object_error((t_object *) x, "zero length file or segment");
        return;
    }
    
    // FIX - could this be a lamda to grab samples, or just pass the ibuffer?
    
    std::vector<double> samples(signal_length);
    
    ibuffer_get_samps(buffer, samples.data(), start_point + start_point, signal_length, buffer_chan);
        
    graph->prepare(x->params);
    graph->run(x->params, samples.data());
    graph->output(x->output_list.data());
    
    // Release buffer
    
    buffer.release();
    
    // Output
    
    outlet_list(x->m_outlet, nullptr, static_cast<short>(x->output_list.size()), x->output_list.data());
}

// Handle Non RT descriptor Calculation

/*
void calc_descriptors_non_rt(t_descriptors *x)
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
	vFloat v_half = {0.5f, 0.5f, 0.5f, 0.5f};
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


		
	// Buffer access variables
	
	double num_frames_recip;
	
	long buffer_pos = 0;
	long buffer_chan = x->buffer_chan;
	long start_point = x->start_point;
	long end_point = x->end_point;
	long num_frames;
	long num_samps;
    t_ptr_int file_length;
	
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
		return;
	}
	
    samps_to_ms_val = 1000. / sr;
    frame_to_ms_val = hop_size * 1000. / sr;
    ms_to_frame_val = sr / (hop_size * 1000.);
    bin_freq = sr / (double) fft_size;
     
     // If the sample rate is different from the last one used, recalculate curves for the new sample rates
     
     if (sr != x->sr)
     calc_curves(x);
     x->sr = sr;
    
	num_frames_recip = 1. / num_frames;
    
	// See how many descriptors we can calculate per loop (hopefully all of them, but this allows more conservative memory allocation)
	
	num_pf_descriptors_per_loop = descriptor_data_size / (num_frames * sizeof(double));
	
	if (!num_pf_descriptors_per_loop)
	{
		error("descriptors~: not enough memory - file is too long!");
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
			
			ibuffer_get_samps(buffer, raw_frame, start_point + buffer_pos, num_samps, buffer_chan);
			
			// Zero pad to fft size
			
			for (j = num_samps; j < fft_size; j++)
				raw_frame[j] = 0.;
			
			// Apply window
			
			for (j = 0; j < window_size >> 2; j++)
				v_windowed_frame[j] = F32_VEC_MUL_OP(v_raw_frame[j], v_window[j]);
			for (j <<= 2; j < window_size; j++)
				windowed_frame[j] = raw_frame[j] * window[j];
			
			// Do FFT straight into position with zero padding
			
            hisstools_rfft(fft_setup_real, windowed_frame, &raw_fft_frame, window_size, fft_size_log2);
			
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
				log_amplitudes[i] = log((sq_amplitudes[i] < POW_MIN) ? POW_MIN : sq_amplitudes[i]) * 0.5;
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
			
			// Calculate Per Frame descriptors 
			
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

		// Derive outputs from the raw per frame data

		for (j = from_pf_descriptor; j < to_pf_descriptor; j++)
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
}
*/
