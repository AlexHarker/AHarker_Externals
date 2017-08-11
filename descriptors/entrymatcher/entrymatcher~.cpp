
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

t_class *this_class;

typedef struct entrymatcher {
    
    t_pxobject x_obj;
    
    EntryDatabase *mDatabase;
    Matchers *mMatchers;
    
    std::vector<long> *mIndices;
    std::vector<double> *mDistances;
    
    long max_matchers;
    long n_limit;
    double ratio_kept;
    
    long num_matched_indices;
    
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

void *entrymatcher_new(long max_entries, long num_columns, long max_matchers);
void entrymatcher_free(t_entrymatcher *x);
void entrymatcher_assist(t_entrymatcher *x, void *b, long m, long a, char *s);

void entrymatcher_clear(t_entrymatcher *x);
void entrymatcher_names(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);
void entrymatcher_entry(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv);

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
    
    x->mDatabase = new EntryDatabase(num_columns);
    x->mMatchers = new Matchers;
    x->mIndices = new std::vector<long>;
    x->mDistances = new std::vector<double>;
    
    x->mDatabase->reserve(max_entries);
    
    
    x->max_matchers = max_matchers;
    x->ratio_kept = 1.0;
    x->n_limit = 0;
    x->num_matched_indices = 0;
    
    rand_seed(&x->gen);
    
    return (x);
}

void entrymatcher_free(t_entrymatcher *x)
{
    dsp_free(&x->x_obj);
    delete x->mDatabase;
    delete x->mMatchers;
    delete x->mIndices;
    delete x->mDistances;
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
// Matchers and Matching routines:
// ========================================================================================================================================== //

void entrymatcher_limit(t_entrymatcher *x, t_symbol *msg, long argc, t_atom *argv)
{
    long n_limit = argc > 0 ?  atom_getlong(argv + 0): 0;
    double ratio_kept = argc > 1 ?  atom_getfloat(argv + 1) : 0.;
    
    if (n_limit < 1)
        n_limit = 0;
    if (ratio_kept <= 0.0 || ratio_kept > 1.0)
        ratio_kept = 1.0;
    
    x->n_limit = n_limit;
    x->ratio_kept = ratio_kept;
    
}

// Set the matching criteria

void entrymatcher_matchers(t_entrymatcher *x, t_symbol *msg, short argc, t_atom *argv)
{
    long max_matchers = x->max_matchers;
    
    while (argc > 1 && x->mMatchers->size() < max_matchers)
    {
        // Find the column index for the test and the test type
        
        long column = x->mDatabase->columnFromSpecifier(*argv++);
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
        
        if (column >= 0 && column < x->mDatabase->numColumns())
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
            }
        }
        else
        {
            object_error((t_object *)x, "specified column in matchers message does not exist");
        }
    }
    
    if (argc > 0)
    {
        if (x->mMatchers->size() < max_matchers)
            object_error((t_object *)x, "too many arguments to matchers message for number of specified tests");
        else
            object_error((t_object *)x, "not enough arguments to matchers message to correctly specify final matcher");
    }
}

// Find the top n matches (those with the smallest distance values)

void entrymatcher_top_n(std::vector<long>& matched_indices, std::vector<double>& distances, long num_matched_indices, long n_limit)
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
    
    t_rand_gen *gen = &x->gen;
    
    long n_limit = x->n_limit;
    long num_matched_indices = x->num_matched_indices;
    
    for (long i = 0; i < vec_size; i++)
    {
        // Assume no match
        
        long index = -1;
        
        if (*match_in++)
        {
            // Do matching (if requested)
            
            for (long j = 0; j < x->max_matchers; j++)
                x->mMatchers->setTarget(j, matcher_ins[j][i]);
            
            num_matched_indices = x->mMatchers->match(x->mDatabase, *x->mIndices, *x->mDistances);
            
            if (num_matched_indices)
            {
                // N.B. If there are no matchers ALWAYS match everything...
                // Else pick the top n matches when there are more than n matches
                
                if (n_limit && n_limit < num_matched_indices && x->mMatchers->size())
                {
                    entrymatcher_top_n(*x->mIndices, *x->mDistances, num_matched_indices, n_limit);
                    num_matched_indices = n_limit;
                }
            }
        }
        
        // Choose a random entry from the valid list (if requested)
        
        if (*choose_in++ && num_matched_indices)
            index = (*x->mIndices)[rand_int_n(gen, num_matched_indices - 1)];
        
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
    
    t_rand_gen *gen = &x->gen;
    
    long n_limit = x->n_limit;
    long num_matched_indices = x->num_matched_indices;
    
    for (long i = 0; i < vec_size; i++)
    {
        // Assume no match
        
        long index = -1;
        
        if (*match_in++) 
        {
            // Do matching (if requested)
            
            for (long j = 0; j < x->max_matchers; j++)
                x->mMatchers->setTarget(j, matcher_ins[j][i]);
            
            num_matched_indices = x->mMatchers->match(x->mDatabase, *x->mIndices, *x->mDistances);
            
            if (num_matched_indices) 
            {
                // N.B. If there are no matchers ALWAYS match everything...
                // Else pick the top n matches when there are more than n matches
                
                if (n_limit && n_limit < num_matched_indices && x->mMatchers->size())
                {
                    entrymatcher_top_n(*x->mIndices, *x->mDistances, num_matched_indices, n_limit);
                    num_matched_indices = n_limit;
                }
            }
        }
        
        // Choose a random entry from the valid list (if requested)
        
        if (*choose_in++ && num_matched_indices)
            index = (*x->mIndices)[rand_int_n(gen, num_matched_indices - 1)];
        
        *out++ = (float) index + 1;
    }
    
    x->num_matched_indices = num_matched_indices;
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