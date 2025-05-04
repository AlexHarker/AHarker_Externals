
/*
 *  timefilter
 *
 *  timefilter is an object for filtering and reordering values in a list of numbers.
 *
 *  The stored list is first optionally sorted either into ascending order or a random order.
 *  Values may then either be thinned randomly and/or according to a minimum required distance between values.
 *
 *  timefilter was created for use with numbers representing time values as part of a system of gestural representation,
 *  The original intention was to originally to avoid events timed too closely together.
 *  However, it might also be useful in other scenarios. name.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
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
    
    ordering_mode ordering;
    
    double filter;
    double rand_filter;

    double stored_list[1024];
    long stored_length;
    
    random_generator<> gen;

    void *list_outlet;
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
                           (method) timefilter_new,
                           (method) timefilter_free,
                           sizeof(t_timefilter),
                           (method) nullptr,
                           0);
    
    class_addmethod(this_class, (method) timefilter_bang, "bang", 0);
    class_addmethod(this_class, (method) timefilter_float, "float", A_FLOAT, 0);
    class_addmethod(this_class, (method) timefilter_reset, "reset", 0);
    class_addmethod(this_class, (method) timefilter_randfilter, "randfilter", A_FLOAT, 0);
    class_addmethod(this_class, (method) timefilter_ordering, "ordering", A_LONG, 0);
    class_addmethod(this_class, (method) timefilter_list, "list", A_GIMME, 0);
    class_addmethod(this_class, (method) timefilter_assist, "assist", A_CANT, 0);
    
    class_register(CLASS_BOX, this_class);

    return 0;
}

// New / Free / Assist

void *timefilter_new()
{
    t_timefilter *x = (t_timefilter *) object_alloc(this_class);
    
    x->list_outlet = listout(x);
    x->stored_length = 0;
        
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
    static constexpr int maxAssist = 256;

    if (m == ASSIST_INLET)
        switch (a)
    {
        case 0:
            snprintf(s, maxAssist, "Lists In / Float (Filter Value) / Bang to Filter");
            break;
    }
    else
        snprintf(s, maxAssist, "List Out");
}

// List Storage

void timefilter_list(t_timefilter *x, t_symbol *msg, long argc, t_atom *argv)
{
    // Store an input list
    
    double *stored_list = x->stored_list;
    long stored_list_length = x->stored_length;
    
    if (argc > 1024)
        object_error((t_object *) x, "maximum list length is 1024 items");
    
    while (argc-- && stored_list_length < 1024)
        stored_list[stored_list_length++] = atom_getfloat(argv++);
        
    x->stored_length = stored_list_length;
}

// Filtering

void timefilter_bang(t_timefilter *x)
{
    t_atom output_list[1024];

    double *vals_pointer = x->stored_list;
    double rand_filter = x->rand_filter;
    double filter = x->filter;
    double last_val = -HUGE_VAL;
    
    long stored_length = x->stored_length;
    long output_length = 0;
    
    // Sort the input
    
    switch (x->ordering)
    {
        case ordering_mode::ascending:      combsort(vals_pointer, stored_length);              break;
        case ordering_mode::random:         randomsort(x->gen, vals_pointer, stored_length);    break;
        case ordering_mode::maintain:                                                           break;
    }
    
    // Filtering is done here
        
    for (long i = 0; i < stored_length; i++)
    {
        double val = *vals_pointer++;
        
        // Check if we are keeping this value
        
        if ((x->gen.rand_double() > rand_filter) && fabs(val - last_val) >= filter)
        {
            // If this value is within the filter distance of the next value randomly decide which one to lose
            
            if (i < stored_length - 1 && fabs(*vals_pointer - val) < filter && x->gen.rand_int(1))
            {
                // Check that the new value is not too close to the last value (if so keep the current value)
                
                if (!output_length || fabs(*vals_pointer - last_val) >= filter)
                {
                    // If we choose to lose this value skip to the next
                
                    val = *vals_pointer++;
                    i++;
                }
            }
            
            atom_setfloat(output_list + output_length++, val);
            last_val = val;
        }
    }
    
    // Output and clear stored list
    
    outlet_list(x->list_outlet, nullptr, static_cast<short>(output_length), output_list);
    x->stored_length = 0;
}

// User Methods Setting Filtering Parameters

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
    x->stored_length = 0;
}

// Sorting Functions

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
        
        swaps = false;
        
        for (long i = 0; i + gap < num_points; i++)
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
        long pos = gen.rand_int(static_cast<uint32_t>(num_points - (i + 1))) + i;
        std::swap(vals[i], vals[pos]);
    }
}
