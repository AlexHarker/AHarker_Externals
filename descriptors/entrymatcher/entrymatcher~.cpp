
/*
 *  entrymatcher~
 *
 *  entrymatcher~ is the audio rate counterpart of entrymatcher, capable of matching with sample-accurate timing.
 *  It is intended primarily for use in granular synthesis applications requiring sample accuracy for audio matching using descriptors.
 *
 *  entrymatcher~ does not offer all the features of entrymatcher, and there are some key differences to the max version:
 *
 *  1 - entrymatcher~ takes values for matching at signal rate, and has a different message structure for setting matching criteria.
 *  2 - entrymatcher~ randomly selects a value from all the valid matches and outputs only that one value.
 *  3 - for reasons of efficiency the matching process and the randomisation processes can be triggered separately to allow variation over small time scales, without the expense of recalculating matches.
 *  4 - only numeric data can be used (although entries and columns can still be referred to using symbols).
 *
 *  In practice a large number of matching scenarios and data lookup requirements can be satisfied.
 *  The features of the entrymatcher~ object are covered in detail in the helpfile documentation.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Lifecycle.hpp>
#include <RandomGenerator.hpp>

#include "entry_database.hpp"
#include "entrymatcher_common.hpp"
#include "matchers.hpp"

t_class *this_class;

struct t_entrymatcher
{
    
    t_pxobject x_obj;
    
    t_entry_database *database_object;
    matchers *matchers;
    
    t_patcher *patcher;
    long embed;

    long max_matchers;
    long n_limit;
    double ratio_kept;
    
    random_generator<> gen;
    
    float *matcher_ins[256];
};

void *entrymatcher_new(t_symbol *sym, long argc, t_atom *argv);
void entrymatcher_free(t_entrymatcher *x);
void entrymatcher_assist(t_entrymatcher *x, void *b, long m, long a, char *s);

void entrymatcher_limit(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);
void entrymatcher_matchers(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);

t_int *entrymatcher_perform(t_int *w);
void entrymatcher_dsp(t_entrymatcher *x, t_signal **sp, short *count);

void entrymatcher_perform64(t_entrymatcher *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void entrymatcher_dsp64(t_entrymatcher *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

/*****************************************/
// Basic Object Routines: main, new, free and assist
/*****************************************/

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
    
    class_addmethod(this_class, (method) entrymatcher_dsp, "dsp", A_CANT, 0);
    class_addmethod(this_class, (method) entrymatcher_dsp64, "dsp64", A_CANT, 0);

    entrymatcher_common<t_entrymatcher>::class_add(this_class);
    
    class_dspinit(this_class);

    class_register(CLASS_BOX, this_class);
        
    return 0;
}

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
    t_atom_long num_columns = argc  ? atom_getlong(argv++) : 0;
    t_atom_long max_matchers = (argc > 1)  ? atom_getlong(argv++) : 0;
    
    t_entrymatcher *x = (t_entrymatcher *) object_alloc(this_class);
    
    create_object(x->gen);

    dsp_setup((t_pxobject *) x, 2 + max_matchers);
    outlet_new((t_object *) x, "signal");
    
    x->database_object = database_create(x, name, num_reserved_entries, num_columns);
    x->matchers = new matchers;
    
    x->max_matchers = std::max(std::min(max_matchers, t_atom_long(256)), t_atom_long(1));
    x->ratio_kept = 1.0;
    x->n_limit = 0;
    
    entrymatcher_common<t_entrymatcher>::object_init(x);

    return x;
}

void entrymatcher_free(t_entrymatcher *x)
{
    dsp_free(&x->x_obj);
    database_release(x, x->database_object);
    delete x->matchers;
    destroy_object(x->gen);
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

/*****************************************/
// Matchers and Matching Routines
/*****************************************/

void entrymatcher_limit(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
    long n_limit = argc > 0 ?  atom_getlong(argv + 0): 0;
    double ratio_kept = argc > 1 ?  atom_getfloat(argv + 1) : 1.0;
    
    // Clip values
    
    x->ratio_kept = std::min(std::max(ratio_kept, 0.0), 1.0);
    x->n_limit = std::max(n_limit, 0L);
}

// Set the matching criteria

void entrymatcher_matchers(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
    using test = matchers::test;
    
    long max_matchers = x->max_matchers;
    
    auto database = database_getptr_read(x->database_object);
    
    x->matchers->clear();
    
    while (argc > 1 && x->matchers->size() < max_matchers)
    {
        // Find the column index for the test and the test type
        
        long column = database->column_from_specifier(argv++);
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
        
        if (column >= 0 && column < database->num_columns())
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
                
                x->matchers->add_matcher(type, column, scale);
            }
        }
        else
        {
            object_error((t_object *) x, "specified column in matchers message does not exist");
        }
    }
    
    if (argc > 0)
    {
        if (x->matchers->size() < max_matchers)
            object_error((t_object *) x, "too many arguments to matchers message for number of specified tests");
        else
            object_error((t_object *) x, "not enough arguments to matchers message to correctly specify final matcher");
    }
}

/*****************************************/
// Perform and DSP Routines
/*****************************************/

t_int *entrymatcher_perform(t_int *w)
{
    // Set pointers
    
    float *choose_in = (float *) w[1];
    float *match_in = (float *) w[2];
    float **matcher_ins = (float **) w[3];
    float *out = (float *) w[4];
    long vec_size = w[5];
    t_entrymatcher *x = (t_entrymatcher *) w[6];
    
    random_generator<>& gen = x->gen;

    auto database = database_getptr_read(x->database_object);
    matchers *matchers = x->matchers;
    
    double ratio_kept = x->ratio_kept;
    
    long n_limit = x->n_limit;
    long num_matched_indices = matchers->get_num_matches();
    
    for (long i = 0; i < vec_size; i++)
    {
        // Assume no match
        
        long index = -1;
        
        if (*match_in++)
        {
            // Do matching (if requested)
            
            for (long j = 0; j < x->max_matchers; j++)
                matchers->set_target(j, matcher_ins[j][i]);
            
            num_matched_indices = matchers->match(database, ratio_kept, n_limit, true);
        }
        
        // Choose a random entry from the valid list (if requested)
        
        if (*choose_in++ && num_matched_indices)
            index = matchers->get_index(gen.rand_int(static_cast<uint32_t>(num_matched_indices - 1)));

        *out++ = (float) index + 1;
    }
    
    return w + 7;
}

void entrymatcher_dsp(t_entrymatcher *x, t_signal **sp, short *count)
{
    float **matcher_ins = x->matcher_ins;
    long max_matchers = x->max_matchers;
    
    for (long i = 0; i < max_matchers; i++)
        matcher_ins[i] = (float *) sp[i + 2]->s_vec;
    
    dsp_add(entrymatcher_perform, 6, sp[0]->s_vec, sp[1]->s_vec, matcher_ins, sp[2 + max_matchers]->s_vec, sp[0]->s_n, x);
}

/*****************************************/
// 64bit Perform and DSP Routines
/*****************************************/

void entrymatcher_perform64(t_entrymatcher *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    // Set pointers
    
    double *choose_in = ins[0];
    double *match_in = ins[1];
    double **matcher_ins = ins + 2;
    double *out = outs[0];
    
    random_generator<>& gen = x->gen;
    
    auto database = database_getptr_read(x->database_object);
    matchers *matchers = x->matchers;
    
    double ratio_kept = x->ratio_kept;

    long n_limit = x->n_limit;
    long num_matched_indices = matchers->get_num_matches();
    
    for (long i = 0; i < vec_size; i++)
    {
        // Assume no match
        
        long index = -1;
        
        if (*match_in++)
        {
            // Do matching (if requested)
            
            for (long j = 0; j < x->max_matchers; j++)
                matchers->set_target(j, matcher_ins[j][i]);
            
            num_matched_indices = matchers->match(database, ratio_kept, n_limit, true);
        }
        
        // Choose a random entry from the valid list (if requested)
        
        if (*choose_in++ && num_matched_indices)
            index = matchers->get_index(gen.rand_int(static_cast<uint32_t>(num_matched_indices - 1)));
        
        *out++ = (float) index + 1;
    }
}

void entrymatcher_dsp64(t_entrymatcher *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    object_method(dsp64, gensym("dsp_add64"), x, entrymatcher_perform64, 0, nullptr);
}
