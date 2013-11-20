
/*
 *  descriptors_conversion_helpers.h
 *
 *	Various conversion routines for the descriptors~ and descriptorsrst~ objects.
 *	All routines are declared static inline to allowing inling from multiple source files without linking issues.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#ifndef _CONVERSION_HELPERS_
#define _CONVERSION_HELPERS_


#include <AH_Win_Math.h>
#include "descriptors_constants.h"

static __inline double dbtoa (double db)
{
	double a;
	
	a =  pow (10, db / 20.); 
	return a;
}


static __inline double atodb (double a)
{
	double db;
	
	if (!a) 
		return DB_MIN;
	
	db =  20. * log10(a);
	
	if (db < DB_MIN) 
		db = DB_MIN;
	
	return db;
}


static __inline double pow_to_db (double p)
{
	double db;
	
	if (!p) return DB_MIN;
	
	db = 10. * log10(p);
	
	if (db < DB_MIN) 
		db = DB_MIN;
	
	return db;
} 


static __inline long freq_to_bin (double freq, double one_over_bin_freq, long num_bins)
{
	freq *= one_over_bin_freq;
	
	if (freq < 0.) 
		freq = 0.;
	
	if (freq > num_bins) 
		freq = num_bins;
	
	return (long) ceil(freq); 
}

static __inline long get_bin_range (long *min_bin, long *max_bin, double *params, double one_over_bin_freq, long num_bins)
{
	*min_bin = freq_to_bin (params[0], one_over_bin_freq, num_bins);
	*max_bin = freq_to_bin (params[1], one_over_bin_freq, num_bins);
	
	if (*max_bin <= *min_bin) 
		return 0;
	
	return 1;
}


#endif /* _CONVERSION_HELPERS_ */
