
/*
 *  ibuffer_access.h
 *
 *	This header file provides an interface for accessing and interpolating samples from an ibuffer (or standard MSP buffer).
 *	You should also compile ibufffer_access.cpp in the project
 *
 *	Various kinds of interpolation are supported along with both SIMD calculation and both 32 bit and 64 bit floating point precision.
 *
 *	All pointers used with the SIMD routines should be 16-byte aligned and a multiple of 4 values wide (regardless of type).
 *  The SIMD routines currently always write a multiple of 4 values, although any extra values will be zero.
 *	Pointers for non-SIMD routines can be arbitrarily aligned.
 *
 *	The init routine should be called in an objects main function to setup the necessary variables.
 *	Use the ibuffer__get_ptr / ibuffer_info / ibuffer_sample_rate functions to get info about the ibuffer (or buffer).
 *	The buffer inuse field should be incremented and decremented beoore and after use using ibuffer_increment_inuse and ibuffer_decrement_inuse.
 *
 *	The main routines for accessing samples are then:
 *
 *	ibuffer_get_samp							- get a single sample
 *	ibuffer_get_samps							- get a number of consecutive samples in forward or reverse order
 *
 *	offsets is a pointer to the sample offsets
 *	fracts is a pointer to the subsample (fractional) offsets
 *	n_samps is the number of samples required.
 *	chan is the channel you wish to access.
 *
 *	Most of these routines have a mul input to multiply by a constant value.
 *	Other variables should be self-explanatory 
 *	
 *  Copyright 2010-13 Alex Harker. All rights reserved.
 *
 */


#ifndef _IBUFFER_ACCESS_
#define _IBUFFER_ACCESS_

#include "ibuffer.h"
#include <TableReader.hpp>

extern t_symbol *ps_none;
extern t_symbol *ps_linear;
extern t_symbol *ps_bspline;
extern t_symbol *ps_hermite;
extern t_symbol *ps_lagrange;
extern t_symbol *ps_buffer;
extern t_symbol *ps_ibuffer;

struct ibuffer_data {
    
    ibuffer_data() : samples(NULL), length(0), num_chans(0), format(0) {}
    
    void *samples;
    long length;
    long num_chans;
    long format;
    
    double sample_rate;
};

// Reading different formats

template <class T, int64_t bit_scale> struct fetch : public table_fetcher<float>
{
    fetch(const ibuffer_data& data, long chan)
    : table_fetcher(1.0 / ((int64_t) 1 << (bit_scale - 1))), samples(((T *) data.samples) + chan), num_chans(data.num_chans) {}
    
    T operator()(intptr_t offset) { return samples[offset * num_chans]; }
    double get(intptr_t offset) { return bit_scale != 1 ? operator()(offset) : scale * operator()(offset); }
    
    T *samples;
    long num_chans;
};

template<> struct fetch<int32_t, 24> : public table_fetcher<float>
{
    fetch(const ibuffer_data& data, long chan)
    : table_fetcher(1.0 / ((int64_t) 1 << 31)), samples(((uint8_t *) data.samples) + 3 * chan), num_chans(data.num_chans) {}
    
    int32_t operator()(intptr_t offset)
    {
        return (*reinterpret_cast<uint32_t *>(samples + (offset * 3 * num_chans - 1)) & 0xFFFFFF00);
    }
    double get(intptr_t offset) { return scale * operator()(offset); }
    
    uint8_t *samples;
    long num_chans;
};

typedef fetch<float, 1> fetch_float;
typedef fetch<int16_t, 16> fetch_16bit;
typedef fetch<int32_t, 24> fetch_24bit;
typedef fetch<int32_t, 32> fetch_32bit;

// Call in main routine to initialise buffer symbols

void ibuffer_init();

// Get the value of an individual sample

static inline double ibuffer_get_samp(const ibuffer_data& data, intptr_t offset, long chan);

// Get consecutive samples (and in reverse)

void ibuffer_get_samps(const ibuffer_data& data, float *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse = false);
void ibuffer_get_samps(const ibuffer_data& data, double *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse = false);

// Read with various forms of interpolation

void ibuffer_read(const ibuffer_data& data, float *out, double *positions, intptr_t n_samps, long chan, float mul, InterpType interp);
void ibuffer_read(const ibuffer_data& data, double *out, double *positions, intptr_t n_samps, long chan, double mul, InterpType interp);

        
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// Get ibuffer and related info //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline void *ibuffer_get_ptr(t_symbol *s)
{
    if (s)
    {
        t_object *b = s->s_thing;
	
        if (b && (ob_sym(b) == ps_ibuffer || ob_sym(b) == ps_buffer))
        {
            if (ob_sym(b) == ps_buffer)
                ATOMIC_INCREMENT(&((t_buffer *)b)->b_inuse);
            else
                ATOMIC_INCREMENT(&((t_ibuffer *)b)->inuse);
    
            return b;
        }
    }
    
    return NULL;
}

static inline void ibuffer_release_ptr(void *thebuffer)
{
    if (!thebuffer)
        return;
    
    if (ob_sym(thebuffer) == ps_buffer)
        ATOMIC_DECREMENT(&((t_buffer *)thebuffer)->b_inuse);
    else
        ATOMIC_DECREMENT(&((t_ibuffer *)thebuffer)->inuse);
}

static inline const ibuffer_data ibuffer_info(void *thebuffer)
{
    ibuffer_data data;
                 
	if (thebuffer)
    {
        if (ob_sym(thebuffer) == ps_buffer)
        {
            t_buffer *buffer = (t_buffer *) thebuffer;
            if (buffer->b_valid)
            {
                data.samples = (void *) buffer->b_samples;
                data.length = buffer->b_frames;
                data.num_chans = buffer->b_nchans;
                data.format = PCM_FLOAT;
                data.sample_rate = buffer->b_sr;
            }
        }
        else
        {
            t_ibuffer *buffer = (t_ibuffer *) thebuffer;
            if (buffer->valid)
            {
                data.samples =  buffer->samples;
                data.length = buffer->frames;
                data.num_chans = buffer->channels;
                data.format = buffer->format;
                data.sample_rate = buffer->sr;
            }
        }
    }

	return data;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// Get individual samples /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline double ibuffer_get_samp(const ibuffer_data& data, intptr_t offset, long chan)
{
    switch (data.format)
    {
        case PCM_INT_16:    return fetch_float(data, chan)(offset);
        case PCM_INT_24:    return fetch_24bit(data, chan)(offset);
        case PCM_INT_32:    return fetch_32bit(data, chan)(offset);
        case PCM_FLOAT:     return fetch_float(data, chan)(offset);
    }

    return 0.0;
}

#endif	/* _IBUFFER_ACCESS_ */
