
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
    
    s_setups.add_module("peak", stat_module_peak::setup);
    s_setups.add_module("trough", stat_module_trough::setup);
    s_setups.add_module("peak_pos", stat_module_peak_pos::setup);
    s_setups.add_module("trough_pos", stat_module_trough_pos::setup);

    s_setups.add_module("longest_cross_above", stat_module_longest_above::setup);
    s_setups.add_module("longest_cross_below", stat_module_longest_below::setup);
    s_setups.add_module("longest_crossings_above", stat_module_longest_above_both::setup);
    s_setups.add_module("longest_crossings_below", stat_module_longest_below_both::setup);

    s_setups.add_module("ratio_above", stat_module_ratio_above::setup);
    s_setups.add_module("ratio_below", stat_module_ratio_below::setup);

    // Specifiers
    
    s_setups.add_module("masktime", specifier_mask_time::setup);
    s_setups.add_module("threshold", specifier_threshold::setup);
    
    // Summaries
    
    s_setups.add_module("duration", summary_module_duration::setup);
    s_setups.add_module("spectral_peaks", summary_module_spectral_peaks::setup);
    
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
