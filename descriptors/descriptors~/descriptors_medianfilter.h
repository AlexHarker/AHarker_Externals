
/*
 *  descriptors_medianfilter.h
 *
 *	A header file for the median filter in the descriptors~ and descriptorsrt~ objects.
 *
 *	This code is used to attempt to differentiate between noise and determinate content within a spectrum. 
 *	There are more state-of-the-art (but also more expensive) methods for doing this, and even in its current state some improvements could be made to its use within the spectral peak-finding routines.
 *	As it is this code is far from fundamental to the functioning of these objects, and could be removed, but with improvements it could be made more useful.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#ifndef _DESCRIPTORS_MEDIAN_FILTER_
#define _DESCRIPTORS_MEDIAN_FILTER_


#include "descriptors_object_structure.h"
#include "descriptors_sort.hpp"


// Double precision median filter functions 

void medianfilter(long *indices, double *medians, double *data, long num_points, long median_span);
long medianfilter_insert(long *indices, double *data, long num_points, long insert_index);
void medianfilter_switch(long *indices, double *data, long num_points, long insert_index, long remove_index);
long medianfilter_remove(long *indices, double *data, long num_points, long remove_index);

#endif /* _DESCRIPTORS_MEDIAN_FILTER_ */
