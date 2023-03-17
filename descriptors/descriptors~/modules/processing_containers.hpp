
#ifndef _PROCESSING_CONTAINERS_HPP_
#define _PROCESSING_CONTAINERS_HPP_

#include <HISSTools_FFT/HISSTools_FFT.hpp>
#include <SIMDSupport.hpp>

// A Memory Aligned Vector

template <class T = double>
struct aligned_vector
{
    aligned_vector() : m_ptr(nullptr), m_size(0) {}
    aligned_vector(size_t size) : aligned_vector() { resize(size); }
    ~aligned_vector() { deallocate_aligned(m_ptr); }
       
    aligned_vector(const aligned_vector&) = delete;
    aligned_vector& operator=(const aligned_vector&) = delete;
    
    // Moveable
    
    aligned_vector(aligned_vector&& b)
    {
        m_ptr = b.m_ptr;
        m_size = b.m_size;
        b.m_ptr = nullptr;
    }
    
    aligned_vector& operator=(aligned_vector&& b)
    {
        m_ptr = b.m_ptr;
        m_size = b.m_size;
        b.m_ptr = nullptr;
        return *this;
    }
    
    void resize(size_t size)
    {
        if (size != m_size)
        {
            deallocate_aligned(m_ptr);
            m_ptr = allocate_aligned<T>(size);
            m_size = size;
        }
    }
    
    T *data() const { return m_ptr; }
    size_t size() const { return m_size; }
    
private:

    T *m_ptr;
    size_t m_size;
};

// An FFT Split

struct fft_split
{
    fft_split() {}
    fft_split(size_t size) : fft_split() { resize(size); }
    
    fft_split(const fft_split&) = delete;
    fft_split& operator=(const fft_split&) = delete;
    
    void resize(size_t size)
    {
        m_vector.resize(size);
        m_split.realp = m_vector.data();
        m_split.imagp = m_vector.data() + (size >> 1);
    }
    
    Split<double>& data() { return m_split; }
    const Split<double>& data() const { return m_split; }
    size_t size() const { return m_vector.size(); }

private:

    aligned_vector<> m_vector;
    Split<double> m_split;
};

// An FFT Setup

struct fft_setup
{
    fft_setup() : m_fft_setup(nullptr), m_size(0) {}
    fft_setup(size_t size) : fft_setup() { resize(size); }
    ~fft_setup() { hisstools_destroy_setup(m_fft_setup); }
    
    fft_setup(const fft_setup&) = delete;
    fft_setup& operator=(const fft_setup&) = delete;
    
    void resize(size_t size)
    {
        if (size != m_size)
        {
            hisstools_destroy_setup(m_fft_setup);
            hisstools_create_setup(&m_fft_setup, size);
            m_size = size;
        }
    }
    
    Setup<double>& get() { return m_fft_setup; }
    size_t size() const { return m_size; }

private:
    
    Setup<double> m_fft_setup;
    size_t m_size;
};

#endif /* _PROCESSING_CONTAINERS_HPP_ */
