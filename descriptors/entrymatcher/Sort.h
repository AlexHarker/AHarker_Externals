
#ifndef SORT_H
#define SORT_H

// Convenience for altering swap flags

template <class T> bool swap(T& a, T& b)
{
    std::swap(a, b);
    return true;
}

// An ascending order index sort (combsort11 algorithm)

template <class T, class U>
void sort(T& indices, U& values, long numValues)
{
    long gap = numValues;
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
        
        for (i = 0, swaps = false; i + gap < numValues; i++)
            if (values[indices[i]] > values[indices[i + gap]])
                swaps = swap(indices[i], indices[i + gap]);
    }
}

// An ascending order sort (combsort11 algorithm)

template <class T>
void sort(T& values, long numValues)
{
    long gap = numValues;
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
        
        for (i = 0, swaps = false; i + gap < numValues; i++)
            if (values[i] > values[i + gap])
                swaps = swap(values[i], values[i + gap]);
    }
}

// An ascending order index sort with a getter (combsort11 algorithm)

template <class T, typename Op>
void sort(T& indices, long numValues, Op& valueGetter)
{
    long gap = numValues;
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
        
        for (i = 0, swaps = false; i + gap < numValues; i++)
            if (valueGetter(indices[i]) > valueGetter(indices[i + gap]))
                swaps = swap(indices[i], indices[i + gap]);
    }
}


#endif
