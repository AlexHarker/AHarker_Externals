
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

#include "descriptors_constants.h"

static inline double dbtoa(double db)
{
	return pow(10, db / 20.0);
}

static inline double atodb(double a)
{
	return std::max(20.0 * log10(a), DB_MIN);
}

static inline double pow_to_db(double p)
{
    return std::max(10.0 * log10(p), DB_MIN);
} 

static inline long freq_to_bin(double freq, double one_over_bin_freq, long num_bins)
{
	return std::min(std::max(static_cast<long>(ceil(freq * one_over_bin_freq)), 0L), num_bins);
}

static inline bool get_bin_range(long *min_bin, long *max_bin, double *params, double one_over_bin_freq, long num_bins)
{
	*min_bin = freq_to_bin(params[0], one_over_bin_freq, num_bins);
	*max_bin = freq_to_bin(params[1], one_over_bin_freq, num_bins);
	
    return *max_bin > *min_bin;
}


#endif /* _CONVERSION_HELPERS_ */
