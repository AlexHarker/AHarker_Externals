
#ifndef _DESCRIPTORS_FFT_PARAMS_HPP_
#define _DESCRIPTORS_FFT_PARAMS_HPP_

static long int_log2(long in)
{
    long count = 0;
    
    if (in <= 0)
        return -1;
    
    while (in >> count)
        count++;
    
    return (count && in == 1L << (count - 1L)) ? count - 1 : count;
}

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
        fft_size_log2 = DEFAULT_MAX_FFT_SIZE_LOG2;
    
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
    
    if (fft_size != 1 << fft_size_log2)
        object_error(x, "%s must be power of two - using %ld", str, 1 << fft_size_log2);
    
    return fft_size_log2;
}

template <class T>
void descriptors_fft_params_internal(T *x, long fft_size, long hop_size, long frame_size, t_symbol *window_type)
{
    long fft_size_log2 = check_fft_size((t_object *) x, "fft size", fft_size, x->max_fft_size_log2);
    
    x->params.m_fft_size_log2 = fft_size_log2;
    
    // Set hop size (default to overlap of two)
    
    x->params.m_hop_size = hop_size > 0 ? hop_size : x->params.fft_size() >> 1;
    x->hop_count = x->params.m_hop_size;
    
    // Set window size (default to fft size and clip at fft_size)
    
    x->params.m_frame_size = std::min(frame_size > 0 ? frame_size : x->params.fft_size(), x->params.fft_size());
    
    // FIX - error checks
    
    x->params.m_window_type = window_type;
    
    if (x->m_graph)
        x->m_graph->prepare(x->params);
    
    x->params.m_signal_length = x->params.m_frame_size;

    x->reset = true;
}

#endif /* _DESCRIPTORS_FFT_PARAMS_HPP_ */
