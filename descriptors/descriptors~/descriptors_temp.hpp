
// FFT params and Window Generation

long int_log2(long in, long *inexact)
{
    // Helper to calculate fft sizes log 2
    
    long temp = in;
    long out = 0;
    
    if (in <= 0)
        return - 1;
    
    while (temp)
    {
        temp >>= 1;
        out++;
    }
    
    if (in == 1 << (out - 1))
    {
        out--;
        *inexact = 0;
    }
    else
        *inexact = 1;
    
    return out;
}

#define DEFAULT_MAX_FFT_SIZE_LOG2 16
#define MIN_FFT_SIZE_LOG2 4
#define MAX_FFT_SIZE_LOG2 20

long descriptors_max_fft_size(t_descriptorsrt *x, long max_fft_size)
{
    // Returns the max fft size log 2 after checking range and value
    
    long inexact = 0;
    long max_fft_size_log2 = int_log2(max_fft_size, &inexact);
    
    if (max_fft_size_log2 < 0)
        max_fft_size_log2 = DEFAULT_MAX_FFT_SIZE_LOG2;

    if (max_fft_size_log2 > MAX_FFT_SIZE_LOG2)
    {
        object_error((t_object *) x, "maximum fft size too large - using %ld", 1 << MAX_FFT_SIZE_LOG2);
        max_fft_size_log2 = MAX_FFT_SIZE_LOG2;
    }
    
    if (max_fft_size_log2 && max_fft_size_log2 < MIN_FFT_SIZE_LOG2)
    {
        object_error((t_object *) x, "maximum fft size too small - using %ld", 1 << MIN_FFT_SIZE_LOG2);
        max_fft_size_log2 = MIN_FFT_SIZE_LOG2;
    }

    if (inexact)
        object_error((t_object *) x, "maximum fft size must be power of two - using %ld", 1 << max_fft_size_log2);

    return max_fft_size_log2;
}

void descriptors_fft_params_internal(t_descriptorsrt *x, long fft_size, long hop_size, long frame_size, t_symbol *window_type);

void descriptors_fft_params(t_descriptorsrt *x, t_symbol *msg, short argc, t_atom *argv)
{
    // Load in args as relevant
    
    long fft_size = (argc > 0) ? atom_getlong(argv + 0) : 0;
    long hop_size = (argc > 1) ? atom_getlong(argv + 1) : 0;
    long frame_size = (argc > 2) ? atom_getlong(argv + 2) : 0;
    t_symbol *window_type = (argc > 3) ? atom_getsym(argv + 3) : gensym("");
    
    // Ignore blank argument set (keep current values)
    
    if (argc < 0)
        return;
    
    descriptors_fft_params_internal(x, fft_size, hop_size, frame_size, window_type);
}
    
void descriptors_fft_params_internal(t_descriptorsrt *x, long fft_size, long hop_size, long frame_size, t_symbol *window_type)
{
    long inexact = 0;
    long fft_size_log2 = int_log2 (fft_size, &inexact);

    // Check for valid fft size
    
    if (fft_size_log2 < MIN_FFT_SIZE_LOG2)
    {
        fft_size_log2 = MIN_FFT_SIZE_LOG2;
        object_error((t_object *) x, "fft_size is too small - using %ld", 1 << fft_size_log2);
    }
    
    if (fft_size_log2 > x->max_fft_size_log2)
    {
        x->params.m_fft_size_log2 = fft_size_log2 = x->max_fft_size_log2;
        object_error((t_object *) x, "fft_size too large - using %ld", 1 << fft_size_log2);
    }
        
    if (inexact)
        object_error((t_object *) x, "fft_size must be power of two - using %ld", 1 << fft_size_log2);
    
    x->params.m_fft_size_log2 = fft_size_log2;

    // Set hop size (default to overlap of two)
    
    x->params.m_hop_size = hop_size > 0 ? hop_size : x->params.fft_size() >> 1;
    x->hop_count = x->params.m_hop_size;
    
    // Set window size (default to fft size and clip at fft_size)
    
    x->params.m_frame_size = std::min(frame_size > 0 ? frame_size : x->params.fft_size(), x->params.fft_size());

    if (x->m_graph)
        x->m_graph->prepare(x->params);
        
    x->reset = true;
}
