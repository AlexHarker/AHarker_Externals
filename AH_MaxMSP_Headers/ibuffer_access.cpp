
/*
 *  ibuffer_access.cpp
 *
 *	This file provides code for accessing and interpolating samples from an ibuffer (or standard MSP buffer).
 *	Various kinds of interpolation are supported.
 *	All pointers used should be 16-byte aligned.
 *
 *	See the accompanying header file for more details.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include "ibuffer_access.hpp"

// IBuffer Proxy

ibuffer_data::ibuffer_data(t_symbol *name) : buffer_type(kBufferNone), samples(NULL), length(0), num_chans(0), format(0), sample_rate(0.0)
{
    buffer_object = name ? name->s_thing : NULL;
    
    if (buffer_object)
    {
        if (ob_sym(buffer_object) == ps_buffer)
        {
            t_buffer *buffer = reinterpret_cast<t_buffer *>(buffer_object);
            buffer_type = kBufferMaxBuffer;
            
            if (buffer->b_valid)
            {
                ATOMIC_INCREMENT(&buffer->b_inuse);
                
                samples = (void *) buffer->b_samples;
                length = buffer->b_frames;
                num_chans = buffer->b_nchans;
                format = PCM_FLOAT;
                sample_rate = buffer->b_sr;
            }
        }
        
        if (ob_sym(buffer_object) == ps_ibuffer)
        {
            t_ibuffer *buffer = reinterpret_cast<t_ibuffer *>(buffer_object);
            buffer_type = kBufferIBuffer;
            
            if (buffer->valid)
            {
                ATOMIC_INCREMENT(&buffer->inuse);
                
                samples =  buffer->samples;
                length = buffer->frames;
                num_chans = buffer->channels;
                format = buffer->format;
                sample_rate = buffer->sr;
            }
        }
    }
}

ibuffer_data::~ibuffer_data()
{
    release_buffer();
}

void ibuffer_data::set_dirty()
{
    if (buffer_type == kBufferMaxBuffer)
        object_method(buffer_object, gensym("dirty"));
}

void ibuffer_data::set_size_in_samples(t_atom_long size)
{
    t_atom temp_atom[2];
    
    atom_setlong(temp_atom, size);
    
    if (buffer_type == kBufferMaxBuffer)
    {
        t_buffer *buffer = reinterpret_cast<t_buffer *>(buffer_object);
        
        ATOMIC_INCREMENT(&(buffer)->b_inuse);
        object_method_typed(buffer_object, gensym("sizeinsamps"), 1, temp_atom, temp_atom + 1);
        ATOMIC_DECREMENT(&(buffer)->b_inuse);
        
        samples = (void *) buffer->b_samples;
        length = buffer->b_frames;
    }
}

void ibuffer_data::release()
{
    release_buffer();
    buffer_type = kBufferNone;
    samples = NULL;
    length = 0;
    num_chans = 0;
    format = 0;
    sample_rate = 0.0;
    buffer_object = NULL;
}

void ibuffer_data::release_buffer()
{
    if (buffer_object)
    {
        if (ob_sym(buffer_object) == ps_buffer)
            ATOMIC_DECREMENT(&((t_buffer *)buffer_object)->b_inuse);
        
        if (ob_sym(buffer_object) == ps_ibuffer)
            ATOMIC_DECREMENT(&((t_ibuffer *)buffer_object)->inuse);
    }
}

t_symbol *ibuffer_data::ps_buffer = gensym("buffer~");
t_symbol *ibuffer_data::ps_ibuffer = gensym("ibuffer~");

// Functions

template <class T, class U>
void ibuffer_read_format(const ibuffer_data& buffer, T *out, U *positions, intptr_t n_samps, long chan, T mul, InterpType interp)
{
    switch(buffer.get_format())
    {
        case PCM_FLOAT:     table_read(fetch_float(buffer, chan), out, positions, n_samps, mul, interp);    break;
        case PCM_INT_16:    table_read(fetch_16bit(buffer, chan), out, positions, n_samps, mul, interp);    break;
        case PCM_INT_24:    table_read(fetch_24bit(buffer, chan), out, positions, n_samps, mul, interp);    break;
        case PCM_INT_32:    table_read(fetch_32bit(buffer, chan), out, positions, n_samps, mul, interp);    break;
    }
}

void ibuffer_read(const ibuffer_data& buffer, double *out, double *positions, intptr_t n_samps, long chan, double mul, InterpType interp)
{
    ibuffer_read_format<double>(buffer, out, positions, n_samps, chan, mul, interp);
}

void ibuffer_read(const ibuffer_data& buffer, float *out, double *positions, intptr_t n_samps, long chan, float mul, InterpType interp)
{
    ibuffer_read_format<float>(buffer, out, positions, n_samps, chan, mul, interp);
}

void ibuffer_read(const ibuffer_data& buffer, float *out, float *positions, intptr_t n_samps, long chan, float mul, InterpType interp)
{
    ibuffer_read_format<float>(buffer, out, positions, n_samps, chan, mul, interp);
}

template <class T, class Ft> void ibuffer_get_samps_loop(Ft fetch, T *out, intptr_t offset, intptr_t n_samps, bool reverse)
{
	if (reverse)
    {
        for (intptr_t i = n_samps - 1; i >= 0; i--)
            *out++ = fetch.get(offset + i);
    }
    else
    {
        for (intptr_t i = 0; i < n_samps; i++)
            *out++ = fetch.get(offset + i);
    }
}

template <class T> void ibuffer_get_samps(const ibuffer_data& buffer, T *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse)
{
    switch(buffer.get_format())
    {
        case PCM_FLOAT:     ibuffer_get_samps_loop(fetch_float(buffer, chan), out, offset, n_samps, reverse);     break;
        case PCM_INT_16:    ibuffer_get_samps_loop(fetch_16bit(buffer, chan), out, offset, n_samps, reverse);     break;
        case PCM_INT_24:    ibuffer_get_samps_loop(fetch_24bit(buffer, chan), out, offset, n_samps, reverse);     break;
        case PCM_INT_32:    ibuffer_get_samps_loop(fetch_32bit(buffer, chan), out, offset, n_samps, reverse);     break;
    }
}

void ibuffer_get_samps(const ibuffer_data& buffer, float *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse)
{
    ibuffer_get_samps<float>(buffer, out, offset, n_samps, chan, reverse);
}

void ibuffer_get_samps(const ibuffer_data& buffer, double *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse)
{
    ibuffer_get_samps<double>(buffer, out, offset, n_samps, chan, reverse);
}
