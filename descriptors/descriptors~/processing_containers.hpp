
#ifndef _PROCESSING_CONTAINERS_HPP_
#define _PROCESSING_CONTAINERS_HPP_

#include <HISSTools_FFT/HISSTools_FFT.h>
#include <SIMDSupport.hpp>

// A Memory Aligned Vector

struct aligned_vector
{
    aligned_vector() : m_ptr(nullptr), m_size(0) {}
    aligned_vector(size_t size) : aligned_vector() { resize(size); }
    ~aligned_vector() { deallocate_aligned(m_ptr); }
        
    void resize(size_t size)
    {
        if (size != m_size)
        {
            deallocate_aligned(m_ptr);
            m_ptr = allocate_aligned<double>(size);
            m_size = size;
        }
    }
    
    double *data() const { return m_ptr; }
    size_t size() const { return m_size; }
    
private:

    double *m_ptr;
    size_t m_size;
};

// An FFT Split

struct fft_split
{
    fft_split() {}
    fft_split(size_t size) : fft_split() { resize(size); }
    
    void resize(size_t size)
    {
        m_vector.resize(size);
        m_split.realp = m_vector.data();
        m_split.imagp = m_vector.data() + (size >> 1);
    }
    
    FFT_SPLIT_COMPLEX_D& data() { return m_split; }
    const FFT_SPLIT_COMPLEX_D& data() const { return m_split; }
    size_t size() const { return m_vector.size(); }

private:

    aligned_vector m_vector;
    FFT_SPLIT_COMPLEX_D m_split;
};

// An FFT Setup

struct fft_setup
{
    fft_setup() : m_size(0) {}
    fft_setup(size_t size) : fft_setup() { resize(size); }
    ~fft_setup() { hisstools_destroy_setup(m_fft_setup); }
    
    void resize(size_t size)
    {
        if (size != m_size)
        {
            hisstools_destroy_setup(m_fft_setup);
            hisstools_create_setup(&m_fft_setup, size);
            m_size = size;
        }
    }
    
    FFT_SETUP_D& get() { return m_fft_setup; }
    size_t size() const { return m_size; }

private:
    
    FFT_SETUP_D m_fft_setup;
    size_t m_size;
};

#endif /* _PROCESSING_CONTAINERS_HPP_ */
