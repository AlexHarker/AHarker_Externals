
/*
 *  descriptors_combsort.c
 *
 *	This code deals with various sorting operations for the descriptors~ and descriptorsrt~ objects (for both single and double precision floating point numbers).
 *	The comb sort algorithm is not the fastest sort under all circumstances (quicksort can be faster), but for its simplicity it offers very good performance.
 *	Quicksort suffers from very low speed performance for already sorted data, or data that all takes one single value (for instance, all zero values).
 *	For real-time audio performance peak cpu usage (lowest speed) is often more crucial than average performance, hence the choice of comb sort.
 *	
 *	This implementation uses the combsort11 algorithm, in which gaps of 9 or 10 are set to use a value of 11 instead, which results in further speed improvements to the algorithm.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include "descriptors_combsort.h"



// Double precision sorting routines 



// In-place acescending sort (double precision)  

void combsort_double (double *data, long num_points)
{
	double d_temp;
	long gap = num_points;
	long swaps = 1;
	long i;
	
	while (gap > 1 || swaps)
	{
		if (gap > 1)
		{
			gap = (gap * 10) / 13;
			if (gap == 9 || gap == 10) 
				gap = 11;
			if (gap < 1) gap = 1;
		}
		
		for (i = 0, swaps = 0; i + gap < num_points; i++)
		{
			if (data[i] > data[i + gap])
			{
				d_temp = data[i];
				data[i] = data[i + gap];
				data[i + gap] = d_temp;
				swaps = 1;	
			}
		}
	}
}


// Index-type acescending sort (double precision)  

void combsort_indices_double (long *indices, double *data, long num_points)
{
	long gap = num_points;
	long swaps = 1;
	long gap_index;
	long index;
	long i;
	
	while (gap > 1 || swaps)
	{
		if (gap > 1)
		{
			gap = (gap * 10) / 13;
			if (gap == 9 || gap == 10) 
				gap = 11;
			if (gap < 1) gap = 1;
		}
		
		for (i = 0, swaps = 0; i + gap < num_points; i++)
		{
			index = indices[i];
			gap_index = indices[i + gap];
			if (data[index] > data[gap_index])
			{
				indices[i] = gap_index;
				indices[i + gap] = index;
				swaps = 1;	
			}
		}
	}
}


// Sorts spectral peaks in-place (frequencies and amplitudes) into decescending amplitude order (double precision) 





// Single precision sorting routines 



// Index-type acescending sort (single precision)

void combsort_indices_float (long *indices, float *data, long num_points)
{
	long gap = num_points;
	long swaps = 1;
	long index;
	long gap_index;
	long i;
	
	while (gap > 1 || swaps)
	{
		if (gap > 1)
		{
			gap = (gap * 10) / 13;
			if (gap == 9 || gap == 10) 
				gap = 11;
			if (gap < 1) gap = 1;
		}
		
		for (i = 0, swaps = 0; i + gap < num_points; i++)
		{
			index = indices[i];
			gap_index = indices[i + gap];
			if (data[index] > data[gap_index])
			{
				indices[i] = gap_index;
				indices[i + gap] = index;
				swaps = 1;	
			}
		}
	}
}


// Sorts spectral peaks in-place (frequencies and amplitudes) into decescending amplitude order (single precision) 

void combsort_peaks_float(float *amps, float *freqs, long num_peaks)			
{
	float f_temp;
	float a_temp;
	long gap = num_peaks;
	long swaps = 1;
	long i;
	
	while (gap > 1 || swaps)
	{
		if (gap > 1)
		{
			gap = (gap * 10) / 13;
			if (gap == 9 || gap == 10) 
				gap = 11;
			if (gap < 1) gap = 1;
		}
		
		for (i = 0, swaps = 0; i + gap < num_peaks; i++)
		{
			if (amps[i] < amps[i + gap])
			{
				a_temp = amps[i];
				f_temp = freqs[i];
				amps[i] = amps[i + gap];
				freqs[i] = freqs[i + gap];
				amps[i + gap] = a_temp;
				freqs[i + gap] = f_temp;
				swaps = 1;	
			}
		}
	}
}

