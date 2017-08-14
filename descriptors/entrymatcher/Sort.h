
#ifndef SORT_H
#define SORT_H

template <class T, class U>
void sort(T& indices, U& values, long numValues)
{
    // An ascending order index sort (combsort11 algorithm)
    
    long gap = numValues;
    bool swaps = 1;
    long i;
    
    while (gap > 1 || swaps)
    {
        if (gap > 1)
        {
            gap = (gap * 10) / 13;
            if (gap == 9 || gap == 10) gap = 11;
            if (gap < 1) gap = 1;
        }
        
        for (i = 0, swaps = 0; i + gap < numValues; i++)
        {
            if (values[indices[i]] > values[indices[i + gap]])
            {
                std::swap(indices[i], indices[i + gap]);
                swaps = true;
            }
        }
    }
}

#endif