
#ifndef _SORT_HPP_
#define _SORT_HPP_

#include <algorithm>
#include <cstdint>

// Sorting Implementation

namespace impl
{
    // Convenience for altering swap flags

    struct comb_sort
    {
        template <class T>
        static bool swap(T& a, T& b)
        {
            std::swap(a, b);
            return true;
        }

        // Gap Update

        static inline uintptr_t update_gap(uintptr_t gap)
        {
            if (gap > 1)
            {
                gap = (gap * 10) / 13;
                gap = (gap == 9 || gap == 10) ? 11 : gap;
            }
            
            return std::max(uintptr_t(1), gap);
        }

        // An ordered sort (combsort11 algorithm)

        template <class Compare, class T>
        static void sort(T& values, uintptr_t size)
        {
            bool swaps = true;
            
            for (uintptr_t gap = size; gap > 1 || swaps; )
            {
                gap = update_gap(gap);
                swaps = false;
                
                for (uintptr_t i = 0; i + gap < size; i++)
                    if (Compare()(values[i], values[i + gap]))
                        swaps = swap(values[i], values[i + gap]);
            }
        }

        // An index sort (combsort11 algorithm)

        template <class Compare, class T, class U>
        static void sort(T& indices, const U& values, uintptr_t size)
        {
            bool swaps = true;

            for (uintptr_t i = 0; i < size; i++)
                indices[i] = i;
            
            for (uintptr_t gap = size; gap > 1 || swaps; )
            {
                gap = update_gap(gap);
                swaps = false;
                
                for (uintptr_t i = 0; i + gap < size; i++)
                    if (Compare()(values[indices[i]], values[indices[i + gap]]))
                        swaps = swap(indices[i], indices[i + gap]);
            }
        }
    };
}

// Sorting Routines

template <class T>
void sort_ascending(T *data, uintptr_t size)
{
    impl::comb_sort::sort<std::greater<T>>(data, size);
}

template <class T>
void sort_descending(T *data, uintptr_t size)
{
    impl::comb_sort::sort<std::less<T>>(data, size);
}

template <class T, class U>
void sort_ascending(T& indices, const U *data, uintptr_t size)
{
    impl::comb_sort::sort<std::greater<U>>(indices, data, size);
}

template <class T, class U>
void sort_descending(T& indices, const U *data, uintptr_t size)
{
    impl::comb_sort::sort<std::less<U>>(indices, data, size);
}

#endif  /* _DESCRIPTORS_SORT_HPP_ */
