
/*
 *  timemap
 *
 *	Originally intended to create lists representing timing values as part of a system of gestural representation , timemap is an object that creates random lists of values with definable characteristics.
 *
 *	The basic principle to start with a list of equidistant values (between 0 and 1 - and subsequently scaled if desired) and through randomisation and warping reach the values in the final list.
 *	The intention is to allow the creation of lists with shapes and charateritics that are to some extent determinate, but randomised within the given parameters.
 *	The values can either be generated independently, or in "streams", where the generated values are accumulated through addition or multiplication.
 *	A number of scalings are supported, appropriate to a range of musical parameters.
 *
 *	The exact functioning of the timemap object is fairly involved and best understood in practice. See the MaxMSP documentation for further information on its use.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>

#include <AH_Random.h>

void *this_class;


typedef enum {kScaleNone, kScale, kScaleLog, kScaleExp, kScaleDiv} t_scale_mode;
typedef enum {kStreamNone, kStreamAdditive, kStreamMultiplicative} t_stream_mode;


typedef struct timemap {
	
    t_object a_obj;
    
    double rand_amount; 
	double centre; 
	double warp;
	double min_dist;
	double max_dist;
	double scale_val1;
	double scale_val2;
	double min_val;
	double max_val;
	double min_sbound;
	double max_sbound;
	double init_val;
	
	t_atom_long max_retries;
	
    t_scale_mode scale_mode;
    t_stream_mode stream_mode;
    
    bool random_order;
	
    t_rand_gen gen;
    
    void *thelistout;
    
} t_timemap;

void timemap_free(t_timemap *x);
void *timemap_new(double rand_amount, double centre, double warp);
void timemap_assist(t_timemap *x, void *b, long m, long a, char *s);

static __inline double timemap_value(t_rand_gen *gen, long i, double points_recip, double rand_amount, double centre, double centre_compliment, double warp);
void timemap_calculate(t_timemap *x, t_atom_long num_points);
double scale_val(double new_val, t_scale_mode scale_mode, double scale_val1, double scale_val2, double min, double max);

bool check_and_insert(t_rand_gen *gen, double new_val, float *vals, double min_dist, double max_dist, long list_length, bool random_order, t_stream_mode stream_mode, double init_val, double min_sbound, double max_sbound);

void timemap_rand_amount(t_timemap *x, double rand_amount);
void timemap_centre(t_timemap *x, double centre);
void timemap_warp(t_timemap *x, double warp);
void timemap_min_dist(t_timemap *x, double min_dist);
void timemap_max_dist(t_timemap *x, double max_dist);
void timemap_max_retries(t_timemap *x, t_atom_long max_retries);
void timemap_random_order(t_timemap *x, t_atom_long random_order);
void timemap_scaling(t_timemap *x, t_symbol *scale_mode_sym, double min_val, double max_val);
void timemap_stream(t_timemap *x, t_atom_long stream_mode, double init_val, double min_sbound, double max_sbound);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// Basic object routines (main / new / free / assist) /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int C74_EXPORT main()
{
    this_class = class_new("timemap",
						   (method)timemap_new, 
						   (method)timemap_free, 
						   sizeof(t_timemap), 
						   NULL, 
						   A_DEFFLOAT, 
						   A_DEFFLOAT, 
						   A_DEFFLOAT,
						   0);
	
	class_addmethod(this_class, (method)timemap_calculate, "int", A_LONG, 0);
	class_addmethod(this_class, (method)timemap_rand_amount, "ft1", A_FLOAT, 0);
	class_addmethod(this_class, (method)timemap_centre, "ft2", A_FLOAT, 0);
	class_addmethod(this_class, (method)timemap_warp, "ft3", A_FLOAT, 0);
	class_addmethod(this_class, (method)timemap_min_dist, "ft4", A_FLOAT, 0);
	class_addmethod(this_class, (method)timemap_max_dist, "ft5", A_FLOAT, 0);
	class_addmethod(this_class, (method)timemap_max_retries, "in6", A_LONG, 0);
	class_addmethod(this_class, (method)timemap_random_order, "random_order", A_LONG, 0);
	class_addmethod(this_class, (method)timemap_scaling, "scaling", A_SYM, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(this_class, (method)timemap_stream, "stream", A_DEFLONG, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(this_class, (method)timemap_assist, "assist", A_CANT, 0);
	
	class_register(CLASS_BOX, this_class);
	
	return 0;
}


void timemap_free(t_timemap *x)
{
}


void *timemap_new(double rand_amount, double centre, double warp)
{
	t_timemap *x = (t_timemap *)object_alloc(this_class);
	
    x->thelistout = listout(x);
	intin(x, 6);
	floatin(x, 5);
	floatin(x, 4);
	floatin(x, 3);
	floatin(x, 2);
	floatin(x, 1);
	
	if (centre > 1.) 
		centre = 1.;
	if (centre < 0.) 
		centre = 0.;
	if (rand_amount > 1.) 
		rand_amount = 1.;
	if (rand_amount < 0) 
		rand_amount = 0.;
	if (warp <= 0.) 
		warp = 1.;
	
	x->centre = centre;
	x->rand_amount = rand_amount;
	x->warp = warp;
	x->min_dist = 0.;
	x->max_dist = 1.;
	x->max_retries = 10;
	x->random_order = FALSE;
	
	timemap_scaling(x, gensym("none"), 0, 1);
	timemap_stream(x, 0, 0, 0, 1);
    
    rand_seed(&x->gen);
	
    return x;
}


void timemap_assist(t_timemap *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) 
	{
        switch (a)
		{
			case 0:
				sprintf(s,"Do Mapping");
				break;
			case 1:
				sprintf(s,"Random Amount (0-1)");
				break;
			case 2:
				sprintf(s,"Centre (0-1)");
				break;
			case 3:
				sprintf(s,"Clumping Factor");
				break;
			case 4:
				sprintf(s,"Min Interval (0-1)");
				break;
			case 5:
				sprintf(s,"Max Interval (0-1)");
				break;
			case 6:
				sprintf(s,"Max Retries");
				break;
		}
	}
    else
        sprintf(s,"List Out");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////// Calculation routines ///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static __inline double timemap_value(t_rand_gen *gen, long i, double points_recip, double rand_amount, double centre, double centre_compliment, double warp)
{
    // Start with equally spaced values (in the range 0 to 1)
    
    double new_val = (i + 1) * points_recip;
    
    // Interpolate with a random value (0 to 1) by the random amount
    
    new_val = new_val - (rand_amount * (new_val -  rand_double(gen)));
    
    // Now warp around the centre value, according to the warp factor (using a pow operation)
    
    if (new_val > centre)
        new_val = centre + (centre_compliment * pow((new_val - centre) / centre_compliment, warp));
    else
        new_val = centre - (centre * pow((centre - new_val) / centre, warp));
    
    return new_val;
}


void timemap_calculate(t_timemap *x, t_atom_long num_points)
{
	t_atom output_list[1024];
	float temp_vals[1024];
	long divisions[1024];
	
	double init_val = x->init_val;
	double scale_val1 = x->scale_val1;
	double scale_val2 = x->scale_val2;
	double min_val = x->min_val;
	double max_val = x->max_val;
	double min_sbound = x->min_sbound;
	double max_sbound = x->max_sbound;
	
	double centre = x->centre;
	double rand_amount = x->rand_amount;
	double warp = x->warp;
	double min_dist = x->min_dist;
	double max_dist = x->max_dist;
	
	t_atom_long max_retries = x->max_retries;

    t_stream_mode stream_mode = x->stream_mode;
	t_scale_mode scale_mode = x->scale_mode;

    bool random_order = x->random_order;

	double centre_compliment = 1. - centre;
	double new_val;
	double cumulate;
	double points_recip;
	
    t_atom_long retries = 0;

    long list_length = 0;
	long division;
	long division_pos = 0;
	long i;
	
    bool suitable_val;
    
    if (num_points < 1)
    {
        object_error((t_object *)x, "requested number of values must be a positive integer");
        return;
    }
    
	if (num_points > 1024)
    {
        object_error((t_object *)x, "maximum number of output values is 1024");
		num_points = 1024;
    }
    
	if (stream_mode != kStreamNone)
		num_points--;

	points_recip = 1. / (double) (num_points + 1);

	// If the ordering is to be random start with a list of potential divisions (0 to num_points - 1)
	
	if (random_order)
	{
		for (i = 0; i < num_points; i++)
			divisions[i] = i;
	}
	
	// Generate each value in turn
	
	for (i = 0; i < num_points; i++)
	{
		// Loop until a suitable value is found (or we reach the maximum number of retries)
		
		for (suitable_val = FALSE, retries = 0, division = i; !suitable_val && retries < max_retries; retries++)
		{
			// Choose which of the remaining divisions to do if the order is random
			
			if (random_order)
			{
                // FIX - this and the below relating to it....
                
                division_pos = rand_int_n(&x->gen, num_points - (list_length + 1));
				division = divisions[division_pos];
			}
			
			new_val = timemap_value(&x->gen, division, points_recip, rand_amount, centre, centre_compliment, warp);
			
			// Scale if relevant
			
			if (scale_mode != kScaleNone)
				new_val = scale_val(new_val, scale_mode, scale_val1, scale_val2, min_val, max_val);
			
			// If the value is good then keep it, increase list_length and replace the division we have just used with the one we would have used if working in order
			// This maintains a valid list of unused divisions for the next time
			
			if (check_and_insert(&x->gen, new_val, temp_vals, min_dist, max_dist, list_length, random_order, stream_mode, init_val, min_sbound, max_sbound))
			{
				suitable_val = TRUE;
				list_length++;
				if (random_order) 
					divisions[division_pos] = divisions[num_points - list_length];
			}
		}
	}
	
	// Do stream mode
	
	switch (stream_mode)
	{
		case kStreamNone:
			
			// Stream mode off (use values as they are)
			
			for (i = 0; i < list_length; i++)
				atom_setfloat(output_list + i, temp_vals[i]);
			break;
			
		case kStreamAdditive:
			
			// Accumulate the values through addition
			
			list_length++;
			atom_setfloat(output_list, init_val);
			cumulate = init_val;
            
			for (i = 1; i < list_length; i++)
			{
				cumulate = temp_vals[i - 1] + cumulate;
				atom_setfloat(output_list + i, cumulate);
			}
			break;
			
		case kStreamMultiplicative:
			
			// Accumulate the values through multiplication
			
			list_length++;
			atom_setfloat(output_list, init_val);
			cumulate = init_val;
            
			for (i = 1; i < list_length; i++)
			{
				cumulate = temp_vals[i - 1] * cumulate;
				atom_setfloat(output_list + i, cumulate);
			}
			break;
	}
	
	
	outlet_list(x->thelistout, 0L, list_length, output_list);
}



double scale_val(double new_val, t_scale_mode scale_mode, double scale_val1, double scale_val2, double min, double max)
{
	// Apply the appropriate scaling
	
	bool reciprocal = FALSE;
	
	if (scale_mode == kScaleDiv && new_val < 0.5)
	{
		reciprocal = TRUE;
		new_val = 1.0 - new_val;
	}
	
	new_val = new_val * scale_val1 + scale_val2;
	
	if (scale_mode == 2) 
		new_val = exp(new_val);
	if (scale_mode == 3) 
		new_val = log(new_val);
	
	if (new_val > max) 
		new_val = max;
	if (new_val < min) 
		new_val = min;
	
	if (reciprocal)
        new_val = 1. / new_val;
	
	return new_val;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////// Sorting function/ ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool check_and_insert(t_rand_gen *gen, double new_val, float *vals, double min_dist, double max_dist, long list_length, bool random_order, t_stream_mode stream_mode, double init_val, double min_sbound, double max_sbound)
{
	// Find Position for New Value 
	
	bool suitable_val = TRUE;
	long new_pos, i, j;
	
	double test_val;
	double cumulate;
	
	// If the order is random then pick randomly, otherwise insert into the list in the correct place
    
	if (random_order)
        new_pos = rand_int_n(gen, list_length);
	else
	{
		for (j = 0; j < list_length; j++)
			if (new_val < vals[j]) 
				break;
		
		new_pos = j;
	}
	
	// Check suitability (according to the minimum and maximum allowable distances
	
	// For the preceding value
	
	if (new_pos)
	{
		test_val = fabs(new_val - vals[new_pos - 1]);
		suitable_val = ((test_val >= min_dist) && (test_val <= max_dist));
	}
	
	if (!suitable_val) 
		return FALSE;
	
	// For the subsequent value

	if (new_pos < list_length)
	{
		test_val = fabs(vals[new_pos] - new_val);
		suitable_val = ((test_val >= min_dist) && (test_val <= max_dist));
	}
	
	if (!suitable_val) 
		return FALSE;
	
	// If we are in stream mode then check that this value will not result in output exceeding the stream bounds
	
	cumulate = init_val;
		
    switch (stream_mode)
    {
        case kStreamNone:
            break;
            
        case kStreamAdditive:
            
            for (i = 0; i < new_pos; i++)
                cumulate += vals[i];
            
            cumulate += new_val;
            if (cumulate < min_sbound || cumulate > max_sbound) 
                return FALSE;
            
            for (i = new_pos; i < list_length; i++)
            {
                cumulate += vals[i];
                if (cumulate < min_sbound || cumulate > max_sbound) 
                    return FALSE;
            }
            break;
        
        case kStreamMultiplicative:
            
            for (i = 0; i < new_pos; i++)
                cumulate *= vals[i];
            
            cumulate *= new_val;

            if (cumulate < min_sbound || cumulate > max_sbound) 
                return FALSE;
            
            for (i = new_pos; i < list_length; i++)
            {
                cumulate *= vals[i];
                if (cumulate < min_sbound || cumulate > max_sbound) 
                    return FALSE;
            }
            break;
    }
	
	// If the value is suitable then here we insert the value
	
	for (j = list_length; j > new_pos; j--) 
		vals[j] = vals[j - 1];
	
	vals[new_pos] = new_val;
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// Various routines for setting the mapping parameters ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void timemap_rand_amount(t_timemap *x, double rand_amount)
{
	if (rand_amount > 1.)
        rand_amount = 1.;
	if (rand_amount < 0.)
        rand_amount = 0.;
	x->rand_amount = rand_amount;
}


void timemap_centre(t_timemap *x, double centre)
{
	if (centre > 1.)
        centre = 1.;
	if (centre < 0.)
        centre = 0.;
	x->centre = centre;
}


void timemap_warp(t_timemap *x, double warp)
{
    x->warp = (warp < 0.) ? 1.0 : warp;
}


void timemap_min_dist(t_timemap *x, double min_dist)
{
	x->min_dist = fabs(min_dist);
}


void timemap_max_dist(t_timemap *x, double max_dist)
{
	x->max_dist = fabs(max_dist);
}


void timemap_max_retries(t_timemap *x, t_atom_long max_retries)
{
    x->max_retries = (max_retries < 0) ? 0 : max_retries;
}


void timemap_random_order(t_timemap *x, t_atom_long random_order)
{
    x->random_order = random_order ? TRUE : FALSE;
}


void timemap_scaling(t_timemap *x, t_symbol *scale_mode_sym, double min_val, double max_val)
{
	t_scale_mode scale_mode = kScaleNone;
	double scale_min = min_val;
	double scale_max = max_val;
	
	if (scale_mode_sym == gensym("scale")) 
		scale_mode = kScale;
	
	if (scale_mode_sym == gensym("amp"))
	{
		scale_mode = kScaleLog;
		scale_min = pow(10, scale_min / 20.);
		scale_max = pow(10, scale_max / 20.);
	}
	
	if (scale_mode_sym == gensym("pitch"))
	{
		scale_mode = kScaleLog;
		scale_min = pow(2, scale_min / 12.);
		scale_max = pow(2, scale_max / 12.);
	}
	
	if (scale_mode_sym == gensym("log")) 
		scale_mode = kScaleLog;
    
	if (scale_mode_sym == gensym("exp")) 
		scale_mode = kScaleExp;
	
	if (scale_mode_sym == gensym("div"))
	{
		scale_mode = kScaleDiv;
		scale_min = max_val - (2 * (max_val - min_val));
	}
	
	if (scale_mode == kScaleLog)
	{
		scale_min = log(scale_min);
		scale_max = log(scale_max);
	}
	
	if (scale_mode == kScaleExp)
	{
		scale_min = exp(scale_min);
		scale_max = exp(scale_max);
	}
	
	x->scale_mode = scale_mode;
	x->scale_val1 = scale_max - scale_min;
	x->scale_val2 = scale_min;
	x->min_val = min_val;
	x->max_val = max_val;
}


void timemap_stream(t_timemap *x, t_atom_long stream_mode, double init_val, double min_sbound, double max_sbound)
{
    t_stream_mode mode;
    
    
    mode = (stream_mode == 1) ? kStreamAdditive : kStreamNone;
    mode = (stream_mode > 1) ? kStreamMultiplicative : mode;
    
	x->stream_mode = mode;
	x->init_val = init_val;
	x->min_sbound = min_sbound;
	x->max_sbound = max_sbound;
}




