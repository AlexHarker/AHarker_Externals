
/*
 *  descriptorsrt~
 *
 *	descriptorsrt~ is used to calculate audio descriptors (or features) in real-time from an incoming signal.
 *  The results are output as a list.
 *	descriptorsrt~ is intended to cover a wide range of tracking / following / detection applications.
 *	It is the real-time counterpart to the descriptors~ object and the two objects share many features.
 *
 *	The object calculates and outputs only the requested descriptors
 *  Descriptors can be changed in realtime).
 *	The object is designed with efficiency in mind, avoiding re-calculations and making use of SIMD operations.
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

#include <AH_Lifecycle.hpp>

#include "descriptors_fft_params.hpp"
#include "descriptors_graph.hpp"

#include "modules_core.hpp"
#include "modules_change.hpp"
#include "modules_content.hpp"
#include "modules_level.hpp"
#include "modules_pitch.hpp"
#include "modules_spectral.hpp"

// Globals and Object Structure

t_class *this_class;

setup_list s_setups;

struct t_descriptorsrt
{
    t_pxobject x_obj;
    
    // RT Data
    
    std::vector<double> rt_buffer;
    
    long rw_counter;
    long hop_count;

    // FFT / Scheduling Data
    
    long max_fft_size_log2;
    long max_fft_size;
    
    global_params params;
    
    bool reset;
    
    // General Parameters
    
    bool descriptors_feedback;
    
    // Descriptors
    
    std::unique_ptr<graph> m_graph;
    
    // Output List
    
    std::vector<t_atom> output_list;
   
    // Outlet
    
    void *m_outlet;
    
    // Output Clock
    
    t_clock *output_clock;
};

// Function Prototypes

void *descriptorsrt_new(t_symbol *s, short argc, t_atom *argv);
void descriptorsrt_free(t_descriptorsrt *x);
void descriptorsrt_assist(t_descriptorsrt *x, void *b, long m, long a, char *s);

void descriptorsrt_fft_params(t_descriptorsrt *x, t_symbol *msg, short argc, t_atom *argv);
void descriptors_energy_thresh(t_descriptorsrt *x, t_symbol *msg, short argc, t_atom *argv);
void descriptorsrt_descriptors(t_descriptorsrt *x, t_symbol *msg, short argc, t_atom *argv);

void descriptorsrt_output(t_descriptorsrt *x);
void descriptorsrt_calculate(t_descriptorsrt *x, double *samples);

void descriptorsrt_dsp(t_descriptorsrt *x, t_signal **sp, short *count);
void descriptorsrt_dsp64(t_descriptorsrt *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags);

// Main

int C74_EXPORT main()
{
	this_class = class_new("descriptorsrt~",
                           (method) descriptorsrt_new,
                           (method) descriptorsrt_free,
                           sizeof(t_descriptorsrt),
                           0L,
                           A_GIMME,
                           0);
	
	class_addmethod(this_class, (method) descriptorsrt_descriptors, "descriptors", A_GIMME, 0);
    class_addmethod(this_class, (method) descriptorsrt_fft_params, "fftparams", A_GIMME, 0);
    class_addmethod(this_class, (method) descriptors_energy_thresh, "energythresh", A_GIMME, 0);
    
    class_addmethod(this_class, (method) descriptorsrt_dsp, "dsp", A_CANT, 0);
    class_addmethod(this_class, (method) descriptorsrt_dsp64, "dsp64", A_CANT, 0);
    
    class_addmethod(this_class, (method) descriptorsrt_assist, "assist", A_CANT, 0);

    class_dspinit(this_class);
    
	class_register(CLASS_BOX, this_class);
	
    // Per-frame descriptors
    
    s_setups.add_module("abs", module_average_abs_amp::setup);
    s_setups.add_module("rms", module_average_rms_amp::setup);
    s_setups.add_module("peakamp", module_peak_amp::setup);
    
    s_setups.add_module("energy", module_energy::setup);
    s_setups.add_module("energy_ratio", module_energy_ratio::setup);
    
    s_setups.add_module("loudness", module_loudness::setup);
    s_setups.add_module("rolloff", module_rolloff::setup);
    s_setups.add_module("sfm", module_sfm::setup);
    s_setups.add_module("spectral_crest", module_spectral_crest::setup);
    
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

    s_setups.add_module("flux", module_flux::setup);
    s_setups.add_module("foote", module_foote::setup);
    s_setups.add_module("mkl", module_mkl::setup);

    s_setups.add_module("spectral_peaks", module_spectral_peaks::setup);
    s_setups.add_module("inharmonicity", module_inharmonicity::setup);
    s_setups.add_module("roughness", module_roughness::setup);

	return 0;
}

// New / Free / Assist

void *descriptorsrt_new(t_symbol *s, short argc, t_atom *argv)
{
    t_descriptorsrt *x = (t_descriptorsrt *) object_alloc(this_class);
	
	long max_fft_size = 0;

	// Get arguments 
	
	if (argc) 
		max_fft_size = atom_getlong(argv++);
	if (argc > 1) 
		x->descriptors_feedback = atom_getlong(argv++);
	
	// Set maximum fft size
	
    x->max_fft_size_log2 = check_fft_size((t_object *) x, "maximum fft size", max_fft_size, 0);
	x->max_fft_size = 1 << x->max_fft_size_log2;
	
    long default_fft_size = 4096 <= x->max_fft_size ? 4096 : x->max_fft_size;
    
    x->params.m_fft_params = check_fft_params((t_object *) x, default_fft_size, 0, 0, nullptr, x->max_fft_size_log2);
    
	dsp_setup((t_pxobject *) x, 1);
			
	// Allocate a clock and call the common new routine
	
	x->output_clock = clock_new(x, (method) descriptorsrt_output);
    x->params.m_energy_threshold = dbtoa(-180.0);
    x->params.m_sr = 44100.0;
    x->m_outlet = listout(x);

    x->hop_count = x->params.hop_size();
    x->params.m_signal_length = x->params.frame_size();
    x->reset = true;
   
    create_object(x->output_list);
    create_object(x->rt_buffer);
    create_object(x->m_graph);

	return x;
}

void descriptorsrt_free(t_descriptorsrt *x)
{
	dsp_free(&x->x_obj);
    object_free(x->output_clock);
    destroy_object(x->output_list);
    destroy_object(x->rt_buffer);
    destroy_object(x->m_graph);
}

void descriptorsrt_assist(t_descriptorsrt *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
    {
        sprintf(s,"Descriptors Out");
    }
    else
    {
        sprintf(s,"(signal / messages) All Messages / Input");
    }
}

// Energy Threshold

void descriptors_energy_thresh(t_descriptorsrt *x, t_symbol *msg, short argc, t_atom *argv)
{
    if (argc)
        x->params.m_energy_threshold = dbtoa(atom_getfloat(argv));
    else
        x->params.m_energy_threshold = 0.0;
    
    if (argc > 2)
        object_error((t_object *) x, "too many arguments to energythresh message");
}

// FFT Params

void descriptorsrt_fft_params(t_descriptorsrt *x, t_symbol *msg, short argc, t_atom *argv)
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
    
    x->params.m_fft_params = params;
    x->params.m_signal_length = x->params.frame_size();

    x->hop_count = x->params.hop_size();
    x->reset = true;
    
    if (x->m_graph)
        x->m_graph->prepare(x->params);
}

// Set Descriptors

void descriptorsrt_descriptors(t_descriptorsrt *x, t_symbol *msg, short argc, t_atom *argv)
{
    auto graph = new class graph();
    
    graph->build(s_setups, x->params, argc, argv);
    x->output_list.resize(graph->size());
    x->m_graph.reset(graph);
}

// RT Output

void descriptorsrt_output(t_descriptorsrt *x)
{
    outlet_list(x->m_outlet, nullptr, static_cast<short>(x->output_list.size()), x->output_list.data());
}

// Handle RT Descriptor Calculation

void descriptorsrt_calculate(t_descriptorsrt *x, double *samples)
{
    auto& graph = x->m_graph;

    // Run and call clock to output

    if (graph)
    {
        if (graph->run(x->params, samples))
        {
            graph->output(x->output_list.data());
            clock_delay(x->output_clock, 0);
        }
    }
}

// Perform

t_int *descriptorsrt_perform(t_int *w)
{
	float *in1 = (float *) w[1];
	t_descriptorsrt *x = (t_descriptorsrt *) w[2];
	long vec_size = w[3];
	
	auto& rt_buffer = x->rt_buffer;
	
    long buffer_size = static_cast<long>(rt_buffer.size());
    long hop_size = x->params.hop_size();
	long hop_count = x->hop_count;
	long rw_counter = x->rw_counter;
    long frame_counter = rw_counter - x->params.frame_size() + (buffer_size >> 1);
	
	// Reset
	
	if (x->reset)
	{
		rw_counter = 0;
		hop_count = hop_size;
        x->reset = false;
	}
	
	// Write input block into double circular buffer
	
	for (long i = 0; i < vec_size; i++)
	{
		rt_buffer[rw_counter + (buffer_size >> 1)] = rt_buffer[rw_counter] = in1[i];
        rw_counter = (rw_counter + 1) % (buffer_size >> 1);
	}
	
    // Check for a frame due and perform calculations

	while (vec_size)
	{
        long hop_to_do = (vec_size < hop_size) ? vec_size : hop_size;
		hop_count -= hop_to_do;
		vec_size -= hop_to_do;
        frame_counter += hop_to_do;
		
		if (!hop_count)
		{
            descriptorsrt_calculate(x, rt_buffer.data() + frame_counter % (buffer_size >> 1));
			hop_count = hop_size;
		}
	}
		
	// Store variables
	
	x->rw_counter = rw_counter;
	x->hop_count = hop_count;
	
	return w + 4;
}

void descriptorsrt_perform64(t_descriptorsrt *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
	double *in1 = ins[0];
	
	auto& rt_buffer = x->rt_buffer;
	
	long buffer_size = static_cast<long>(rt_buffer.size());
    long hop_size = x->params.hop_size();
	long hop_count = x->hop_count;
	long rw_counter = x->rw_counter;
    long frame_counter = rw_counter - x->params.frame_size() + (buffer_size >> 1);

    // Reset

	if (x->reset)
	{
		rw_counter = 0;
		hop_count = hop_size;
		x->reset = false;
	}
	
	// Write input block into double circular buffer
	
	for (long i = 0; i < vec_size; i++)
	{
        rt_buffer[rw_counter + (buffer_size >> 1)] = rt_buffer[rw_counter] = in1[i];
        rw_counter = (rw_counter + 1) % (buffer_size >> 1);
	}
	
	// Check for a frame due and perform calculations
	
	while (vec_size)
	{
		long hop_to_do = (vec_size < hop_count) ? vec_size : hop_count;
		hop_count -= hop_to_do;
		vec_size -= hop_to_do;
        frame_counter += hop_to_do;

		if (!hop_count)
		{
            descriptorsrt_calculate(x, rt_buffer.data() + frame_counter % (buffer_size >> 1));
			hop_count = hop_size;
		}
	}
	
	// Store variables
	
	x->rw_counter = rw_counter;
	x->hop_count = hop_count;
}

// DSP

void descriptorsrt_dsp(t_descriptorsrt *x, t_signal **sp, short *count)
{
    // Allocate the correct buffer size and zero

    x->rt_buffer.resize(2 * (x->max_fft_size + sp[0]->s_n), 0.0);
        
    // Set variables
    
    x->rw_counter = 0;
    x->params.m_sr = sp[0]->s_sr;

    // Add the perform routine

    dsp_add((t_perfroutine) descriptorsrt_perform, 3, sp[0]->s_vec, x, sp[0]->s_n);
}

void descriptorsrt_dsp64(t_descriptorsrt *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags)
{
	// Allocate the correct buffer size and zero

    x->rt_buffer.resize(2 * (x->max_fft_size + max_vec), 0.0);
	
	// Set variables
	
	x->rw_counter = 0;
    x->params.m_sr = sample_rate;
	
	// Add the perform routine
	
    object_method(dsp64, gensym("dsp_add64"), x, descriptorsrt_perform64, 0, nullptr);
}
