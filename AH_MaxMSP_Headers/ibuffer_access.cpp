
/*
 *  ibuffer_access.cpp
 *
 *	This file provides code for accessing and interpolating samplesfrom an ibuffer (or standard MSP buffer).
 *	Various kinds of interpolation are supported.
 *	All pointers used should be 16-byte aligned.
 *
 *	See the accompanying header file for more details.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "ibuffer_access.hpp"

t_symbol *ps_none;
t_symbol *ps_linear;
t_symbol *ps_bspline;
t_symbol *ps_hermite;
t_symbol *ps_lagrange;
t_symbol *ps_buffer;
t_symbol *ps_ibuffer;

void ibuffer_init()
{
    ps_buffer = gensym("buffer~");
    ps_ibuffer = gensym("ibuffer~");
    ps_none = gensym("none");
    ps_linear = gensym("linear");
    ps_bspline = gensym("bspline");
    ps_hermite = gensym("hermite");
    ps_lagrange = gensym("lagrange");    
}

template <class T>
void ibuffer_read_format(const ibuffer_data& data, T *out, double *positions, intptr_t n_samps, long chan, T mul, InterpType interp)
{
    switch(data.format)
    {
        case PCM_FLOAT:     table_read(fetch_float(data, chan), out, positions, n_samps, mul, interp);    break;
        case PCM_INT_16:    table_read(fetch_16bit(data, chan), out, positions, n_samps, mul, interp);    break;
        case PCM_INT_24:    table_read(fetch_24bit(data, chan), out, positions, n_samps, mul, interp);    break;
        case PCM_INT_32:    table_read(fetch_32bit(data, chan), out, positions, n_samps, mul, interp);    break;
    }
}

void ibuffer_read(const ibuffer_data& data, double *out, double *positions, intptr_t n_samps, long chan, double mul, InterpType interp)
{
    ibuffer_read_format<double>(data, out, positions, n_samps, chan, mul, interp);
}

void ibuffer_read(const ibuffer_data& data, float *out, double *positions, intptr_t n_samps, long chan, float mul, InterpType interp)
{
    ibuffer_read_format<float>(data, out, positions, n_samps, chan, mul, interp);
}

template <class T, class Ft> void ibuffer_get_samps_loop(Ft fetch, T *out, intptr_t offset, intptr_t n_samps, bool reverse)
{
	if (reverse)
    {
        for (intptr_t i = n_samps - 1; i >= 0; i--)
            *out++ = fetch(offset + i);
    }
    else
    {
        for (intptr_t i = 0; i < n_samps; i++)
            *out++ = fetch(offset + i);
    }
}

template <class T> void ibuffer_get_samps(const ibuffer_data& data, T *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse)
{
    switch(data.format)
    {
        case PCM_FLOAT:     ibuffer_get_samps_loop(fetch_float(data, chan), out, offset, n_samps, reverse);     break;
        case PCM_INT_16:    ibuffer_get_samps_loop(fetch_16bit(data, chan), out, offset, n_samps, reverse);     break;
        case PCM_INT_24:    ibuffer_get_samps_loop(fetch_24bit(data, chan), out, offset, n_samps, reverse);     break;
        case PCM_INT_32:    ibuffer_get_samps_loop(fetch_32bit(data, chan), out, offset, n_samps, reverse);     break;
    }
}

void ibuffer_get_samps(const ibuffer_data& data, float *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse)
{
    ibuffer_get_samps<float>(data, out, offset, n_samps, chan, reverse);
}

void ibuffer_get_samps(const ibuffer_data& data, double *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse)
{
    ibuffer_get_samps<double>(data, out, offset, n_samps, chan, reverse);
}
