
#ifndef _EDGES_HPP_
#define _EDGES_HPP_

#include <TableReader.hpp>

namespace impl
{
    // Underlying fetch class

    template <class T>
    struct edge_fetch : table_fetcher<T>
    {
        edge_fetch(const T *data, intptr_t size)
        : table_fetcher<T>(size, 1.0), m_data(data) {}

        T operator()(intptr_t idx) { return m_data[idx]; }

        const T *m_data;
    };

    // Base to hide unneeded functionality

    template <class T, template <class U> class fetch>
    struct edges_base
    {
        edges_base(const T *data, intptr_t size)
        : m_fetch(edge_fetch<T>(data, size)) {}

        T operator()(intptr_t idx) { return m_fetch(idx); }

    private:

        fetch<edge_fetch<T>> m_fetch;
    };
}

// edge types (padding type is explicit / others use table_reader.hpp)

template <class T>
struct edges_pad : private impl::edge_fetch<T>
{
    using fetch = impl::edge_fetch<T>;
    
    edges_pad(const T *data, intptr_t size, T padding)
    :  impl::edge_fetch<T>(data, size), m_padding(padding) {}

    T operator()(intptr_t idx)
    {
        return (idx >= 0 && idx < fetch::size) ?  fetch::m_data[idx] : m_padding;
    }

private:

    T m_padding;
};

template <class T>
using edges_extend  = impl::edges_base<T, table_fetcher_extend>;

template <class T>
using edges_wrap    = impl::edges_base<T, table_fetcher_wrap>;

template <class T>
using edges_fold    = impl::edges_base<T, table_fetcher_fold>;

template <class T>
using edges_mirror  = impl::edges_base<T, table_fetcher_mirror>;

#endif /* _EDGES_HPP_ */
