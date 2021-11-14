
/*
 *  timefilter
 *
 *  timefilter is an object for filtering and reordering values in a list of numbers.
 *
 *  timefilter was originally intended for use with lists representing time values as part of a system of gestural representation,
 *  The stored list is first sorted (if desired) either into ascending order or a random order.
 *  Values may then either be thinned (filterd) randomly and/or according to a minimum required distance between values.
 *  The original intention was to originally to avoid events timed too closely together.
 *
 *  The object may be used on lists representing any parameter / in other scenarios than the one from which it takes its name.
 *
 *  Copyright 2010-21 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>

#include <AH_Lifecycle.hpp>
#include <RandomGenerator.hpp>

#include <algorithm>


// Globals and Object Structure

t_class *this_class;

enum class ordering_mode
{
    ascending,
    random,
    maintain
};

struct t_timefilter
{
    t_object a_obj;
    
    double stored_list[1024];
    long stored_list_length;
    
    random_generator<> gen;
    ordering_mode ordering;
    
    double filter;
    double rand_filter;
    
    void *the_list_outlet;
};

// Function Prototypes

void *timefilter_new();
void timefilter_free(t_timefilter *x);
void timefilter_assist(t_timefilter *x, void *b, long m, long a, char *s);

void timefilter_list(t_timefilter *x, t_symbol *msg, long argc, t_atom *argv);
void timefilter_bang(t_timefilter *x);

void timefilter_float(t_timefilter *x, double filter);
void timefilter_randfilter(t_timefilter *x, double rand_filter);
void timefilter_ordering(t_timefilter *x, t_atom_long ordering);
void timefilter_reset(t_timefilter *x);

void combsort(double *vals, long num_points);
void randomsort(random_generator<>& gen, double *vals, long num_points);

// Main

int C74_EXPORT main()
{
    this_class = class_new("timefilter",
                           (method)timefilter_new,
                           (method)timefilter_free,
                           sizeof(t_timefilter),
                           nullptr,
                           0);
    
    class_addmethod(this_class, (method)timefilter_bang, "bang", 0);
    class_addmethod(this_class, (method)timefilter_float, "float", A_FLOAT, 0);
    class_addmethod(this_class, (method)timefilter_reset, "reset", 0);
    class_addmethod(this_class, (method)timefilter_randfilter, "randfilter", A_FLOAT, 0);
    class_addmethod(this_class, (method)timefilter_ordering, "ordering", A_LONG, 0);
    class_addmethod(this_class, (method)timefilter_list, "list", A_GIMME, 0);
    class_addmethod(this_class, (method)timefilter_assist, "assist", A_CANT, 0);
    
    class_register(CLASS_BOX, this_class);

    return 0;
}

// New / Free / Assist

void *timefilter_new()
{
    t_timefilter *x = (t_timefilter *)object_alloc(this_class);
    
    x->the_list_outlet = listout(x);
    x->stored_list_length = 0;
        
    x->filter = 0.0;
    x->rand_filter = 0.0;
    x->ordering = ordering_mode::ascending;
    
    create_object(x->gen);
    
    return x;
}

void timefilter_free(t_timefilter *x)
{
    destroy_object(x->gen);
}

void timefilter_assist(t_timefilter *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        switch (a)
    {
        case 0:
            sprintf(s,"Lists In / Float (Filter Value) / Bang to Filter");
            break;
    }
    else
        sprintf(s,"List Out");
}

// List storage

void timefilter_list(t_timefilter *x, t_symbol *msg, long argc, t_atom *argv)
{
    // Store an input list
    
    double *stored_list = x->stored_list;
    long stored_list_length = x->stored_list_length;
    
    if (argc > 1024)
        object_error((t_object *)x, "maximum list length is 1024 items");
    
    while (argc-- && stored_list_length < 1024)
        stored_list[stored_list_length++] = atom_getfloat(argv++);
        
    x->stored_list_length = stored_list_length;
}

// Filtering

void timefilter_bang(t_timefilter *x)
{
    t_atom output_list[1024];
    t_atom *list_pointer = output_list;

    double *temp_vals = x->stored_list;
    double rand_filter = x->rand_filter;
    double filter = x->filter;
    double last_val = -HUGE_VAL
    
    long stored_list_length = x->stored_list_length;
    long output_list_length = stored_list_length;
    
    // Sort the input
    
    switch (x->ordering)
    {
        case ordering_mode::ascending:      combsort(temp_vals, stored_list_length);            break;
        case ordering_mode::random:         randomsort(x->gen, temp_vals, stored_list_length);  break;
        case ordering_mode::maintain:                                                           break;
    }
    
    // Filtering is done here
        
    for (long i = 0; i < stored_list_length; i++)
    {
        new_val = *temp_vals++;
        
        // Check if we are keeping this value (if not decrease output count)
        
        if ((x->gen.rand_double() > rand_filter) && fabs(new_val - last_val) >= filter)
        {
            // If this value is within the filter distance of the next value  randomly decide which one to lose
            
            if (i < output_list_length - 1 && fabs(*temp_vals - new_val) < filter && x->gen.rand_int(1))
            {
                // Skip ahead if we choose to lose this value so that the filtering works on distance, regardless of ordering...
                
                // FIX - need to check for distance with previous value also?
                
                new_val = *temp_vals++;
                output_list_length--;
                i++;
            }
            
            atom_setfloat(list_pointer++, new_val);
            last_val = new_val;
        }
        else
            output_list_length--;
    }
    
    // Output and clear stored list
    
    outlet_list(x->the_list_outlet, 0L, output_list_length, output_list);
    x->stored_list_length = 0;
}

// Filering Parameters

void timefilter_float(t_timefilter *x, double filter)
{
    x->filter = fabs(filter);
}

void timefilter_randfilter(t_timefilter *x, double rand_filter)
{
    x->rand_filter = rand_filter;
}

void timefilter_ordering(t_timefilter *x, t_atom_long ordering)
{
    ordering_mode mode;
    
    mode = (ordering == 1) ? ordering_mode::random : ordering_mode::ascending;
    mode = (ordering > 1) ? ordering_mode::maintain : mode;
    
    x->ordering = mode;
}

void timefilter_reset(t_timefilter *x)
{
    x->stored_list_length = 0;
}

// Sorting functions

void combsort(double *vals, long num_points)
{
    long gap = num_points;
    bool swaps = true;
    
    while (gap > 1 || swaps)
    {
        if (gap > 1)
        {
            gap = (gap * 10) / 13;
            if (gap == 9 || gap == 10) gap = 11;
            if (gap < 1) gap = 1;
        }
        
        for (long i = 0, swaps = 0; i + gap < num_points; i++)
        {
            if (vals[i] > vals[i + gap])
            {
                std::swap(vals[i], vals[i + gap]);
                swaps = true;
            }
        }
    }
}

void randomsort(random_generator<>& gen, double *vals, long num_points)
{
    // Put the input in a random order
    
    for (long i = 0; i < num_points - 1; i++)
    {
        long pos = gen.rand_int(static_cast<uint32>(num_points - (i + 1))) + i;
        std::swap(vals[i], vals[pos]);
    }
}
