
#ifndef _DESCRIPTORS_FFT_PARAMS_HPP_
#define _DESCRIPTORS_FFT_PARAMS_HPP_

#include "descriptors_modules.hpp"

static long check_fft_size(t_object *x, const char *str, long fft_size, long max_fft_size_log2)
{
    constexpr long DEFAULT_MAX_FFT_SIZE_LOG2 = 16;
    constexpr long MIN_FFT_SIZE_LOG2 = 4;
    constexpr long MAX_FFT_SIZE_LOG2 = 20;
    
    bool use_defaults = max_fft_size_log2 <= 0;
    
    long fft_size_log2 = int_log2(fft_size);
    
    max_fft_size_log2 = use_defaults ? MAX_FFT_SIZE_LOG2 : max_fft_size_log2;
    
    // Check if the default is required
    
    if (use_defaults && fft_size_log2 < 0)
    {
        fft_size_log2 = DEFAULT_MAX_FFT_SIZE_LOG2;
        fft_size = 1 << fft_size_log2;
    }
    
    // Check for valid fft size
    
    if (fft_size_log2 < MIN_FFT_SIZE_LOG2)
    {
        fft_size_log2 = MIN_FFT_SIZE_LOG2;
        object_error(x, "%s is too small - using %ld", str, 1 << fft_size_log2);
    }
    
    if (fft_size_log2 > max_fft_size_log2)
    {
        fft_size_log2 = max_fft_size_log2;
        object_error(x,  "%s is too large - using %ld", str, 1 << fft_size_log2);
    }
    
    if (fft_size != (1 << fft_size_log2))
        object_error(x, "%s must be power of two - using %ld", str, 1 << fft_size_log2);
    
    return fft_size_log2;
}

static fft_params check_fft_params(t_object *x, long fft_size, long hop_size, long frame_size, t_symbol *window_type, long max_fft_size_log2)
{
    fft_params params;
    
    long fft_size_log2 = check_fft_size(x, "fft size", fft_size, max_fft_size_log2);
    
    // Set fft size
    // Set hop size (default to overlap of two)
    // Set window size (default to fft size and clip at fft_size)
    // Set window type
    
    params.m_fft_size_log2 = fft_size_log2;
    params.m_hop_size = hop_size > 0 ? hop_size : params.fft_size() >> 1;
    params.m_frame_size = std::min(frame_size > 0 ? frame_size : params.fft_size(), params.fft_size());
    params.m_window_type = window_type;
    
    // FIX - error checks
    
    return params;
}

#endif /* _DESCRIPTORS_FFT_PARAMS_HPP_ */
