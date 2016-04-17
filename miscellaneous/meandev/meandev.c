
/*
 *  meandev
 *
 *	meandev is an object for calculating the mean and variance or standard deviation of a set of values over various ranges in the data's history.
 *	This makes it possible to compare these values for different ranges in time and make some assesment of how the data is changing.
 *	This object is an adapted version of the version submitted as part of my MPhil thesis at the University of Birmingham (this one works in milliseconds).
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <ext.h>
#include <ext_obex.h>


void *this_class;


typedef struct {
    
    float *the_means;
    long *mean_ages;
    
    float last_mean;
	
    long min_age;
	long max_age;
	long oldest_data;
	long next_data_to_add;
	long age_span;
	long total_count;
	
	long mean_data_size;
	long oldest_mean;
	long newest_mean;
	long mean_count;
 
	char add_remove;
	char add_valid;
	char buffer_full;
	char buffer_ready;
    
    void *f_out;
    void *f_out2;
    void *f_clock;
    void *f_clock2;
    
} t_dur_data;


typedef struct _meandev
{
    t_object a_obj;
    
    float *the_data;
    long *the_ages;
    float *the_weights;
    
    t_dur_data *duration_pointer;
    
    double last_weight_in;
	
    long newest_data;
	long oldest_data;
	long data_size;
	
    char num_dur;
	char mode;
	char weights_mode;
	char log_vals_in;
	char standard_var;
	char mean_mode;
	
    long last_post_time;
	long last_mean_post_time;
   
	char mem_alloc;
	
    void *f_proxy;
    long f_inletNumber;

    
} t_meandev;


void *meandev_new(t_symbol *s, short argc, t_atom *argv);
void meandev_free(t_meandev *x);
void meandev_assist(t_meandev *x, void *b, long m, long a, char *s);

void meandev_bang (t_meandev  *x);
void meandev_int(t_meandev *x, long data);
void meandev_float(t_meandev *x, double data);
void meandev_add_data(t_meandev *x, double data, float weight);

void meandev_tick0(t_meandev *x);
void meandev_tick1(t_meandev *x);
void meandev_tick2(t_meandev *x);
void meandev_tick3(t_meandev *x);
void meandev_tick4(t_meandev *x);

void meandev_tick0s(t_meandev *x);
void meandev_tick1s(t_meandev *x);
void meandev_tick2s(t_meandev *x);
void meandev_tick3s(t_meandev *x);

void meandev_timed_add_remove(t_meandev *x, char dur_num, t_dur_data *duration_pointer, char add_remove);
void meandev_remove_mean(t_meandev *x, char dur_num, t_dur_data *duration_pointer);

void meandev_output(t_meandev *x, char dur);
void meandev_output_weights_mode(t_meandev *x, char dur);
void meandev_output_mean_removed (t_meandev *x, char dur);
void meandev_new_mean(t_meandev *x, double mean, char dur, t_dur_data *duration_pointer);
double meandev_calc_variance (t_dur_data *duration_pointer);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// basic object routines (main / new / free / assist) ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int C74_EXPORT main(void)
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


void *meandev_new(t_symbol *s, short argc, t_atom *argv)
{
    t_dur_data *duration_pointers[5];
	long max_age;
	char num_dur = 1;
	
    t_meandev *x = (t_meandev *)object_alloc(this_class);
    
    // Type-check arguments and put them all in longs (args) for ease
    
    long args[12];
    short i;
    
    for (i = 0; i < 12; i++)
    {
        if (i < argc)
        {
            switch (atom_gettype(argv + i))
            {
                case A_LONG:
                    args[i] = atom_getlong(argv + i);
					break;
                case A_SYM:
					args[i] = 0;
					break;
                case A_FLOAT:
					args[i] = (long) atom_getfloat(argv + i);
            }
        }
        else
        {
            args[i] = 0;
        }
    }
	
    // Range check arguments and initialise variables
    
	x->mode = 0;
	x->weights_mode = 0;
    
    if (args[0] < 0 || args[0] > 5)
        post ("meandev: mode not given / out of range - set to 0 (Last n values)");
    else
    {
		// Set Mode
		
		// mode 0 sets ranges in values (this is not timed but updates each time a new value is added)
		// mode 1 is by millisecond ranges
		// mode 2 is as mode 0 but with weighting of values
		// mode 3 is as mode 1 but with weihgting of values.
		
        switch (args[0])
        {
			case 1:
                x->mode = 1;
                x->weights_mode = 0;
				break;
            case 2:
                x->mode = 0;
                x->weights_mode = 1;
				break;	
			case 3:
                x->mode = 1;
                x->weights_mode = 1;
				break;	
        }
    }
    
	// Set mean mode
	
    if (args[1] > 0 && args[1] < 3)
        x->mean_mode = args[1];
    else
        x->mean_mode = 0;
	
	// Take in the age ranges
	
    if (args[2] <= 1)
    {
        args[2] = 10;
        post ("meandev: max age not given / out of range - set to 10");
    }
    
    if (args[3] != 0 && (args[3] <= 1 || args[3] >= args[2]))
    {
        args[3] = 0;
        post ("meandev: min age not given / out of range - set to 0");
    }
	    
    for (i = 1; i < 5; i++)
    {
        if (args[(2 * i) + 2] > 1 && args[(2 * i) + 2] < args[(2 * i)])
            num_dur = i + 1;
        else
            break;
        if (args[(2 * i) + 3] <= 0 || args[(2 * i) + 3] >= args[(2 * i) + 1] || args[(2 * i) + 3] >= args[(2 * i) + 2])
            break;
    }
    
	// Standard variation mode
	
    if (args[(2 * num_dur) + 1])
        x->standard_var = 0;
    else
        x->standard_var = 1;
	
	// Allocate memory for duration ranges
	// Assume malloc calls are successful and set this to 0 if this turns out not to be the case........
	
    x->mem_alloc = 1;				
	x->num_dur = num_dur;
    
    x->duration_pointer = (t_dur_data *)malloc((unsigned short)(num_dur * sizeof(t_dur_data)));
	
	if (!x->duration_pointer)
	{
		x->mem_alloc = 0;
		x->the_data = 0;
		x->the_ages = 0;
		x->the_weights = 0;
		post ("meandev: cannot allocate memory for meandev - bailing Out........");
		goto out;
	}
    	
	// Store age ranges
	
    for (i = num_dur - 1; i >= 0; i--)
    {	
        duration_pointers[i] = x->duration_pointer + i;
        duration_pointers[i]->f_out2 = floatout(x);
        duration_pointers[i]->f_out = floatout(x); 
        duration_pointers[i]->max_age = args[(2 * i) + 2];
        if (i == num_dur - 1)
            duration_pointers[i]->min_age = 0;
        else
            duration_pointers[i]->min_age = args[(2 * i) + 3];		
        duration_pointers[i]->age_span = duration_pointers[i]->max_age - duration_pointers[i]->min_age;
    }
    
	// Allocate clocks
	
    if (x->mode)
    {
        duration_pointers[0]->f_clock = clock_new(x, (method)meandev_tick0);
        if (num_dur > 1)
        {
            duration_pointers[0]->f_clock2 = clock_new(x, (method)meandev_tick0s);
            duration_pointers[1]->f_clock = clock_new(x, (method)meandev_tick1);
            duration_pointers[1]->f_clock2 = clock_new(x, (method)meandev_tick1s);
        }
        if (num_dur > 2)
        {
            duration_pointers[2]->f_clock = clock_new(x, (method)meandev_tick2);
            duration_pointers[2]->f_clock2 = clock_new(x, (method)meandev_tick2s);
        }
        if (num_dur > 3)
        {
            duration_pointers[3]->f_clock = clock_new(x, (method)meandev_tick3);
            duration_pointers[3]->f_clock2 = clock_new(x, (method)meandev_tick3s);
        }
        if (num_dur > 4)
            duration_pointers[4]->f_clock = clock_new(x, (method)meandev_tick4);	// Shouldn't ever need mean clock (clock2) for shortest.
    }
    
    max_age = duration_pointers[0]->max_age;
    
	// Allocate arrays for data and each duration range
	
    switch (x->mode)
    {
        case 0:
            x->the_data = (float *) malloc (max_age * sizeof(float));
            x->data_size = max_age;
			if (!x->the_data)
				x->mem_alloc = 0;
            for (i = 0; i < num_dur - 1; i++)
            {
                duration_pointers[i]->the_means = (float *) malloc (duration_pointers[i]->age_span * sizeof(float));
                duration_pointers[i]->mean_data_size = duration_pointers[i]->age_span;
				if (!duration_pointers[i]->the_means)
					x->mem_alloc = 0;
            }
			break;
        case 1:
            x->the_data = (float *) malloc (max_age * sizeof(float));			// Allow data of up to once per ms
            x->the_ages = (long *) malloc (max_age * sizeof(long));
            x->data_size = max_age;
			if (!x->the_data || !x->the_ages)
				x->mem_alloc = 0;
            for (i = 0; i < num_dur - 1; i++)
            {
                duration_pointers[i]->the_means = (float *) malloc (duration_pointers[i]->age_span / 5 * sizeof(float));
                duration_pointers[i]->mean_ages = (long *) malloc (duration_pointers[i]->age_span / 5 * sizeof(long));
                duration_pointers[i]->mean_data_size = duration_pointers[i]->age_span / 5;
				if (!duration_pointers[i]->the_means || !duration_pointers[i]->mean_ages)
					x->mem_alloc = 0;
            }
			break;
    }
    
	// Allocate memory for weightings
	
    if (x->weights_mode == 1)
	{
        x->the_weights = (float *) malloc (max_age * sizeof(float));
		if (!x->the_weights) 
			x->mem_alloc = 0;
	}
	
	// Memory check
	
	if (!x->mem_alloc)
	{
		post ("meandev: could not allocate memory For meandev - bailing out....");
		goto out;
	}
	
    x->f_proxy = proxy_new(x, 1 ,&x->f_inletNumber);
    
    // Initialise data
    
    x->newest_data = 0;
    x->oldest_data = 0;
    x->last_weight_in = 0;
    
    for (i = 0; i < x->num_dur; i++)
    {
        duration_pointers[i]->add_remove = 1;
        duration_pointers[i]->add_valid = 0;
        duration_pointers[i]->buffer_full = 0;
        duration_pointers[i]->buffer_ready = 0;
        duration_pointers[i]->next_data_to_add = 0;
        duration_pointers[i]->oldest_data = 0;
        duration_pointers[i]->total_count = 0;
        
        duration_pointers[i]->oldest_mean = 0;
        duration_pointers[i]->newest_mean = 0;
        duration_pointers[i]->mean_count = 0;
    }
	
out:
	
    return (x);
}


void meandev_free(t_meandev *x)
{
	t_dur_data *duration_pointer;
    char num_dur = x->num_dur;
	long i;

	free(x->the_data);
    freeobject((t_object *) x->f_proxy);
	
	// Free basic data
	
    if (x->mode == 1)
    {
        if (x->the_ages) 
			free(x->the_ages);
        if (x->weights_mode == 1)
		{
			if (x->the_weights) 
				free(x->the_weights);
		}
        duration_pointer = x->duration_pointer + (num_dur - 1);
        freeobject((t_object *) duration_pointer->f_clock);
    }
        
	// Free duration data
	
	for (i = 0; i < num_dur - 1; i++)
    {
        duration_pointer = x->duration_pointer + i;
        if (duration_pointer->the_means) free (duration_pointer->the_means);
        
        if (x->mode == 1)
        {
            if (duration_pointer->mean_ages) free (duration_pointer->mean_ages);
            freeobject((t_object *) duration_pointer->f_clock);
            freeobject((t_object *) duration_pointer->f_clock2);
        }
    }
    
	if (x->duration_pointer) 
		free(x->duration_pointer);
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////// User routines /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void meandev_bang (t_meandev  *x)		
{
	  short i;
	  
	  // Reset routine
	
    if (proxy_getinlet((t_object *) x) == 1 && x->mem_alloc)
    {
        // Reset data
        
        t_dur_data *duration_pointer;
        
        x->newest_data = 0;
        x->oldest_data = 0;
        x->last_weight_in = 0;

        for (i = x->num_dur - 1; i >=  0; i--)
        {
            duration_pointer = x->duration_pointer + i;
            if (x->mode == 1)
			{
                clock_unset(duration_pointer->f_clock);
				if (i != x->num_dur -1)
					clock_unset(duration_pointer->f_clock2);
			}
            duration_pointer->add_remove = 1;
            duration_pointer->add_valid = 0;
            duration_pointer->buffer_full = 0;
            duration_pointer->buffer_ready = 0;
            duration_pointer->next_data_to_add = 0;
            duration_pointer->oldest_data = 0;
            duration_pointer->total_count = 0;
            
            duration_pointer->oldest_mean = 0;
            duration_pointer->newest_mean = 0;
            duration_pointer->mean_count = 0;
            outlet_float(duration_pointer->f_out2, 0);
			outlet_float(duration_pointer->f_out, 0);
        }
    }
}


void meandev_int(t_meandev *x, long data)		
{
	// Gives int compatibility
	
    float datafloat = (float) data;	
    meandev_float(x, datafloat);
}


void meandev_float(t_meandev *x, double data)
{	
	char weights_mode = x->weights_mode;
	
    long got_inlet = proxy_getinlet((t_object *) x);

	// Accept weights and data in, storing weights and add data as appropriate

	if (x->mem_alloc)
	{
		if (weights_mode)
		{
			if (got_inlet)
				x->last_weight_in = data;
			else 
				meandev_add_data (x, data, x->last_weight_in);
		}
		
		if (got_inlet == 0 && weights_mode == 0)
			meandev_add_data(x, data, 1);
	}
}


void meandev_add_data(t_meandev *x, double data, float weight)
{
    t_dur_data *duration_pointers[5];
	
	char mode = x->mode;
    char weights_mode = x->weights_mode;
    char num_dur = x->num_dur;
    char overwrite = 0;
	
    long time = gettime();
    long data_size = x->data_size;
    long oldest_data = x->oldest_data;
	long prev_oldest_data;
	long next_data_to_add;
	long next_space;
	long min_time;
	long test_time;
	long short_dur = num_dur - 1;
	long any_data = 0;
	long i;
	
	// Set pointers to duration ranges and check if there is any data currently stored

    for (i = 0; i < num_dur; i++)
    {
        duration_pointers[i] = x->duration_pointer + i;
        any_data += duration_pointers[i]->total_count;
        duration_pointers[i]->add_valid = 1;
    }
    
	// If there's no data put the new data at the beginning

    if (any_data == 0)					
    {			
		// Data

		x->the_data[0] = (float) data;
        
		// Age
		
		if (mode)
            x->the_ages[0] = time;
        
		// Weight
		
		if (weights_mode)
            x->the_weights[0] = weight;
          
		// Update variables
		
        duration_pointers[short_dur]->total_count++;
        duration_pointers[short_dur]->next_data_to_add = 1;
        duration_pointers[short_dur]->oldest_data = 0;

        for (i = 0; i < short_dur; i++)
            duration_pointers[i]-> next_data_to_add = 0;
            
        x->newest_data = 0;
        x->oldest_data = 0;
	}
    else									
    {
		// Else write to the next space - (check for data wrap/overwrite first)

		next_space = x->newest_data + 1;
		
		// Check for data wraparound

        if (next_space >= data_size)				
            next_space = 0;
            
		// Check for data overwrite - also does largest band in "last n values" mode

        if (next_space == oldest_data)	
        {
            overwrite = 1;
			
            if (x->mode == 1 && x->last_post_time < time - 1000 )
                post("meandev: data overwrite - data does not stretch back full duration");
            x->last_post_time = time;
        
			prev_oldest_data = oldest_data; 
            if (++oldest_data >= data_size)		
                oldest_data = 0;
            x->oldest_data = oldest_data;
            
			// Update each duration range
			
            for (i = 0; i < num_dur; i++)
            {
                if (duration_pointers[i]->oldest_data == prev_oldest_data && duration_pointers[i]->total_count != 0)
                {
                    duration_pointers[i]->total_count--;
                    duration_pointers[i]->oldest_data = oldest_data;
                }
                
                if (duration_pointers[i]->next_data_to_add == prev_oldest_data && prev_oldest_data != duration_pointers[i]->oldest_data)		
                    duration_pointers[i]->next_data_to_add = oldest_data;
            }
        }
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        
		// Read in new data

		// Data
        
		x->the_data[next_space] = (float) data;
        
		//Age
		
		if (mode)
            x->the_ages[next_space] = time;
        
		// Weight
		
		if (weights_mode)
            x->the_weights[next_space] = weight;
		
		// Update variables

        x->newest_data = next_space;
        
        if (duration_pointers[short_dur]->total_count == 0)
            duration_pointers[short_dur]->oldest_data = next_space;
		duration_pointers[short_dur]->total_count++;
		
        if (++next_space >= data_size)
            next_space = 0;
        duration_pointers[short_dur]->next_data_to_add = next_space;
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
                
    if (mode)	
    {
		// In any of the timed modes find the minimium times until the next piece of data should be added/removed in each duration range

        for (i = 0; i < num_dur; i++)
        {
            min_time = (duration_pointers[i]->max_age) - (time - x->the_ages[duration_pointers[i]->oldest_data]) + 1;
            test_time = (duration_pointers[i]->min_age) - (time - x->the_ages[duration_pointers[i]->next_data_to_add]) + 1;

            if ((test_time <= min_time || duration_pointers[i]->total_count == 0) && duration_pointers[i]->add_valid && i != short_dur)
            {
				// Next action for this duration range is to add data

            	if (test_time < 0) test_time = 0;
                duration_pointers[i]->add_remove = 0;
                clock_unset(duration_pointers[i]->f_clock);
                clock_delay(duration_pointers[i]->f_clock, test_time);
            }
            else
            {
				// Unset clock

                clock_unset(duration_pointers[i]->f_clock);
				
                if (duration_pointers[i]->total_count != 0)
                {
					// Next action for this duration range is to remove data

             		if (min_time < 0) min_time = 0; 
                    duration_pointers[i]->add_remove = 1;
                    clock_delay(duration_pointers[i]->f_clock, min_time);
                }
            }
        }
		
		// Output
        
        if (weights_mode)
            meandev_output_weights_mode(x,short_dur);
        else
            meandev_output(x,short_dur);
        
		// If data has been overwritten then any of the duration ranges may now be incorrect so output for all ranges

        if (overwrite)
        {
            if (weights_mode)
            {
                for (i = short_dur - 1; i >=  0; i--)
                    meandev_output_weights_mode(x, i);
            }
            else
            {
                for (i = short_dur - 1; i >=  0; i--)
                    meandev_output(x, i);
            }
        }
    }
    else
    {
		// In the non-timed mode

        for (i = 0; i < short_dur; i++)
        {
            if (duration_pointers[i]->buffer_ready)
            {
				// Read in new data (if we have reached the minimum number of pieces of data for this range) and update variables

                next_data_to_add = duration_pointers[i]->next_data_to_add;		
																
                if (duration_pointers[i]->total_count == 0)
                    duration_pointers[i]->oldest_data = next_data_to_add;
                
				duration_pointers[i]->total_count++;
				
                if (++next_data_to_add >= data_size)
                    next_data_to_add = 0;
                duration_pointers[i]->next_data_to_add = next_data_to_add;
            }
        }    

        for (i = 1; i < num_dur; i++)
        {
            if (duration_pointers[i]->buffer_full)
            {
				// If the buffer full then update variables (keeping count constant)

                oldest_data = duration_pointers[i]->oldest_data;
                duration_pointers[i]->total_count--;
                                
				// Increment oldest_data checking for wraparound	
				
                if (++oldest_data >= data_size)		
                    oldest_data = 0;
                duration_pointers[i]->oldest_data = oldest_data;
            }
            else
            {
				// If the buffer is not full already check whether it has just become full

                if (x->newest_data == (duration_pointers[i]->max_age - 1))
                    duration_pointers[i]->buffer_full = 1;
            }
        }
        
		// Output the first range

		meandev_output(x, short_dur);
		
		// Output the other ranges if relevant and check if they are ready to be written to

        for (i = short_dur - 1; i >=  0; i--)
		{
			if (duration_pointers[i]->buffer_ready)
            meandev_output(x, i);
			if (x->newest_data == (duration_pointers[i]->min_age - 1))
				duration_pointers[i]->buffer_ready = 1;
		}
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////// Tick routines /////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void meandev_tick0(t_meandev *x)
{
    t_dur_data *duration_pointer = x->duration_pointer;
    meandev_timed_add_remove (x, (char) 0, duration_pointer, duration_pointer->add_remove);		
}


void meandev_tick1(t_meandev *x)
{
    t_dur_data *duration_pointer = x->duration_pointer + 1;
    meandev_timed_add_remove (x, (char) 1, duration_pointer, duration_pointer->add_remove);
}


void meandev_tick2(t_meandev *x)
{
    t_dur_data *duration_pointer = x->duration_pointer + 2;
    meandev_timed_add_remove (x, (char) 2, duration_pointer, duration_pointer->add_remove);
}


void meandev_tick3(t_meandev *x)
{
    t_dur_data *duration_pointer = x->duration_pointer + 3;
    meandev_timed_add_remove (x, (char) 3, duration_pointer, duration_pointer->add_remove);
}


void meandev_tick4(t_meandev *x)
{
    t_dur_data *duration_pointer = x->duration_pointer + 4;
    meandev_timed_add_remove (x, (char) 4, duration_pointer, duration_pointer->add_remove);					
}


void meandev_tick0s(t_meandev *x)
{
    t_dur_data *duration_pointer = x->duration_pointer;
    meandev_remove_mean (x, (char) 0, duration_pointer);		
}


void meandev_tick1s(t_meandev *x)
{
    t_dur_data *duration_pointer = x->duration_pointer + 1;
    meandev_remove_mean (x, (char) 1, duration_pointer);
}


void meandev_tick2s(t_meandev *x)
{
    t_dur_data *duration_pointer = x->duration_pointer + 2;
    meandev_remove_mean (x, (char) 2, duration_pointer);
}


void meandev_tick3s(t_meandev *x)
{
    t_dur_data *duration_pointer = x->duration_pointer + 3;
    meandev_remove_mean (x, (char) 3, duration_pointer);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////// Timed routines ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void meandev_timed_add_remove(t_meandev *x, char dur_num, t_dur_data *duration_pointer, char add_remove)
{
    long time = gettime();
    long min_time;
	long test_time;
	long next_data_to_add;
    long oldest_data;
	long data_size = x->data_size;
	
	char num_dur = x->num_dur;
    char change = 0;
    
    if (add_remove)
    {
		// Remove data
		
		oldest_data = duration_pointer->oldest_data;			
        
		// Check that something is due to happen

		if ((time - x->the_ages[oldest_data]) > (duration_pointer->max_age)) 		      
        {   										
            change = 1;
            duration_pointer->total_count--;	
            
			// Check for discarding the last piece of data
			
            if (duration_pointer->total_count != 0)
            {
				// Increment oldest data checking for wraparound
				
                if (++oldest_data >= data_size)
                    oldest_data = 0;
                duration_pointer->oldest_data = oldest_data;
				
				// This is the oldest piece of valid data 

                if (dur_num == 0)
                    x->oldest_data = oldest_data;
            }
        }
    }
    else
    {
		// Add routine
		
		next_data_to_add = duration_pointer->next_data_to_add;
		
		// Check that something is due to happen

        if ((time - x->the_ages[next_data_to_add]) > (duration_pointer->min_age))		
        {
            change = 1;
            
			// Read in new data
			
            if (duration_pointer->total_count == 0)				
                duration_pointer->oldest_data = next_data_to_add;
			
			duration_pointer->total_count ++;	
			
			// There is no newer data to add

            if (next_data_to_add == x->newest_data)
                duration_pointer->add_valid = 0;
			
			// Update pointer to next data

            if (++next_data_to_add >= data_size)
                next_data_to_add = 0;
            duration_pointer->next_data_to_add = next_data_to_add;
        }
    }
	
	// If there has been a change then output

    if (change)
    {
        if (x->weights_mode)
            meandev_output_weights_mode(x, dur_num);
        else
            meandev_output(x, dur_num);
    }
	
	// Find the next timed action to perform

    min_time = (duration_pointer->max_age) - (time - x->the_ages[duration_pointer->oldest_data]) + 1;
	test_time = (duration_pointer->min_age) - (time - x->the_ages[duration_pointer->next_data_to_add]) + 1;

    if ((test_time <= min_time || duration_pointer->total_count == 0) && duration_pointer->add_valid && dur_num != (num_dur - 1))
    {
		// Next action is to add data

		if (test_time < 0) 
			test_time = 0; 
		
        duration_pointer->add_remove = 0;
        clock_unset(duration_pointer->f_clock);
        clock_delay(duration_pointer->f_clock, test_time);
    }
    else
    {
		// Unset clock (there may be no action to perform)

		clock_unset(duration_pointer->f_clock);
		
        if (duration_pointer->total_count != 0)
        {
			// Next action is to remove data

        	if (min_time < 0) min_time = 0; 
            duration_pointer->add_remove = 1;
            clock_delay(duration_pointer->f_clock, min_time);
        }
    }
}


void meandev_remove_mean(t_meandev *x, char dur_num, t_dur_data *duration_pointer)
{
    long time = gettime();
	long min_time;
    long mean_data_size = duration_pointer->mean_data_size;
	long oldest_mean;
    
	oldest_mean = duration_pointer->oldest_mean;					
	
    if ((time - duration_pointer->mean_ages[oldest_mean]) > (duration_pointer->age_span)) 		      
    {   										
        duration_pointer->mean_count--;	
        
		// Increment oldest data checking for wraparound
		
		if (++oldest_mean >= mean_data_size)					
			oldest_mean = 0;            
		duration_pointer->oldest_mean = oldest_mean;
        
        meandev_output_mean_removed(x, dur_num);
    }
    
	min_time = (duration_pointer->age_span) - (time - duration_pointer->mean_ages[duration_pointer->oldest_mean]) + 1;
    
    clock_unset(duration_pointer->f_clock2);
    if (duration_pointer->mean_count != 0)
        clock_delay(duration_pointer->f_clock2, min_time);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////// Output routines ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void meandev_output (t_meandev *x, char dur)
{
    t_dur_data *duration_pointer = x->duration_pointer + dur;
    
    float *the_data = x->the_data;
	
	char short_durtest = 0;
    
    long data_size = x->data_size;
    long oldest_data = duration_pointer->oldest_data;
    long newest_data =  duration_pointer->next_data_to_add;
    long i;
    
	double total_count = duration_pointer->total_count;
    double sum = 0;
    double mean = 0;
    double variance = 0;
	double difference;
	
	if (dur == x->num_dur - 1)
        short_durtest = 1;
	
    if (total_count != 0)
    {
        // First Find Mean
        
        if (--newest_data < 0)
            newest_data = data_size - 1;
		
		// Sum data from newest to oldest data
		
        for (i = newest_data; i >= 0 && i >= oldest_data; i--)
            sum += the_data[i];
		
		// If the data wraps - nothing has been done yet
		
        if (newest_data < oldest_data)									
        {
			// Sum data from newest to 0
			
            for (i = newest_data; i >= 0; i--)
                sum += the_data[i];
			
			// Sum data from end of buffer to oldest data
			
            for (i = data_size - 1; i >= 0 && i >= oldest_data; i--)
                sum += the_data[i];
        }
        mean = sum / total_count;
    }
        
    if ((x->mean_mode == 0 || short_durtest) && total_count != 0)
    {
        // Now Find Variance
        
		// Sum data from newest to oldest data
		
        for (i = newest_data; i >= 0 && i >= oldest_data; i--)
        {
            difference = the_data[i] - mean;
            difference = difference * difference;
            variance += difference;
        }
		
		// If the data wraps - nothing has been done yet
		
        if (newest_data < oldest_data)
        {
			// Sum data from newest to 0
			
            for (i = newest_data; i >= 0; i--)
            {
                difference = the_data[i] - mean;
                difference = difference * difference;
                variance += difference;
            }
			
			// Sum data from end of buffer to oldest data
			
            for (i = data_size - 1; i >= 0 && i >= oldest_data; i--)
            {
                difference = the_data[i] - mean;
                difference = difference * difference;
                variance += difference;
            }
        }
        variance = variance / total_count;
    }
    else
    {
        if (x->mean_mode && duration_pointer->total_count != 0)				
		{
            meandev_new_mean (x, mean, dur, duration_pointer);
            variance = meandev_calc_variance(duration_pointer);
		}
        if (x->mean_mode == 2 && duration_pointer->total_count != 0)
            mean = duration_pointer->last_mean;
    }
    
    if(x->standard_var && variance != 0)
        variance = sqrt(variance);
        
    outlet_float(duration_pointer->f_out2, variance);
    outlet_float(duration_pointer->f_out, mean);
}


void meandev_output_weights_mode (t_meandev *x, char dur)
{
    t_dur_data *duration_pointer = x->duration_pointer + dur;
    
	float *the_data = x->the_data;
	    
    float *the_weights = x->the_weights;
    
	char short_durtest = 0;
        
    long data_size = x->data_size;
    long oldest_data = duration_pointer->oldest_data;
	long newest_data =  duration_pointer->next_data_to_add;
    long i;
        
	double total_count = duration_pointer->total_count;
	double sum = 0;
    double mean = 0;
    double variance = 0;
	double difference;

	if (dur == x->num_dur - 1)
        short_durtest = 1;
	
    if (total_count != 0)
    {
        // First Find Mean
        
        if (--newest_data < 0)
            newest_data = data_size - 1;
		
		// Sum data from newest to oldest data
		
        for (i = newest_data; i >= 0 && i >= oldest_data; i--)
        {
            sum += the_data[i] * the_weights[i];
            total_count += (the_weights[i] - 1);
        }
		
		// If the data wraps - nothing has been done yet
		
        if (newest_data < oldest_data)
        {
			// Sum data from newest to 0
			
            for (i = newest_data; i >= 0; i--)
            {
                sum += the_data[i] * the_weights[i];
                total_count += (the_weights[i] - 1);
            }
			
			// Sum data from end of buffer to oldest data
			
            for (i = data_size - 1; i >= 0 && i >= oldest_data; i--)
            {
                sum += the_data[i] * the_weights[i];
                total_count += (the_weights[i] - 1);
            }
        }
        mean = sum / total_count;
    }
    
    if ((x->mean_mode == 0 || short_durtest) && total_count != 0)
    {
        // Now Find Variance

		// Sum data from newest to oldest data
		
        for (i = newest_data; i >= 0 && i >= oldest_data; i--)
        {
            difference = the_data[i] - mean;
            difference = difference * difference;
            variance += (difference * the_weights[i]);
        }
		
		// If the data wraps - nothing has been done yet
		
        if (newest_data < oldest_data)
        {
			// Sum data from newest to 0
			
            for (i = newest_data; i >= 0; i--)							
            {
                difference = the_data[i] - mean;
                difference = difference * difference;
                variance += (difference * the_weights[i]);
            }
			
			// Sum data from end of buffer to oldest data
			
            for (i = data_size - 1; i >= 0 && i >= oldest_data; i--)		
            {
                difference = the_data[i] - mean;
                difference = difference * difference;
                variance += (difference * the_weights[i]);
            }
        }
        variance = variance / total_count;
    }
    else
    {
		if (x->mean_mode && duration_pointer->total_count != 0)				
		{
			meandev_new_mean (x, mean, dur, duration_pointer);
            variance = meandev_calc_variance (duration_pointer);
		}
        if (x->mean_mode == 2 && duration_pointer->total_count != 0)			
            mean = duration_pointer->last_mean;
    }
	
	// Sqrt if standard deviation is requested
    
    if(x->standard_var && variance != 0)
		variance = sqrt(variance);
        
    outlet_float(duration_pointer->f_out2, variance);
    outlet_float(duration_pointer->f_out, mean);
}


void meandev_output_mean_removed (t_meandev *x, char dur)
{
    t_dur_data *duration_pointer = x->duration_pointer + dur;
    
    double variance = meandev_calc_variance(duration_pointer);
	
    if(x->standard_var && variance != 0)
        variance = sqrt(variance);
	if (duration_pointer->total_count != 0)
		outlet_float(duration_pointer->f_out2, variance);
    if (x->mean_mode == 2 && duration_pointer->total_count != 0)
		outlet_float(duration_pointer->f_out, duration_pointer->last_mean);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////// Calculation routines /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


double meandev_calc_variance (t_dur_data *duration_pointer)
{
    float *the_means = duration_pointer->the_means;

    double mean_count = duration_pointer->mean_count;
    double sum = 0;
    double mean = 0;
    double variance = 0;
	double difference;
	
	long mean_data_size;
	long oldest_mean;
	long newest_mean;
	long i;

    if (mean_count != 0)
    {
        // First Find Mean
		
		mean_data_size = duration_pointer->mean_data_size;
        oldest_mean = duration_pointer->oldest_mean;
        newest_mean =  duration_pointer->newest_mean;
        
		// Sum data from newest to oldest data
		
        for (i = newest_mean; i >= 0 && i >= oldest_mean; i--)
            sum += the_means[i];
		
		// If the data wraps - nothing has been done yet
		
        if (newest_mean < oldest_mean)										
        {
			// Sum data from newest to 0
			
            for (i = newest_mean; i >= 0; i--)
                sum += the_means[i];
			
			// Sum data from end of buffer to oldest data
			
            for (i = mean_data_size - 1; i >= 0 && i >= oldest_mean; i--)
                sum += the_means[i];
        }
        
        mean = sum / mean_count;
       
        // Now Find Variance
		
		// Sum data from newest to oldest data
		
        for (i = newest_mean; i >= 0 && i >= oldest_mean; i--)				
        {
            difference = the_means[i] - mean;
            difference = difference * difference;
            variance += difference;
        }
		
		// If the data wraps - nothing has been done yet
		
        if (newest_mean < oldest_mean)										
        {
			// Sum data from newest to 0
			
            for (i = newest_mean; i >= 0; i--)								
            {
                difference = the_means[i] - mean;
                difference = difference * difference;
                variance += difference;
            }
			
			// Sum data from end of buffer to oldest data
			
            for (i = mean_data_size - 1; i >= 0 && i >= oldest_mean; i--)		
            {
                difference = the_means[i] - mean;
                difference = difference * difference;
                variance += difference;
            }
        }
        
        variance = variance / mean_count;
    }
    
    duration_pointer->last_mean = mean;
	
    return (variance);
}


void meandev_new_mean(t_meandev *x, double mean, char dur, t_dur_data *duration_pointer)
{
    long time = gettime();
    long mean_data_size = duration_pointer->mean_data_size;
    long oldest_mean = duration_pointer->oldest_mean;
	long next_space;
	
    char mode = x->mode;
    
	// If there's no data then put the new mean at the beginning
	
    if (duration_pointer->mean_count == 0)				
    {			
        duration_pointer->the_means[0] = (float) mean;
        if (mode)
            duration_pointer->mean_ages[0] = time;
            
        duration_pointer->mean_count++;
        duration_pointer->newest_mean = 0;
        duration_pointer->oldest_mean = 0;
    }
    else											
    {
		// Else put it in the next available space - check for data wrap / overwrite first)
		
		next_space = duration_pointer->newest_mean + 1;
		
		// Check for data wraparound
		
        if (next_space >= mean_data_size)				
            next_space = 0;
            
		// Check for data overwrite - also does "last n values" mode
		
        if (next_space == oldest_mean)				
        {
            if (x->mode == 1 && x->last_mean_post_time < time - 1000 )
                post("meandev: mean overwrite - data does not stretch back full duration");
           
			x->last_mean_post_time = time;
    
            if (++oldest_mean >= mean_data_size)		
                oldest_mean = 0;
            duration_pointer->oldest_mean = oldest_mean;
            duration_pointer->mean_count--;
            duration_pointer->oldest_mean = oldest_mean;
        }
        
		// Read in new mean
		
        duration_pointer->the_means[next_space] = mean;		
        
		if (mode)
            duration_pointer->mean_ages[next_space] = time;
        
		duration_pointer->newest_mean = next_space;
        duration_pointer->mean_count++;
    }
    
	// Find the minimium times until the next mean should be removed                
	
    if (mode)							
    {
        long min_time = (duration_pointer->age_span) - (time - duration_pointer->mean_ages[duration_pointer->oldest_mean]) + 1;
    
        clock_unset(duration_pointer->f_clock2);
		clock_delay(duration_pointer->f_clock2, min_time);
    }
}
