
/*
 *  entrymatcher
 *
 *	entrymatcher is an object for finding the most closely matching items in a scalable N-dimensional space, using variable matching criteria.
 *
 *	The original reason for creating the entrymatcher object was to provide a way of matching audio samples using audio features (or descriptors) that had been calculated using the descriptors~ object.
 *	
 *	Data is added to the object in "entries", which are rows of values (ints, floats or symbols) that fill a specified number of columns.
 *	Columns are named to allow them to be specified by descriptive name, rather than only by index number, making it possible to alter the order and size of data sets easily.
 *	Each entry is specified with an identifier (of any basic max data type), so that it can also be referred to regardless of position in the data set.
 *	Various tests may be performed on each column of data, either individually or in combination, as appropriate to the data (exact match, within given distance, greater than, less than etc.) to determine matching entries.
 *	Values from specified columns may be queried at will (in the case where each entry represents a sample this is useful in retrieving data once a particular sample has been matched).
 *	Matched entries are returned as a set of lists, with the best (closest) matches first.
 *
 *	In practice a large number of matching scenarios and data lookup requirements can be satisfied.
 *	The features of the entrymatcher object are covered in detail in the helpfile documentation.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#define SETSYM(ap, x) ((ap)->a_type = A_SYM, (ap)->a_w.w_sym = (x))
#define SETLONG(ap, x) ((ap)->a_type = A_LONG, (ap)->a_w.w_long = (x))
#define SETFLOAT(ap, x) ((ap)->a_type = A_FLOAT, (ap)->a_w.w_float = (x))

#include <ext.h>
#include <ext_obex.h>
#include <math.h>


t_class *this_class;


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


typedef struct entrymatcher{

    t_object a_obj;
    
	long max_num_entries;
	long num_columns;
	
	long num_entries;
	
	double *the_data;
	long *data_types;
	
	t_atom *entry_identifiers;
	long *chosen_indices;
	
	double *matching_data;
	double *distances;
	
	t_symbol **names;
	char *label_modes;
	
	// Outlets
	
	void *the_indices_outlet;
	void *the_identifiers_outlet;
	void *the_distances_outlet;
	void *the_data_outlet;
    
} t_entrymatcher;


t_symbol *ps_null;
t_symbol *ps_list;

t_symbol *ps_lookup;
t_symbol *ps_identifier;

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


inline double atom_getdouble_translate (t_atom *arg);

void *entrymatcher_new(long max_num_entries, long num_columns);
void entrymatcher_free(t_entrymatcher *x);
void entrymatcher_assist(t_entrymatcher *x, void *b, long m, long a, char *s);

void entrymatcher_clear(t_entrymatcher *x);
void entrymatcher_labelmodes(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv);
void entrymatcher_names(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv);
void entrymatcher_entry(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv);

char entrymatcher_compare_identifiers(t_atom *identifier1, t_atom *identifier2);
enum TestType entrymatcher_test_types(t_atom *argv);
long entrymatcher_column_from_specifier(t_entrymatcher *x, t_atom *arg);

void entrymatcher_lookup(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv);

void entrymatcher_matchers(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv);
void entrymatcher_match_all(t_entrymatcher *x);void entrymatcher_match_user(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv);
void entrymatcher_match(t_entrymatcher *x, double ratio_kept, double distance_limit, long n_limit);
long entrymatcher_calculate(t_entrymatcher *x);

void entrymatcher_combsort(long *indices, double *distances, long num_points);


/*****************************************/
// Helper for translating any of the three basic MaxMSP data types to double
/*****************************************/


inline double atom_getdouble_translate (t_atom *arg)
{
	switch (arg->a_type)
	{
		case A_LONG:
			return arg->a_w.w_long;
			break;
		case A_FLOAT:
			return arg->a_w.w_float;
			break;
		case A_SYM:
			return (long) arg->a_w.w_sym;
			break;
		default:
			return 0.;
	}
}

/*****************************************/
// Basic object routines: main, new, free and assist
/*****************************************/


int C74_EXPORT main()
{
	this_class	= class_new	("entrymatcher",
				(method) entrymatcher_new, 
				(method) entrymatcher_free, 
				sizeof(t_entrymatcher), 
				NULL, 
				A_DEFLONG, 
				A_DEFLONG, 
				0);
	
	class_addmethod(this_class, (method) entrymatcher_clear,"clear", 0);
	class_addmethod(this_class, (method) entrymatcher_clear,"reset", 0);

	class_addmethod(this_class, (method) entrymatcher_entry,"entry", A_GIMME, 0);
	class_addmethod(this_class, (method) entrymatcher_matchers,"matchers", A_GIMME, 0);
	class_addmethod(this_class, (method) entrymatcher_match_user,"match", A_GIMME, 0);
	class_addmethod(this_class, (method) entrymatcher_labelmodes,"labelmodes", A_GIMME, 0);
	class_addmethod(this_class, (method) entrymatcher_names,"names", A_GIMME, 0);
	class_addmethod(this_class, (method) entrymatcher_lookup, "lookup", A_GIMME, 0);
	class_addmethod(this_class, (method) entrymatcher_lookup, "index", A_GIMME, 0);
	class_addmethod(this_class, (method) entrymatcher_assist, "assist", A_CANT, 0);
	
	class_addmethod(this_class, (method) entrymatcher_match_all, "bang", 0);
	
	class_register(CLASS_BOX, this_class);
	
	ps_list = gensym("list");
	
	ps_null = gensym("");
	
	ps_lookup = gensym("lookup");
	ps_identifier = gensym("identifier");
	
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


void *entrymatcher_new(long max_num_entries, long num_columns)
{
	t_entrymatcher *x = (t_entrymatcher *) object_alloc(this_class);
	
	t_symbol **names;
	char *label_modes;
	long i;
	
	if (max_num_entries < 1) 
		max_num_entries = 1;
	if (num_columns < 1) 
		num_columns = 1;
	
	x->the_data_outlet = listout(x);
	x->the_distances_outlet = listout(x);
	x->the_identifiers_outlet = outlet_new(x, 0);
    x->the_indices_outlet = listout(x);
	
	// Allocate large memory block and then assign to various pointers
	
	void *allocated_mem = malloc (((((2 + num_columns) * max_num_entries) + 1024) * sizeof(double)) + (max_num_entries * (1 + num_columns) * sizeof(long)) + (max_num_entries * sizeof(t_atom)) + (num_columns * sizeof(char)) + (num_columns * sizeof(t_symbol *)));
	
	if (!allocated_mem) 
		return 0;
	
	x->entry_identifiers = (t_atom *) allocated_mem;
	allocated_mem = ((t_atom *)allocated_mem) + max_num_entries;
	
	x->chosen_indices = (long *) allocated_mem;
    allocated_mem = ((long *)allocated_mem) + max_num_entries;
	
	x->matching_data = (double *) allocated_mem;
    allocated_mem = ((double *)allocated_mem) + 1024;
    
	x->distances = (double *) allocated_mem;
    allocated_mem = ((double *)allocated_mem) + max_num_entries;

	x->names = names = (t_symbol **) allocated_mem;
    allocated_mem = ((t_symbol **)allocated_mem) + num_columns;

	x->label_modes = label_modes = (char *) allocated_mem;
    allocated_mem = ((char *)allocated_mem) + num_columns;

	x->the_data = (double *) allocated_mem;
    allocated_mem = ((double *)allocated_mem) + (num_columns * max_num_entries);

	x->data_types = (long *) allocated_mem;
	
	// Zero column label modes and names
	
	for (i = 0; i < num_columns; i++)
	{
		label_modes[i] = 0;
		names[i] = ps_null;
	}
		
	x->max_num_entries = max_num_entries;
	x->num_columns = num_columns;
	x->num_entries = 0;
	
    return (x);
}


void entrymatcher_free(t_entrymatcher *x)
{
	free(x->entry_identifiers);
}


void entrymatcher_assist(t_entrymatcher *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) 
		sprintf(s,"Entries / Names / Matching Instructions / Lookup Instructions");
	else
	{
		 switch (a)
		{
			case 0:
				sprintf(s,"Matched Indices");
				break;
			case 1:
				sprintf(s,"Matched Identifiers");
				break;
			case 2:
				sprintf(s,"Matched Distances");
				break;
			case 3:
				sprintf(s,"Data from Lookup");
				break;
		}
	}
}


/*****************************************/
// Entry routines: clear, labelmodes, names and entry
/*****************************************/


void entrymatcher_clear(t_entrymatcher *x)
{
	// This is all we need to do to clear the data
	
	x->num_entries = 0;
	x->matching_data[0] = 0;
}


void entrymatcher_labelmodes(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv)
{
	char *label_modes = x->label_modes;
	long num_columns = x->num_columns;
	
	if (argc > num_columns) 
		argc = num_columns;
	
	entrymatcher_clear(x);
	
	while(argc--)
		*label_modes++ = atom_getlong(argv++) ? 1 : 0;
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
	t_atom *entry_identifiers = x->entry_identifiers;
	double *the_data = x->the_data;
	long *data_types = x->data_types;
	char *label_modes = x->label_modes;

	long max_num_entries = x->max_num_entries;
	long num_entries = x->num_entries;
	long num_columns = x->num_columns;
	long entry_pos;	
	long i;
	
	if (!argc)
		return;
	
	// See if an entry with this identifier already exists (if so replace it)
	
	for (i = 0; i < num_entries; i++)
		if (entrymatcher_compare_identifiers(argv, entry_identifiers + i)) 
			break;
	
	entry_pos = i;
	
	// Check that we have space for a new entry in the case that this identifier does *not* exist
	
	if (entry_pos == num_entries)
	{
		if (++num_entries > max_num_entries || !argc) 
			return;
	}
	
	// Get ready to store the entry
	
	the_data += (entry_pos) * num_columns;
	data_types += (entry_pos) * num_columns;
	if (argc > num_columns + 1) 
		argc = num_columns + 1;
	
	// Store the entry identifier
	
	entry_identifiers[entry_pos] = *argv++;
	argc--;
		
	// Store each piece of data in turn, checking that it is of the correct data type
	
	for (i = 0; i < argc; i++)
	{
		if (label_modes[i] == (argv->a_type == A_SYM))
		{
			data_types[i] = argv->a_type;
			the_data[i] = atom_getdouble_translate(argv);
		}
		else
		{
			error ("entrymatcher: incorrect type in entry - column number %ld", i + 1);
			data_types[i] = label_modes[i] ? A_SYM : A_LONG;
			the_data[i] = label_modes[i] ?  (long) ps_null : 0.;
		}
		argv++;
	}
	
	// Store null data for any unspecified columns
	
	for (; i < num_columns; i++)
	{
		data_types[i] = label_modes[i] ? A_SYM : A_LONG;
		the_data[i] = label_modes[i] ?  (long) ps_null : 0.;
	}
		
	x->num_entries = num_entries;
	
	return;
}


/*****************************************/
// Lookup Routine
/*****************************************/


void entrymatcher_lookup(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv)
{
	double *the_data = x->the_data;
	t_atom *entry_identifiers = x->entry_identifiers;
	long *data_types = x->data_types;
	
	long num_entries = x->num_entries;
	long num_columns = x->num_columns;
	long output_size = num_columns;
	long column_index;
	long to_lookup;
	long i = -1;
	
	t_atom output_data[output_size];
	
	if (!argc--)
		return;
	
	// Limit number of items to lookup
	
	if (argc > num_columns) 
		output_size = argc;
	
	// If called using the lookup message then attempt to find the relevant entry by identifier
	// Otherwise, use the given number to lookup the entry in that numerical row
	
	if (msg == ps_lookup)
	{
		switch (argv->a_type)
		{
			case A_SYM:
				
				for (i = 0; i < num_entries; i++)
					if (entry_identifiers[i].a_type == A_SYM && entry_identifiers[i].a_w.w_sym == argv->a_w.w_sym) 
						break;
				break;
				
			case A_LONG:

				for (i = 0; i < num_entries; i++)
					if (entry_identifiers[i].a_type == A_LONG && entry_identifiers[i].a_w.w_long == argv->a_w.w_long)
						break;
				break;
				
			case A_FLOAT:
				
				for (i = 0; i < num_entries; i++)
					if (entry_identifiers[i].a_type == A_FLOAT && entry_identifiers[i].a_w.w_float == argv->a_w.w_float) break;
				break;
		}
		to_lookup = i;
		argv++;
	}
	else 
		to_lookup = atom_getlong(argv++) - 1;
	
	if (to_lookup < 0 || to_lookup >= num_entries) 
	{
		object_error((t_object *) x, "entry does not exist %s", (argv - 1)->a_w.w_sym->s_name);
		return;
	}
	
	// Set pointers to the relevant entry
	
	the_data += to_lookup * num_columns;
	data_types += to_lookup * num_columns;
	
	if (!argc)
	{
		// If no columns are specified construct a list of all colums for that entry
		
		for (i = 0; i < num_columns; i++)
		{
			switch (data_types[i])
			{
				case A_SYM:
					SETSYM (output_data + i, (t_symbol *) (long) the_data[i]);
					break;
				case A_LONG:
					SETLONG (output_data + i, (long) the_data[i]);
					break;
				case A_FLOAT:
					SETFLOAT (output_data + i, the_data[i]);
					break;
				default:
					SETFLOAT (output_data + i, 0.);
			}
		}
	}
	else
	{
		// Construct a list of the data in the specified columns (in the specified order)
		
		for (i = 0; i < argc; i++)
		{
			// Get column - if not valid output from the first column
			
			column_index = entrymatcher_column_from_specifier(x, argv++);
			if (column_index < -1 || column_index >= num_columns) 
				column_index = 0;

			if (column_index == -1)
			{
				switch (entry_identifiers[to_lookup].a_type)
				{
					case A_SYM:
						SETSYM (output_data + i, entry_identifiers[to_lookup].a_w.w_sym);
						break;
					case A_LONG:
						SETLONG (output_data + i, entry_identifiers[to_lookup].a_w.w_long);
						break;
					case A_FLOAT:
						SETFLOAT (output_data + i, entry_identifiers[to_lookup].a_w.w_float);
						break;
					default:
						SETFLOAT (output_data + i, 0.);
				}
			}
			else 
			{
				switch (data_types[column_index])
				{
					case A_SYM:
						SETSYM (output_data + i, (t_symbol *) (long) the_data[column_index]);
						break;
					case A_LONG:
						SETLONG (output_data + i, (long) the_data[column_index]);
						break;
					case A_FLOAT:
						SETFLOAT (output_data + i, the_data[column_index]);
						break;
					default:
						SETFLOAT (output_data + i, 0.);
				}
			}
		}
		output_size = argc;
	}
	
	// Output
	
	outlet_list (x->the_data_outlet, 0L, output_size, output_data);
}


/*****************************************/
// Matchers and Matching routines:
/*****************************************/


// Set the matching criteria

void entrymatcher_matchers(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv)
{
	enum TestType test_type;
	enum TestType next_test_type = TEST_NONE;
	
	double *matching_data = x->matching_data;
	char *label_modes = x->label_modes;

	double scale_ratio;
	
	long num_columns = x->num_columns;
	long column_index;
	long column_test;
	long length;
	long from, to;
	long i = 1;
	long j = 0;
	long k;
	
	char label_mode;
	
	// Limit arguments (this value looks too high) - it is possible that a large criteria list will cause a crash
	
	if (argc > 768) 
		argc = 768;
	
	// Get the first test type
	
	if (argc > 1) 
		next_test_type = entrymatcher_test_types(argv + 1);
		
	// Check that the first test is valid
	
	while (i < argc)
	{
		// Update the test type and get the column to test
		
		test_type = next_test_type;
		next_test_type = TEST_NONE;
		
		if (test_type == TEST_NONE)
		{
			object_error((t_object *) x, "invalid test / no test specified in unparsed segment of matchers message");
			break;
		}
		
		from = i + 1;
		column_index = entrymatcher_column_from_specifier(x, argv + from - 2);
		column_test = column_index >= 0 && column_index < num_columns;
		label_mode = label_modes[column_index];
		
		// Search right for the next test specifier (so we know how many values to perform this test against)
		
		while (next_test_type == TEST_NONE && i < argc)
			next_test_type = entrymatcher_test_types(argv + ++i);
			
		if (i == argc) 
			to = i;
		else 
			to = i - 1;
		
		length = to - from;
		
		// Check for tests that require an extra piece of scaling data
		
		if (length && (test_type == TEST_SCALE || test_type == TEST_WITHIN || test_type == TEST_SCALE_RATIO ||test_type == TEST_WITHIN_RATIO))		
			length--;	
		
		// Check for values present for this test
		
		if (!length)
		{
			error ("entrymatcher: no target values given for specified test in matchers message");
			continue;
		}
		
		// Test for invalid column
		
		if (!column_test)
		{
			error ("entrymatcher: specified column in matchers message does not exist");
			continue;
		}
		
		if (label_mode)
		{
			// If this column is for labels
		
			if (test_type == TEST_MATCH)
			{
				// Store details of a valid match test (other tests are not valid)
			
				matching_data[j++] = TEST_MATCH;
				matching_data[j++] = length;
				matching_data[j++] = column_index;
		
				for (k = from; k < to; k++)
					matching_data[j++] = (long) atom_getsym(argv + k);
			}
			else
			{
				// Disallow invalid column numbers and incorrect test types
				
				error ("entrymatcher: incorrect matcher for label type column (should be equals or ==)  column number %ld", column_index + 1);
			}
		}
		else
		{
			// If this column is for values
			// Store the parameters for any valid test
			
			matching_data[j++] = test_type;
			matching_data[j++] = length;
			matching_data[j++] = column_index;
		
			if (test_type == TEST_SCALE || test_type == TEST_WITHIN)
				matching_data[j++] = fabs(1. / atom_getfloat(argv + from++));
			
			if (test_type == TEST_SCALE_RATIO || test_type == TEST_WITHIN_RATIO)
			{
				scale_ratio = fabs(atom_getfloat(argv + from++));
				if (scale_ratio < 1.) 
					scale_ratio = 1. / scale_ratio;
				matching_data[j++] = 1. / (scale_ratio - 1.);
			}
		
			for (k = from; k < to; k++)
				matching_data[j++] = atom_getfloat(argv + k);
		}
	}
	
	// Terminate with a zero to indicate the end of the parameter list

	matching_data[j] = 0;
}


void entrymatcher_match_all(t_entrymatcher *x)
{
	entrymatcher_match(x, 1., HUGE_VAL, 1024);
}


// Handle more complex matching requests from the user

void entrymatcher_match_user(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv)
{
	double ratio_kept = 1.;
	double distance_limit = HUGE_VAL;
	long n_limit = 1024;
	
	if (argc > 0 && argv->a_type == A_LONG)
	{
		// Limit by maximum specified number
		// Limit by ratio
		
		n_limit = atom_getlong(argv++);
		argc--;
		if (argc-- > 0) 
			ratio_kept = atom_getfloat(argv++);
	}
	else
	{
		// Limit by a ratio
		// Then limit by maximum specified number
		
		if (argc-- > 0)
			ratio_kept = atom_getfloat(argv++);
		if (argc-- > 0) 
			n_limit = atom_getlong(argv++);
	}

	// Limit by a maximum distance value

	if (argc-- > 0) 
		distance_limit = atom_getfloat(argv++);
	
	// Clip ratio values
	
	if (ratio_kept < 0.)
		ratio_kept = 0.;
	if (ratio_kept > 1.)
		ratio_kept = 1.;
	
	if (n_limit > 1024 || n_limit < 0)
		n_limit = 1024;
	
	// Match
	
	entrymatcher_match(x, ratio_kept, distance_limit, n_limit);
}


// Handle matching of all types 

void entrymatcher_match(t_entrymatcher *x, double ratio_kept, double distance_limit, long n_limit)
{
	t_atom *entry_identifiers = x->entry_identifiers;
	double *distances = x->distances;
	long *chosen_indices = x->chosen_indices;
	
	t_atom output_identifiers[1024];
	t_atom output_indices[1024];
	t_atom output_distances[1024];
	
	t_symbol *msg = ps_list;
	
	double distance;
	long nudge = 0;
	long num_identifier_args;
	long index;
	long i;
	
	// Calculate potential matches and sort if there are matches
	
	long num_chosen_indices = entrymatcher_calculate(x);
	
	if (!num_chosen_indices) 
		return;
	
	entrymatcher_combsort(chosen_indices, distances, num_chosen_indices);
		
	// N.B. If there are no matchers ALWAYS match everything (up to max output size)...
	
	if (x->matching_data[0])
	{
		// If there are matchers, then limit by ratio and maximum number
		
		num_chosen_indices = num_chosen_indices * ratio_kept;
	
		if (num_chosen_indices > n_limit) 
			num_chosen_indices = n_limit;
		else	
			if (num_chosen_indices < 1) 
				num_chosen_indices = 1;
	}
	else 
	{
		if (num_chosen_indices > 1024) 
			num_chosen_indices = 1024;
	}
	
	// Limit matches by maximum distance if specified
	
	for (i = 0; i < num_chosen_indices; i++)
	{
		if (i > 1023)
			break;
			
		index = chosen_indices[i];
		distance = distances[index]; 
		
		if (distance > distance_limit) 
			break;
		
		SETFLOAT(output_distances + i, distance);
		output_identifiers[i] = entry_identifiers[index];
		SETLONG(output_indices + i, index + 1);
	}
		
	num_chosen_indices = i;
	
	// Return if nothing has been matched
	
	if (!num_chosen_indices) 
		return;
	
	// Handle identifiers correctly whether the first identifier is numeric (output list), or symbol (output message)
	
	if (output_identifiers[0].a_type == A_SYM)
	{
		msg = output_identifiers[0].a_w.w_sym;
		nudge = 1;
		num_identifier_args = num_chosen_indices - 1;
	}
	else 
		num_identifier_args = num_chosen_indices;
	
	// Output
	
	outlet_list (x->the_distances_outlet, 0L, num_chosen_indices, output_distances);
	outlet_anything(x->the_identifiers_outlet, msg, num_identifier_args, output_identifiers + nudge);
	outlet_list (x->the_indices_outlet, 0L, num_chosen_indices, output_indices);
}


// The main calculation routine - returns a list of distances for any valid matches

long entrymatcher_calculate(t_entrymatcher *x)
{
	long *chosen_indices = x->chosen_indices;
	
	double *matching_data = x->matching_data;
	double *the_data = x->the_data;	
	double *distances = x->distances;
	
	double distance;
	double new_distance;
	double test_distance;
	double data_to_compare;
	double compare_to;
	double scale;
	
	long num_columns = x->num_columns;
	long num_entries = x->num_entries;	
	long num_chosen_indices = 0;
	long num_matchers;
	long column_index;
	long test_type;
	long chosen;
	long from, to;	
	long i, j;

	for (i = 0; i < num_entries; i++)
	{
		// Assume a match for each entry (for the case of no matchers)
		
		chosen = 1;
		test_distance = 0.;
		distance = 0.;
		j = 0;
		
		while (matching_data[j])
		{
			// Get details of the test to perform
			
			test_type = matching_data[j++];
			num_matchers = matching_data[j++];
			column_index = matching_data[j++];
			new_distance = HUGE_VAL;
			
			from = j;
			to = j + num_matchers;
			
			// Perform test
			
			switch (test_type)
			{
				case TEST_MATCH:
					
					/* Match One of a Number of Values */
					
					for (j = from; j < to; j++)
						if (matching_data[j] == the_data[i * num_columns + column_index]) break;
					
					if (j == to) 
						chosen = 0;
					break;
					
				case TEST_DISTANCE:
				
					/* Find Minimum distance to One of a Number of Values */

					for (j = from; j < to; j++)
					{
						test_distance = matching_data[j] - the_data[i * num_columns + column_index];
						test_distance *= test_distance;
						if (test_distance < new_distance) 
							new_distance = test_distance;
					}
				
					distance += new_distance;
					break;
					
				case TEST_LESS_THAN:
					
					/* Less Than One of a Number of Values */
					
					for (j = from; j < to; j++)
						if (matching_data[j] > the_data[i * num_columns + column_index]) break;
					
					if (j == to) 
						chosen = 0;
					break;
				
				case TEST_GREATER_THAN:
					
					/* Greater Than One of a Number of Values */
					
					for (j = from; j < to; j++)
						if (matching_data[j] < the_data[i * num_columns + column_index]) break;
					
					if (j == to) 
						chosen = 0;
					break;
					
				case TEST_LESS_THAN_EQ:
					
					/* Less Than or Equal to One of a Number of Values */
					
					for (j = from; j < to; j++)
						if (matching_data[j] >= the_data[i * num_columns + column_index]) break;
					
					if (j == to) 
						chosen = 0;
					break;
				
				case TEST_GREATER_THAN_EQ:
					
					/* Greater Than or Equal to One of a Number of Values */
					
					for (j = from; j < to; j++)
						if (matching_data[j] <= the_data[i * num_columns + column_index]) break;
					
					if (j == to) 
						chosen = 0;
					break;
					
				case TEST_SCALE:
				case TEST_WITHIN:
				
					/* Find Minimum Scaled distance to One of a Number of Values */
					/* If test_type is TEST_WITHIN Reject Values Over 1. */
					
					scale = matching_data[j];
					from++;
					to++;
					
					for (j = from; j < to; j++)
					{
						test_distance = (matching_data[j] - the_data[i * num_columns + column_index]) * scale;
						test_distance *= test_distance;
						if (test_distance < new_distance) 
							new_distance = test_distance;
					}
				
					distance += new_distance;
					if (test_type == TEST_WITHIN && new_distance > 1.) 
						chosen = 0;
					break;
				
				case TEST_DISTANCE_RATIO:
				case TEST_SCALE_RATIO:
				case TEST_WITHIN_RATIO:
				
					/* Find Minimum Scaled Ratio to One of a Number of Values */
					/* Scaling is 1 for test_type TEST_DISTANCE_RATIO */
					/* If test_type is TEST_WITHIN_RATIO Reject Values Over 1. */
					
					if (test_type == TEST_DISTANCE_RATIO)
						scale = 1.;
					else 
					{
						scale = matching_data[j];
						from++;
						to++;
					}
					
					for (j = from; j < to; j++)
					{
						compare_to = matching_data[j]; 
						data_to_compare = the_data[i * num_columns + column_index];
						if (compare_to > data_to_compare)
							test_distance = compare_to / data_to_compare;
						else
							test_distance = data_to_compare / compare_to;
						test_distance = (test_distance - 1.) * scale;
						test_distance *= test_distance;
						if (test_distance < new_distance) 
							new_distance = test_distance;
					}
				
					distance += new_distance;
					if (test_type == TEST_WITHIN_RATIO && new_distance > 1.) 
						chosen = 0;
					break;
			}
			
			// If this entry is not a match then stop testing (for efficiency) and move to the next entry
			// Else go onto the next test
			
			j = to;
			if (!chosen) 
				break;
		}
		
		// Store the entry if it is a valid match
		
		if (chosen)
		{
			chosen_indices[num_chosen_indices++] = i;
			distances[i] = sqrt(distance);
		}
	}
	
	return num_chosen_indices;
}

/*****************************************/
// Utilities: for comparing identifiers, finding columns and matcher types, 
/*****************************************/


// Compares entry identifiers (which may be of any type) for equality

char entrymatcher_compare_identifiers(t_atom *identifier1, t_atom *identifier2)
{
	if (identifier1->a_type != identifier2->a_type)
		return 0;
		
	switch (identifier1->a_type)
	{
		case A_LONG:
			if (identifier1->a_w.w_long != identifier2->a_w.w_long)
				return 0;
			break;
		
		case A_FLOAT:
			if (identifier1->a_w.w_float != identifier2->a_w.w_float)
				return 0;
			break;
			
		case A_SYM:
			if (identifier1->a_w.w_sym != identifier2->a_w.w_sym)
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
	
	if (arg->a_type != A_SYM) 
		return atom_getlong(arg) - 1;

	named_column = atom_getsym(arg);

	if (named_column == ps_identifier)
		return -1;
	
	for (i = 0; i < num_columns; i++)
	{
		if (named_column == names[i]) 
			return i;
	}
	
	return -2;
}


// Attempts to match an atom with any of the symbols representing a valid test type

enum TestType entrymatcher_test_types(t_atom *argv)
{
	if (argv->a_type != A_SYM) return TEST_NONE;
	
	if (argv->a_w.w_sym == ps_match || argv->a_w.w_sym == ps_sym_match) return TEST_MATCH;
	if (argv->a_w.w_sym == ps_distance || argv->a_w.w_sym == ps_sym_distance) return TEST_DISTANCE;
	if (argv->a_w.w_sym == ps_less || argv->a_w.w_sym == ps_sym_less) return TEST_LESS_THAN;
	if (argv->a_w.w_sym == ps_greater || argv->a_w.w_sym == ps_sym_greater) return TEST_GREATER_THAN;
	if (argv->a_w.w_sym == ps_lesseq || argv->a_w.w_sym == ps_sym_lesseq) return TEST_LESS_THAN_EQ;
	if (argv->a_w.w_sym == ps_greatereq || argv->a_w.w_sym == ps_sym_greatereq) return TEST_GREATER_THAN_EQ;
	if (argv->a_w.w_sym == ps_scale || argv->a_w.w_sym == ps_sym_scale) return TEST_SCALE;
	if (argv->a_w.w_sym == ps_within || argv->a_w.w_sym == ps_sym_within) return TEST_WITHIN;
	if (argv->a_w.w_sym == ps_distance_ratio || argv->a_w.w_sym == ps_sym_distance_ratio) return TEST_DISTANCE_RATIO; 
	if (argv->a_w.w_sym == ps_scale_ratio || argv->a_w.w_sym == ps_sym_scale_ratio) return TEST_SCALE_RATIO;
	if (argv->a_w.w_sym == ps_within_ratio || argv->a_w.w_sym == ps_sym_within_ratio) return TEST_WITHIN_RATIO;
	
	return TEST_NONE;
}

/*****************************************/
// Comb Sort routine
/*****************************************/


// An ascending order index sort (combsort11 algorithm)

void entrymatcher_combsort(long *indices, double *distances, long num_points)
{
	long index;
	long gap_index;
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
			index = indices[i];
			gap_index = indices[i + gap];
			if (distances[index] > distances[gap_index])
			{
				indices[i] = gap_index;
				indices[i + gap] = index;
				swaps = 1;	
			}
		}
	}
}

