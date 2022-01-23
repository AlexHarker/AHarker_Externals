
/*
 *  meandev
 *
 *  meandev is an object for calculating the mean and variance or standard deviation of a set of values over various ranges in the data's history.
 *  This makes it possible to compare these values for different ranges in time and make some assesment of how the data is changing.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#include <ext.h>
#include <ext_obex.h>

#include <AH_Lifecycle.hpp>

#include <algorithm>
#include <utility>
#include <vector>


t_class *this_class;

// Core Types

struct t_data_base
{
    void set_size(long size)        { m_size = size; }
    long count() const              { return m_count; }
    long postion_idx() const        { return m_position; }
    long oldest_idx() const         { return (m_count - 1 <= m_position ? m_position - m_count : m_size + m_position - m_count) + 1; }
    long next_idx() const           { return (m_position >= m_size - 1) ? 0 : m_position + 1; }
    bool is_newest(long idx) const  { return idx == m_position; }
    void remove()                   { m_count = m_count ? m_count - 1 : 0; }

    void reset()
    {
        m_count = 0;
        m_position = -1;
    }
    
    bool add()
    {
        bool full = m_count == m_size;

        m_position = next_idx();
        m_count = full ? m_count : m_count + 1;
    
        return full;
    }
    
    template <typename T>
    double sum_data(const T& data) const
    {
        double sum = 0.0;
        
        long oldest = oldest_idx();
        
        // Sum data

        for (long i = oldest > m_position ? 0 : oldest; i <= m_position; i++)
            sum += data(i);
        for (long i = oldest > m_position ? oldest : m_size; i < m_size; i++)
            sum += data(i);
        
        return sum / data.total(*this);
    }
    
protected:
    
    long m_size = 0;
    long m_count = 0;
    long m_position = -1;
};

struct t_data : t_data_base
{
    void set_size(long size, bool allocate_ages)
    {
        t_data_base::set_size(size);
        m_data.resize(size);
        if (allocate_ages)
            m_ages.resize(size);
    }
    
    bool add(double data, long age)
    {
        // (N.B. only those needed for the mode are stored)

        bool overflow = t_data_base::add();
        
        m_data[m_position] = data;
        if (m_ages.size())
            m_ages[m_position] = age;
        
        return overflow;
    }
 
    const double *data() const   { return m_data.data(); }
    const long   *ages() const   { return m_ages.data(); }
    
    long oldest_age() const      { return m_ages[oldest_idx()]; }
    
protected:
    
    std::vector<double> m_data;
    std::vector<long> m_ages;
};

struct t_weighted_data : t_data
{
    void set_size(long size, bool allocate_ages, bool allocate_weights)
    {
        t_data::set_size(size, allocate_ages);
        if (allocate_weights)
            m_weights.resize(size);
    }
    
    bool add(double data, long age, double weight)
    {
        // (N.B. only those needed for the mode are stored)
        
        bool overflow = t_data::add(data, age);

        if (m_weights.size())
            m_weights[m_position] = weight;
        
        return overflow;
    }
 
    const double *weights() const   { return m_weights.data(); }

protected:

    std::vector<double> m_weights;
};

struct t_duration_data
{
    t_duration_data() = default;
    t_duration_data(const t_duration_data&) = delete;
    t_duration_data& operator=(const t_duration_data&) = delete;
    
    ~t_duration_data()
    {
        if (f_data_clock)
            freeobject((t_object *) f_data_clock);
        if (f_mean_clock)
            freeobject((t_object *) f_mean_clock);
    }
    
    long age_span() const { return max_age - min_age; }
    
    void reset()
    {
        core_data.reset();
        mean_data.reset();
        
        if (f_data_clock)
            clock_unset(f_data_clock);
        if (f_mean_clock)
            clock_unset(f_mean_clock);
    }
    
    t_data_base core_data;
    t_data mean_data;
    
    long min_age = 0;
    long max_age = 0;

    bool add_remove = false;
    
    t_outlet *f_out1 = nullptr;
    t_outlet *f_out2 = nullptr;
    t_clock *f_data_clock = nullptr;
    t_clock *f_mean_clock = nullptr;
};

double square_difference(double data, double mean)
{
    double difference = data - mean;
    return difference * difference;
}

struct sum_functor
{
    sum_functor(const double *data) : m_data(data) {}
    
    double operator()(long i) const                 { return m_data[i]; }
    double total(const t_data_base& base) const     { return 1.0; }
    
    const double *m_data;
};

struct data_functor : sum_functor
{
    data_functor(const t_data& data) : sum_functor(data.data()) {}
        
    double total(const t_data_base& base) const { return base.count(); }
};

struct weighted_data_functor : data_functor
{
    weighted_data_functor(const t_weighted_data& data) : data_functor(data), m_weights(data.weights()) {}
    
    double operator()(long i) const                 { return m_data[i] * m_weights[i]; }
    double total(const t_data_base base) const      { return base.sum_data(sum_functor(m_weights)); }
    
    const double *m_weights;
};
    
struct variance_functor : data_functor
{
    variance_functor(const t_data& data, double mean) : data_functor(data), m_mean(mean) {}

    double operator()(long i) const { return square_difference(m_data[i], m_mean); }
    
    double m_mean;
};
    
struct variance_weighted_functor : weighted_data_functor
{
    variance_weighted_functor(const t_weighted_data& data, double mean) : weighted_data_functor(data), m_mean(mean) {}

    double operator()(long i) const { return square_difference(m_data[i], m_mean) * m_weights[i]; }
    
    double m_mean;
};

struct t_meandev
{
    bool is_first(t_duration_data *duration) { return duration == durations + num_dur - 1; }
    
    t_object a_obj;
    
    // Data
    
    t_weighted_data data;
    t_duration_data *durations;
    
    t_atom_long num_dur;
    t_atom_long mean_mode;

    double last_weight_in;

    // Flags
    
    bool timed_mode;
    bool weights_mode;
    bool standard_var;
    
    // Cached Post Times
    
    long last_post_time_data;
    long last_post_time_mean;
    
    void *f_proxy;
    long f_inletNumber;
};

void *meandev_new(t_symbol *s, short argc, t_atom *argv);
void meandev_free(t_meandev *x);
void meandev_assist(t_meandev *x, void *b, long m, long a, char *s);

void meandev_bang(t_meandev  *x);
void meandev_int(t_meandev *x, t_atom_long data);
void meandev_float(t_meandev *x, double data);
void meandev_add_data(t_meandev *x, double data, double weight);

void meandev_timed_add_remove_data(t_meandev *x, t_atom_long dur_num);
void meandev_timed_remove_mean(t_meandev *x, t_duration_data *duration);

void meandev_set_clock_data(t_meandev *x, t_duration_data *duration, long time);
void meandev_set_clock_mean(t_meandev *x, t_duration_data *duration, long time);

void meandev_output(t_meandev *x, t_atom_long dur);
void meandev_output_mean_removed(t_meandev *x, t_duration_data *duration);

std::pair<double, double> meandev_new_mean(t_meandev *x, t_duration_data *duration, double mean);
std::pair<double, double> meandev_calc_stats(t_duration_data *duration);

// Main

int C74_EXPORT main()
{
    this_class = class_new("meandev",
                           (method)meandev_new,
                           (method)meandev_free,
                           sizeof(t_meandev),
                           NULL,
                           A_GIMME,
                           0);
    
    class_addmethod(this_class, (method)meandev_float, "float", A_FLOAT, 0);
    class_addmethod(this_class, (method)meandev_int, "int", A_LONG, 0);
    class_addmethod(this_class, (method)meandev_bang, "bang", 0);
    class_addmethod(this_class, (method)meandev_assist, "assist", A_CANT, 0);
    
    class_register(CLASS_BOX, this_class);

    return 0;
}

// Clock Tick Templates

template <int N>
void meandev_tick_data(t_meandev *x)
{
    meandev_timed_add_remove_data(x, N);
}

template <int N>
void meandev_tick_mean(t_meandev *x)
{
    meandev_timed_remove_mean(x, x->durations + N);
}

// New / Free / Assist

void *meandev_new(t_symbol *s, short argc, t_atom *argv)
{
    t_meandev *x = (t_meandev *) object_alloc(this_class);
    
    // Type-check arguments and put them all in t_atom_longs (args) for ease
    
    t_atom_long args[12];
    
    for (short i = 0; i < 12; i++)
        args[i] = (i < argc) ? atom_getlong(argv + i) : 0;
    
    // Range check arguments and initialise variables
    
    // Set Mode
        
    // mode 0 sets ranges in values (this is not timed but updates each time a new value is added)
    // mode 1 is by millisecond ranges
    // mode 2 is as mode 0 but with weighting of values
    // mode 3 is as mode 1 but with weihgting of values.
        
    switch (args[0])
    {
        case 0:
            x->timed_mode = false;
            x->weights_mode = false;
            break;
        case 1:
            x->timed_mode = true;
            x->weights_mode = false;
            break;
        case 2:
            x->timed_mode = false;
            x->weights_mode = true;
            break;
        case 3:
            x->timed_mode = true;
            x->weights_mode = true;
            break;
        
        default:
            x->timed_mode = false;
            x->weights_mode = false;
            object_post((t_object *) x, "mode out of range: setting to 0 (Last n values)");
            break;
    }
    
    // Set mean mode
    
    if (args[1] > 0 && args[1] < 3)
        x->mean_mode = args[1];
    else
        x->mean_mode = 0;
    
    // Take in the age ranges ensuring the correct ordering of max ages
    
    if (args[2] <= 1)
    {
        args[2] = 10;
        object_post((t_object *) x, "meandev: max age not given / out of range: setting to 10");
    }
    
    if (args[3] != 0 && (args[3] <= 1 || args[3] >= args[2]))
    {
        args[3] = 0;
        object_post((t_object *) x, "min age not given / out of range: setting to 0");
    }
        
    for (int i = 1; i < 5; i++)
    {
        if (args[(2 * i) + 2] > 1 && args[(2 * i) + 2] < args[(2 * i)])
            x->num_dur = i + 1;
        else
            break;
        if (args[(2 * i) + 3] <= 0 || args[(2 * i) + 3] >= args[(2 * i) + 1] || args[(2 * i) + 3] >= args[(2 * i) + 2])
            break;
    }
    
    // Standard variation mode
    
    x->standard_var = (args[(2 * x->num_dur) + 1]) ? false : true;
    
    // Allocate storage for duration ranges then store age ranges and outlets
        
    x->durations = new t_duration_data[x->num_dur];
        
    for (t_atom_long i = x->num_dur - 1; i >= 0; i--)
    {
        x->durations[i].f_out2 = floatout(x);
        x->durations[i].f_out1 = floatout(x);
        x->durations[i].max_age = args[(2 * i) + 2];
        x->durations[i].min_age = i == (x->num_dur - 1) ? 0 : args[(2 * i) + 3];
    }
    
    // Allocate clocks
    
    if (x->timed_mode)
    {
        // Note that a mean clock is not required for the shortest duration
        
        x->durations[0].f_data_clock = clock_new(x, (method) meandev_tick_data<0>);
        
        if (x->num_dur > 1)
        {
            x->durations[0].f_mean_clock = clock_new(x, (method) meandev_tick_mean<0>);
            x->durations[1].f_data_clock = clock_new(x, (method) meandev_tick_data<1>);
        }
        if (x->num_dur > 2)
        {
            x->durations[1].f_mean_clock = clock_new(x, (method) meandev_tick_mean<1>);
            x->durations[2].f_data_clock = clock_new(x, (method) meandev_tick_data<2>);
        }
        if (x->num_dur > 3)
        {
            x->durations[2].f_mean_clock = clock_new(x, (method) meandev_tick_mean<2>);
            x->durations[3].f_data_clock = clock_new(x, (method) meandev_tick_data<3>);
        }
        if (x->num_dur > 4)
        {
            x->durations[3].f_mean_clock = clock_new(x, (method) meandev_tick_mean<3>);
            x->durations[4].f_data_clock = clock_new(x, (method) meandev_tick_data<4>);
        }
    }
        
    long max_age = x->durations[0].max_age;
    
    // Allocate data
    
    create_object(x->data);
    
    if (x->timed_mode)
    {
        // Allow data of up to once per ms (and once every 5 ms for the means
        
        x->data.set_size(max_age, true, x->weights_mode);
        
        for (t_atom_long i = 0; i < x->num_dur; i++)
        {
            x->durations[i].core_data.set_size(max_age);
            x->durations[i].mean_data.set_size(x->durations[i].age_span() / 5, true);
        }
    }
    else
    {
        x->data.set_size(max_age, false, x->weights_mode);
        
        for (t_atom_long i = 0; i < x->num_dur - 1; i++)
        {
            x->durations[i].core_data.set_size(max_age);
            x->durations[i].mean_data.set_size(x->durations[i].age_span(), false);
        }
    }
    
    x->f_proxy = proxy_new(x, 1 ,&x->f_inletNumber);
    
    // Initialise data
    
    x->last_weight_in = 0;
        
    return x;
}

void meandev_free(t_meandev *x)
{
    destroy_object(x->data);
    
    freeobject((t_object *) x->f_proxy);
        
    delete[] x->durations;
}

void meandev_assist(t_meandev *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
    {
        if (a % 2)
        {
            if (x->standard_var)
                sprintf(s,"Stardard Deviation Out %ld", (a >> 1) + 1);
            else
                sprintf(s,"Variance Out %ld", (a >> 1) + 1);
        }
        else
        {
            sprintf(s,"Mean Out %ld", a >> 1);
        }
    }
    else
    {
        switch (a)
        {
            case 0:
                sprintf(s,"Data In");
                break;
            case 1:
                if (x->weights_mode)
                    sprintf(s,"Weights In / Reset");
                else
                    sprintf(s,"Reset: bang To Reset");
                break;
        }
    }
}

// User Methods

void meandev_bang(t_meandev *x)
{
      // Reset routine
    
    if (proxy_getinlet((t_object *) x) == 1)
    {                
        x->last_weight_in = 0.0;
        x->data.reset();
 
        for (t_atom_long i = x->num_dur - 1; i >=  0; i--)
        {
            x->durations[i].reset();
            outlet_float(x->durations[i].f_out2, 0.0);
            outlet_float(x->durations[i].f_out1, 0);
        }
    }
}

void meandev_int(t_meandev *x, t_atom_long data)
{
    meandev_float(x, (double) data);
}

void meandev_float(t_meandev *x, double data)
{
    // Accept weights and data in, storing weights and add data as appropriate

    if (proxy_getinlet((t_object *) x))
        x->last_weight_in = data;
    else
        meandev_add_data(x, data, x->weights_mode ? x->last_weight_in : 1.0);
}

void meandev_add_data(t_meandev *x, double data, double weight)
{
    // Cache some useful values
    
    t_atom_long num_dur = x->num_dur;
    t_atom_long first = num_dur - 1;
    long prev_oldest_idx = x->data.oldest_idx();
    long time = gettime();

    // Store data, age and weight and update the shortest duration

    bool overflow = x->data.add(data, time, weight);
    x->durations[first].core_data.add();
    
    // Check for overflow (also does largest band in "last n values" mode)

    if (overflow)
    {
        if (x->timed_mode && x->last_post_time_data < time - 1000)
            object_post((t_object *) x, "data overwrite: data does not stretch back full duration");
        
        x->last_post_time_data = time;
                
        // Update each duration range in case the data needs to be removed
            
        for (long i = 0; i < num_dur; i++)
        {
            if (x->durations[i].core_data.oldest_idx() == prev_oldest_idx)
                x->durations[i].core_data.remove();
        }
    }
                
    // Operate according to the mode
    
    if (x->timed_mode)
    {
        // In the timed modes find time the next piece of data should be added/removed in each duration range

        for (t_atom_long i = 0; i < num_dur; i++)
            meandev_set_clock_data(x, x->durations + i, time);
        
        // Always output the shortest duration and if data has overflowed then output all durations
 
        for (t_atom_long i = first; i >= (overflow ? 0 : first) ; i--)
            meandev_output(x, i);
    }
    else
    {
        // In the non-timed mode read in new data if each duration range is ready

        for (t_atom_long i = 0; i < first; i++)
        {
            if (x->data.count() >= x->durations[i].min_age)
                x->durations[i].core_data.add();
        }
        
        // Always output the shortest duration and if ready output others
        // FIX - check what happens with the shortest!!
        
        for (t_atom_long i = first; i >= 0; i--)
        {
            if (x->data.count() >= x->durations[i].min_age)
                meandev_output(x, i);
        }
    }
}

// Timed Routines

void meandev_timed_add_remove_data(t_meandev *x, t_atom_long dur_num)
{
    t_duration_data *duration = x->durations + dur_num;
    long time = gettime();
    bool change = false;
    
    if (duration->add_remove)
    {
        // Remove data (first check that something is due to happen (not sure this should be needed))

        if (time - x->data.ages()[duration->core_data.oldest_idx()] > duration->max_age)
        {
            change = true;
            
            // Remove and then check for discarding the data entirely
            
            duration->core_data.remove();
            if (!dur_num)
                x->data.remove();
        }
    }
    else
    {
        // Add routine (first check that something is due to happen)
        
        if ((time - x->data.ages()[duration->core_data.next_idx()]) > (duration->min_age))
        {
            change = true;
            duration->core_data.add();
        }
    }
    
    // If there has been a change then output

    if (change)
        meandev_output(x, dur_num);
    
    meandev_set_clock_data(x, duration, time);
}

void meandev_timed_remove_mean(t_meandev *x, t_duration_data *duration)
{
    long time = gettime();
        
    if ((time - duration->mean_data.oldest_age()) > duration->age_span())
    {
        duration->mean_data.remove();
        meandev_output_mean_removed(x, duration);
    }
    
    meandev_set_clock_mean(x, duration, time);
}

// Clock Setting Routines

void meandev_set_clock_data(t_meandev *x, t_duration_data *duration, long time)
{
    // Find the next timed action to perform

    long del_time = (duration->max_age) - (time - x->data.ages()[duration->core_data.oldest_idx()]) + 1;
    long add_time = (duration->min_age) - (time - x->data.ages()[duration->core_data.next_idx()]) + 1;

    // Unset clock (there may be no action to perform)

    clock_unset(duration->f_data_clock);
    
    if ((add_time <= del_time || !duration->core_data.count()) && !x->data.is_newest(duration->core_data.postion_idx()) && !x->is_first(duration))
    {
        // Next action is to add data

        duration->add_remove = false;
        clock_delay(duration->f_data_clock, std::max(0L, add_time));
    }
    else
    {
        if (duration->core_data.count())
        {
            // Next action is to remove data

            duration->add_remove = true;
            clock_delay(duration->f_data_clock, std::max(0L, del_time));
        }
    }
}

void meandev_set_clock_mean(t_meandev *x, t_duration_data *duration, long time)
{
    long min_time = (duration->age_span()) - (time - duration->mean_data.oldest_age()) + 1;
    
    clock_unset(duration->f_mean_clock);
    if (duration->mean_data.count())
        clock_delay(duration->f_mean_clock, min_time);
}

// Output Routines

template <typename mean_calc, typename var_calc>
void meandev_output_type(t_meandev *x, t_duration_data *duration)
{
    double mean = 0.0;
    double variance = 0.0;

    if (duration->core_data.count())
    {
        mean = duration->core_data.sum_data(mean_calc(x->data));
        
        if (x->mean_mode && !x->is_first(duration))
        {
            auto result = meandev_new_mean(x, duration, mean);
                
            mean = (x->mean_mode == 2) ? result.first : mean;
            variance = result.second;
        }
        else
            variance = duration->core_data.sum_data(var_calc(x->data, mean));
    }
    
    variance = x->standard_var ? sqrt(variance) : variance;
        
    outlet_float(duration->f_out2, variance);
    outlet_float(duration->f_out1, mean);
}

void meandev_output(t_meandev *x, t_atom_long dur)
{
    if (x->weights_mode)
        meandev_output_type<weighted_data_functor, variance_weighted_functor>(x, x->durations + dur);
    else
        meandev_output_type<data_functor, variance_functor>(x, x->durations + dur);
}

void meandev_output_mean_removed(t_meandev *x, t_duration_data *duration)
{
    auto result = meandev_calc_stats(duration);
    
    const double mean = result.first;
    const double variance = x->standard_var ? sqrt(result.second) : result.second;

    if (duration->core_data.count())
        outlet_float(duration->f_out2, variance);
    if (duration->core_data.count() && x->mean_mode == 2)
        outlet_float(duration->f_out1, mean);
}

// Calculation Routines

std::pair<double, double> meandev_calc_stats(t_duration_data *duration)
{
    if (duration->mean_data.count())
    {
        // Find mean and then variance
    
        double mean     = duration->mean_data.sum_data(data_functor(duration->mean_data));
        double variance = duration->mean_data.sum_data(variance_functor(duration->mean_data, mean));
        
        return { mean, variance };
    }
        
    return { 0.0, 0.0 };
}

std::pair<double, double> meandev_new_mean(t_meandev *x, t_duration_data *duration, double mean)
{
    long time = gettime();
    
    if (duration->mean_data.add(mean, time))
    {
        if (x->timed_mode && x->last_post_time_mean < time - 1000)
            object_post((t_object *) x, "mean overwrite: data does not stretch back full duration");
           
        x->last_post_time_mean = time;
    }
        
    // Find the minimium times until the next mean should be removed
    
    if (x->timed_mode)
        meandev_set_clock_mean(x, duration, time);
    
    return meandev_calc_stats(duration);
}
