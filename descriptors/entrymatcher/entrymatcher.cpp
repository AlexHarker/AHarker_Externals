
/*
 *  entrymatcher
 *
 *  entrymatcher is an object for finding the most closely matching items in a scalable N-dimensional space, using variable matching criteria.
 *
 *  The original reason for creating the entrymatcher object was to provide a way of matching audio samples using audio features (or descriptors) that had been calculated using the descriptors~ object.
 *
 *  Data is added to the object in "entries", which are rows of values (ints, floats or symbols) that fill a specified number of columns.
 *  Columns are named to allow them to be specified by descriptive name, rather than only by index number, making it possible to alter the order and size of data sets easily.
 *  Each entry is specified with an identifier (of any basic max data type), so that it can also be referred to regardless of position in the data set.
 *  Various tests may be performed on each column of data, either individually or in combination, as appropriate to the data (exact match, within given distance, greater than, less than etc.) to determine matching entries.
 *  Values from specified columns may be queried at will (in the case where each entry represents a sample this is useful in retrieving data once a particular sample has been matched).
 *  Matched entries are returned as a set of lists, with the best (closest) matches first.
 *
 *  In practice a large number of matching scenarios and data lookup requirements can be satisfied.
 *  The features of the entrymatcher object are covered in detail in the helpfile documentation.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <ext.h>
#include <ext_obex.h>

#include "entry_database.hpp"
#include "entrymatcher_common.hpp"
#include "matchers.hpp"

t_class *this_class;

t_symbol *ps_lookup = gensym("lookup");

struct t_entrymatcher
{

    t_object x_obj;
    
    t_entry_database *database_object;
    matchers *matchers;
    
    long embed;
    t_patcher *patcher;
    
    // Outlets
    
    void *indices_outlet;
    void *identifiers_outlet;
    void *distances_outlet;
    void *data_outlet;
};

using atom_vector = std::vector<t_atom>;
constexpr long max_matches = 1024;

void *entrymatcher_new(t_symbol *sym, long argc, t_atom *argv);
void entrymatcher_free(t_entrymatcher *x);
void entrymatcher_assist(t_entrymatcher *x, void *b, long m, long a, char *s);

void entrymatcher_dump(t_entrymatcher *x);
void entrymatcher_lookup(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);
bool entrymatcher_query(t_entrymatcher *x, atom_vector& output, long idx, long argc, t_atom *argv, bool report);

void entrymatcher_stats(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);

void entrymatcher_matchers(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);
void entrymatcher_match_all(t_entrymatcher *x);
void entrymatcher_match_user(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv);
void entrymatcher_match(t_entrymatcher *x, double ratio_kept, double distance_limit, long n_limit);
long entrymatcher_do_match(t_entrymatcher *x, double ratio, double limit, long n_limit, t_atom output[3][max_matches]);

// Main

int C74_EXPORT main()
{
    this_class = class_new("entrymatcher",
                           (method) entrymatcher_new,
                           (method) entrymatcher_free,
                           sizeof(t_entrymatcher),
                           (method) nullptr,
                           A_GIMME,
                           0);
    
    class_addmethod(this_class, (method) entrymatcher_matchers,"matchers", A_GIMME, 0);
    class_addmethod(this_class, (method) entrymatcher_match_user,"match", A_GIMME, 0);
    
    class_addmethod(this_class, (method) entrymatcher_dump, "dump", 0);
    class_addmethod(this_class, (method) entrymatcher_lookup, "lookup", A_GIMME, 0);
    class_addmethod(this_class, (method) entrymatcher_lookup, "index", A_GIMME, 0);
    class_addmethod(this_class, (method) entrymatcher_assist, "assist", A_CANT, 0);

    class_addmethod(this_class, (method) entrymatcher_stats, "stats", A_GIMME, 0);

    class_addmethod(this_class, (method) entrymatcher_match_all, "bang", 0);
    
    entrymatcher_common<t_entrymatcher>::class_add(this_class);
    
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / Free / Assist

void *entrymatcher_new(t_symbol *sym, long argc, t_atom *argv)
{
    t_symbol *name = nullptr;
    
    if (argc && atom_gettype(argv) == A_SYM)
    {
        name = atom_getsym(argv++);
        argc--;
    }
    else
        name = symbol_unique();
    
    t_atom_long num_reserved_entries = (argc > 1) ? atom_getlong(argv++) : 0;
    t_atom_long num_columns = argc ? atom_getlong(argv++) : 0;
    
    t_entrymatcher *x = (t_entrymatcher *) object_alloc(this_class);
    
    x->data_outlet = listout(x);
    x->distances_outlet = listout(x);
    x->identifiers_outlet = outlet_new(x, nullptr);
    x->indices_outlet = listout(x);
    
    x->database_object = database_create(x, name, num_reserved_entries, num_columns);
    x->matchers = new matchers;
    
    entrymatcher_common<t_entrymatcher>::object_init(x);

    return x;
}

void entrymatcher_free(t_entrymatcher *x)
{
    database_release(x, x->database_object);
    delete x->matchers;
}

void entrymatcher_assist(t_entrymatcher *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"Entries / Names / Instructions");
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

// Lookup Methods

void entrymatcher_dump(t_entrymatcher *x)
{
    atom_vector output;

    for (long i = 0; ; i++)
    {
        if (!entrymatcher_query(x, output, i, 0, nullptr, false))
            break;
            
        outlet_list(x->data_outlet, nullptr, output.size(), output.data());
    }
}

void entrymatcher_lookup(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
    atom_vector output;

    if (!argc)
        return;

    auto database = database_get_read_access(x->database_object);

    // Use identifier or index depending on the message received
    
    long idx = (msg == ps_lookup) ? database.get_entry_index(argv) : atom_getlong(argv) - 1;
    
    if (entrymatcher_query(x, output, idx, --argc, ++argv, true))
        outlet_list(x->data_outlet, nullptr, output.size(), output.data());

}

bool entrymatcher_query(t_entrymatcher *x, atom_vector& output, long idx, long argc, t_atom *argv, bool report)
{
    // N.B. - We cannot output in this function whilst we hold the lock
    
    auto database = database_get_read_access(x->database_object);

    long num_items = database.num_items();
    long num_columns = database.num_columns();
        
    if (idx < 0 || idx >= num_items)
    {
        if (report)
            object_error((t_object *) x, "entry does not exist");
        return false;
    }
    else if (!argc)
    {
        // If no columns are specified construct a list of all columns for that entry

        output.resize(num_columns);
                
        for (long i = 0; i < num_columns; i++)
            database.get_atom(&output[i], idx, i);
    }
    else
    {
        // Construct a list of the data in the specified columns (in the specified order)
        
        output.resize(argc);
        
        for (long i = 0; i < argc; i++)
        {
            // Get column - if not valid output from the first column
            
            long column = database.get_column_index(argv++);
            column = (column < -1) ? 0 : column;
            
            if (column == -1)
                database.get_entry_identifier(idx, &output[i]);
            else
                database.get_atom(&output[i], idx, column);
        }
    }
    
    return true;
}

// Stats Method

void entrymatcher_stats(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
    atom_vector output(argc);
    database_get_read_access(x->database_object).stats(x, output, argc, argv);

    if (output.size())
        outlet_list(x->data_outlet, nullptr, output.size(), output.data());
    else
        object_error((t_object *) x, "no stats specified");
}

// Matchers and Matching Methods

// Set the matching criteria

void entrymatcher_matchers(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
    x->matchers->set_matchers(x, argc, argv, database_get_read_access(x->database_object));
}

// Handle Matching (no limits)

void entrymatcher_match_all(t_entrymatcher *x)
{
    entrymatcher_match(x, 1.0, HUGE_VAL, max_matches);
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
        n_limit = (argc-- > 0) ? atom_getlong(argv++) : max_matches;
        ratio_kept = (argc-- > 0) ? atom_getfloat(argv++) : 1.0;
    }
    else
    {
        ratio_kept = (argc-- > 0) ? atom_getfloat(argv++) : 1.0;
        n_limit = (argc-- > 0) ? atom_getlong(argv++) : max_matches;
    }

    // Limit by a maximum distance value

    distance_limit = (argc-- > 0) ? atom_getfloat(argv++) : HUGE_VAL;
    
    // Clip values
    
    ratio_kept = std::min(std::max(ratio_kept, 0.0), 1.0);
    n_limit = std::min(std::max(n_limit, 0L), max_matches);
    
    // Match
    
    entrymatcher_match(x, ratio_kept, distance_limit, n_limit);
}

// Handle Matching Output

void entrymatcher_match(t_entrymatcher *x, double ratio_kept, double distance_limit, long n_limit)
{
    t_atom output[3][max_matches];
    
    long num_matches = entrymatcher_do_match(x, ratio_kept, distance_limit, n_limit, output);
   
    // Output if something has been matched, handling the first identifier correctly
    
    if (num_matches)
    {
        outlet_list(x->distances_outlet, nullptr, num_matches, output[0]);
        if (atom_gettype(output[1]) == A_SYM)
            outlet_anything(x->identifiers_outlet, atom_getsym(output[1]), num_matches - 1, output[1] + 1);
        else
            outlet_list(x->identifiers_outlet, nullptr, num_matches, output[1]);
        outlet_list(x->indices_outlet, nullptr, num_matches, output[2]);
    }
}

// Do Underlying Matching / Thinning

long entrymatcher_do_match(t_entrymatcher *x, double ratio, double limit, long n_limit, t_atom output[3][max_matches])
{
    // N.B. We can't output whilst holding the lock
    
    matchers *matchers = x->matchers;

    auto database = database_get_read_access(x->database_object);
        
    // Calculate potential matches and sort if there are matches
        
    long num_matches = std::min(matchers->match(database, ratio, n_limit, true), max_matches);

    // Limit matches by maximum distance if specified
    
    for (long i = 0; i < num_matches; i++)
    {
        long index = matchers->get_index(i);
        double distance = matchers->get_distance(i);
        
        if (distance > limit)
        {
            num_matches = i;
            break;
        }
        
        atom_setfloat(output[0] + i, distance);
        database.get_entry_identifier(index, output[1] + i);
        atom_setlong(output[2] + i, index + 1);
    }
    
    return num_matches;
}
