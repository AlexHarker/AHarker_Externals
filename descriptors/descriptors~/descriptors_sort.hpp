
/*
 *  descriptors_sort.hpp
 *
 *	A header file for the sort routines in the descriptors~ and descriptorsrt~ objects.
 *	This code deals with various sorting operations in the descriptors~ and descriptorsrt~ objects.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#ifndef _DESCRIPTORS_SORT_HPP_
#define _DESCRIPTORS_SORT_HPP_

// Sorting Routines

// Convenience for altering swap flags

template <class T>
bool swap(T& a, T& b)
{
    std::swap(a, b);
    return true;
}

// An ascending order sort (combsort11 algorithm)

template <class T>
void sort(T& values, long size)
{
    long gap = size;
    bool swaps = true;
    long i;
    
    while (gap > 1 || swaps)
    {
        if (gap > 1)
        {
            gap = (gap * 10) / 13;
            if (gap == 9 || gap == 10) gap = 11;
            if (gap < 1) gap = 1;
        }
        
        for (i = 0, swaps = false; i + gap < size; i++)
            if (values[i] > values[i + gap])
                swaps = swap(values[i], values[i + gap]);
    }
}

// An ascending order index sort (combsort11 algorithm)

template <class T, class U>
void sort(T& indices, const U& values, long size)
{
    long gap = size;
    bool swaps = true;
    long i;
    
    while (gap > 1 || swaps)
    {
        if (gap > 1)
        {
            gap = (gap * 10) / 13;
            if (gap == 9 || gap == 10) gap = 11;
            if (gap < 1) gap = 1;
        }
        
        for (i = 0, swaps = false; i + gap < size; i++)
            if (values[indices[i]] > values[indices[i + gap]])
                swaps = swap(indices[i], indices[i + gap]);
    }
}
/*
void combsort_peaks_double(double *amps, double *freqs, long num_peaks)
{
    double f_temp;
    double a_temp;
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
}*/
void sort_peaks(double *amps, double *freqs, long num_peaks);

#endif	/* _DESCRIPTORS_SORT_HPP_ */
