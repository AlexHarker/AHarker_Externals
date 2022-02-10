
/*
 *  descriptorsrt~
 *
 *	descriptorsrt~ can be used to calculate audio descriptors (or features) in real-time from an incoming signal.
 *  The results are output as a list.
 *	descriptorsrt~ is intended to cover a wide range of tracking / following / detection applications.
 *	It is the real-time counterpart to the descriptors~ object and the two objects share many features.
 *
 *	The object only calculates and outputs the descriptors that the user requests (these can be changed in realtime).
 *	The object is designed to be as efficient as possible, avoiding re-calculations and making use of SIMD operations.
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
#include <HISSTools_FFT/HISSTools_FFT.h>

#include "descriptors_graph.hpp"


// Globals and Object Structure

t_class *this_class;

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
    
    // Sample Rate

    double sr;
    
    // General Parameters
    
    double energy_thresh;
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

#include "descriptors_temp.hpp"

// Function Prototypes

void *descriptorsrt_new(t_symbol *s, short argc, t_atom *argv);
void descriptorsrt_free(t_descriptorsrt *x);
void descriptorsrt_assist(t_descriptorsrt *x, void *b, long m, long a, char *s);

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
    class_addmethod(this_class, (method) descriptors_fft_params, "fftparams", A_GIMME, 0);
    //class_addmethod(this_class, (method) descriptors_energy_thresh, "energythresh", A_GIMME, 0);
    
    class_addmethod(this_class, (method) descriptorsrt_dsp, "dsp", A_CANT, 0);
    class_addmethod(this_class, (method) descriptorsrt_dsp64, "dsp64", A_CANT, 0);
    
    class_addmethod(this_class, (method) descriptorsrt_assist, "assist", A_CANT, 0);

	class_register(CLASS_BOX, this_class);
	
	return 0;
}

// New / Free / Assist

void *descriptorsrt_new(t_symbol *s, short argc, t_atom *argv)
{
    t_descriptorsrt *x = (t_descriptorsrt *) object_alloc(this_class);
	
	long descriptor_feedback = 0;
	long max_fft_size_in = 0;

	// Get arguments 
	
	if (argc) 
		max_fft_size_in = atom_getlong(argv++);
	if (argc > 1) 
		descriptor_feedback = atom_getlong(argv++);
	
	// Set maximum fft size
	
	long max_fft_size_log2 = descriptors_max_fft_size(x, max_fft_size_in);
	x->max_fft_size_log2 = max_fft_size_log2;
	x->max_fft_size = 1 << (max_fft_size_log2);
	
	dsp_setup((t_pxobject *) x, 1);
			
	// Allocate a clock and call the common new routine
	
	x->output_clock = clock_new(x, (method) descriptorsrt_output);
    x->sr = 44100.0;
    	
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

// Set Descriptors

void descriptorsrt_descriptors(t_descriptorsrt *x, t_symbol *msg, short argc, t_atom *argv)
{
    auto graph = new class graph();
    
    graph->build(x->params, argc, argv);
    
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

    // FIX - add energy thresh stuff
    
    //if (!x->use_energy_thresh || cumulate_sq_amps[fft_size_halved - 1] > x->energy_thresh)

    if (graph)
    {
        
        graph->run(samples, x->params.m_frame_size);
        graph->output(x->output_list.data());
        
        // Call clock to output
        
        clock_delay(x->output_clock, 0);
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
    long hop_size = x->params.m_hop_size;
	long hop_count = x->hop_count;
	long rw_counter = x->rw_counter;
	long frame_counter = rw_counter - x->params.m_frame_size + (buffer_size >> 1);
	
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
    long hop_size = x->params.m_hop_size;
	long hop_count = x->hop_count;
	long rw_counter = x->rw_counter;
    long frame_counter = rw_counter - x->params.m_frame_size + (buffer_size >> 1);

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
    x->sr = sp[0]->s_sr;

    // Add the perform routine

    dsp_add((t_perfroutine) descriptorsrt_perform, 3, sp[0]->s_vec, x, sp[0]->s_n);
}

void descriptorsrt_dsp64(t_descriptorsrt *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags)
{
	// Allocate the correct buffer size and zero

    x->rt_buffer.resize(2 * (x->max_fft_size + max_vec), 0.0);
	
	// Set variables
	
	x->rw_counter = 0;
	x->sr = sample_rate;
	
	// Add the perform routine
	
    object_method(dsp64, gensym("dsp_add64"), x, descriptorsrt_perform64, 0, nullptr);
}
