

#include "descriptors_edges.hpp"
#include "descriptors_sort.hpp"

#include <cmath>
#include <vector>

template <class T>
class median_filter
{
public:
    
    median_filter(uintptr_t max_width) : m_data(max_width), m_indices(max_width){}
    
    // Enum for options (if implementing all)

    enum class Edges { Pad, Extend, Wrap, Fold, Mirror };

    // Process

    void operator()(T *output, const T *input, uintptr_t size, uintptr_t width, Edges edges, double percentile, T pad = 0.0)
    {
        uintptr_t pos = static_cast<uintptr_t>(std::round(percentile * (width - 1) / 100.0));
        pos = std::min(pos, static_cast<uintptr_t>(width - 1));

        switch (edges)
        {
            case Edges::Pad:      filter(edges_pad<T>(input, size, pad), output, width, size, pos);     break;
            case Edges::Extend:   filter(edges_extend<T>(input, size), output, width, size, pos);       break;
            case Edges::Wrap:     filter(edges_wrap<T>(input, size), output, width, size, pos);         break;
            case Edges::Fold:     filter(edges_fold<T>(input, size), output, width, size, pos);         break;
            case Edges::Mirror:   filter(edges_mirror<T>(input, size), output, width, size, pos);       break;
        }
    }
    
private:

    // Main filter loop
    
    template <class U>
    void filter(U in, T *out, intptr_t width, intptr_t size, uintptr_t pos)
    {
        T *data = m_data.data();
        uintptr_t *indices = m_indices.data();
        
        intptr_t o1 = width >> 1;
        intptr_t o2 = width - o1;

        // Calculate the first percentile

        for (intptr_t i = 0; i < width; i++)
            data[i] = in(i - o1);

        sort_ascending(indices, data, width);
        out[0] = data[indices[pos]];

        // Do other values using insertion

        for (intptr_t i = 1; i < size; i++)
            out[i] = insert(data, indices, in(i + o2), (i - 1) % width, width, pos);
    }
    
    // Insertion

    T insert(T *data, uintptr_t *indices, T value, intptr_t index, intptr_t width, uintptr_t pos)
    {
        intptr_t current = -1, insert = 0, gap = 0;

        // Insert actual data

        data[index] = value;

        // Find sort position for the index

        for (current = 0; (current < width) && (indices[current] != index); current++);

        // Search right

        for (insert = current, gap = std::max(1L, (width - current) >> 1); gap; gap >>= 1)
            for (intptr_t i = insert + gap; (i < width) && (value > data[indices[i]]); i += gap)
                insert = i;

        // Swaps

        std::copy(indices + current + 1, indices + insert + 1, indices + current);
        indices[insert] = index;

        // Search left

        for (current = insert, gap = std::max(1L, current >> 1); gap; gap >>= 1)
            for (intptr_t i = insert - gap; (i >= 0) && (value < data[indices[i]]); i -= gap)
                insert = i;

        // Swaps

        std::copy_backward(indices + insert, indices + current, indices + current + 1);
        indices[insert] = index;

        return data[indices[pos]];
    }
    
    std::vector<T> m_data;
    std::vector<uintptr_t> m_indices;
};
