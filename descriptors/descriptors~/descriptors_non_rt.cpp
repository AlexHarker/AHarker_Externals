
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
#include <AH_Locks.hpp>
#include <ibuffer_access.hpp>

#include "descriptors_fft_params.hpp"
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
    bool reset;

    global_params params;
    
    // General Parameters
    
    bool descriptors_feedback;
    
    // Lock
    
    thread_lock m_lock;
    
    // Descriptors
    
    std::unique_ptr<summary_graph> m_graph;
    
    // Output List
    
    std::vector<t_atom> output_list;
    
    // Outlet
    
    void *m_outlet;
    
    // Output Clock
    
    t_clock *output_clock;
};

// Function Prototypes

void *descriptors_new(t_symbol *s, short argc, t_atom *argv);
void descriptors_free(t_descriptors *x);
void descriptors_assist(t_descriptors *x, void *b, long m, long a, char *s);

void descriptors_fft_params(t_descriptors *x, t_symbol *msg, short argc, t_atom *argv);
void descriptors_energy_thresh(t_descriptors *x, t_symbol *msg, short argc, t_atom *argv);
void descriptors_descriptors(t_descriptors *x, t_symbol *msg, short argc, t_atom *argv);

void descriptors_analyse(t_descriptors *x, t_symbol *msg, short argc, t_atom *argv);

// Object Basics (main / new / free)

int C74_EXPORT main()
{	
    this_class = class_new("descriptorstest~",
                           (method) descriptors_new,
                           (method) descriptors_free,
                           sizeof(t_descriptors),
                           0L,
                           A_GIMME,
                           0);
	
	class_addmethod(this_class, (method) descriptors_analyse, "analyse", A_GIMME, 0);
	class_addmethod(this_class, (method) descriptors_analyse, "analyze", A_GIMME, 0);
	
    class_addmethod(this_class, (method) descriptors_descriptors, "descriptors", A_GIMME, 0);
    class_addmethod(this_class, (method) descriptors_fft_params, "fftparams", A_GIMME, 0);
    class_addmethod(this_class, (method) descriptors_energy_thresh, "energythresh", A_GIMME, 0);

	class_register(CLASS_BOX, this_class);
	
    // Issues
    //
    // Padding issue (not yet found)
    // Old no RT object didn't respond to different buffer sample rates correctly (need to reset fft params)
    //
    // Rolloff now has interpolation (so reports slightly lower) and uses power (as Peeters / unlike flucoma default)
    // SFM *was* using power rather than amplitudes
    // Spectral crest *was* using power rather than amplitudes
    // Linear spread was squaring the raw value before the Hz adjustment (so not a useful value)
    // Higher spectral shapes were wrong in early versions due to a lack of division by the overall amplitude sum
    // Log higher spectral shape was still wrong in more recent versions
    // Flux *had* a fixed index bug in the code producing consistently incorrect results
    // Foote *did* return inf for the change between two zero frames (now returns zero which seems more correct)
    // Pitch / confidence *had* a significant issue where pitch reports as 0 when invalid / there is a precision effect
    // Pitch *now* reports as inf for no pitch (not zero) solving averaging and stats errors
    //
    // Spectral peak finding currently has no median filtering
    // RT spectral_peaks reports in linear amps but non RT in db (with no options)
    //
    // Precision etc. - some small differences in various places
    // Shape desciptors (crest/sfm/skewness/kurtosis) - some differences for large fft with sine input
    
    // Need to check volume compesation points
    // Need to check lags and other things with fftparams that have mismatch window and FFT
    // Need to investigate speeds
    // Need to investigate zero inputs
    // Need to investigate min return values
    
    // Per-frame Descriptors
    
    s_setups.add_module("abs", module_average_abs_amp::setup);              // Match
    s_setups.add_module("rms", module_average_rms_amp::setup);              // Match
    s_setups.add_module("peakamp", module_peak_amp::setup);                 // Match

    s_setups.add_module("energy", module_energy::setup);                    // Match
    s_setups.add_module("energy_ratio", module_energy_ratio::setup);        // Match
    
    s_setups.add_module("loudness", module_loudness::setup);                // Match
    s_setups.add_module("rolloff", module_rolloff::setup);                  // ** Fixed [previously no interpolation]
                                                                            // N.B. uses power / not same as flucoma
    
    s_setups.add_module("sfm", module_sfm::setup);                          // ** Fixed [used power not amps]
    s_setups.add_module("spectral_crest", module_spectral_crest::setup);    // ** Fixed [used power not amps]

    s_setups.add_module("lin_centroid", module_lin_centroid::setup);        // Match
    s_setups.add_module("lin_spread", module_lin_spread::setup);            // ** Fixed [was ^2 before Hz conversion]
    s_setups.add_module("lin_skewness", module_lin_skewness::setup);        // Match (fixed from earlier)
    s_setups.add_module("lin_kurtosis", module_lin_kurtosis::setup);        // Match (fixed from earlier)
                                                                            
    s_setups.add_module("log_centroid", module_log_centroid::setup);        // * Partial (precision + bin zero)
    s_setups.add_module("log_spread", module_log_spread::setup);            // * Partial (precision + bin zero)
    s_setups.add_module("log_skewness", module_log_skewness::setup);        // * Differences (sum div missing + above)
    s_setups.add_module("log_kurtosis", module_log_kurtosis::setup);        // * Differences (sum div missing + above)
    
    s_setups.add_module("pitch", module_pitch::setup);                      // ** Fixed [return for no pitch]
    s_setups.add_module("confidence", module_confidence::setup);            // ** Fixed [return for no pitch / precision]
    s_setups.add_module("lin_brightness", module_lin_brightness::setup);    // ** Fixed [removed threshold / pitch fixed]
    s_setups.add_module("log_brightness", module_log_brightness::setup);    // ** Fixed [removed threshold / pitch fixed]
    
    s_setups.add_module("noise_ratio", module_noise_ratio::setup);          // * Big differences (median filter?)
    s_setups.add_module("harmonic_ratio", module_harmonic_ratio::setup);    // Is 1- noise_ratio so sort there
    
    s_setups.add_module("flux", module_flux::setup);                        // ** Fixed [fixed index mistake]
    s_setups.add_module("foote", module_foote::setup);                      // Match
    s_setups.add_module("mkl", module_mkl::setup);
    
    s_setups.add_module("inharmonicity", module_inharmonicity::setup);
    s_setups.add_module("roughness", module_roughness::setup);

    // Stats
    
    s_setups.add_module("mean", stat_module_mean::setup);                           // Match (apart from padding)
    s_setups.add_module("standard_dev", stat_module_stddev::setup);                 // Match
    s_setups.add_module("time_centroid", stat_module_centroid::setup);              // Match
    s_setups.add_module("median", stat_module_median::setup);                       // Match
    s_setups.add_module("range", stat_module_range::setup);                         // Match

    s_setups.add_module("min", stat_module_min::setup);                             // Match
    s_setups.add_module("max", stat_module_max::setup);                             // Match
    s_setups.add_module("min_pos", stat_module_min_pos::setup);                     // Match
    s_setups.add_module("max_pos", stat_module_max_pos::setup);                     // Match
    
    s_setups.add_module("peak", stat_module_peak::setup);                           // * Partial (end position behaviour)
    s_setups.add_module("trough", stat_module_trough::setup);
    s_setups.add_module("peak_pos", stat_module_peak_pos::setup);                   // * Partial (end position behaviour)
    s_setups.add_module("trough_pos", stat_module_trough_pos::setup);

    s_setups.add_module("longest_cross_above", stat_module_longest_above::setup);
    s_setups.add_module("longest_cross_below", stat_module_longest_below::setup);
    s_setups.add_module("longest_crossings_above", stat_module_longest_above_both::setup);  // * Partial (masktime)
    s_setups.add_module("longest_crossings_below", stat_module_longest_below_both::setup);  // * Partial (masktime)
    
    s_setups.add_module("crossing_peak", stat_module_crossing_peak::setup);
    s_setups.add_module("crossing_trough", stat_module_crossing_trough::setup);
    s_setups.add_module("crossing_peak_pos", stat_module_crossing_peak_pos::setup);
    s_setups.add_module("crossing_trough_pos", stat_module_crossing_trough_pos::setup);
    
    s_setups.add_module("cross_above", stat_module_cross_above::setup);
    s_setups.add_module("cross_below", stat_module_cross_below::setup);
    s_setups.add_module("crossings_above", stat_module_crossings_above::setup);
    s_setups.add_module("crossings_below", stat_module_crossings_below::setup);

    s_setups.add_module("ratio_above", stat_module_ratio_above::setup);                 // Match
    s_setups.add_module("ratio_below", stat_module_ratio_below::setup);

    // Specifiers
    
    s_setups.add_module("masktime", specifier_mask_time::setup);
    s_setups.add_module("threshold", specifier_threshold::setup);
    
    // Summaries
    
    s_setups.add_module("duration", summary_module_duration::setup);                    // Match
    s_setups.add_module("spectral_peaks", summary_module_spectral_peaks::setup);        // Match [minus median filter]
    
	return 0;
}

void *descriptors_new(t_symbol *s, short argc, t_atom *argv)
{
    t_descriptors *x = (t_descriptors *) object_alloc(this_class);
	
	long max_fft_size = 0;
	long descriptor_data_size = 0;
	long descriptor_feedback = 0;

	// Get arguments 

	if (argc) 
        max_fft_size = atom_getlong(argv++);
	if (argc > 1) 
		descriptor_data_size = atom_getlong(argv++);
	if (argc > 2)
		descriptor_feedback = atom_getlong(argv++);
	
	// Set maximum fft size

    x->max_fft_size_log2 = check_fft_size((t_object *) x, "maximum fft size", max_fft_size, 0);
	x->max_fft_size = 1 << x->max_fft_size_log2;

    long default_fft_size = 4096 <= x->max_fft_size ? 4096 : x->max_fft_size;
    
    x->params.m_fft_params = check_fft_params((t_object *) x, default_fft_size, 0, 0, nullptr, x->max_fft_size_log2);

    x->m_outlet = listout(x);

    create_object(x->output_list);
    create_object(x->m_graph);
    create_object(x->m_lock);
    
    return x;
}

void descriptors_free(t_descriptors *x)
{
    destroy_object(x->m_lock);
    destroy_object(x->output_list);
    destroy_object(x->m_graph);
}

// FFT Params

void descriptors_fft_params(t_descriptors *x, t_symbol *msg, short argc, t_atom *argv)
{
    // Ignore blank argument set (keep current values)
    
    if (argc < 0)
        return;
    
    // Load in args as relevant
    
    long fft_size = (argc > 0) ? atom_getlong(argv + 0) : 0;
    long hop_size = (argc > 1) ? atom_getlong(argv + 1) : 0;
    long frame_size = (argc > 2) ? atom_getlong(argv + 2) : 0;
    t_symbol *window_type = (argc > 3) ? atom_getsym(argv + 3) : gensym("");
    
    auto params = check_fft_params((t_object *) x, fft_size, hop_size, frame_size, window_type, x->max_fft_size_log2);
    
    safe_lock_hold hold(&x->m_lock);

    x->params.m_fft_params = params;
    
    if (x->m_graph)
        x->m_graph->prepare(x->params);
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

// Set Descriptors

void descriptors_descriptors(t_descriptors *x, t_symbol *msg, short argc, t_atom *argv)
{
    auto graph = new class summary_graph();
    
    graph->build(s_setups, x->params, argc, argv);
    
    safe_lock_hold hold(&x->m_lock);
    
    x->output_list.resize(graph->size());
    x->m_graph.reset(graph);
}

// User Routines For Performing Analysis

void descriptors_analyse(t_descriptors *x, t_symbol *msg, short argc, t_atom *argv)
{
    safe_lock_hold hold(&x->m_lock);

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
