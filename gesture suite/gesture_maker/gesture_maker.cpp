
/*
 *  gesture_maker
 *
 *  gesture_maker is an object for creating multipart gestural control in MaxMSP.
 *  Typically, gesture_maker is used to drive a line~ object for audio rate control, and the output is internally scaled to give useful output for musical control.
 *
 *  Each gesture has two layers - the main layer, and the inflection layer.
 *  Each layer is scaled according to a certain range and scaling type, and the two layers are then combined through multiplication (which is appropriate for most musical cases).
 *
 *  The basis of the gesture is the kernel which is made up of up to three curves, which may take a variety of shapes, using power and s-shaped curves.
 *  The main layer has only one kernel shape per gesture - the inflection layer has one or more kernel shapes (as specified), which occur one after the other.
 *  Thus, the gestural model is hierarchical, and this may be extended thorough further linear combinations of gestures.
 *
 *  The shape of each gestural kernel over time can be given with varying degrees of specificity to allow either micro variation of a gesture, or radically different shapes from a single set of values.
 *  The timings of the inflection gestures can also be specified as desired - although these must be specified exactly in a normalised range (0 - 1).
 *
 *  The object can either be triggered to output values at regular intervals (drive mode) over a specified time period, or to produce output at given points in time (events mode).
 *  Additionally, gesture_maker objects may be chained together to control multiple parameters in parallel.
 *
 *  The details of the gesture_maker object are fairly involved, and the concepts are easier to understand in practice, rather than in verbal form.
 *  For this reason - it is probably best to look at the helpfile documentation for further information (it may be wise to do so before examining this code in detail).
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>

#include "gesture_kernel.hpp"
#include "gesture_convert.hpp"
#include "gesture_multipart.hpp"


constexpr int max_num_events = 256;

t_class *this_class;
    
t_symbol *ps_list;

struct t_gesture_maker
{
    t_object a_obj;
    
    gesture_kernel kernel_main;
    gesture_multipart multipart_inflections;
    
    gesture_convert convert_main;
    gesture_convert convert_inflections;
        
    double event_times[max_num_events];
    double grain_time;
    
    long num_events;
    long current_event;
    
    double gesture_length;
    double gesture_time;
    
    void *gesture_clock;
    
    void *gesture_drive_out;
    void *gesture_done_out;
    void *gesture_vals_out;
};

// Function Prototypes

void gesture_maker_free(t_gesture_maker *x);
void *gesture_maker_new();

void gesture_maker_stop(t_gesture_maker *x);
void gesture_maker_doclock(t_gesture_maker *x);
void gesture_maker_events(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv);
void gesture_maker_drive(t_gesture_maker *x, double val);
void gesture_maker_list(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv);
void gesture_maker_calc(t_gesture_maker *x, double phase, double grain_time);
void gesture_maker_graintime(t_gesture_maker *x, double val);

void gesture_maker_gesture_main(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv);
void gesture_maker_gesture_inflections(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv);

void gesture_maker_initial_main(t_gesture_maker *x, double val);
void gesture_maker_initial_inflections(t_gesture_maker *x, double val);

void gesture_maker_start_main(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv);
void gesture_maker_start_inflections(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv);

void gesture_maker_timings(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv);

void gesture_maker_scaling_main(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv);
void gesture_maker_scaling_inflections(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv);

void gesture_maker_reset(t_gesture_maker *x);

void gesture_maker_assist(t_gesture_maker *x, void *b, long m, long a, char *s);

// Main

int C74_EXPORT main()
{
    this_class = class_new("gesture_maker",
                           (method) gesture_maker_new,
                           (method) gesture_maker_free,
                           sizeof(t_gesture_maker),
                           (method) nullptr,
                           0);
    
    class_addmethod(this_class, (method) gesture_maker_drive, "drive", A_FLOAT, 0);
    class_addmethod(this_class, (method) gesture_maker_events, "events", A_GIMME, 0);
    class_addmethod(this_class, (method) gesture_maker_stop, "stop", 0);
    class_addmethod(this_class, (method) gesture_maker_graintime, "graintime", A_FLOAT,  0);
    class_addmethod(this_class, (method) gesture_maker_list, "list", A_GIMME,  0);
    class_addmethod(this_class, (method) gesture_maker_gesture_main, "gesture_main", A_GIMME,  0);
    class_addmethod(this_class, (method) gesture_maker_gesture_inflections, "gesture_inflections", A_GIMME,  0);
    class_addmethod(this_class, (method) gesture_maker_initial_main, "initial_main", A_FLOAT,  0);
    class_addmethod(this_class, (method) gesture_maker_initial_inflections, "initial_inflections", A_FLOAT,  0);
    class_addmethod(this_class, (method) gesture_maker_start_main, "start_main", A_GIMME,  0);
    class_addmethod(this_class, (method) gesture_maker_start_inflections, "start_inflections", A_GIMME,  0);
    class_addmethod(this_class, (method) gesture_maker_timings, "timings", A_GIMME,  0);
    class_addmethod(this_class, (method) gesture_maker_scaling_main, "scaling_main", A_GIMME,  0);
    class_addmethod(this_class, (method) gesture_maker_scaling_inflections, "scaling_inflections", A_GIMME,  0);
    class_addmethod(this_class, (method) gesture_maker_reset, "reset",  0);
    
    class_addmethod(this_class, (method) gesture_maker_assist, "assist", A_CANT, 0);
    
    class_addmethod(this_class, (method) gesture_maker_assist, "assist", A_CANT, 0);
    
    class_register(CLASS_BOX, this_class);
    
    ps_list = gensym("list");
        
    return 0;
}

// New / Free / Assist

void *gesture_maker_new()
{
    t_gesture_maker *x = (t_gesture_maker *) object_alloc(this_class);
    
    x->gesture_drive_out = listout(x);
    x->gesture_vals_out = listout(x);
    x->gesture_done_out = bangout(x);
    
    inlet_new(x, "list");
    
    // Set some default values
    
    x->gesture_clock = clock_new(x, (method) gesture_maker_doclock);
    x->grain_time = 20.0;
    
    gesture_maker_stop(x);
    gesture_maker_reset(x);
    x->convert_main = gesture_convert();
    x->convert_inflections = gesture_convert();
    
    return x;
}

void gesture_maker_free(t_gesture_maker *x)
{
    if (x->gesture_clock)
        freeobject((t_object *) x->gesture_clock);
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

// Initialisation Methods

void gesture_maker_reset(t_gesture_maker *x)
{
    // Reset the gesture_maker object
    
    x->kernel_main.reset();
    x->multipart_inflections.reset();
}

void gesture_maker_stop(t_gesture_maker *x)
{
    // Used internally and by users to cancel any gesture that is currently executing
    // NOte that this will *not* cause a bang message to be sent out of the done outlet
    
    clock_unset(x->gesture_clock);
    
    x->num_events = 0;
    x->current_event = -1;
    
    x->gesture_length = 0;
    x->gesture_time = 0;
}

// Timing and Calculation Methods

void gesture_maker_doclock(t_gesture_maker *x)
{
    // Called when the internal drive determines that the output should be calculated
    
    double *event_times = x->event_times;
    
    double grain_time = x->grain_time;
    double clock_time = grain_time;
    double gesture_time = x->gesture_time;
    double gesture_length = x->gesture_length;
    double phase = (gesture_length - gesture_time) / gesture_length;
    
    long current_event = x->current_event;
    long num_events = x->num_events;
    
    gesture_time = (gesture_time < 0.0) ? 0.0 : gesture_time;
    
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
    
    argc = (argc > max_num_events) ? max_num_events : argc;
    
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

void gesture_maker_drive(t_gesture_maker *x, double val)
{
    // Trigger the gesture in drive mode (regular output)
    
    val = fabs(val);
    
    gesture_maker_stop(x);
    
    x->gesture_length = val;
    x->gesture_time = val;
    
    clock_fdelay(x->gesture_clock, 0);
}

void gesture_maker_list(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv)
{
    if (argc < 2)
        return;
    
    // Accepts phase and grain_time input, typically for use when chaining objects together for parallel operation
    
    const double phase = atom_getfloat(argv++);
    const double grain_time = atom_getfloat(argv++);
    
    gesture_maker_calc(x, phase, grain_time);
}

void gesture_maker_calc(t_gesture_maker *x, double phase, double grain_time)
{
    t_atom output_list[2];
    atom_setfloat(output_list, phase);
    atom_setfloat(output_list + 1, grain_time);
    
    outlet_list(x->gesture_drive_out, ps_list, 2, output_list);
    
    // Calculate the main gesture value and the inflection value
    
    double main_val = x->kernel_main(phase);
    double inflection_val = x->multipart_inflections((t_object*) x, phase);
    
    // Convert and combine
    
    main_val = x->convert_main(main_val);
    inflection_val = x->convert_inflections(inflection_val);
    
    atom_setfloat(output_list, main_val * inflection_val);
    
    outlet_list(x->gesture_vals_out, ps_list, 2, output_list);
}

// Methods for Setting Gesture Parameters

void gesture_maker_graintime(t_gesture_maker *x, double val)
{
    if (val)
        x->grain_time = fabs(val);
}

void gesture_maker_gesture_main(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv)
{
    x->kernel_main.params((t_object *) x, argc, argv);
}

void gesture_maker_gesture_inflections(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv)
{
    x->multipart_inflections.params((t_object *) x, argc, argv);
}

void gesture_maker_initial_main(t_gesture_maker *x, double val)
{
    x->kernel_main.initial(val);
}

void gesture_maker_initial_inflections(t_gesture_maker *x, double val)
{
    x->multipart_inflections.initial(val);
}

void gesture_maker_start_main(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv)
{
    if (argc)
        x->kernel_main.initial_specifier((t_object *) x, argv);
}

void gesture_maker_start_inflections(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv)
{
    if (argc)
        x->multipart_inflections.initial_specifier((t_object *) x, argv);
}

void gesture_maker_timings(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv)
{
    x->multipart_inflections.timings((t_object *) x, argc, argv);
}

void gesture_maker_scaling_main(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv)
{
    x->convert_main.params((t_object *) x, argc, argv);
}

void gesture_maker_scaling_inflections(t_gesture_maker *x, t_symbol *s, long argc, t_atom *argv)
{
    x->convert_inflections.params((t_object *) x, argc, argv);
}
