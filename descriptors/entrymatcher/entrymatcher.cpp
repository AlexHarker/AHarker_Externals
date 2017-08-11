
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


#include <ext.h>
#include <ext_obex.h>

#include "utilities.h"
#include "EntryDatabase.h"
#include "Matchers.h"

t_class *this_class;


typedef struct entrymatcher{

    t_object a_obj;
    
    EntryDatabase *mDatabase;
    Matchers *mMatchers;
    
    std::vector<long> *mIndices;
    std::vector<double> *mDistances;
    
    // Outlets
	
	void *the_indices_outlet;
	void *the_identifiers_outlet;
	void *the_distances_outlet;
	void *the_data_outlet;
    
} t_entrymatcher;


t_symbol *ps_null;
t_symbol *ps_list;

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

void *entrymatcher_new(t_atom_long max_num_entries, t_atom_long num_columns);
void entrymatcher_free(t_entrymatcher *x);
void entrymatcher_assist(t_entrymatcher *x, void *b, long m, long a, char *s);

void entrymatcher_clear(t_entrymatcher *x);
void entrymatcher_labelmodes(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);
void entrymatcher_names(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);
void entrymatcher_entry(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);

void entrymatcher_dump(t_entrymatcher *x);
void entrymatcher_lookup(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);
void entrymatcher_lookup_output(t_entrymatcher *x, long idx, long argc, t_atom *argv);

void entrymatcher_matchers(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv);
void entrymatcher_match_all(t_entrymatcher *x);void entrymatcher_match_user(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv);
void entrymatcher_match(t_entrymatcher *x, double ratio_kept, double distance_limit, long n_limit);

void entrymatcher_combsort(std::vector<long> *indices, std::vector<double> *distances, long num_points);

TestType entrymatcher_test_types(t_atom *argv);

// ========================================================================================================================================== //
// Basic object routines: main, new, free and assist
// ========================================================================================================================================== //

int C74_EXPORT main(void)
{
	this_class	= class_new	("entrymatcher",
				(method) entrymatcher_new, 
				(method)entrymatcher_free, 
				(short)sizeof(t_entrymatcher), 
				NULL, 
				A_DEFLONG, 
				A_DEFLONG, 
				0);
	
	class_addmethod(this_class, (method)entrymatcher_clear,"clear", 0);
	class_addmethod(this_class, (method)entrymatcher_clear,"reset", 0);

	class_addmethod(this_class, (method)entrymatcher_entry,"entry", A_GIMME, 0);
	class_addmethod(this_class, (method)entrymatcher_matchers,"matchers", A_GIMME, 0);
	class_addmethod(this_class, (method)entrymatcher_match_user,"match", A_GIMME, 0);
	class_addmethod(this_class, (method)entrymatcher_labelmodes,"labelmodes", A_GIMME, 0);
	class_addmethod(this_class, (method)entrymatcher_names,"names", A_GIMME, 0);
	class_addmethod(this_class, (method)entrymatcher_dump, "dump", 0);
	class_addmethod(this_class, (method)entrymatcher_lookup, "lookup", A_GIMME, 0);
	class_addmethod(this_class, (method)entrymatcher_lookup, "index", A_GIMME, 0);
	class_addmethod(this_class, (method)entrymatcher_assist, "assist", A_CANT, 0);
	
	class_addmethod(this_class, (method)entrymatcher_match_all, "bang", 0);
	
	class_register(CLASS_BOX, this_class);
	
	ps_list = gensym("list");
	
	ps_null = gensym("");
	
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

void *entrymatcher_new(t_atom_long max_num_entries, t_atom_long num_columns)
{
	t_entrymatcher *x = (t_entrymatcher *)object_alloc(this_class);

    max_num_entries = std::max(max_num_entries, t_atom_long(1));
    num_columns = std::max(num_columns, t_atom_long(1));
	
	x->the_data_outlet = listout(x);
	x->the_distances_outlet = listout(x);
	x->the_identifiers_outlet = outlet_new(x, 0);
    x->the_indices_outlet = listout(x);
		
    x->mDatabase = new EntryDatabase(num_columns);
    x->mMatchers = new Matchers;
    x->mIndices = new std::vector<long>;
    x->mDistances = new std::vector<double>;
    
    x->mDatabase->reserve(max_num_entries);
    
    return (x);
}

void entrymatcher_free(t_entrymatcher *x)
{
    delete x->mDatabase;
    delete x->mMatchers;
    delete x->mIndices;
    delete x->mDistances;
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

// ========================================================================================================================================== //
// Entry routines: clear, labelmodes, names and entry
// ========================================================================================================================================== //

void entrymatcher_clear(t_entrymatcher *x)
{
    x->mDatabase->clear();
}

void entrymatcher_labelmodes(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
    x->mDatabase->labelModes(argc, argv);
}

void entrymatcher_names(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
    x->mDatabase->names(argc, argv);
}

void entrymatcher_entry(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{	
    x->mDatabase->entry(argc, argv);
}

// ========================================================================================================================================== //
// Lookup Routine
// ========================================================================================================================================== //

void entrymatcher_dump(t_entrymatcher *x)
{
	for (long i = 0; i < x->mDatabase->numItems(); i++)
        entrymatcher_lookup_output(x, i, 0, NULL);
}

void entrymatcher_lookup(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
	if (!argc)
		return;
	
	// Use identifier or index depending on the message received
	
    long idx = (msg == ps_lookup) ? x->mDatabase->itemFromIdentifier(*argv) :atom_getlong(argv) - 1;
	
    entrymatcher_lookup_output(x, idx, --argc, ++argv);
}

void entrymatcher_lookup_output(t_entrymatcher *x, long idx, long argc, t_atom *argv)
{
    std::vector<t_atom> output;
    
    x->mDatabase->lookup(output, idx, argc, argv);
    
    if (!output.size())
        object_error((t_object *)x, "entry does not exist");
    else
        outlet_list(x->the_data_outlet, 0L, output.size(), &output[0]);
}

// ========================================================================================================================================== //
// Matchers and Matching Routines:
// ========================================================================================================================================== //

// Set the matching criteria

void entrymatcher_matchers(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv)
{
    // Empty the matchers
    
    x->mMatchers->clear();
    
    // Loop over arguments
    
    while (argc)
	{
        if (argc < 2)
        {
            object_error((t_object *) x, "insufficient items in matchers message for unparsed test");
            break;
        }
        
        // Get the column and test type

        long column = x->mDatabase->columnFromSpecifier(*argv++);
        TestType type = entrymatcher_test_types(argv++);
        argc -= 2;
        
        // Test for issues

        if (type == TEST_NONE)
        {
            object_error((t_object *) x, "invalid test / no test specified in unparsed segment of matchers message");
            break;
        }
		else if (column < 0 || column >= x->mDatabase->numColumns())
		{
			object_error((t_object *) x, "specified column in matchers message does not exist");
			continue;
		}
		else if (x->mDatabase->getLabelMode(column) && type != TEST_MATCH)
        {
            object_error((t_object *) x, "incorrect matcher for label type column (should be equals or ==)  column number %ld", column + 1);
            continue;
        }
        
        bool hasTarget = false;
        
        // Parse values
        
        if (x->mDatabase->getLabelMode(column))
		{
			// If this column is for labels store details of a valid match test (other tests are not valid)
		
            x->mMatchers->addMatcher(Matchers::kTestMatch, column);
		
            for ( ; argc; argc--, argv++)
            {
                if (entrymatcher_test_types(argv) != TEST_NONE)
                    break;
                x->mMatchers->addTarget(atom_getsym(argv));
                hasTarget = true;
            }
        }
		else
		{
			// If this column is for values store the parameters for any valid test
			
            double scale = 1.0;
            
			if (argc && (type == TEST_SCALE || type == TEST_WITHIN))
            {
                scale = fabs(1.0 / atom_getfloat(argv++));
            }
			else if (argc && (type == TEST_SCALE_RATIO || type == TEST_WITHIN_RATIO))
			{
				scale = fabs(atom_getfloat(argv++));
                scale = (scale < 1.0) ? 1.0 / scale : scale;
				scale = 1.0 / (scale - 1.0);
			}
		
            switch (type)
            {
                case TEST_NONE:                 break;
                case TEST_MATCH:                x->mMatchers->addMatcher(Matchers::kTestMatch, column);                      break;
                case TEST_LESS_THAN:            x->mMatchers->addMatcher(Matchers::kTestLess, column);                       break;
                case TEST_GREATER_THAN:         x->mMatchers->addMatcher(Matchers::kTestGreater, column);                    break;
                case TEST_LESS_THAN_EQ:         x->mMatchers->addMatcher(Matchers::kTestLessEqual, column);                  break;
                case TEST_GREATER_THAN_EQ:      x->mMatchers->addMatcher(Matchers::kTestGreaterEqual, column);               break;
                case TEST_DISTANCE:             x->mMatchers->addMatcher(Matchers::kTestDistance, column);                   break;
                case TEST_SCALE:                x->mMatchers->addMatcher(Matchers::kTestDistance, column, scale);            break;
                case TEST_WITHIN:               x->mMatchers->addMatcher(Matchers::kTestDistance, column, scale, true);      break;
                case TEST_DISTANCE_RATIO:       x->mMatchers->addMatcher(Matchers::kTestRatio, column);                      break;
                case TEST_SCALE_RATIO:          x->mMatchers->addMatcher(Matchers::kTestRatio, column, scale);               break;
                case TEST_WITHIN_RATIO:         x->mMatchers->addMatcher(Matchers::kTestRatio, column, scale, true);         break;
            }
            
            for ( ; argc; argc--, argv++)
            {
                if (entrymatcher_test_types(argv) != TEST_NONE)
                    break;
                x->mMatchers->addTarget(atom_getfloat(argv));
                hasTarget = true;
            }
        }
        
        if (!hasTarget)
            object_error((t_object *) x, "no target values given for specified test in matchers message");
	}
}

void entrymatcher_match_all(t_entrymatcher *x)
{
	entrymatcher_match(x, 1.0, HUGE_VAL, 1024);
}

// Handle more complex matching requests from the user

void entrymatcher_match_user(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv)
{
	double ratio_kept = 1.0;
	double distance_limit = HUGE_VAL;
	long n_limit = 1024;
	
	if (argc > 0 && atom_gettype(argv) == A_LONG)
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
	
    ratio_kept = std::min(std::max(ratio_kept, 0.0), 1.0);
    n_limit = std::min(std::max(n_limit, 0L), 1024L);
	
	// Match
	
	entrymatcher_match(x, ratio_kept, distance_limit, n_limit);
}


// Handle matching of all types 

void entrymatcher_match(t_entrymatcher *x, double ratio_kept, double distance_limit, long n_limit)
{
	t_atom output_identifiers[1024];
	t_atom output_indices[1024];
	t_atom output_distances[1024];
	
	// Calculate potential matches and sort if there are matches
	
    long num_matches = x->mMatchers->match(x->mDatabase, *x->mIndices, *x->mDistances);
	
	if (!num_matches)
		return;
	
	entrymatcher_combsort(x->mIndices, x->mDistances, num_matches);
		
	// N.B. If there are no matchers ALWAYS match everything (up to max output size)...
	
	if (x->mMatchers->size())
	{
		// If there are matchers, then limit by ratio and maximum number
		
		num_matches *= ratio_kept;
        num_matches = std::min(std::max(num_matches, 1L), n_limit);
	}
    
    num_matches = std::min(num_matches, 1024L);
    
	// Limit matches by maximum distance if specified
	
	for (long i = 0; i < num_matches; i++)
	{
		long index = (*x->mIndices)[i];
		double distance = (*x->mDistances)[index];
		
		if (distance > distance_limit)
        {
            num_matches = i;
			break;
        }
		
		atom_setfloat(output_distances + i, distance);
		output_identifiers[i] = x->mDatabase->getIdentifier(index);
		atom_setlong(output_indices + i, index + 1);
	}
    
	// Return if nothing has been matched
	
	if (!num_matches)
		return;
	
	// Handle identifiers correctly whether the first identifier is numeric (output list), or symbol (output message)
	
    t_symbol *msg = ps_list;
    long nudge = 0;
    long num_identifier_args = num_matches;

    if (atom_gettype(output_identifiers) == A_SYM)
	{
        msg = atom_getsym(output_identifiers);
		nudge = 1;
		num_identifier_args = num_matches - 1;
	}
	
	// Output
	
	outlet_list(x->the_distances_outlet, 0L, num_matches, output_distances);
	outlet_anything(x->the_identifiers_outlet, msg, num_identifier_args, output_identifiers + nudge);
	outlet_list(x->the_indices_outlet, 0L, num_matches, output_indices);
}

// ========================================================================================================================================== //
// Utilities: for comparing identifiers, finding columns and matcher types, 
// ========================================================================================================================================== //

// Attempts to match an atom with any of the symbols representing a valid test type

TestType entrymatcher_test_types(t_atom *argv)
{
	if (atom_gettype(argv) != A_SYM)
        return TEST_NONE;
	
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
	
	return TEST_NONE;
}

// ========================================================================================================================================== //
// Comb Sort routine
// ========================================================================================================================================== //

// An ascending order index sort (combsort11 algorithm)

void entrymatcher_combsort(std::vector<long> *indices, std::vector<double> *distances, long num_points)
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
			index = (*indices)[i];
			gap_index = (*indices)[i + gap];
			if ((*distances)[index] > (*distances)[gap_index])
			{
				(*indices)[i] = gap_index;
				(*indices)[i + gap] = index;
				swaps = 1;	
			}
		}
	}
}
