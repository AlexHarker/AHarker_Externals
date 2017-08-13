
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
#include <z_dsp.h>

#include <AH_Random.h>

#include "EntryDatabase.h"
#include "Matchers.h"
#include "utilities.h"
#include "entry_database_max.h"

t_class *this_class;

typedef struct entrymatcher {
    
    t_pxobject x_obj;
    
    t_entry_database *database_object;
    EntryDatabase *database;
    Matchers *matchers;
    
    long max_matchers;
    long n_limit;
    double ratio_kept;
    
    t_rand_gen gen;
    
    float *matcher_ins[256];
    
} t_entrymatcher;

void *entrymatcher_new(t_atom_long num_reserved_entries, t_atom_long num_columns, t_atom_long max_matchers);
void entrymatcher_free(t_entrymatcher *x);
void entrymatcher_assist(t_entrymatcher *x, void *b, long m, long a, char *s);

void entrymatcher_refer(t_entrymatcher *x, t_symbol *name);
void entrymatcher_clear(t_entrymatcher *x);
void entrymatcher_labelmodes(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);
void entrymatcher_names(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);
void entrymatcher_entry(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);
void entrymatcher_remove(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);

void entrymatcher_limit(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);
void entrymatcher_matchers(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);

t_int *entrymatcher_perform (t_int *w);
void entrymatcher_dsp(t_entrymatcher *x, t_signal **sp, short *count);

void entrymatcher_perform64 (t_entrymatcher *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void entrymatcher_dsp64(t_entrymatcher *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

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

    class_addmethod(this_class, (method)entrymatcher_refer,"refer", A_SYM, 0);
    class_addmethod(this_class, (method)entrymatcher_entry,"entry", A_GIMME, 0);
    class_addmethod(this_class, (method)entrymatcher_remove,"remove", A_GIMME, 0);
    class_addmethod(this_class, (method)entrymatcher_limit,"limit", A_GIMME, 0);
    class_addmethod(this_class, (method)entrymatcher_matchers,"matchers", A_GIMME, 0);
    class_addmethod(this_class, (method)entrymatcher_labelmodes,"labelmodes", A_GIMME, 0);
    class_addmethod(this_class, (method)entrymatcher_names,"names", A_GIMME, 0);
    class_addmethod(this_class, (method)entrymatcher_assist, "assist", A_CANT, 0);
    
    class_addmethod(this_class, (method)entrymatcher_dsp, "dsp", A_CANT, 0);
    class_addmethod(this_class, (method)entrymatcher_dsp64, "dsp64", A_CANT, 0);
    
    class_dspinit(this_class);
    
    class_register(CLASS_BOX, this_class);
    
    init_test_symbols();
    
    return 0;
}

void *entrymatcher_new(t_atom_long num_reserved_entries, t_atom_long num_columns, t_atom_long max_matchers)
{
    t_entrymatcher *x = (t_entrymatcher *)object_alloc(this_class);
    
    dsp_setup((t_pxobject *)x, 2 + max_matchers);
    outlet_new((t_object *)x, "signal");
    
    x->database_object = entry_database_get_database_object(&x->database, symbol_unique(), num_reserved_entries, num_columns);
    x->matchers = new Matchers;
    
    x->max_matchers = std::max(std::min(num_columns, t_atom_long(256)), t_atom_long(1));;
    x->ratio_kept = 1.0;
    x->n_limit = 0;
    
    rand_seed(&x->gen);
    
    return (x);
}

void entrymatcher_free(t_entrymatcher *x)
{
    dsp_free(&x->x_obj);
    entry_database_release(x->database_object);
    delete x->matchers;
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
// Entry routines: refer, clear, labelmodes, names, entry and removal
// ========================================================================================================================================== //

void entrymatcher_refer(t_entrymatcher *x, t_symbol *name)
{
    x->database_object = entry_database_get_database_object(x->database_object, &x->database, name);
}

void entrymatcher_clear(t_entrymatcher *x)
{
    x->database->clear();
}

void entrymatcher_labelmodes(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
    x->database->setLabelModes(x, argc, argv);
}

void entrymatcher_names(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
    x->database->setNames(x, argc, argv);
}

void entrymatcher_entry(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
    x->database->addEntry(x, argc, argv);
}

void entrymatcher_remove(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
    if (!argc)
        object_error((t_object *)x, "no identifier given for remove message");
    else
    {
        while(argc--)
            x->database->removeEntry(x, argv++);
    }
}

// ========================================================================================================================================== //
// Matchers and Matching routines:
// ========================================================================================================================================== //

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
    long max_matchers = x->max_matchers;
    
    x->matchers->clear();
    
    while (argc > 1 && x->matchers->size() < max_matchers)
    {
        // Find the column index for the test and the test type
        
        long column = x->database->columnFromSpecifier(argv++);
        TestType type = entrymatcher_test_types(argv++);
        
        // If that fails we are done
        
        if (type == TEST_NONE)
        {
            object_error((t_object *)x, "incorrect test type");
            break;
        }
        
        argc -= 2;
        
        // Get number of expected arguments for this test type
        
        long arg_check = (type == TEST_SCALE || type == TEST_WITHIN || type == TEST_SCALE_RATIO || type == TEST_WITHIN_RATIO) ? 1 : 0;
        
        // Check that the arguments are all valid
        
        if (column >= 0 && column < x->database->numColumns())
        {
            if (argc >= arg_check)
            {
                // Store parameters for this test
                
                double scale = 1.0;
                
                if (type == TEST_SCALE || type == TEST_WITHIN)
                    scale = fabs(1. / atom_getfloat(argv++));
                else if (type == TEST_SCALE_RATIO || type == TEST_WITHIN_RATIO)
                {
                    scale = fabs(atom_getfloat(argv++));
                    scale = (scale < 1.0) ? 1.0 / scale : scale;
                    scale = 1.0 / (scale - 1.0);
                }
                argc -= arg_check;
                
                switch (type)
                {
                    case TEST_NONE:                 break;
                    case TEST_MATCH:                x->matchers->addMatcher(Matchers::kTestMatch, column);                      break;
                    case TEST_LESS_THAN:            x->matchers->addMatcher(Matchers::kTestLess, column);                       break;
                    case TEST_GREATER_THAN:         x->matchers->addMatcher(Matchers::kTestGreater, column);                    break;
                    case TEST_LESS_THAN_EQ:         x->matchers->addMatcher(Matchers::kTestLessEqual, column);                  break;
                    case TEST_GREATER_THAN_EQ:      x->matchers->addMatcher(Matchers::kTestGreaterEqual, column);               break;
                    case TEST_DISTANCE:             x->matchers->addMatcher(Matchers::kTestDistance, column);                   break;
                    case TEST_SCALE:                x->matchers->addMatcher(Matchers::kTestDistance, column, scale);            break;
                    case TEST_WITHIN:               x->matchers->addMatcher(Matchers::kTestDistanceReject, column, scale);      break;
                    case TEST_DISTANCE_RATIO:       x->matchers->addMatcher(Matchers::kTestRatio, column);                      break;
                    case TEST_SCALE_RATIO:          x->matchers->addMatcher(Matchers::kTestRatio, column, scale);               break;
                    case TEST_WITHIN_RATIO:         x->matchers->addMatcher(Matchers::kTestRatioReject, column, scale);         break;
                }
            }
        }
        else
        {
            object_error((t_object *)x, "specified column in matchers message does not exist");
        }
    }
    
    if (argc > 0)
    {
        if (x->matchers->size() < max_matchers)
            object_error((t_object *)x, "too many arguments to matchers message for number of specified tests");
        else
            object_error((t_object *)x, "not enough arguments to matchers message to correctly specify final matcher");
    }
}

// ========================================================================================================================================== //
// Perform and DSP routines:
// ========================================================================================================================================== //

t_int *entrymatcher_perform(t_int *w)
{
    // Set pointers
    
    float *choose_in = (float *) w[1];
    float *match_in = (float *) w[2];
    float **matcher_ins = (float **) w[3];
    float *out = (float *) w[4];
    long vec_size = w[5];
    t_entrymatcher *x = (t_entrymatcher *) w[6];
    
    t_rand_gen *gen = &x->gen;
    
    Matchers *matchers = x->matchers;
    
    long n_limit = x->n_limit;
    long num_matched_indices = matchers->getNumMatches();
    
    for (long i = 0; i < vec_size; i++)
    {
        // Assume no match
        
        long index = -1;
        
        if (*match_in++)
        {
            // Do matching (if requested)
            
            for (long j = 0; j < x->max_matchers; j++)
                matchers->setTarget(j, matcher_ins[j][i]);
            
            num_matched_indices = matchers->match(x->database);
            
            // N.B. If there are no matchers ALWAYS match everything...
            // Else pick the top n matches when there are more than n matches
            
            if (n_limit && matchers->size())
                num_matched_indices = matchers->getTopN(n_limit);
        }
        
        // Choose a random entry from the valid list (if requested)
        
        if (*choose_in++ && num_matched_indices)
            index = matchers->getIndex(rand_int_n(gen, num_matched_indices - 1));
        
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

// ========================================================================================================================================== //
// 64bit Perform and DSP routines:
// ========================================================================================================================================== //

void entrymatcher_perform64(t_entrymatcher *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{	
    // Set pointers
    
    double *choose_in = ins[0];
    double *match_in = ins[1];
    double **matcher_ins = ins + 2;
    double *out = outs[0];
    
    t_rand_gen *gen = &x->gen;
    
    Matchers *matchers = x->matchers;
    
    long n_limit = x->n_limit;
    long num_matched_indices = matchers->getNumMatches();
    
    for (long i = 0; i < vec_size; i++)
    {
        // Assume no match
        
        long index = -1;
        
        if (*match_in++) 
        {
            // Do matching (if requested)
            
            for (long j = 0; j < x->max_matchers; j++)
                matchers->setTarget(j, matcher_ins[j][i]);
            
            num_matched_indices = matchers->match(x->database);
            
            // N.B. If there are no matchers ALWAYS match everything...
            // Else pick the top n matches when there are more than n matches
            
            if (n_limit && matchers->size())
                num_matched_indices = matchers->getTopN(n_limit);
        }
        
        // Choose a random entry from the valid list (if requested)
        
        if (*choose_in++ && num_matched_indices)
            index = matchers->getIndex(rand_int_n(gen, num_matched_indices - 1));
        
        *out++ = (float) index + 1;
    }
}

void entrymatcher_dsp64(t_entrymatcher *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{				
    object_method(dsp64, gensym("dsp_add64"), x, entrymatcher_perform64, 0, NULL);
}

/*
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
*/