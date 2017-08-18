
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

#include "EntryDatabase.h"
#include "Matchers.h"
#include "utilities.h"
#include "entry_database_max.h"
#include "entrymatcher_common.h"

t_class *this_class;

t_symbol *ps_lookup;
t_symbol *ps_mean;
t_symbol *ps_min;
t_symbol *ps_minimum;
t_symbol *ps_max;
t_symbol *ps_maximum;
t_symbol *ps_median;
t_symbol *ps_stddev;
t_symbol *ps_deviation;
t_symbol *ps_standard_dev;
t_symbol *ps_standard_deviation;
t_symbol *ps_centile;
t_symbol *ps_percentile;

typedef struct entrymatcher{

    t_object a_obj;
    
    t_object *database_object;
    Matchers *matchers;
    
    // Outlets
	
	void *the_indices_outlet;
	void *the_identifiers_outlet;
	void *the_distances_outlet;
	void *the_data_outlet;
    
} t_entrymatcher;

void *entrymatcher_new(t_symbol *sym, long argc, t_atom *argv);
void entrymatcher_free(t_entrymatcher *x);
void entrymatcher_assist(t_entrymatcher *x, void *b, long m, long a, char *s);

void entrymatcher_dump(t_entrymatcher *x);
void entrymatcher_lookup(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);
void entrymatcher_lookup_output(t_entrymatcher *x, const EntryDatabase::ReadPointer& database, long idx, long argc, t_atom *argv);

void entrymatcher_stats(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);

void entrymatcher_matchers(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);
void entrymatcher_match_all(t_entrymatcher *x);
void entrymatcher_match_user(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv);
void entrymatcher_match(t_entrymatcher *x, double ratio_kept, double distance_limit, long n_limit);

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
				A_GIMME,
				0);
    
	class_addmethod(this_class, (method)entrymatcher_matchers,"matchers", A_GIMME, 0);
	class_addmethod(this_class, (method)entrymatcher_match_user,"match", A_GIMME, 0);
	
    class_addmethod(this_class, (method)entrymatcher_dump, "dump", 0);
	class_addmethod(this_class, (method)entrymatcher_lookup, "lookup", A_GIMME, 0);
	class_addmethod(this_class, (method)entrymatcher_lookup, "index", A_GIMME, 0);
	class_addmethod(this_class, (method)entrymatcher_assist, "assist", A_CANT, 0);

    class_addmethod(this_class, (method)entrymatcher_stats, "stats", A_GIMME, 0);

	class_addmethod(this_class, (method)entrymatcher_match_all, "bang", 0);
	
    entrymatcher_add_common<t_entrymatcher>(this_class);
    
	class_register(CLASS_BOX, this_class);
	
	ps_lookup = gensym("lookup");
    ps_mean = gensym("mean");
    ps_min = gensym("min");
    ps_minimum = gensym("minimum");
    ps_max = gensym("max");
    ps_maximum = gensym("maximum");
    ps_median = gensym("median");
    ps_stddev = gensym("stddev");
    ps_deviation = gensym("deviation");
    ps_standard_dev = gensym("standard_dev");
    ps_standard_deviation= gensym("standard_deviation");
    ps_centile = gensym("centile");
    ps_percentile = gensym("percentile");
    
    init_test_symbols();
	
	return 0;
}

void *entrymatcher_new(t_symbol *sym, long argc, t_atom *argv)
{
    t_symbol *name = NULL;
    
    if (argc && atom_gettype(argv) == A_SYM)
    {
        name = atom_getsym(argv++);
        argc--;
    }
    else
        name = symbol_unique();
    
    t_atom_long num_reserved_entries = (argc > 1) ? atom_getlong(argv++) : 0;
    t_atom_long num_columns = argc ? atom_getlong(argv++) : 0;
    
	t_entrymatcher *x = (t_entrymatcher *)object_alloc(this_class);
	
	x->the_data_outlet = listout(x);
	x->the_distances_outlet = listout(x);
	x->the_identifiers_outlet = outlet_new(x, 0);
    x->the_indices_outlet = listout(x);
	
    x->database_object = database_create(name, num_reserved_entries, num_columns);
    x->matchers = new Matchers;
    
    return (x);
}

void entrymatcher_free(t_entrymatcher *x)
{
    database_release(x->database_object);
    delete x->matchers;
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
// Lookup Routines
// ========================================================================================================================================== //

void entrymatcher_dump(t_entrymatcher *x)
{
    EntryDatabase::ReadPointer database = database_getptr_read(x->database_object);

	for (long i = 0; i < database->numItems(); i++)
        entrymatcher_lookup_output(x, database, i, 0, NULL);
}

void entrymatcher_lookup(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
	if (!argc)
		return;

    EntryDatabase::ReadPointer database = database_getptr_read(x->database_object);

	// Use identifier or index depending on the message received
	
    long idx = (msg == ps_lookup) ? database->itemFromIdentifier(argv) : atom_getlong(argv) - 1;
    entrymatcher_lookup_output(x, database, idx, --argc, ++argv);
}

void entrymatcher_lookup_output(t_entrymatcher *x, const EntryDatabase::ReadPointer& database, long idx, long argc, t_atom *argv)
{
    std::vector<t_atom> output;
    
    long numItems = database->numItems();
    long numColumns = database->numColumns();
    
    EntryDatabase::Accessor accessor = database->accessor();
    
    if (idx < 0 || idx >= numItems)
    {
        object_error((t_object *)x, "entry does not exist");
        return;
    }
    else if (!argc)
    {
        output.resize(numColumns);
        
        // If no columns are specified construct a list of all colums for that entry
        
        for (long i = 0; i < numColumns; i++)
            accessor.getDataAtom(&output[i], idx, i);
    }
    else
    {
        // Construct a list of the data in the specified columns (in the specified order)
        
        output.resize(argc);
        
        for (long i = 0; i < argc; i++)
        {
            // Get column - if not valid output from the first column
            
            long column = database->columnFromSpecifier(argv++);
            column = (column < -1 || column >= numColumns) ? 0 : column;
            
            if (column == -1)
                database->getIdentifier(&output[i], idx);
            else
                accessor.getDataAtom(&output[i], idx, column);
        }
    }
    
    outlet_list(x->the_data_outlet, NULL, output.size(), &output[0]);
}

// ========================================================================================================================================== //
// Stats Routine
// ========================================================================================================================================== //

void entrymatcher_stats(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
    if (argc < 2)
        return;
    
    EntryDatabase::ReadPointer database = database_getptr_read(x->database_object);

    std::vector<t_atom> output(argc);
    t_atom *column = argv++;
    argc--;

    long i;
    
    for (i = 0; argc; i++)
    {
        t_symbol *test = atom_getsym(argv++);
        argc--;

        if (test == ps_mean)
            atom_setfloat(&output[i], database->columnMean(column));
        else if (test == ps_min || test == ps_minimum)
            atom_setfloat(&output[i], database->columnMin(column));
        else if (test == ps_max || test == ps_maximum)
            atom_setfloat(&output[i], database->columnMax(column));
        else if (test == ps_median)
            atom_setfloat(&output[i], database->columnMedian(column));
        else if (test == ps_stddev || test == ps_deviation || test == ps_standard_dev || test == ps_standard_deviation)
            atom_setfloat(&output[i], database->columnStandardDeviation(column));
        else if (test == ps_centile || test == ps_percentile)
        {
            if (argc)
            {
                atom_setfloat(&output[i], database->columnPercentile(column, atom_getfloat(argv)));
                argc--;
            }
            else
                object_error((t_object *) x, "no percentile given for percentile stat");
        }
        else
            object_error((t_object *) x, "unknown stat type");
    }
    
    outlet_list(x->the_data_outlet, NULL, i, &output[0]);
}

// ========================================================================================================================================== //
// Matchers and Matching Routines:
// ========================================================================================================================================== //

// Set the matching criteria

void entrymatcher_matchers(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
    x->matchers->setMatchers(x, argc, argv, database_getptr_read(x->database_object));
}

void entrymatcher_match_all(t_entrymatcher *x)
{
	entrymatcher_match(x, 1.0, HUGE_VAL, 1024);
}

// Handle more complex matching requests from the user

void entrymatcher_match_user(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv)
{
	double ratio_kept;
	double distance_limit;
	long n_limit;
	
    // Accept maximum specified number and limit by ratio in either order

	if (argc > 0 && atom_gettype(argv) == A_LONG)
	{
        n_limit = (argc-- > 0) ? atom_getlong(argv++) : 1024;
		ratio_kept = (argc-- > 0) ? atom_getfloat(argv++) : 1.0;
	}
	else
	{
	    ratio_kept = (argc-- > 0) ? atom_getfloat(argv++) : 1.0;
        n_limit = (argc-- > 0) ? atom_getlong(argv++) : 1024;
	}

	// Limit by a maximum distance value

    distance_limit = (argc-- > 0) ? atom_getfloat(argv++) : HUGE_VAL;
	
	// Clip values
	
    ratio_kept = std::min(std::max(ratio_kept, 0.0), 1.0);
    n_limit = std::min(std::max(n_limit, 0L), 1024L);
	
	// Match
	
	entrymatcher_match(x, ratio_kept, distance_limit, n_limit);
}

// Handle matching of all types 

void entrymatcher_match(t_entrymatcher *x, double ratio_kept, double distance_limit, long n_limit)
{
    long num_matches;
    
    t_atom output_identifiers[1024];
	t_atom output_indices[1024];
	t_atom output_distances[1024];
	
    // Don't keep the lock when outputing...
    
    {
        EntryDatabase::ReadPointer database = database_getptr_read(x->database_object);
        Matchers *matchers = x->matchers;
        
        // Calculate potential matches and sort if there are matches
        
        matchers->setMaxMatches(ratio_kept, n_limit);
        num_matches = matchers->match(database);
        
        if (!num_matches)
            return;
        
        num_matches = std::min(num_matches, 1024L);
        
        // Limit matches by maximum distance if specified
        
        for (long i = 0; i < num_matches; i++)
        {
            long index = matchers->getIndex(i);
            double distance = matchers->getDistance(index);
            
            if (distance > distance_limit)
            {
                num_matches = i;
                break;
            }
            
            atom_setfloat(output_distances + i, distance);
            database->getIdentifier(output_identifiers + i, index);
            atom_setlong(output_indices + i, index + 1);
        }
    }

    // Output if something has been matched, handling identifiers correctly whether the first identifier is numeric, or symbol
	
	if (num_matches)
    {	
        outlet_list(x->the_distances_outlet, NULL, num_matches, output_distances);
        if (atom_gettype(output_identifiers) == A_SYM)
            outlet_anything(x->the_identifiers_outlet, atom_getsym(output_identifiers), num_matches - 1, output_identifiers + 1);
        else
            outlet_list(x->the_identifiers_outlet, NULL, num_matches, output_identifiers);
        outlet_list(x->the_indices_outlet, NULL, num_matches, output_indices);
    }
}
