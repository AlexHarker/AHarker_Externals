
// FFT params and Window Generation

long int_log2(long in)
{
    long count = 0;
    
    if (in <= 0)
        return -1;
    
    while (in >> count)
        count++;
    
    return (count && in == 1L << (count - 1L)) ? count - 1 : count;
}

long check_fft_size(t_object *x, const char *str, long fft_size, long max_fft_size_log2, bool use_default)
{
    constexpr long DEFAULT_MAX_FFT_SIZE_LOG2 = 16;
    constexpr long MIN_FFT_SIZE_LOG2 = 4;
    constexpr long MAX_FFT_SIZE_LOG2 = 20;
    
    long fft_size_log2 = int_log2(fft_size);
    
    max_fft_size_log2 = max_fft_size_log2 ? max_fft_size_log2 : MAX_FFT_SIZE_LOG2;
    
    // Check if the default is required
    
    if (use_default && fft_size_log2 < 0)
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
    long fft_size_log2 = check_fft_size((t_object *) x, "fft size", fft_size, x->max_fft_size_log2, false);
    
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
    
    x->reset = true;
}

template <class T>
void descriptors_fft_params(T *x, t_symbol *msg, short argc, t_atom *argv)
{
    // Ignore blank argument set (keep current values)
    
    if (argc < 0)
        return;
    
    // Load in args as relevant
    
    long fft_size = (argc > 0) ? atom_getlong(argv + 0) : 0;
    long hop_size = (argc > 1) ? atom_getlong(argv + 1) : 0;
    long frame_size = (argc > 2) ? atom_getlong(argv + 2) : 0;
    t_symbol *window_type = (argc > 3) ? atom_getsym(argv + 3) : gensym("");
    
    descriptors_fft_params_internal(x, fft_size, hop_size, frame_size, window_type);
    
    x->params.m_signal_length = x->params.m_frame_size;
}
