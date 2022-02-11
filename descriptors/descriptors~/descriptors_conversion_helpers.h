
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

#include <algorithm>

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

static inline long freq_to_bin(double freq, double sr, long nyquist_bin)
{
    const double bin = 2.0 * nyquist_bin * (freq / sr);
	return std::min(std::max(static_cast<long>(ceil(bin)), 0L), nyquist_bin);
}

#endif /* _CONVERSION_HELPERS_ */
