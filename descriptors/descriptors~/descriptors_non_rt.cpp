
/*
 *  descriptors~
 *
 *  descriptors~ is used to calculate audio descriptors (or features) on a sample stored in an buffer~ (or an ibuffer~).
 *  Various statistical calculations may be performed on the descriptors  to summarise across the sample.
 *  Examples include mean / median / standard deviation / max n values etc.
 *
 *  descriptors~ is intended to cover a wide range of analysis applications.
 *  It is the non real-time counterpart to the descriptorsrt~ object and the two objects share many features.
 *
 *  The object calculates and outputs only the requested descriptors.
 *  The object can also operate on subsections of a buffer~ / ibuffer~.
 *  The object is designed with efficiency in mind, avoiding re-calculations and making use of SIMD operations.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <algorithm>
#include <memory>
#include <vector>

#include <AH_Int_Handler.hpp>
#include <AH_Lifecycle.hpp>
#include <AH_Locks.hpp>
#include <ibuffer_access.hpp>

#include "descriptors_fft_params.hpp"
#include "descriptors_graph.hpp"
#include "descriptors_summary_graph.hpp"

#include "modules/modules_core.hpp"
#include "modules/modules_change.hpp"
#include "modules/modules_content.hpp"
#include "modules/modules_level.hpp"
#include "modules/modules_pitch.hpp"
#include "modules/modules_spectral.hpp"
#include "modules/modules_summary.hpp"


// Globals and Object Structure

t_class *this_class;

setup_list s_setups;

enum class audio_edges { none, allow_overlap, zero_pad };

struct t_descriptors
{
    t_pxobject x_obj;
    
    // FFT / Scheduling Data
    
    long max_fft_size_log2;
    long max_fft_size;
    
    global_params params;
    
    // General Parameters
    
    bool descriptors_feedback;
    
    // Lock / Descriptor Graph / Output List
    
    thread_lock m_lock;
    std::unique_ptr<summary_graph> m_graph;
    std::vector<t_atom> output_list;
    
    // Outlet
    
    void *m_outlet;
    //void *m_per_Frame_outlet;

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
    
    // Per-frame Descriptors
    
    s_setups.add_module("abs", module_average_abs_amp::setup);
    s_setups.add_module("rms", module_average_rms_amp::setup);
    s_setups.add_module("peakamp", module_peak_amp::setup);

    s_setups.add_module("energy", module_energy::setup);
    s_setups.add_module("energy_ratio", module_energy_ratio::setup);
    
    s_setups.add_module("loudness", module_loudness::setup);
    s_setups.add_module("rolloff", module_rolloff::setup);                  // ** Improved [previously no interpolation]
    s_setups.add_module("sfm", module_sfm::setup);                          // ** Fixed [used power not amps]
    s_setups.add_module("spectral_crest", module_spectral_crest::setup);    // ** Fixed [used power not amps]

    s_setups.add_module("lin_centroid", module_lin_centroid::setup);
    s_setups.add_module("lin_spread", module_lin_spread::setup);            // ** Fixed [was ^2 before Hz conversion]
    s_setups.add_module("lin_skewness", module_lin_skewness::setup);
    s_setups.add_module("lin_kurtosis", module_lin_kurtosis::setup);
                                                                            
    s_setups.add_module("log_centroid", module_log_centroid::setup);        // * Improved (precision + bin zero)
    s_setups.add_module("log_spread", module_log_spread::setup);            // * Improved (precision + bin zero)
    s_setups.add_module("log_skewness", module_log_skewness::setup);        // * Fixed (sum div missing + above)
    s_setups.add_module("log_kurtosis", module_log_kurtosis::setup);        // * Fixed (sum div missing + above)
    
    s_setups.add_module("pitch", module_pitch::setup);                      // ** Improved [return for no pitch / precision]
    s_setups.add_module("confidence", module_confidence::setup);            // ** Improved [return for no pitch / precision]
    s_setups.add_module("lin_brightness", module_lin_brightness::setup);    // ** Improved [removed threshold / pitch fixed]
    s_setups.add_module("log_brightness", module_log_brightness::setup);    // ** Improved [removed threshold / pitch fixed]
    
    s_setups.add_module("noise_ratio", module_noise_ratio::setup);          // ** Fixed [multiple errors]
    s_setups.add_module("harmonic_ratio", module_harmonic_ratio::setup);    // ** Depends on above [nothing to do here]
    
    s_setups.add_module("flux", module_flux::setup);                        // ** Fixed [fixed index mistake]
    s_setups.add_module("foote", module_foote::setup);
    s_setups.add_module("mkl", module_mkl::setup);                          // ** Fixed [normalisation]
    
    s_setups.add_module("inharmonicity", module_inharmonicity::setup);      // ** Improved [calcs match but spurious peaks an issue]
    s_setups.add_module("roughness", module_roughness::setup);              // ** Improved [calcs match but spurious peaks an issue]

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

    s_setups.add_module("ratio_above", stat_module_ratio_above::setup);
    s_setups.add_module("ratio_below", stat_module_ratio_below::setup);
    
    s_setups.add_module("longest_cross_above", stat_module_longest_above::setup);
    s_setups.add_module("longest_cross_below", stat_module_longest_below::setup);
    s_setups.add_module("longest_crossings_above", stat_module_longest_above_both::setup);
    s_setups.add_module("longest_crossings_below", stat_module_longest_below_both::setup);
    
    s_setups.add_module("crossing_peak", stat_module_crossing_peak::setup);
    s_setups.add_module("crossing_trough", stat_module_crossing_trough::setup);
    s_setups.add_module("crossing_peak_pos", stat_module_crossing_peak_pos::setup);
    s_setups.add_module("crossing_trough_pos", stat_module_crossing_trough_pos::setup);
    
    s_setups.add_module("cross_above", stat_module_cross_above::setup);
    s_setups.add_module("cross_below", stat_module_cross_below::setup);
    s_setups.add_module("crossings_above", stat_module_crossings_above::setup);
    s_setups.add_module("crossings_below", stat_module_crossings_below::setup);

    // Specifiers
    
    s_setups.add_module("masktime", specifier_mask_time::setup);
    s_setups.add_module("threshold", specifier_threshold::setup);
    
    // Summaries
    
    s_setups.add_module("duration", summary_module_duration::setup);
    s_setups.add_module("spectral_peaks", summary_module_spectral_peaks::setup);            // ** Improved [spurious peaks an issue]
    
    return 0;
}

void *descriptors_new(t_symbol *s, short argc, t_atom *argv)
{
    t_descriptors *x = (t_descriptors *) object_alloc(this_class);
    
    long max_fft_size = 0;
    t_atom_long descriptor_data_size = 0;

    // Get arguments

    if (argc)
        max_fft_size = limit_int<long>(atom_getlong(argv++));
    if (argc > 1)
        descriptor_data_size = atom_getlong(argv++);
    if (argc > 2)
        x->descriptors_feedback = atom_getlong(argv++);
    
    // Set maximum fft size

    x->max_fft_size_log2 = check_fft_size((t_object *) x, "maximum fft size", max_fft_size, 0);
    x->max_fft_size = 1 << x->max_fft_size_log2;

    long default_fft_size = 4096 <= x->max_fft_size ? 4096 : x->max_fft_size;
    
    x->params.m_fft_params = check_fft_params((t_object *) x, default_fft_size, 0, 0, nullptr, x->max_fft_size_log2);

    //x->m_per_Frame_outlet = listout(x);
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
    
    long fft_size = (argc > 0) ? limit_int<long>(atom_getlong(argv + 0)) : 0;
    long hop_size = (argc > 1) ? limit_int<long>(atom_getlong(argv + 1)) : 0;
    long frame_size = (argc > 2) ? limit_int<long>(atom_getlong(argv + 2)) : 0;
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
    auto graph = new class summary_graph((t_object *) x, s_setups, x->params, argc, argv);
    
    safe_lock_hold hold(&x->m_lock);
    
    x->output_list.resize(graph->output_size());
    x->m_graph.reset(graph);
    
    if (x->descriptors_feedback)
        object_post((t_object *) x, "set %lu descriptors", graph->size());
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
    
    long start_point = static_cast<long>(start_point_ms * mstosamps_mul);
    long end_point = static_cast<long>(end_point_ms * mstosamps_mul);
    
    // Access buffer and increment pointer
    
    if (!buffer.get_sample_rate())
    {
        object_error((t_object *) x, "could not access buffer");
        return;
    }
    
    // Range check buffer access variables and calculate numer of frames
    
    if (buffer.get_num_chans() < buffer_chan + 1)
        buffer_chan = buffer_chan % buffer.get_num_chans();
    
    audio_edges edges = audio_edges::none;
    
    auto signal_length = buffer.get_length();
    
    if (edges == audio_edges::allow_overlap)
    {
        if (end_point && end_point < signal_length)
        {
            global_params calculation = x->params;
        
            // How many frames are available in the buffer
        
            calculation.m_signal_length = static_cast<long>(signal_length - start_point);
            calculation.m_pad = false;
            long available_frames = calculation.num_frames();
        
            // How many frames would be calculated with padding?
        
            calculation.m_signal_length = static_cast<long>(end_point - start_point);
            calculation.m_pad = true;
            long required_frames = calculation.num_frames();
        
            long num_frames = std::min(available_frames, required_frames);
            
            signal_length = num_frames ? calculation.frame_size() + num_frames * calculation.hop_size() : 0;
            signal_length += start_point;
        }
    }
    else
    {
        if (end_point && end_point < signal_length)
            signal_length = end_point;
    }
    
    signal_length -= start_point;

    
    x->params.m_pad = edges == audio_edges::zero_pad;
    x->params.m_signal_length = static_cast<long>(signal_length);
    x->params.m_sr = buffer.get_sample_rate();
    
    if (x->params.num_frames() < 1)
    {
        object_error((t_object *) x, "zero length file or segment");
        return;
    }
    
    // FIX - could this be a lamda to grab samples, or just pass the ibuffer?
    
    std::vector<double> samples(signal_length);
    
    ibuffer_get_samps(buffer, samples.data(), start_point + start_point, signal_length, static_cast<long>(buffer_chan));
        
    graph->prepare(x->params);
    graph->run(x->params, samples.data());
    graph->output(x->output_list.data());
    
    // Release buffer
    
    buffer.release();
    /*
    // Per frame values out
    
    std::vector<t_atom> per_frame_atoms(x->params.num_frames());
    
    for (size_t i = 0; i < graph->per_frame_descriptors_size(); i++)
    {
        graph->output_per_frame_descriptors(i, per_frame_atoms.data());
        outlet_list(x->m_features_outlet, nullptr, static_cast<short>(per_frame_atoms.size()), per_frame_atoms.data());
    }
    */
    // Output
    
    outlet_list(x->m_outlet, nullptr, static_cast<short>(x->output_list.size()), x->output_list.data());
}
