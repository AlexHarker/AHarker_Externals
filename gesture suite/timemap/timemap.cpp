
/*
 *  timemap
 *
 *  timemap is an object that creates randomised lists of values with definable characteristics.
 *
 *  The process starts with a list of equidistant values (between 0 and 1) which are processed to create the final list.
 *  The list is randomised, warped and scaled to create the final list.
 *  Different output scalings are supported, appropriate to a range of musical parameters (as in the valconvert object).
 *  Values can be generated directly or alternateively in "streams".
 *  When in a streaming mode generated values are accumulated through addition or multiplication from an initial value.

 *  timemap was originally intended to create lists of timing values as part of a system of gestural representation.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>

#include <AH_Lifecycle.hpp>
#include <random_generator.hpp>

#include <algorithm>
#include <functional>


// Globals and Object Structure

using rand_gen = htl::random_generator<>;

t_class *this_class;

enum class scaling_mode
{
    none,
    scale,
    log,
    exp,
    div
};

enum class streaming_mode
{
    none,
    additive,
    multiplicative
};

struct scaling_parameters
{
    double scale_val1;
    double scale_val2;
    double min_val;
    double max_val;
    
    scaling_mode mode;
};

struct streaming_parameters
{
    double init_val;
    double min_val;
    double max_val;
    
    streaming_mode mode;
};

struct parameters
{
    double rand_amount;
    double centre;
    double warp;
    
    double min_dist;
    double max_dist;
    
    scaling_parameters scale_params;
    streaming_parameters stream_params;
    
    t_atom_long max_retries;

    bool random_order;
};

struct t_timemap
{
    t_object a_obj;
    
    parameters params;
    
    rand_gen gen;
    
    void *list_outlet;
};

// Function Prototypes

void *timemap_new(double rand_amount, double centre, double warp);
void timemap_free(t_timemap *x);
void timemap_assist(t_timemap *x, void *b, long m, long a, char *s);

void timemap_calculate(t_timemap *x, t_atom_long num_points);

inline double timemap_value(rand_gen& gen, long i, double points_recip, double rand_amount, double centre, double warp);
double timemap_scale(double val, const scaling_parameters& params);
bool check_and_insert(rand_gen& gen, double val, double *vals, long N, const parameters& params);

void timemap_rand_amount(t_timemap *x, double rand_amount);
void timemap_centre(t_timemap *x, double centre);
void timemap_warp(t_timemap *x, double warp);
void timemap_min_dist(t_timemap *x, double min_dist);
void timemap_max_dist(t_timemap *x, double max_dist);
void timemap_max_retries(t_timemap *x, t_atom_long max_retries);
void timemap_random_order(t_timemap *x, t_atom_long random_order);
void timemap_scaling(t_timemap *x, t_symbol *scale_mode_sym, double min_val, double max_val);
void timemap_stream(t_timemap *x, t_atom_long stream_mode, double init_val, double min_val, double max_val);

// Clip Helper

double clip(double value, double min_val, double max_val)
{
    return std::max(min_val, std::min(max_val, value));
}

// Main

int C74_EXPORT main()
{
    this_class = class_new("timemap",
                           (method) timemap_new,
                           (method) timemap_free,
                           sizeof(t_timemap),
                           (method) nullptr,
                           A_DEFFLOAT,
                           A_DEFFLOAT,
                           A_DEFFLOAT,
                           0);
    
    class_addmethod(this_class, (method) timemap_calculate, "int", A_LONG, 0);
    class_addmethod(this_class, (method) timemap_rand_amount, "ft1", A_FLOAT, 0);
    class_addmethod(this_class, (method) timemap_centre, "ft2", A_FLOAT, 0);
    class_addmethod(this_class, (method) timemap_warp, "ft3", A_FLOAT, 0);
    class_addmethod(this_class, (method) timemap_min_dist, "ft4", A_FLOAT, 0);
    class_addmethod(this_class, (method) timemap_max_dist, "ft5", A_FLOAT, 0);
    class_addmethod(this_class, (method) timemap_max_retries, "in6", A_LONG, 0);
    class_addmethod(this_class, (method) timemap_random_order, "random_order", A_LONG, 0);
    class_addmethod(this_class, (method) timemap_scaling, "scaling", A_SYM, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(this_class, (method) timemap_stream, "stream", A_DEFLONG, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
    class_addmethod(this_class, (method) timemap_assist, "assist", A_CANT, 0);
    
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / Free / Assist

void *timemap_new(double rand_amount, double centre, double warp)
{
    t_timemap *x = (t_timemap *) object_alloc(this_class);
    
    create_object(x->gen);

    x->list_outlet = listout(x);
    intin(x, 6);
    floatin(x, 5);
    floatin(x, 4);
    floatin(x, 3);
    floatin(x, 2);
    floatin(x, 1);
    
    x->params.centre = clip(centre, 0.0, 1.0);
    x->params.rand_amount = clip(rand_amount, 0.0, 1.0);
    x->params.warp = (warp <= 0.) ? 1.0 : warp;
    x->params.min_dist = 0.0;
    x->params.max_dist = 1.0;
    x->params.max_retries = 10;
    x->params.random_order = false;
    
    timemap_scaling(x, gensym("none"), 0, 1);
    timemap_stream(x, 0, 0, 0, 1);
        
    return x;
}

void timemap_free(t_timemap *x)
{
    destroy_object(x->gen);
}

void timemap_assist(t_timemap *x, void *b, long m, long a, char *s)
{
    static constexpr int maxAssist = 256;

    if (m == ASSIST_INLET)
    {
        switch (a)
        {
            case 0:
                snprintf(s, maxAssist, "Do Mapping");
                break;
            case 1:
                snprintf(s, maxAssist, "Random Amount (0-1)");
                break;
            case 2:
                snprintf(s, maxAssist, "Centre (0-1)");
                break;
            case 3:
                snprintf(s, maxAssist, "Clumping Factor");
                break;
            case 4:
                snprintf(s, maxAssist, "Min Interval (0-1)");
                break;
            case 5:
                snprintf(s, maxAssist, "Max Interval (0-1)");
                break;
            case 6:
                snprintf(s, maxAssist, "Max Retries");
                break;
        }
    }
    else
        snprintf(s, maxAssist, "List Out");
}

// Streaming Template

template <typename T>
void timemap_do_stream(t_atom *output_list, double *vals, double cumulate, long& N, T op)
{
    N++;
    atom_setfloat(output_list, cumulate);
    
    for (long i = 1; i < N; i++)
        atom_setfloat(output_list + i, (cumulate = op(cumulate, vals[i - 1])));
}

// Calculation Method

void timemap_calculate(t_timemap *x, t_atom_long num_points)
{
    t_atom output_list[1024];
    double vals[1024];
    long items[1024];
    
    const parameters params = x->params;
    
    long list_length = 0;
    
    if (num_points < 1)
    {
        object_error((t_object *) x, "requested number of values must be a positive integer");
        return;
    }
    
    if (num_points > 1024)
    {
        object_error((t_object *) x, "maximum number of output values is 1024");
        num_points = 1024;
    }
    
    if (params.stream_params.mode != streaming_mode::none)
        num_points--;

    const double points_recip = 1.0 / (double) (num_points + 1);

    // If the ordering is to be random start with a list of potential items (0 to num_points - 1)
    
    if (params.random_order)
    {
        for (long i = 0; i < num_points; i++)
            items[i] = i;
    }
    
    // Generate each value in turn
    
    for (long i = 0; i < num_points; i++)
    {
        long item = i;
        long item_pos = 0;
        t_atom_long retries = 0;
        
        // Loop until a suitable value is found (or we reach the maximum number of retries)
        
        for (bool suitable = false; !suitable && retries < params.max_retries; retries++)
        {
            // Choose which of the remaining divisions to do if the order is random
            
            if (params.random_order)
            {
                item_pos = x->gen.rand_int(static_cast<uint32_t>(num_points - (list_length + 1)));
                item = items[item_pos];
            }
            
            // Generate a new value
            
            double val = timemap_value(x->gen, item, points_recip, params.rand_amount, params.centre, params.warp);
            
            // Scale if relevant
            
            if (params.scale_params.mode != scaling_mode::none)
                val = timemap_scale(val, params.scale_params);
            
            // If the value is good then keep it and increase list_length
                        
            if (check_and_insert(x->gen, val, vals, list_length, params))
            {
                suitable = true;
                list_length++;
                
                // Replace the used value with the one about to be removed (maintaing a valid list of unused items)

                if (params.random_order)
                    items[item_pos] = items[num_points - list_length];
            }
        }
    }
    
    // Do stream mode
    
    switch (params.stream_params.mode)
    {
        case streaming_mode::none:
            for (long i = 0; i < list_length; i++)
                atom_setfloat(output_list + i, vals[i]);
            break;
            
        case streaming_mode::additive:
            timemap_do_stream(output_list, vals, params.stream_params.init_val, list_length, std::plus<double>());
            break;
            
        case streaming_mode::multiplicative:
            timemap_do_stream(output_list, vals, params.stream_params.init_val, list_length, std::multiplies<double>());
            break;
    }
    
    outlet_list(x->list_outlet, nullptr, static_cast<short>(list_length), output_list);
}

// Value Generation

inline double timemap_value(rand_gen& gen, long i, double points_recip, double rand_amount, double centre, double warp)
{
    // Start with equally spaced values (in the range 0 to 1)
    
    double val = (i + 1) * points_recip;
    
    // Interpolate with a random value (0 to 1) by the random amount
    
    val = val - (rand_amount * (val -  gen.rand_double()));
    
    // Now warp around the centre value, according to the warp factor (using a pow operation)
    
    if (val > centre)
        return centre + ((1.0 - centre) * pow((val - centre) / (1.0 - centre), warp));
    else
        return centre - (centre * pow((centre - val) / centre, warp));
}

// Value Scaling (applies scaling for modes other than none)

double timemap_scale(double val, const scaling_parameters& params)
{
    bool reciprocal = false;
    
    if (params.mode == scaling_mode::div && val < 0.5)
    {
        reciprocal = true;
        val = 1.0 - val;
    }
    
    val = val * params.scale_val1 + params.scale_val2;
    
    if (params.mode == scaling_mode::log)
        val = exp(val);
    if (params.mode == scaling_mode::exp)
        val = log(val);
    
    val = clip(val, params.min_val, params.max_val);
    
    if (reciprocal)
        val = 1.0 / val;
    
    return val;
}

// Sorting and Checking Functions

template <typename T>
bool check_stream(double *vals, long pos, long N, double val, double cumulate, double min_val, double max_val, T op)
{
    for (long i = 0; i < pos; i++)
        cumulate = op(vals[i], cumulate);
    
    cumulate = op(val, cumulate);

    if (cumulate < min_val || cumulate > max_val)
        return false;
    
    for (long i = pos; i < N; i++)
    {
        cumulate = op(vals[i], cumulate);
        if (cumulate < min_val || cumulate > max_val)
            return false;
    }
    
    return true;
}

bool check_and_insert(rand_gen& gen, double val, double *vals, long N, const parameters& params)
{
    const double& init_val = params.stream_params.init_val;
    const double& min_val = params.stream_params.min_val;
    const double& max_val = params.stream_params.max_val;
    
    long pos;
        
    // Find Position for New Value (if order is random then pick randomly, otherwise find thex correct place)
    
    if (params.random_order)
        pos = gen.rand_int(static_cast<uint32_t>(N));
    else
    {
        long j = 0;
        
        for (; j < N; j++)
            if (val < vals[j])
                break;
        
        pos = j;
    }
    
    // Check the minimum and maximum allowable distances - both preceding and subsequent values
        
    if (pos)
    {
        const double test = fabs(val - vals[pos - 1]);
        if ((test < params.min_dist) || (test > params.max_dist))
            return false;
    }
    
    if (pos < N)
    {
        const double test = fabs(vals[pos] - val);
        if ((test < params.min_dist) || (test > params.max_dist))
            return false;
    }

    // If we are in stream mode then check that this value will not result in output exceeding the stream bounds
            
    switch (params.stream_params.mode)
    {
        case streaming_mode::none:
            break;
            
        case streaming_mode::additive:
            if (!check_stream(vals, pos, N, val, init_val, min_val, max_val, std::plus<double>()))
                return false;
            break;
        
        case streaming_mode::multiplicative:
            if (!check_stream(vals, pos, N, val, init_val, min_val, max_val, std::multiplies<double>()))
                return false;
            break;
    }
    
    // If the value is suitable then here we insert the value
    
    for (long j = N; j > pos; j--)
        vals[j] = vals[j - 1];
    
    vals[pos] = val;
    
    return true;
}

// Mapping Parameter Methods

void timemap_rand_amount(t_timemap *x, double rand_amount)
{
    x->params.rand_amount = clip(rand_amount, 0.0, 1.0);
}

void timemap_centre(t_timemap *x, double centre)
{
    x->params.centre = clip(centre, 0.0, 1.0);
}

void timemap_warp(t_timemap *x, double warp)
{
    x->params.warp = (warp < 0.0) ? 1.0 : warp;
}

void timemap_min_dist(t_timemap *x, double min_dist)
{
    x->params.min_dist = fabs(min_dist);
}

void timemap_max_dist(t_timemap *x, double max_dist)
{
    x->params.max_dist = fabs(max_dist);
}

void timemap_max_retries(t_timemap *x, t_atom_long max_retries)
{
    x->params.max_retries = (max_retries < 0) ? 0 : max_retries;
}

void timemap_random_order(t_timemap *x, t_atom_long random_order)
{
    x->params.random_order = random_order ? true : false;
}

// Mode Methods

void timemap_scaling(t_timemap *x, t_symbol *scale_mode_sym, double min_val, double max_val)
{
    scaling_mode mode = scaling_mode::none;
    
    double scale_min = min_val;
    double scale_max = max_val;
    
    if (scale_mode_sym == gensym("scale"))
        mode = scaling_mode::scale;
    
    if (scale_mode_sym == gensym("amp"))
    {
        mode = scaling_mode::log;
        scale_min = min_val = pow(10, scale_min / 20.0);
        scale_max = max_val = pow(10, scale_max / 20.0);
    }
    
    if (scale_mode_sym == gensym("pitch"))
    {
        mode = scaling_mode::log;
        scale_min = min_val = pow(2, scale_min / 12.0);
        scale_max = max_val = pow(2, scale_max / 12.0);
    }
    
    if (scale_mode_sym == gensym("log"))
        mode = scaling_mode::log;
    
    if (scale_mode_sym == gensym("exp"))
        mode = scaling_mode::exp;
    
    if (scale_mode_sym == gensym("div"))
    {
        mode = scaling_mode::div;
        scale_min = max_val - (2.0 * (max_val - min_val));
    }
    
    if (mode == scaling_mode::log)
    {
        scale_min = log(scale_min);
        scale_max = log(scale_max);
    }
    
    if (mode == scaling_mode::exp)
    {
        scale_min = exp(scale_min);
        scale_max = exp(scale_max);
    }
    
    x->params.scale_params = scaling_parameters { scale_max - scale_min, scale_min, min_val, max_val, mode };
}

void timemap_stream(t_timemap *x, t_atom_long stream_mode, double init_val, double min_val, double max_val)
{
    streaming_mode mode;
    
    mode = (stream_mode == 1) ? streaming_mode::additive : streaming_mode::none;
    mode = (stream_mode > 1) ? streaming_mode::multiplicative : mode;
    
    x->params.stream_params = streaming_parameters { init_val, min_val, max_val, mode };
}
