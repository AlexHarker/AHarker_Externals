
/*
 *  entrymatcher~
 *
 *	entrymatcher~ is the audio rate counterpart of entrymatcher, capable of matching with sample-accurate timing.
 *	It is intended primarily for use in granular synthesis applications requiring sample accuracy for audio matching using descriptors.
 *
 *	entrymatcher~ does not offer all the features of entrymatcher, and there are some key differences to the max version:
 *
 *	1 - entrymatcher~ takes values for matching at signal rate, and has a different message structure for setting matching criteria.
 *	2 - entrymatcher~ randomly selects a value from all the valid matches and outputs only that one value.
 *	3 - for reasons of efficiency the matching process and the randomisation processes can be triggered separately to allow variation over small time scales, without the expense of recalculating matches. 
 *	4 - only numeric data can be used (although entries and columns can still be referred to using symbols).
 *
 *	In practice a large number of matching scenarios and data lookup requirements can be satisfied.
 *	The features of the entrymatcher~ object are covered in detail in the helpfile documentation.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */



#include <ext.h>
#include <ext_obex.h>
#include <math.h>
#include <z_dsp.h>

#include <AH_Random.h>


void *this_class;
    

enum TestType {
	
	TEST_NONE,
	TEST_MATCH,
	TEST_DISTANCE,
	TEST_LESS_THAN,
	TEST_GREATER_THAN,
	TEST_LESS_THAN_EQ,
	TEST_GREATER_THAN_EQ,
	TEST_SCALE,
	TEST_WITHIN,
	TEST_DISTANCE_RATIO,
	TEST_SCALE_RATIO,
	TEST_WITHIN_RATIO
};


typedef struct entrymatcher {

    t_pxobject x_obj;
   
	long num_entries;
	long max_entries;
	long num_columns;
	
	long max_matchers;
	long n_limit;
	double ratio_kept;
	
	long num_matched_indices;
	
	double *the_data;
	double *match_params;
	double *distances;
	
	t_atom *entry_identifiers;
	long *matched_indices;
		
	t_symbol **names;
	
	t_rand_gen gen;
	
	float *matcher_ins[256];
    
} t_entrymatcher;


t_symbol *ps_null;
t_symbol *ps_list;
t_symbol *ps_match_ratio;
t_symbol *ps_lookup;

t_symbol *ps_distance;
t_symbol *ps_match;
t_symbol *ps_scale;
t_symbol *ps_within;
t_symbol *ps_less;
t_symbol *ps_greater;
t_symbol *ps_lesseq;
t_symbol *ps_greatereq;
t_symbol *ps_distance_ratio;
t_symbol *ps_scale_ratio;
t_symbol *ps_within_ratio;

t_symbol *ps_sym_distance;
t_symbol *ps_sym_match;
t_symbol *ps_sym_scale;
t_symbol *ps_sym_within;
t_symbol *ps_sym_less;
t_symbol *ps_sym_greater;
t_symbol *ps_sym_lesseq;
t_symbol *ps_sym_greatereq;
t_symbol *ps_sym_distance_ratio;
t_symbol *ps_sym_scale_ratio;
t_symbol *ps_sym_within_ratio;

static __inline double atom_getdouble_translate (t_atom *a);

void *entrymatcher_new(long max_entries, long num_columns, long max_matchers);
void entrymatcher_free(t_entrymatcher *x);
void entrymatcher_assist(t_entrymatcher *x, void *b, long m, long a, char *s);

void entrymatcher_clear(t_entrymatcher *x);
void entrymatcher_names(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv);
void entrymatcher_entry(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv);

char entrymatcher_compare_identifiers(t_atom *identifier1, t_atom *identifier2);
long entrymatcher_test_types(t_atom *argv);
long entrymatcher_column_from_specifier(t_entrymatcher *x, t_atom *arg);

void entrymatcher_limit(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv);
void entrymatcher_matchers(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv);
long entrymatcher_calculate(t_entrymatcher *x, double *comparision_vals);

t_int *entrymatcher_perform (t_int *w);
void entrymatcher_dsp(t_entrymatcher *x, t_signal **sp, short *count);

void entrymatcher_perform64 (t_entrymatcher *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void entrymatcher_dsp64(t_entrymatcher *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

// ========================================================================================================================================== //
// Helper for translating any of the three basic MaxMSP data types to double
// ========================================================================================================================================== //


static __inline double atom_getdouble_translate (t_atom *arg)
{
	switch (atom_gettype(arg))
	{
		case A_LONG:
            return atom_getlong(arg);
			break;
		case A_FLOAT:
			return atom_getfloat(arg);
			break;
		case A_SYM:
			return (long) atom_getsym(arg);
			break;
		default:
			return 0.;
	}
}


// ========================================================================================================================================== //
// Basic object routines: main, new, free and assist
// ========================================================================================================================================== //


int C74_EXPORT main(void)
{
	this_class	= class_new	("entrymatcher~",
				(method) entrymatcher_new, 
				(method)entrymatcher_free, 
				(short)sizeof(t_entrymatcher), 
				NULL, 
				A_DEFLONG, 
				A_DEFLONG,
				A_DEFLONG
,				0);
	
	class_addmethod(this_class, (method)entrymatcher_clear,"clear", 0);
	class_addmethod(this_class, (method)entrymatcher_clear,"reset", 0);

	class_addmethod(this_class, (method)entrymatcher_entry,"entry", A_GIMME, 0);
	class_addmethod(this_class, (method)entrymatcher_limit,"limit", A_GIMME, 0);
	class_addmethod(this_class, (method)entrymatcher_matchers,"matchers", A_GIMME, 0);
	class_addmethod(this_class, (method)entrymatcher_names,"names", A_GIMME, 0);
	class_addmethod(this_class, (method)entrymatcher_assist, "assist", A_CANT, 0);
	
	class_addmethod(this_class, (method)entrymatcher_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)entrymatcher_dsp64, "dsp64", A_CANT, 0);

	class_dspinit(this_class);
	
	class_register(CLASS_BOX, this_class);
	
	ps_null = gensym("");
	ps_list = gensym("list");
	ps_match_ratio = gensym("match_ratio");
	ps_lookup = gensym("lookup");
	
	ps_distance = gensym("distance");
	ps_match = gensym("match");
	ps_scale = gensym("scale");
	ps_within = gensym("within");
	ps_less = gensym("less");
	ps_greater = gensym("greater");
	ps_lesseq = gensym("lesseq");
	ps_greatereq = gensym("greatereq");
	ps_distance_ratio = gensym("distance_ratio");
	ps_scale_ratio = gensym("scale_ratio");
	ps_within_ratio = gensym("within_ratio");
	
	ps_sym_distance = gensym("-");
	ps_sym_match = gensym("==");
	ps_sym_scale = gensym("%");
	ps_sym_within = gensym("<->"); 
	ps_sym_less = gensym("<");
	ps_sym_greater = gensym(">");
	ps_sym_lesseq = gensym("<=");
	ps_sym_greatereq = gensym(">=");
	ps_sym_distance_ratio = gensym("/");
	ps_sym_scale_ratio = gensym("%%");
	ps_sym_within_ratio = gensym("</>");
	
	return 0;
}


void *entrymatcher_new(long max_entries, long num_columns, long max_matchers)
{
	t_entrymatcher *x = (t_entrymatcher *)object_alloc(this_class);
	
	t_symbol **names;
	void *allocated_mem;
	long i;
	
	if (max_entries < 1) 
		max_entries = 1;
	if (num_columns < 1) 
		num_columns = 1;
	if (max_matchers < 1)
		max_matchers = 1;
	if (max_matchers > 256)
		max_matchers = 256;
	
	dsp_setup((t_pxobject *)x, 2 + max_matchers);
	outlet_new((t_object *)x, "signal");

	// Allocate large memory block and then assign to various pointers

	allocated_mem = malloc (((((2 + num_columns) * max_entries) + 1024) * sizeof(double)) + (max_entries * sizeof(long)) + (max_entries * sizeof(t_atom)) + (num_columns * sizeof(t_symbol *)));
	
	if (!allocated_mem) return 0;
	
	x->entry_identifiers = allocated_mem;
	allocated_mem = (void *) ((t_atom *) allocated_mem + max_entries);	
	
	x->matched_indices = allocated_mem;
	allocated_mem = (void *) ((long *) allocated_mem + max_entries);	
	
	x->match_params = allocated_mem;
	allocated_mem = (void *) ((double *) allocated_mem + 1024);	
	
	x->distances = allocated_mem;
	allocated_mem = (void *) ((double *) allocated_mem + max_entries);	
	
	x->names = names = allocated_mem;
	allocated_mem = (void *) ((t_symbol **) allocated_mem + num_columns);	
	
	x->the_data = allocated_mem;
	allocated_mem = (void *) ((double *) allocated_mem + max_entries);	
	
	// Set column names to null values
	
	for (i = 0; i < num_columns; i++)
		names[i] = ps_null;
		
	x->max_entries = max_entries;
	x->num_columns = num_columns;
	x->num_entries = 0;
	x->max_matchers = max_matchers;
	x->ratio_kept = 1.;
	x->n_limit = 0;
	x->num_matched_indices = 0;
	
	rand_seed(&x->gen);
	
    return (x);
}


void entrymatcher_free(t_entrymatcher *x)
{
	dsp_free(&x->x_obj);
	free(x->entry_identifiers);
}


void entrymatcher_assist(t_entrymatcher *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) 
		switch (a)
		{
			case 0:
				sprintf(s,"(signal) Choose Triggers / Matching Parameters");
				break;
			case 1:
				sprintf(s,"(signal) Match Triggers");
				break;
			default:
				sprintf(s,"(signal) Matcher Value %ld", a - 1);
				break;
		}
	else
	{
		switch (a)
		{
			case 0:
				sprintf(s,"Matched Indices");
				break;
			case 1:
				sprintf(s,"Matched Labels");
				break;
			case 2:
				sprintf(s,"Matched distances");
				break;
			case 3:
				sprintf(s,"Data from Lookup");
				break;
		}
	}
}


// ========================================================================================================================================== //
// Entry routines: clear, names and entry
// ========================================================================================================================================== //


void entrymatcher_clear(t_entrymatcher *x)
{
	x->num_entries = 0;
}


void entrymatcher_names(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv)
{
	t_symbol **names = x->names;
	long num_columns = x->num_columns;
	
	if (argc > num_columns) 
		argc = num_columns;
	
	while(argc--)
		*names++ = atom_getsym(argv++);
}


void entrymatcher_entry(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv)
{
	long max_entries = x->max_entries;
	long num_entries = x->num_entries;
	long num_columns = x->num_columns;
	long entry_pos;
	
	t_atom *entry_identifiers = x->entry_identifiers;
	double *the_data = x->the_data;
	
	long i;
	
	if (!argc)
		return;
		
	// See if an entry with this identifier already exists (if so replace it)

	for (i = 0; i < num_entries; i++)
		if (entrymatcher_compare_identifiers(argv, entry_identifiers + i)) break;
	
	entry_pos = i;
	
	// Check that we have space for a new entry in the case that this identifier does *not* exist

	if (entry_pos == num_entries)
	{
		if (++num_entries > max_entries || !argc) 
			return;
	}
	
	// Get ready to store the entry

	the_data += (entry_pos) * num_columns;
	if (argc > num_columns + 1) 
		argc = num_columns + 1;
	
	// Store the entry identifier

	entry_identifiers[entry_pos] = *argv++;
	argc--;
		
	// Store each piece of data in turn, checking that it is of the correct data type

	for (i = 0; i < argc; i++)
	{
		if (atom_gettype(argv) != A_SYM)
			the_data[i] = atom_getdouble_translate(argv);
		else
		{
			error ("entrymatcher~: incorrect type in entry - column number %ld", i + 1);
			the_data[i] = 0.;
		}
		argv++;
	}
	
	// Store null data for any unspecified columns
	
	for (; i < num_columns; i++)
		the_data[i] = 0.;
		
	x->num_entries = num_entries;
	
	return;
}


// ========================================================================================================================================== //
// Matchers and Matching routines:
// ========================================================================================================================================== //


void entrymatcher_limit(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv)
{
	long n_limit = argc > 0 ?  atom_getlong(argv + 0): 0; 
	double ratio_kept = argc > 1 ?  atom_getfloat(argv + 1) : 0.;
	
	if (n_limit < 1)
		n_limit = 0;
	if (ratio_kept <= 0. || ratio_kept > 1.)
		ratio_kept = 1.;
	
	x->n_limit = n_limit;
	x->ratio_kept = ratio_kept;
		
}


// Set the matching criteria

void entrymatcher_matchers(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv)
{
	enum TestType test_type;
	
	double *match_params = x->match_params;
	double scale_ratio;
	
	long num_columns = x->num_columns;	
	long max_matchers = x->max_matchers;
	long column_index;
	long arg_check;
	long i = 0;
	long j = 0;	
	
	// Limit to 255 criteria for now (probably more than enough)
	
	if (argc > 255) 
		argc = 255;
	
	while (argc > 1 && i < max_matchers)
	{
		// Find the column index for the test
		
		column_index = entrymatcher_column_from_specifier(x, argv++);
		
		// Now ry to match a new test type
		
		test_type = entrymatcher_test_types(argv++);
		
		// If that fails we are done
		
		if (test_type == TEST_NONE)
		{
			error("entrymatcher~: incorrect test type");
			break;
		}
		
		argc -= 2;
		
		// Get number of expected arguments for this test type
		
		if (test_type == TEST_SCALE || test_type == TEST_WITHIN || test_type == TEST_SCALE_RATIO ||test_type == TEST_WITHIN_RATIO)			
			arg_check = 1;
		else 
			arg_check = 0;
		
		// Check that the arguments are all valid
		
		if (column_index >= 0 && column_index < num_columns)
		{
			if (argc >= arg_check)
			{
				// Store parameters for this test
			
				match_params[j++] = test_type;
				match_params[j++] = column_index;
					
				if (test_type == TEST_SCALE || test_type == TEST_WITHIN)
					match_params[j++] = fabs(1. / atom_getfloat(argv++));
			
				if (test_type == TEST_SCALE_RATIO || test_type == TEST_WITHIN_RATIO)
				{
					scale_ratio = fabs(atom_getfloat(argv++));
					if (scale_ratio < 1.) 
						scale_ratio = 1. / scale_ratio;
					match_params[j++] = 1. / (scale_ratio - 1.);
				}
				i++;
				argc -= arg_check;
			}	
		}
		else 
		{
			error("entrymatcher~: specified column in matchers message does not exist (test number %ld)", i + 1);
		}
	}
	
	if (argc > 0)
	{
		if (i == max_matchers)
			error("entrymatcher~: too many arguments to matchers message for number of specified tests");
		else 
			error("entrymatcher~: not enough arguments to matchers message to correctly specify final matcher");
	}
	
	// Terminate with a zero to indicate the end of the parameter list
	
	match_params[j] = 0;
}


// The main calculation routine - returns a list of distances for any valid matches

long entrymatcher_calculate(t_entrymatcher *x, double *comparision_vals)
{
	long *matched_indices = x->matched_indices;

	double *distances = x->distances;
	double *match_params = x->match_params;
	double *the_data = x->the_data;
	
	double current_distance;
	double current_data;
	double comparison_value;
	double scale;

	long num_columns = x->num_columns;
	long max_matchers = x->max_matchers;
	long num_entries = x->num_entries;
	long num_matched_indices = num_entries;
	long column_index;
	long test_type;
	long i, j, k, m;
	
	// Start by matching everything
	
	for (i = 0; i < num_entries; i++)
	{
		matched_indices[i] = i;
		distances[i] = 0.;
	}
	
	// Here we loop through each test in turn, then by entry - maintaing a list of valid matches at each stage
	
	for (j = 0, k = 0, m = 0; match_params[j] && k < max_matchers; k++, m = 0)
	{
		// Get test details
		
		test_type = match_params[j++];
		column_index = match_params[j++];
		comparison_value = comparision_vals[k];
		
		switch (test_type)
		{			
			case TEST_MATCH:
				
				for (i = 0; i < num_matched_indices; i++)
				{
					if (comparison_value == the_data[matched_indices[i] * num_columns + column_index])
						matched_indices[m++] = matched_indices[i];
				}
				num_matched_indices = m;
				break;
					
			case TEST_DISTANCE:
				
				// Find minimum distance to values 
				
				for (i = 0; i < num_matched_indices; i++)
				{
					current_distance = comparison_value - the_data[matched_indices[i] * num_columns + column_index];
					current_distance *= current_distance;
					distances[matched_indices[i]] += current_distance;
				}
				break;
					
			case TEST_LESS_THAN:
					
				// Less than one of a number of values 
				
				for (i = 0; i < num_matched_indices; i++)
				{
					if (comparison_value > the_data[matched_indices[i] * num_columns + column_index])
						matched_indices[m++] = matched_indices[i];
				}
				num_matched_indices = m;
				break;
					
			case TEST_GREATER_THAN:
					
				// Greater than one of a number of values 
					
				for (i = 0; i < num_matched_indices; i++)
				{
					if (comparison_value < the_data[matched_indices[i] * num_columns + column_index])
						matched_indices[m++] = matched_indices[i];
				}
				num_matched_indices = m;
				break;
					
			case TEST_LESS_THAN_EQ:
					
				// Less than or equal to one of a number of values 
				
				for (i = 0; i < num_matched_indices; i++)
				{
					if (comparison_value >= the_data[matched_indices[i] * num_columns + column_index])
						matched_indices[m++] = matched_indices[i];
				}
				num_matched_indices = m;
				break;
					
			case TEST_GREATER_THAN_EQ:
					
				// Greater than or equal to One of a number of values 
				
				for (i = 0; i < num_matched_indices; i++)
				{
					if (comparison_value <= the_data[matched_indices[i] * num_columns + column_index])
						matched_indices[m++] = matched_indices[i];
				}
				num_matched_indices = m;
				break;
					
			case TEST_SCALE:
				
				// Find minimum scaled distance to value 

				scale = match_params[j++];
				
				for (i = 0; i < num_matched_indices; i++)
				{
					current_distance = (comparison_value - the_data[matched_indices[i] * num_columns + column_index]) * scale;
					current_distance *= current_distance;
					distances[matched_indices[i]] += current_distance;
				}
				
				break;
				
				
			case TEST_WITHIN:
					
				// Find minimum scaled distance to value 
				// If test_type is TEST_WITHIN reject values over 1. 
					
				scale = match_params[j++];
				
				for (i = 0; i < num_matched_indices; i++)
				{
					current_distance = (comparison_value - the_data[matched_indices[i] * num_columns + column_index]) * scale;
					current_distance *= current_distance;
					distances[matched_indices[i]] += current_distance;
				
					if (current_distance < 1.) 
						matched_indices[m++] = matched_indices[i];
				}
				num_matched_indices = m;				
				break;
					
			case TEST_DISTANCE_RATIO:
			case TEST_SCALE_RATIO:
					
				// Find minimum scaled ratio to value 
				// Scaling is 1 for test_type TEST_DISTANCE_RATIO 
				
				if (test_type == TEST_DISTANCE_RATIO)
					scale = 1.;
				else 
					scale = match_params[j++];
					
				for (i = 0; i < num_matched_indices; i++)
				{
					// FIX - why?
                    //comparison_value = comparison_value;
					current_data = the_data[matched_indices[i] * num_columns + column_index];
					if (comparison_value > current_data)
						current_distance = comparison_value / current_data;
					else
						current_distance = current_data / comparison_value;
					current_distance = (current_distance - 1.) * scale;
					current_distance *= current_distance;
				
					distances[matched_indices[i]] += current_distance;
				}
				break;
				
			case TEST_WITHIN_RATIO:
				
				// Find minimum scaled ratio to value 
				// If test_type is TEST_WITHIN_RATIO reject values over 1.
				
				scale = match_params[j++];
				
				for (i = 0; i < num_matched_indices; i++)
				{
                    // FIX - why?
                    //comparison_value = comparison_value;
					current_data = the_data[matched_indices[i] * num_columns + column_index];
					if (comparison_value > current_data)
						current_distance = comparison_value / current_data;
					else
						current_distance = current_data / comparison_value;
					current_distance = (current_distance - 1.) * scale;
					current_distance *= current_distance;
					
					distances[matched_indices[i]] += current_distance;
					if (current_distance < 1.) 
						matched_indices[m++] = matched_indices[i];
				}
				num_matched_indices = m;
				break;
		}
	}
	
	// The correct distances requireq a sqrt, but  will not change the result so for now, so leave this for now....
	
	return num_matched_indices;
}


// Find the top n matches (those with the smallest distance values)

void entrymatcher_top_n (long *matched_indices, double *distances, long num_matched_indices, long n_limit)
{
	long temp_index;
	long swap;
	long i, j;
	
	double min_dist;
	
	// Partial insertion sort (faster sorting for small numbers of n)...
	
	for (i = 0; i < n_limit; i++)
	{
		min_dist = distances[matched_indices[i]];
		swap = i;
		
		for (j = i + 1; j < num_matched_indices; j++)
		{
			if (distances[matched_indices[j]] < min_dist)
			{
				min_dist = distances[matched_indices[j]];
				swap = j;
			}
		}
		
		temp_index = matched_indices[swap];
			
		matched_indices[swap] = matched_indices[i];
		matched_indices[i] = temp_index;
	}
}


// ========================================================================================================================================== //
// Perform and DSP routines:
// ========================================================================================================================================== //


t_int *entrymatcher_perform (t_int *w)
{	
	// Set pointers
	
	float *choose_in = (float *) w[1];
	float *match_in = (float *) w[2];
	float **matcher_ins = (float **) w[3];
	float *out = (float *) w[4];
	long vec_size = w[5];
	t_entrymatcher *x = (t_entrymatcher *) w[6];
	
	double comparision_vals[256];

	t_rand_gen *gen = &x->gen;
	long *matched_indices = x->matched_indices;
	double *distances = x->distances;
	
	long n_limit = x->n_limit;
	long num_matched_indices = x->num_matched_indices;
	long i, j;
	
	for (i = 0; i < vec_size; i++)
	{
		// Assume no match
		
		long index = -1;

		if (*match_in++) 
		{
			// Do matching (if requested)
			
			for (j = 0; j < x->max_matchers; j++)
				comparision_vals[j] = matcher_ins[j][i];
			
			num_matched_indices = entrymatcher_calculate(x, comparision_vals);
			
			if (num_matched_indices) 
			{
				// N.B. If there are no matchers ALWAYS match everything...
				// Else pick the top n matches when there are more than n matches
				
				if (n_limit && n_limit < num_matched_indices && x->match_params[0]) 
				{
					entrymatcher_top_n(matched_indices, distances, num_matched_indices, n_limit);
					num_matched_indices = n_limit;
				}
			}
		}
		
		// Choose a random entry from the valid list (if requested)
		
		if (*choose_in++ && num_matched_indices)
			index = matched_indices[rand_int_n(gen, num_matched_indices - 1)];
		
		*out++ = (float) index + 1;
	}
	
	x->num_matched_indices = num_matched_indices;
	
	return w + 7;
}


void entrymatcher_dsp(t_entrymatcher *x, t_signal **sp, short *count)
{				
	float **matcher_ins = x->matcher_ins;
	long max_matchers = x->max_matchers;
	long i;
	
	for (i = 0; i < max_matchers; i++)
		matcher_ins[i] = (float *) sp[i + 2]->s_vec;
	
	dsp_add(entrymatcher_perform, 6, sp[0]->s_vec, sp[1]->s_vec, matcher_ins, sp[2 + max_matchers]->s_vec, sp[0]->s_n, x);
}


// ========================================================================================================================================== //
// 64bit Perform and DSP routines:
// ========================================================================================================================================== //


void entrymatcher_perform64 (t_entrymatcher *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
	// Set pointers
	
	double *choose_in = ins[0];
	double *match_in = ins[1];
	double **matcher_ins = ins + 2;
	double *out = outs[0];
	
	double comparision_vals[256];
	
	t_rand_gen *gen = &x->gen;
	long *matched_indices = x->matched_indices;
	double *distances = x->distances;
	
	long n_limit = x->n_limit;
	long num_matched_indices = x->num_matched_indices;
	long i, j;
	
	for (i = 0; i < vec_size; i++)
	{
		// Assume no match
		
		long index = -1;
		
		if (*match_in++) 
		{
			// Do matching (if requested)
			
			for (j = 0; j < x->max_matchers; j++)
				comparision_vals[j] = matcher_ins[j][i];
			
			num_matched_indices = entrymatcher_calculate(x, comparision_vals);
			
			if (num_matched_indices) 
			{
				// N.B. If there are no matchers ALWAYS match everything...
				// Else pick the top n matches when there are more than n matches
				
				if (n_limit && n_limit < num_matched_indices && x->match_params[0]) 
				{
					entrymatcher_top_n(matched_indices, distances, num_matched_indices, n_limit);
					num_matched_indices = n_limit;
				}
			}
		}
		
		// Choose a random entry from the valid list (if requested)
		
		if (*choose_in++ && num_matched_indices)
			index = matched_indices[rand_int_n(gen, num_matched_indices - 1)];
		
		*out++ = (float) index + 1;
	}
	
	x->num_matched_indices = num_matched_indices;
}


void entrymatcher_dsp64(t_entrymatcher *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{				
	object_method(dsp64, gensym("dsp_add64"), x, entrymatcher_perform64, 0, NULL);
}


// ========================================================================================================================================== //
// Utilities: for comparing identifiers, finding columns and matcher types, 
// ========================================================================================================================================== //


// Compares entry identifiers (which may be of any type) for equality

char entrymatcher_compare_identifiers(t_atom *identifier1, t_atom *identifier2)
{
	if (atom_gettype(identifier1) != atom_gettype(identifier2))
		return 0;
		
	switch (atom_gettype(identifier1))
	{
		case A_LONG:
			if (atom_getlong(identifier1) != atom_getlong(identifier2))
				return 0;
			break;
		
		case A_FLOAT:
			if (atom_getfloat(identifier1) != atom_getfloat(identifier2))
				return 0;
			break;
			
		case A_SYM:
			if (atom_getsym(identifier1) != atom_getsym(identifier2))
				return 0;
			break;
			
		default:
			return 0;
	}
	
	return 1;
}


// Takes either a numeric index specifier, or a column name in a t_atom, and returns an index number for the column, if it exists

long entrymatcher_column_from_specifier(t_entrymatcher *x, t_atom *arg)
{
	long num_columns = x->num_columns;
	t_symbol **names = x->names;
	t_symbol *named_column;
	long i;
	
	if (atom_gettype(arg) != A_SYM)
		return atom_getlong(arg) - 1;
	
	named_column = atom_getsym(arg);
	
	for (i = 0; i < num_columns; i++)
	{
		if (named_column == names[i]) 
			return i;
	}
	
	return -1;
}


// Attempts to match an atom with any of the symbols representing a valid test type

long entrymatcher_test_types(t_atom *argv)
{
	if (atom_gettype(argv) != A_SYM) return 0;
	
	if (atom_getsym(argv) == ps_match || atom_getsym(argv) == ps_sym_match) return TEST_MATCH;
	if (atom_getsym(argv) == ps_distance || atom_getsym(argv) == ps_sym_distance) return TEST_DISTANCE;
	if (atom_getsym(argv) == ps_less || atom_getsym(argv) == ps_sym_less) return TEST_LESS_THAN;
	if (atom_getsym(argv) == ps_greater || atom_getsym(argv) == ps_sym_greater) return TEST_GREATER_THAN;
	if (atom_getsym(argv) == ps_lesseq || atom_getsym(argv) == ps_sym_lesseq) return TEST_LESS_THAN_EQ;
	if (atom_getsym(argv) == ps_greatereq || atom_getsym(argv) == ps_sym_greatereq) return TEST_GREATER_THAN_EQ;
	if (atom_getsym(argv) == ps_scale || atom_getsym(argv) == ps_sym_scale) return TEST_SCALE;
	if (atom_getsym(argv) == ps_within || atom_getsym(argv) == ps_sym_within) return TEST_WITHIN;
	if (atom_getsym(argv) == ps_distance_ratio || atom_getsym(argv) == ps_sym_distance_ratio) return TEST_DISTANCE_RATIO; 
	if (atom_getsym(argv) == ps_scale_ratio || atom_getsym(argv) == ps_sym_scale_ratio) return TEST_SCALE_RATIO;
	if (atom_getsym(argv) == ps_within_ratio || atom_getsym(argv) == ps_sym_within_ratio) return TEST_WITHIN_RATIO;
	
	return 0;
}
 
