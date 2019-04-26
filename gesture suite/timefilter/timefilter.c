
/*
 *  timefilter
 *
 *	Originally intended for use with lists representing time values as part of a system of gestural representation, timefilter is an object for filtering and reordering values in a list of numbers.
 *
 *	The stored list is first sorted (if desired) either into ascending order or randomly.
 *	Values may then either be filtered randomly, or according to a minimum required distance between values (originally to avoid events timed too closely together), or a combination of the two.
 *	
 *	The object may obviously be used on lists representing any parameter and in various other scenarios than the one from which it takes its name.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>

#include <AH_Random.h>

void *this_class;


typedef enum {kOrderAscending, kOrderRandom, kOrderMaintain} t_ordering_mode;


typedef struct timefilter{

    t_object a_obj;
    
    float stored_list[1024];
	long stored_list_length;
	
    t_rand_gen gen;
    t_ordering_mode ordering;
	
	float filter;
	float randfilter;
	
    void *the_list_outlet;
    
} t_timefilter;


void timefilter_free(t_timefilter *x);
void *timefilter_new();
void timefilter_assist(t_timefilter *x, void *b, long m, long a, char *s);

void timefilter_list(t_timefilter *x, t_symbol *msg, long argc, t_atom *argv);
void timefilter_bang(t_timefilter *x);

void timefilter_float(t_timefilter *x, double filter);
void timefilter_randfilter(t_timefilter *x, double randfilter);
void timefilter_ordering(t_timefilter *x, t_atom_long ordering);
void timefilter_reset(t_timefilter *x);

void combsort(float *vals, long num_points);
void randomsort(t_rand_gen *gen, float *vals, long num_points);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// Basic object routines (main / new / free / assist) /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int C74_EXPORT main()
{
	this_class = class_new("timefilter",
						   (method)timefilter_new, 
						   (method)timefilter_free, 
						   sizeof(t_timefilter), 
						   NULL, 
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

void timefilter_free(t_timefilter *x)
{
}


void *timefilter_new()
{
	t_timefilter *x = (t_timefilter *)object_alloc(this_class);
	
    x->the_list_outlet = listout(x);
	x->stored_list_length = 0;
		
	x->filter = 0.;
	x->randfilter = 0.;
	x->ordering = kOrderAscending;
	
    rand_seed(&x->gen);
    
    return x;
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// List storage and filtering ////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void timefilter_list(t_timefilter *x, t_symbol *msg, long argc, t_atom *argv)
{
	// Store an input list
	
	float *stored_list = x->stored_list;
	long stored_list_length = x->stored_list_length;
	
    if (argc > 1024)
        object_error((t_object *)x, "maximum list length is 1024 items");
    
	while (argc-- && stored_list_length < 1024)
		stored_list[stored_list_length++] = atom_getfloat(argv++);
		
	x->stored_list_length = stored_list_length;
}


void timefilter_bang(t_timefilter *x)
{
	t_atom output_list[1024];
	t_atom *list_pointer = output_list;

	float *temp_vals = x->stored_list;
	float randfilter = x->randfilter;
	float filter = x->filter;
	float last_val = -FLT_MAX;
	float new_val;
	
	long stored_list_length = x->stored_list_length;
	long output_list_length = stored_list_length;
	long i;
	
	// Sort the input
	
	switch (x->ordering)
	{
		case kOrderAscending:
			combsort(temp_vals, stored_list_length);
			break;
		case kOrderRandom:
			randomsort(&x->gen, temp_vals, stored_list_length);
			break;
        case kOrderMaintain:
			break;
	}
	
	// Filtering is done here
		
	for (i = 0; i < stored_list_length; i++)
	{
		new_val = *temp_vals++;
		
		// Check if we are keeping this value
		
		if ((rand_double(&x->gen) > randfilter) && fabs(new_val - last_val) >= filter)
		{
			// If this value is within the filter distance of the next value, we randomly decide which one to lose and skip ahead if we choose this one 
			// This way the filtering works on distance, regardless of ordering...
			
			if (i < output_list_length - 1 && fabs(*temp_vals - new_val) < filter && rand_int_n(&x->gen, 1))
			{
				new_val = *temp_vals++;
				output_list_length--;
				i++;
			}
			
			atom_setfloat(list_pointer, new_val);
			list_pointer++;
			last_val = new_val;
		}
		else 
		{
			// If we aren't keeping it then the output size is one less
			
			output_list_length--;
		}
	}
	
	// Output and clear stored list
	
	outlet_list(x->the_list_outlet, 0L, output_list_length, output_list);
	x->stored_list_length = 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// Various routines for setting the filtering parameters ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void timefilter_float(t_timefilter *x, double filter)
{
	x->filter = fabs(filter);
}


void timefilter_randfilter(t_timefilter *x, double randfilter)
{
	x->randfilter = randfilter;
}


void timefilter_ordering(t_timefilter *x, t_atom_long ordering)
{
    t_ordering_mode mode;
    
    mode = (ordering == 1) ? kOrderRandom : kOrderAscending;
    mode = (ordering > 1) ? kOrderMaintain : mode;
    
    x->ordering = mode;
}


void timefilter_reset(t_timefilter *x)
{
	x->stored_list_length = 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////// Sorting functions ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void combsort(float *vals, long num_points)
{
	float f_temp;
	long gap = num_points;
	long swaps = 1;
	long i;
	
	
	while (gap > 1 || swaps)
	{
		if (gap > 1)
		{
			gap = (gap * 10) / 13;
			if (gap == 9 || gap == 10) gap = 11;
			if (gap < 1) gap = 1;
		}
		
		for (i = 0, swaps = 0; i + gap < num_points; i++)
		{
			if (vals[i] > vals[i + gap])
			{
				f_temp = vals[i];
				vals[i] = vals[i + gap];
				vals[i + gap] = f_temp;
				swaps = 1;	
			}
		}
	}
}


void randomsort(t_rand_gen *gen, float *vals, long num_points)
{
	// Put the input in a random order
	
	float f_temp;
	long pos;
	long i;
	
	for (i = 0; i < num_points - 1; i++)
	{
		f_temp = vals[i];
        pos = rand_int_n(gen, (num_points - (i + 1))) + i;
		vals[i] = vals[pos];
		vals[pos] = f_temp;
	}
}
