
/*
 *  descriptors_combsort.h
 *
 *	A header file for the combsort routines in the descriptors~ and descriptorsrt~ objects.
 *	This code deals with various sorting operations in the descriptors~ and descriptorsrt~ objects (for both single and double precision floating point numbers).
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#ifndef _DESCRIPTORS_COMB_SORT_
#define _DESCRIPTORS_COMB_SORT_


// Double precision combosrt routines

void combsort_double (double *TempVals, long num_points);
void combsort_indices_double (long *indices, double *data, long num_points);
void combsort_peaks_double (double *amps, double *freqs, long num_peaks);

// Single precision combsort routines

void combsort_peaks_float (float *amps, float *freqs, long num_peaks);
void combsort_indices_float (long *indices, float *data, long num_points);


#endif	/* _DESCRIPTORS_COMB_SORT_ */