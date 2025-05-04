
/*
 *  entrymatcher~
 *
 *  entrymatcher~ is the audio rate counterpart of entrymatcher, capable of matching with sample-accurate timing.
 *  It is intended primarily for sample accurate granular synthesis applications requiring descriptor matching
 *
 *  entrymatcher~ does not offer all features of entrymatcher, and there are some key differences to the max version:
 *
 *  1 - entrymatcher~ takes values for matching at signal rate
 *  2 - entrymatcher~ has a different message structure for setting matching criteria.
 *  3 - entrymatcher~ randomly selects a value from all the valid matches and outputs only that one value.
 *  4 - the matching and randomisation processes can be triggered separately to allow efficient rapid variation
 *  5 - only numeric data can be used (although entries and columns can still be referred to using symbols).
 *
 *  In practice a large number of matching scenarios and data lookup requirements can be satisfied.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Int_Handler.hpp>
#include <AH_Lifecycle.hpp>
#include <random_generator.hpp>

#include "../entrymatcher/entry_database.hpp"
#include "../entrymatcher/entrymatcher_common.hpp"
#include "../entrymatcher/matchers.hpp"


// Globals and Object Structure

t_class *this_class;

struct t_entrymatcher
{
    t_pxobject x_obj;
    
    t_entry_database *database_object;
    matchers matchers;
    
    t_patcher *patcher;
    long embed;

    long max_matchers;
    long n_limit;
    double ratio_kept;
    
    bool invalid_matchers;
    t_clock *invalid_clock;
    
    htl::random_generator<> gen;
    
    const float *matcher_ins[256];
};

// Function Prototypes

void *entrymatcher_new(t_symbol *sym, long argc, t_atom *argv);
void entrymatcher_free(t_entrymatcher *x);
void entrymatcher_assist(t_entrymatcher *x, void *b, long m, long a, char *s);

void entrymatcher_limit(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);
void entrymatcher_matchers(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);

void entrymatcher_tick(t_entrymatcher *x);

void entrymatcher_perform64(t_entrymatcher *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void entrymatcher_dsp64(t_entrymatcher *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags);

// Main

int C74_EXPORT main()
{
    this_class = class_new("entrymatcher~",
                           (method) entrymatcher_new,
                           (method) entrymatcher_free,
                           sizeof(t_entrymatcher),
                           (method) nullptr,
                           A_GIMME,
                           0);
    
    class_addmethod(this_class, (method) entrymatcher_limit,"limit", A_GIMME, 0);
    class_addmethod(this_class, (method) entrymatcher_matchers,"matchers", A_GIMME, 0);
    class_addmethod(this_class, (method) entrymatcher_assist, "assist", A_CANT, 0);
    
    class_addmethod(this_class, (method) entrymatcher_dsp64, "dsp64", A_CANT, 0);

    entrymatcher_common<t_entrymatcher>::class_add(this_class);
    
    class_dspinit(this_class);

    class_register(CLASS_BOX, this_class);
        
    return 0;
}

// New  / Free / Assist

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
    
    t_atom_long num_reserved_entries = (argc > 2) ? atom_getlong(argv++) : 0;
    t_atom_long num_columns = argc ? atom_getlong(argv++) : 0;
    t_atom_long max_matchers = (argc > 1) ? atom_getlong(argv++) : 0;
    
    t_entrymatcher *x = (t_entrymatcher *) object_alloc(this_class);
   
    x->database_object = database_create(x, name, num_reserved_entries, num_columns);
    create_object(x->matchers);
    create_object(x->gen);
    
    x->max_matchers = std::max(std::min(limit_int<long>(max_matchers), 256L), 1L);
    x->ratio_kept = 1.0;
    x->n_limit = 0;
    x->invalid_matchers = false;
    
    x->invalid_clock = clock_new(x, (method) entrymatcher_tick);
    
    dsp_setup((t_pxobject*) x, 2 + x->max_matchers);
    outlet_new((t_object*) x, "signal");

    entrymatcher_common<t_entrymatcher>::object_init(x);

    return x;
}

void entrymatcher_free(t_entrymatcher *x)
{
    dsp_free(&x->x_obj);
    database_release(x, x->database_object);
    destroy_object(x->matchers);
    destroy_object(x->gen);
    object_free(x->invalid_clock);
}

void entrymatcher_assist(t_entrymatcher *x, void *b, long m, long a, char *s)
{
    static constexpr int maxAssist = 256;

    if (m == ASSIST_INLET)
        switch (a)
    {
        case 0:
            snprintf(s, maxAssist, "(signal) Choose Triggers / Matching Parameters");
            break;
        case 1:
            snprintf(s, maxAssist, "(signal) Match Triggers");
            break;
        default:
            snprintf(s, maxAssist, "(signal) Matcher Value %ld", a - 1);
            break;
    }
    else
    {
        switch (a)
        {
            case 0:
                snprintf(s, maxAssist, "Matched Indices");
                break;
            case 1:
                snprintf(s, maxAssist, "Matched Labels");
                break;
            case 2:
                snprintf(s, maxAssist, "Matched Distances");
                break;
            case 3:
                snprintf(s, maxAssist, "Data from Lookup");
                break;
        }
    }
}

// Set Matching Limits

void entrymatcher_limit(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
    long n_limit = argc > 0 ? limit_int<long>(atom_getlong(argv + 0)) : 0;
    double ratio_kept = argc > 1 ? atom_getfloat(argv + 1) : 1.0;
    
    // Clip values
    
    x->ratio_kept = std::min(std::max(ratio_kept, 0.0), 1.0);
    x->n_limit = std::max(n_limit, 0L);
}

// Set Matching Criteria

void entrymatcher_matchers(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
    using test = matchers::test;
    
    long max_matchers = x->max_matchers;
    
    auto database = database_get_read_access(x->database_object);
    
    x->matchers.clear();
    
    while (argc > 1 && x->matchers.size() < max_matchers)
    {
        // Find the column index for the test and the test type
        
        long column = database.get_column_index(argv++);
        bool get_scale = matchers::needs_scale(argv);
        test type = matchers::test_type(argv++);
        
        // If that fails we are done
        
        if (type == test::none)
        {
            object_error((t_object *) x, "incorrect test type");
            break;
        }
        
        argc -= 2;
        
        // Get number of expected arguments for this test type
        
        long arg_check = get_scale ? 1 : 0;
        
        // Check that the arguments are all valid
        
        if (column >= 0)
        {
            if (argc >= arg_check)
            {
                // Store parameters for this test
                
                double scale = 1.0;
                
                if (type == test::diff || type == test::diff_reject)
                    scale = fabs(1. / atom_getfloat(argv++));
                else if (type == test::ratio || type == test::ratio_reject)
                {
                    scale = fabs(atom_getfloat(argv++));
                    scale = (scale < 1.0) ? 1.0 / scale : scale;
                    scale = 1.0 / (scale - 1.0);
                }
                argc -= arg_check;
                
                x->matchers.add_matcher(type, column, scale);
            }
        }
        else
        {
            object_error((t_object *) x, "specified column in matchers message does not exist");
        }
    }
    
    if (argc > 0)
    {
        if (x->matchers.size() < max_matchers)
            object_error((t_object *) x, "too many arguments to matchers message for number of specified tests");
        else
            object_error((t_object *) x, "not enough arguments to matchers message to correctly specify final matcher");
    }
    
    x->invalid_matchers = false;
}

// Invalid matchers clock

void entrymatcher_tick(t_entrymatcher *x)
{
    object_error((t_object *) x, "matchers are not currently valid for this database");
}

// Perform

void entrymatcher_perform64(t_entrymatcher *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    // Set pointers
    
    const double *choose_in = ins[0];
    const double *match_in = ins[1];
    double *const *matcher_ins = ins + 2;
    double *out = outs[0];
    
    htl::random_generator<>& gen = x->gen;
    
    auto database = database_get_read_access(x->database_object);
    matchers& matchers = x->matchers;
    
    double ratio_kept = x->ratio_kept;

    long n_limit = x->n_limit;
    long num_matched_indices = matchers.get_num_matches();
    
    if (!matchers.validate(database))
    {
        if (!x->invalid_matchers)
        {
            clock_set(x->invalid_clock, 0);
            x->invalid_matchers = true;
        }
        
        while (vec_size--)
            *out++ = 0.0;
    }
    
    for (long i = 0; i < vec_size; i++)
    {
        // Assume no match
        
        long index = -1;
        
        if (*match_in++)
        {
            // Do matching (if requested)
            
            for (long j = 0; j < x->max_matchers; j++)
                matchers.set_target(j, matcher_ins[j][i]);
            
            num_matched_indices = matchers.match(database, ratio_kept, n_limit, true);
        }
        
        // Choose a random entry from the valid list (if requested)
        
        if (*choose_in++ && num_matched_indices)
            index = matchers.get_index(gen.rand_int(static_cast<uint32_t>(num_matched_indices - 1)));
        
        *out++ = (float) index + 1;
    }
}

// DSP

void entrymatcher_dsp64(t_entrymatcher *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags)
{
    object_method(dsp64, gensym("dsp_add64"), x, entrymatcher_perform64, 0, nullptr);
}
