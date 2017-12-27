
/*
 *  gesture_maker
 *
 *	gesture_maker is an object for creating multipart gestural control in MaxMSP.
 *	Typically, gesture_maker is used to drive a line~ object for audio rate control, and the output is internally scaled to give useful output for musical control.
 *	
 *	Each gesture has two layers - the main layer, and the inflection layer. 
 *	Each layer is scaled according to a certain range and scaling type, and the two layers are then combined through multiplication (which is appropriate for most musical cases).
 *
 *	The basis of the gesture is the kernel which is made up of up to three curves, which may take a variety of shapes, using power and s-shaped curves.
 *	The main layer has only one kernel shape per gesture - the inflection layer has one or more kernel shapes (as specified), which occur one after the other.
 *	Thus, the gestural model is hierarchical, and this may be extended thorough further linear combinations of gestures.
 *
 *	The shape of each gestural kernel over time can be given with varying degrees of specificity to allow either micro variation of a gesture, or radically different shapes from a single set of values.
 *	The timings of the inflection gestures can also be specified as desired - although these must be specified exactly in a normalised range (0 - 1).
 *
 *	The object can either be triggered to output values at regular intervals (drive mode) over a specified time period, or to produce output at given points in time (events mode).
 *	Additionally, gesture_maker objects may be chained together to control multiple parameters in parallel.
 *
 *	The details of the gesture_maker object are fairly involved, and the concepts are easier to understand in practice, rather than in verbal form.
 *	For this reason - it is probably best to look at the helpfile documentation for further information (it may be wise to do so before examining this code in detail).
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>

#include "gesture_maker_kernel.h"
#include "gesture_maker_convert.h"
#include "gesture_maker_multipart.h"

#define MUX_NUM_EVENTS  256

void *this_class;
    

typedef struct gesture_maker
{
    t_object a_obj;
	
	t_gesture_kernel kernel_main;
	t_gesture_kernel kernel_inflections;
	
	t_gesture_maker_convert convert_main;
	t_gesture_maker_convert convert_inflections;
	
	t_gesture_multipart multipart_inflections;
	
	double event_times[MUX_NUM_EVENTS];
	double grain_time;
	
	long num_events;
	long current_event;
	
	double gesture_length;
	double gesture_time;
	
	void *gesture_clock;
	
	void *gesture_drive_out;
	void *gesture_done_out;
	void *gesture_vals_out;
    
} t_gesture_maker;


t_symbol *ps_list;


void gesture_maker_free(t_gesture_maker *x);
void *gesture_maker_new();
void gesture_maker_init(t_gesture_maker *x);

void gesture_maker_stop(t_gesture_maker *x);
void gesture_maker_doclock(t_gesture_maker *x);
void gesture_maker_events(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv);
void gesture_maker_drive(t_gesture_maker *x, double in_val);
void gesture_maker_list(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv);
void gesture_maker_calc(t_gesture_maker *x, double phase, double grain_time);
void gesture_maker_graintime(t_gesture_maker *x, double in_val);

void gesture_maker_gesture_main(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv);
void gesture_maker_gesture_inflections(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv);

void gesture_maker_initial_main(t_gesture_maker *x, double in_val);
void gesture_maker_initial_inflections(t_gesture_maker *x, double in_val);

void gesture_maker_timings(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv);

void gesture_maker_scaling_main(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv);
void gesture_maker_scaling_inflections(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv);

void gesture_maker_reset(t_gesture_maker *x);

void gesture_maker_assist(t_gesture_maker *x, void *b, long m, long a, char *s);


void gesture_maker_fixit(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv){}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// Basic object routines (main / new / free / assist) /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// FIX - review and reconsider non-GIMME methods...

// Safe methods for Max 5 threadsafety

#define SAFE_FLOAT_METHOD(base_method) void base_method##_safe(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv){if (argc) base_method(x, atom_getfloat(argv));}

SAFE_FLOAT_METHOD(gesture_maker_drive)
SAFE_FLOAT_METHOD(gesture_maker_graintime)
SAFE_FLOAT_METHOD(gesture_maker_initial_main)
SAFE_FLOAT_METHOD(gesture_maker_initial_inflections)

int C74_EXPORT main()
{
    this_class = class_new ("gesture_maker",
								(method) gesture_maker_new, 
								(method)gesture_maker_free, 
								sizeof(t_gesture_maker),
								NULL, 
								0);
	
	class_addmethod(this_class, (method)gesture_maker_drive_safe, "drive", A_GIMME, 0);
	class_addmethod(this_class, (method)gesture_maker_events, "events", A_GIMME, 0);
	class_addmethod(this_class, (method)gesture_maker_stop, "stop", 0);
	class_addmethod(this_class, (method)gesture_maker_graintime_safe, "graintime", A_GIMME,  0);
	class_addmethod(this_class, (method)gesture_maker_list, "list", A_GIMME,  0);
	class_addmethod(this_class, (method)gesture_maker_gesture_main, "gesture_main", A_GIMME,  0);
	class_addmethod(this_class, (method)gesture_maker_gesture_inflections, "gesture_inflections", A_GIMME,  0);
	class_addmethod(this_class, (method)gesture_maker_initial_main_safe, "initial_main", A_GIMME,  0);
	class_addmethod(this_class, (method)gesture_maker_initial_inflections_safe, "initial_inflections", A_GIMME,  0);
	
    // FIX - remove and fix patches...
	// N.B. The below solves some errors with a large number of incorrect message spellings in Fluence - to be corrected more properly at a later date
	
	class_addmethod(this_class, (method)gesture_maker_fixit, "inital_inflections", A_GIMME,  0);
	class_addmethod(this_class, (method)gesture_maker_fixit, "inital_main", A_GIMME,  0);
	class_addmethod(this_class, (method)gesture_maker_fixit, "initial_inflectiosn", A_GIMME,  0);
	class_addmethod(this_class, (method)gesture_maker_fixit, "initial", A_GIMME,  0);

	// End fixes
	
	class_addmethod(this_class, (method)gesture_maker_timings, "timings", A_GIMME,  0);
	class_addmethod(this_class, (method)gesture_maker_scaling_main, "scaling_main", A_GIMME,  0);
	class_addmethod(this_class, (method)gesture_maker_scaling_inflections, "scaling_inflections", A_GIMME,  0);
	class_addmethod(this_class, (method)gesture_maker_reset, "reset",  0);	
		
	class_addmethod(this_class, (method)gesture_maker_assist, "assist", A_CANT, 0);	
	
	class_register(CLASS_BOX, this_class);
	
	ps_list = gensym("list");
	
	gesture_maker_convert_setup();
	gesture_maker_kernel_params_setup();
		
	return 0;
}

void gesture_maker_free(t_gesture_maker *x)
{
	if (x->gesture_clock)
		freeobject((t_object *)x->gesture_clock);
}

void *gesture_maker_new()
{
	t_gesture_maker *x = (t_gesture_maker *)object_alloc(this_class);
	
	x->gesture_drive_out = listout(x);
	x->gesture_vals_out = listout(x);
	x->gesture_done_out = bangout(x);
	
	inlet_new(x, "list");
	
	gesture_maker_init(x);
	gesture_maker_kernel_reset (&x->kernel_main);
	gesture_maker_kernel_reset(&x->kernel_inflections);
	gesture_maker_multipart_reset(&x->multipart_inflections);
	gesture_maker_convert_init(&x->convert_main);
	gesture_maker_convert_init(&x->convert_inflections);
	
    return (x);
}

void gesture_maker_assist(t_gesture_maker *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) 
        switch (a)
	{
		case 0:
			sprintf(s,"Control");
			break;
			
		case 1:
			sprintf(s,"External Drive (for chaining)");
			break;
	}
    else
        switch (a)
	{
		case 0:
			sprintf(s,"Gesture Done (bang)");
			break;
			
		case 1:
			sprintf(s,"Gesture Value and Time");
			break;
			
		case 2:
			sprintf(s,"Drive Out (for chaining)");
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////// Initialisation routines //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void gesture_maker_init(t_gesture_maker *x)
{
	// Set some default values
	
	x->gesture_clock = clock_new(x, (method) gesture_maker_doclock);
	x->grain_time = 20.;
	
	gesture_maker_stop(x);
}

void gesture_maker_reset(t_gesture_maker *x)
{
	// Reset the gesture_maker object
	
	gesture_maker_kernel_reset(&x->kernel_main);
	gesture_maker_kernel_reset(&x->kernel_inflections);
	gesture_maker_multipart_reset(&x->multipart_inflections);
}

void gesture_maker_stop(t_gesture_maker *x)
{	
	// The stop routine is used internally and by users to cancel any gesture that is currently exectuing (this will *not* cause a bang message to be sent out of the done outlet)
	
    clock_unset(x->gesture_clock);
	
	x->num_events = 0;
	x->current_event = -1;
	
	x->gesture_length = 0;
	x->gesture_time = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////// Timing and calculation routines //////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void gesture_maker_doclock(t_gesture_maker *x)
{	
	// This routine is the clock routine which is called when the internal drive determines that the output should be calculated
	
	double *event_times = x->event_times;
	
	double grain_time = x->grain_time;
	double clock_time = grain_time;
	double gesture_time = x->gesture_time;
	double gesture_length = x->gesture_length;
	double phase = (gesture_length - gesture_time) / gesture_length;
	
	long current_event = x->current_event;
	long num_events = x->num_events;
	
	if (gesture_time < 0.)
		gesture_time = 0.;
	
	// Determine the time until the next output
	
	if (current_event == -1)
	{
		if (clock_time > gesture_time)
			clock_time = gesture_time;
	}
	else 
	{
		if (current_event != num_events)
		   clock_time = grain_time = event_times[current_event];
		else
			clock_time = grain_time = 0.0;
		x->current_event = current_event + 1;
	}
	
	// Calculate output
	
	gesture_maker_calc(x, phase, grain_time);
	
	// If the gesture is over then bang the done outlet, otherwise set the clock for the next output
	
	if (gesture_time == 0.0 || (current_event != -1 && current_event > num_events))
		outlet_bang(x->gesture_done_out);
	else 
	{
		x->gesture_time = gesture_time - clock_time;
		clock_fdelay(x->gesture_clock, clock_time);
	}
}

void gesture_maker_events(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv)
{
	double *event_times = x->event_times;
	double event_time;
	double total_time = 0.;
	
	// Set the object going in events mode, storing the relevant timings and variables first
	
	if (argc > MUX_NUM_EVENTS)
		argc = MUX_NUM_EVENTS;
	
	gesture_maker_stop(x);
	
	x->num_events = argc;
	
	while (argc--) 
	{
		event_time = fabs(atom_getfloat(argv++));
		total_time += event_time;
		*event_times++ = event_time;
	}
	
	x->gesture_length = total_time;
	x->gesture_time = total_time;
	x->current_event = 0;
	
	clock_fdelay(x->gesture_clock, 0);
}

void gesture_maker_drive(t_gesture_maker *x, double in_val)
{
	// Trigger the gesture in drive mode (regular output)
	
	in_val = fabs(in_val);
	
	gesture_maker_stop(x);
	
	x->gesture_length = in_val;
	x->gesture_time = in_val;
	
	clock_fdelay(x->gesture_clock, 0);
}

void gesture_maker_list(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv)
{
	double phase;
	double grain_time;
	
	if (argc < 2)
		return;
	
	// The list input is for accepting phase and grain_time input, typically for use when chaining objects together for parallel operation
	
	phase = atom_getfloat(argv++);
	grain_time = atom_getfloat(argv++);
	
	gesture_maker_calc(x, phase, grain_time);
}

void gesture_maker_calc(t_gesture_maker *x, double phase, double grain_time)
{
	double main_val, multiphase, inflection_val;
	t_atom output_list[2];
	atom_setfloat(output_list, phase);
	atom_setfloat(output_list + 1, grain_time);

	outlet_list(x->gesture_drive_out, ps_list, 2, output_list);
	
	// get the main gesture value, update the inflection gesture and calculate the inflection value
	
	main_val = gesture_maker_kernel_calc(&x->kernel_main, phase);
	multiphase = gesture_maker_multipart_phase(&x->multipart_inflections, &x->kernel_inflections, phase);
	inflection_val = gesture_maker_kernel_calc(&x->kernel_inflections, multiphase);
	
	// Convert and combine
	
	main_val = gesture_maker_convert_scale(&x->convert_main, main_val);
	inflection_val = gesture_maker_convert_scale(&x->convert_inflections, inflection_val);
	
	atom_setfloat(output_list, main_val * inflection_val);
	
	outlet_list(x->gesture_vals_out, ps_list, 2, output_list);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// Various routines for setting the gesture parameters ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void gesture_maker_graintime(t_gesture_maker *x, double in_val)
{
	if (in_val)
		x->grain_time = fabs(in_val);
}

void gesture_maker_gesture_main(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv)
{
	gesture_maker_kernel_params(&x->kernel_main, argc, argv);
}

void gesture_maker_gesture_inflections(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv)
{
	gesture_maker_multipart_params(&x->multipart_inflections, argc, argv);

}	

void gesture_maker_initial_main(t_gesture_maker *x, double in_val)
{
	gesture_maker_kernel_initial(&x->kernel_main, in_val);
}

void gesture_maker_initial_inflections(t_gesture_maker *x, double in_val)
{
	gesture_maker_kernel_initial(&x->kernel_inflections, in_val);
}

void gesture_maker_timings(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv)
{
	gesture_maker_multipart_timings(&x->multipart_inflections, argc, argv);
}

void gesture_maker_scaling_main(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv)
{
	gesture_maker_convert_params(&x->convert_main, argc, argv);
}

void gesture_maker_scaling_inflections(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv)
{
	gesture_maker_convert_params(&x->convert_inflections, argc, argv);
}


