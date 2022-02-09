
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


#include "descriptors_sort.hpp"

#include <TableReader.hpp>

// Enum for options (if implementing all)

enum Edges { kEdgePad, kEdgeExtend, kEdgeWrap, kEdgeFold, kEdgeMirror };

// Underlying fetch class

struct edge_fetch : table_fetcher<double>
{
    edge_fetch(const double *data, unsigned long size)
    : table_fetcher<double>(static_cast<intptr_t>(size), 1.0), m_data(data) {}

    double operator()(intptr_t idx) { return m_data[idx]; }

    const double *m_data;
};

// base to hide unneeded functionality

template <template <class t> class fetch>
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
        return (idx >= 0 && idx < edge_fetch::size) ? m_data[idx] : m_padding;
    }

private:

    double m_padding;
};

struct edges_extend : edges_base<table_fetcher_extend>
{
    edges_extend(const double *data, unsigned long size)
    : edges_base<table_fetcher_extend>(edges_base(data, size)) {}
};

struct edges_wrap : edges_base<table_fetcher_wrap>
{
    edges_wrap(const double *data, unsigned long size)
    : edges_base<table_fetcher_wrap>(edges_base(data, size)) {}
};

struct edges_fold : edges_base<table_fetcher_fold>
{
    edges_fold(const double *data, unsigned long size)
    : edges_base<table_fetcher_fold>(edges_base(data, size)) {}
};

struct edges_mirror : edges_base<table_fetcher_mirror>
{
    edges_mirror(const double *data, unsigned long size)
    : edges_base<table_fetcher_mirror>(edges_base(data, size)) {}
};

// Helpers

double insert(double *data, unsigned long *indices, double value, long index, long width, unsigned long pos)
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

template <class T>
void filter(const T in, double *out, double *data, unsigned long* indices, long width, long size, unsigned long pos)
{
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

// Process

void percentilefilter(double *output, const double *input, long size, double percentile, long width, Edges edges, double pad = 0.0)
{
    unsigned long pos = roundToUInt(percentile * (width - 1) / 100.0);
    pos = std::min(pos, static_cast<unsigned long>(width - 1));

    // Temporary Memory

    auto data = allocAutoArray<double>(width);
    auto indices = allocAutoArray<unsigned long>(width);

    switch (edges)
    {
        case kEdgePad:      filter(edges_pad(input, size, pad), output, data, indices, width, size, pos);    break;
        case kEdgeExtend:   filter(edges_extend(input, size), output, data, indices, width, size, pos);      break;
        case kEdgeWrap:     filter(edges_wrap(input, size), output, data, indices, width, size, pos);        break;
        case kEdgeFold:     filter(edges_fold(input, size), output, data, indices, width, size, pos);        break;
        case kEdgeMirror:   filter(edges_mirror(input, size), output, data, indices, width, size, pos);      break;
    }
}
