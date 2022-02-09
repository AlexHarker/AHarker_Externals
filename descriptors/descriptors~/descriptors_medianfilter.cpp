
/*
 *  descriptors_medianfilter.c
 *
 *	This code is used to attempt to differentiate between noise and determinate content within a spectrum. 
 *	There are more state-of-the-art (but also more expensive) methods for doing this, and even in its current state some improvements could be made to its use within the spectral peak-finding routines.
 *	As it is this code is far from fundamental to the functioning of these objects, and could be removed, but possibly with improvements it could be made more useful.
 *
 *	Specifically, the results of the median filter could be scaled (using a factor greater than 1) to more accurately remove noise components from the spectral peak-finding routines.
 *	However, if the number of spectral peaks to find is not large, the median filtering is still likely to have little effect.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include "descriptors_medianfilter.h"



// Double precision median filtering 




// Main routine for filter 



void medianfilter_double (long *indices, double *medians, double *data, long num_points, long median_span)
{	
	long median_size = median_span + 1;
	long median_max_size = 2 * median_span + 1;
	long new_median_size = median_size - 1;
	long median_val_index;
	long i;
	
	if (median_size > num_points) 
		median_size = num_points;
	if (median_max_size > num_points) 
		median_max_size = num_points;

	new_median_size = median_size - 1;
	median_val_index = (median_max_size >> 1);

	for (i = 0; i < median_size; i ++)
		indices[i] = i;
		
	combsort_indices_double (indices, data, median_size);
	medians[0] = data[indices[median_size >> 1]];

	// LH edge
	
	for (i = 1; i < (median_max_size - median_size) + 1; i++)										
	{
		median_size = medianfilter_insert_double(indices, data, median_size, i + new_median_size);
		medians[i] = data[indices[median_size >> 1]];
	}

	// centre
	
	for (; i < num_points - (median_max_size - median_size); i++)								
	{
		medianfilter_switch_double(indices, data, median_size, i + new_median_size, i - (new_median_size + 1));
		medians[i] = data[indices[median_val_index]];
	}
	
	// RH edge

	for (; i < num_points; i++)														
	{
		median_size = medianfilter_remove_double(indices, data, median_size, i - (new_median_size + 1));
		medians[i] = data[indices[median_size >> 1]];
	}
}	



// Routines for inserting / switching / removing values 



long medianfilter_insert_double(long *indices, double *data, long num_points, long insert_index)
{
	double insert_val = data[insert_index];
	long gap = num_points >> 1;
	long insert_point = 0;
	long i = 0;
	
	if (gap > 1) 
		gap = 1;
	
	// Find insert point

	while (gap > 0)
	{
		for (i = insert_point + gap; i < num_points; i += gap)
		{
			if (data[indices[i]] > insert_val) 
				break;
			else 
				insert_point = i;
		}
		gap >>= 1;
	}
			
	// Do swaps
	
	for (i = num_points; i > insert_point; i--)					
		indices[i] = indices[i - 1];
		
	indices[insert_point] = insert_index;
	
	return num_points + 1;
}


void medianfilter_switch_double (long *indices, double *data, long num_points, long insert_index, long remove_index)
{
	double insert_val = data[insert_index];
	double remove_val = data[remove_index];
	long gap = num_points >> 1;
	long insert_point = 0;
	long remove_point = 0;
	long i = 0;
	
	if (gap > 1) 
		gap = 1;
	
	// Find the index to replace
	
	while (gap > 0)
	{
		for (i = remove_point + gap; i < num_points; i += gap)
		{
			if (data[indices[i]] > remove_val) 
				break;
			else 
				remove_point = i;
		}
		gap >>= 1;
	}
	
	// Note - we might actually be removing the wrong index, but we're leaving one with the same value - so it doesn't matter
	
	// Find where to put it
	
	if (insert_val > data[remove_index])
	{	
		// Search right
		
		insert_point = remove_point;
		gap = (num_points - remove_point) >> 1;
		if (gap > 1) 
			gap = 1;
		
		while (gap > 0)
		{
			for (i = insert_point + gap; i < num_points; i += gap)
			{
				if (data[indices[i]] > insert_val) 
					break;
				else 
					insert_point = i;
			}
			gap >>= 1;
		}
		
		// Do swaps
			
		for (i = remove_point; i < insert_point; i++)				
			indices[i] = indices[i + 1];
	}
	else
	{
		// Search left
		
		insert_point = remove_point;
		gap = remove_point >> 1;
		if (gap > 1) 
			gap = 1;
		
		while (gap > 0)
		{
			for (i = insert_point - gap; i >= 0; i -= gap)
			{
				if (data[indices[i]] > insert_val) 
					break;
				else 
					insert_point = i;
			}
			gap >>= 1;
		}
		
		// Do swaps
		
		for (i = remove_point; i > insert_point; i--)				
			indices[i] = indices[i - 1];
	}
	
	// Replace the index
	
	indices[insert_point] = insert_index;	
}


long medianfilter_remove_double (long *indices, double *data, long num_points, long remove_index)
{
	double remove_val = data[remove_index]; 
	long gap = num_points >> 1;
	long remove_point = 0;
	long i;

	if (gap > 1) 
		gap = 1;
	
	// Find remove point		
	
	while (gap > 0)
	{
		for (i = remove_point + gap; i < num_points; i += gap)
		{
			if (data[indices[i]] > remove_val) 
				break;
			else 
				remove_point = i;
		}
		gap >>= 1;
	}
	
	// Do swaps
	
	for (i = remove_point; i < num_points; i++)					
		indices[i] = indices[i + 1];
			
	return num_points - 1;
}



// Single precision median filtering 




// Main routine for filter 



void medianfilter_float (t_descriptors *x, long *indices, float *medians, float *data, long num_points, long median_span)
{	
	long median_size = median_span + 1;
	long median_max_size = 2 * median_span + 1;
	long new_median_size = median_size - 1;
	long median_val_index;
	long i;

	// Note this function is only ever called in one place, and sets flags so that it is never recalculated unnecessarily
	
	if (x->median_flag && x->last_median_span == median_span) 
		return;
	
	x->median_flag = 1;
	x->last_median_span = median_span;
	
	if (median_size > num_points) 
		median_size = num_points;
	if (median_max_size > num_points) 
		median_max_size = num_points;
	
	new_median_size = median_size - 1;
	median_val_index = (median_max_size >> 1);

	for (i = 0; i < median_size; i ++)
		indices[i] = i;
		
	combsort_indices_float (indices, data, median_size);
	medians[0] = data[indices[median_size >> 1]];

	// LH edge
	
	for (i = 1; i < (median_max_size - median_size) + 1; i++)
	{
		median_size = medianfilter_insert_float(indices, data, median_size, i + new_median_size);
		medians[i] = data[indices[median_size >> 1]];
	}

	// centre
	
	for (; i < num_points - (median_max_size - median_size); i++)
	{
		medianfilter_switch_float(indices, data, median_size, i + new_median_size, i - (new_median_size + 1));
		medians[i] = data[indices[median_val_index]];
	}

	// RH edge
	
	for (; i < num_points; i++)
	{
		median_size = medianfilter_remove_float(indices, data, median_size, i - (new_median_size + 1));
		medians[i] = data[indices[median_size >> 1]];
	}
}	



// Routines for inserting / switching / removing values 



long medianfilter_insert_float (long *indices, float *data, long num_points, long insert_index)
{
	float insert_val = data[insert_index];
	long gap = num_points >> 1;
	long insert_point = 0;
	long i = 0;
	
	if (gap > 1) 
		gap = 1;
	
	// Find insert point

	while (gap > 0)
	{
		for (i = insert_point + gap; i < num_points; i += gap)
		{
			if (data[indices[i]] > insert_val) 
				break;
			else 
				insert_point = i;
		}
		gap >>= 1;
	}
			
	// Do swaps
	
	for (i = num_points; i > insert_point; i--)
		indices[i] = indices[i - 1];
		
	indices[insert_point] = insert_index;
	
	return num_points + 1;
}


void medianfilter_switch_float (long *indices, float *data, long num_points, long insert_index, long remove_index)
{	
	float remove_val = data[remove_index];
	float insert_val = data[insert_index];
	long gap = num_points >> 1;
	long remove_point = 0;
	long insert_point = 0;
	long i = 0;	
	
	if (gap > 1) 
		gap = 1;
	
	// Find the value to replace
	
	while (gap > 0)
	{
		for (i = remove_point + gap; i < num_points; i += gap)
		{
			if (data[indices[i]] > remove_val) 
				break;
			else 
				remove_point = i;
		}
		gap >>= 1;
	}
	
	// Note - we might actually be removing the wrong index, but we're leaving one with the same value - so it doesn't matter
	
	// Find where to put it
		
	if (insert_val > data[remove_index])							
	{	
		// Search right

		insert_point = remove_point;
		gap = (num_points - remove_point) >> 1;
		if (gap > 1) 
			gap = 1;
		
		while (gap > 0)
		{
			for (i = insert_point + gap; i < num_points; i += gap)
			{
				if (data[indices[i]] > insert_val) 
					break;
				else 
					insert_point = i;
			}
			gap >>= 1;
		}
			
		// Do swaps
		
		for (i = remove_point; i < insert_point; i++)				
			indices[i] = indices[i + 1];
	}
	else														
	{
		// Search left
		
		insert_point = remove_point;
		gap = remove_point >> 1;
		if (gap > 1) 
			gap = 1;
		
		while (gap > 0)
		{
			for (i = insert_point - gap; i >= 0; i -= gap)
			{
				if (data[indices[i]] > insert_val) 
					break;
				else 
					insert_point = i;
			}
			gap >>= 1;
		}
			
		// Do swaps
		
		for (i = remove_point; i > insert_point; i--)				
			indices[i] = indices[i - 1];
	}
	
	// Replace the index
	
	indices[insert_point] = insert_index;	
}


long medianfilter_remove_float (long *indices, float *data, long num_points, long remove_index)
{
	float remove_val = data[remove_index]; 
	long gap = num_points >> 1;
	long remove_point = 0;
	long i;
	
	if (gap > 1) 
		gap = 1;
	
	// Find remove point
	
	while (gap > 0)
	{
		for (i = remove_point + gap; i < num_points; i += gap)
		{
			if (data[indices[i]] > remove_val) 
				break;
			else 
				remove_point = i;
		}
		gap >>= 1;
	}
	
	// Do swaps
	
	for (i = remove_point; i < num_points; i++)
		indices[i] = indices[i + 1];
			
	return num_points - 1;
}
