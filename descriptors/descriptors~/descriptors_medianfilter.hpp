

#include "descriptors_sort.hpp"

#include <TableReader.hpp>

#include <cmath>
#include <vector>

template <class T>
class median_filter
{
public:
    
    median_filter(long max_width) : m_data(max_width), m_indices(max_width){}
    
    // Enum for options (if implementing all)

    enum class Edges { Pad, Extend, Wrap, Fold, Mirror };

    // Process

    void operator()(T *output, const T *input, long size, double percentile, long width, Edges edges, double pad = 0.0)
    {
        unsigned long pos = static_cast<unsigned long>(std::round(percentile * (width - 1) / 100.0));
        pos = std::min(pos, static_cast<unsigned long>(width - 1));

        switch (edges)
        {
            case Edges::Pad:      filter(edges_pad(input, size, pad), output, width, size, pos);    break;
            case Edges::Extend:   filter(edges_extend(input, size), output, width, size, pos);      break;
            case Edges::Wrap:     filter(edges_wrap(input, size), output, width, size, pos);        break;
            case Edges::Fold:     filter(edges_fold(input, size), output, width, size, pos);        break;
            case Edges::Mirror:   filter(edges_mirror(input, size), output, width, size, pos);      break;
        }
    }
    
private:
    
    // Underlying fetch class

    struct edge_fetch : table_fetcher<T>
    {
        edge_fetch(const double *data, unsigned long size)
        : table_fetcher<double>(static_cast<intptr_t>(size), 1.0), m_data(data) {}

        double operator()(intptr_t idx) { return m_data[idx]; }

        const double *m_data;
    };

    // base to hide unneeded functionality

    template <template <class U> class fetch>
    struct edges_base
    {
        edges_base(const double *data, unsigned long size)
        : m_fetch(edge_fetch(data, size)) {}

        double operator()(intptr_t idx) { return m_fetch(idx); }

    private:

        fetch<edge_fetch> m_fetch;
    };

    // edge types (padding type is explicit / others use table_reader.hpp)

    struct edges_pad : private edge_fetch
    {
        edges_pad(const double *data, unsigned long size, double padding)
        : edge_fetch(data, size), m_padding(padding) {}

        double operator()(intptr_t idx)
        {
            return (idx >= 0 && idx < edge_fetch::size) ? edge_fetch::m_data[idx] : m_padding;
        }

    private:

        double m_padding;
    };

    struct edges_extend : edges_base<table_fetcher_extend>
    {
        edges_extend(const double *data, unsigned long size)
        : edges_base<table_fetcher_extend>(data, size) {}
    };

    struct edges_wrap : edges_base<table_fetcher_wrap>
    {
        edges_wrap(const double *data, unsigned long size)
        : edges_base<table_fetcher_wrap>(data, size) {}
    };

    struct edges_fold : edges_base<table_fetcher_fold>
    {
        edges_fold(const double *data, unsigned long size)
        : edges_base<table_fetcher_fold>(data, size) {}
    };

    struct edges_mirror : edges_base<table_fetcher_mirror>
    {
        edges_mirror(const double *data, unsigned long size)
        : edges_base<table_fetcher_mirror>(data, size) {}
    };

    // Helpers

    double insert(T *data, unsigned long *indices, T value, long index, long width, unsigned long pos)
    {
        long current = -1, insert = 0, gap = 0;

        // Insert actual data

        data[index] = value;

        // Find sort position for the index

        for (current = 0; (current < width) && (indices[current] != index); current++);

        // Search right

        for (insert = current, gap = std::max(1L, (width - current) >> 1); gap; gap >>= 1)
            for (long i = insert + gap; (i < width) && (value > data[indices[i]]); i += gap)
                insert = i;

        // Swaps

        std::copy(indices + current + 1, indices + insert + 1, indices + current);
        indices[insert] = index;

        // Search left

        for (current = insert, gap = std::max(1L, current >> 1); gap; gap >>= 1)
            for (long i = insert - gap; (i >= 0) && (value < data[indices[i]]); i -= gap)
                insert = i;

        // Swaps

        std::copy_backward(indices + insert, indices + current, indices + current + 1);
        indices[insert] = index;

        return data[indices[pos]];
    }

    template <class U>
    void filter(U in, T *out, long width, long size, unsigned long pos)
    {
        T *data = m_data.data();
        unsigned long *indices = m_indices.data();
        
        long o1 = width >> 1;
        long o2 = width - o1;

        // Calculate the first percentile

        for (long i = 0; i < width; i++)
            data[i] = in(i - o1);

        sort(indices, data, width);
        out[0] = data[indices[pos]];

        // Do other values using insertion

        for (long i = 1; i < size; i++)
            out[i] = insert(data, indices, in(i + o2), (i - 1) % width, width, pos);
    }
    
    std::vector<T> m_data;
    std::vector<unsigned long> m_indices;
};
