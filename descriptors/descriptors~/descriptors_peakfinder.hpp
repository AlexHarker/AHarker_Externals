

#ifndef _PEAKFINDER_HPP_
#define _PEAKFINDER_HPP_

#include "descriptors_edges.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

template <class T>
class peak_finder
{
    static constexpr int pad_size = 4;

public:
    
    // Enums for options
    
    enum class Edges { Pad, Extend, Wrap, Fold, Mirror };
    enum class Refine { None, Parabolic, ParabolicLog  };
    enum class Boundaries { Midpoint, Minimum };

    struct parameters
    {
        Edges edges = Edges::Fold;
        Refine refine = Refine::ParabolicLog;
        Boundaries bounds = Boundaries::Midpoint;
        bool always_detect = false;
        int neighbours = 1;
        T threshold = 0.0;
        T pad_value = 0.0;
    };
    
    peak_finder(uintptr_t max_size)
    {
        m_padded_data.resize(padded_size(max_size));
        m_peaks.resize(max_size >> 1);
    }
    
    void detect(const T *data, uintptr_t size, const parameters& params = parameters())
    {
        int neighbours = std::min(std::max(1, params.neighbours), pad_size);
        
        // Reset Peaks
        
        reset();
        
        // If there is no data or the size is too big there are no peaks
        
        if (!size || size > max_size())
            return;
                                   
        // Copy input and prepare edges
            
        std::copy_n(data, size, stored_data());
        
        switch (params.edges)
        {
            case Edges::Pad:      copy_edges(edges_pad<T>(data, size, params.pad_value), size);     break;
            case Edges::Extend:   copy_edges(edges_extend<T>(data, size), size);                    break;
            case Edges::Wrap:     copy_edges(edges_wrap<T>(data, size), size);                      break;
            case Edges::Fold:     copy_edges(edges_fold<T>(data, size), size);                      break;
            case Edges::Mirror:   copy_edges(edges_mirror<T>(data, size), size);                    break;
        }
        
        // N.B. parabolic log interpolation can only work on +ve values
        
        double threshold = params.threshold;
        
        if (params.refine == Refine::ParabolicLog)
            threshold = std::max(0.0, threshold);
        
        // Find and store peaks using stored data
        
        switch (neighbours)
        {
            case 1:  find_peaks<check_for_peak<1>>(size, threshold);  break;
            case 2:  find_peaks<check_for_peak<2>>(size, threshold);  break;
            case 3:  find_peaks<check_for_peak<3>>(size, threshold);  break;
            case 4:  find_peaks<check_for_peak<4>>(size, threshold);  break;
        }
        
        // Find boundaries
        
        auto peaks = get_peaks();

        switch (params.bounds)
        {
            case Boundaries::Midpoint:
                for (unsigned long i = 0; i < num_peaks() - 1; i++)
                    peaks[i].m_end = (peaks[i].m_index + peaks[i].m_index) >> 1;
                break;
                
            case Boundaries::Minimum:
                for (unsigned long i = 0; i < num_peaks() - 1; i++)
                {
                    auto it = std::min_element(data + peaks[i].m_index + 1, data + peaks[i + 1].m_index);
                    peaks[i].m_end = iterator_delta(data, it);
                }
                break;
        }
        
        // Final end point
        
        peaks[num_peaks() - 1].m_end = size;
        
        // If required create a single peak at the maximum, (place central to multiple consecutive maxima)
        
        if (!num_peaks() && params.always_detect)
        {
            T *fwd1 = data;
            T *fwd2 = data + size;
            std::reverse_iterator<T *> rev1(data + size);
            std::reverse_iterator<T *> rev2(data);

            uintptr_t max = iterator_delta(data, std::max_element(fwd1, fwd2));
            uintptr_t beg = iterator_delta(fwd1, std::find(fwd1, fwd2, data[max]));
            uintptr_t end = size - (delta(rev1, std::find(rev1, rev2, data[max])) + 1UL);
            uintptr_t centre = (beg + end) >> 1;
            
            peaks[m_num_peaks++] = data[centre] == data[max] ? centre : max;
        }

        // Refine peaks

        switch (params.refine)
        {
            case Refine::None:                                                          break;
            case Refine::Parabolic:         refine_peaks<refine_parabolic>();           break;
            case Refine::ParabolicLog:      refine_peaks<refine_parabolic_log>();       break;
        }
    }
    
    T peak_value(uintptr_t idx) const { return get_peaks()[idx].m_value; }
    T peak_position(uintptr_t idx) const { return get_peaks()[idx].m_position; }
    uintptr_t peak_index(uintptr_t idx) const { return get_peaks()[idx].m_index; }
    uintptr_t peak_begin(uintptr_t idx) const { return idx ? get_peaks()[idx - 1].m_position : 0; }
    uintptr_t peak_end(uintptr_t idx) const { return get_peaks()[idx].m_end; }
    uintptr_t num_peaks() const { return m_num_peaks; }
    
private:
    
    // Peak Data
    
    struct peak_data
    {
        uintptr_t m_index;
        uintptr_t m_end;
        T m_position;
        T m_value;
    };
    
    // Iterator Helper
    
    template <typename U>
    uintptr_t iterator_delta(U a, U b)
    {
        return static_cast<unsigned long>(std::distance(a, b));
    }
    
    // Reset Peaks
    
    void reset()
    {
        m_num_peaks = 0;
    }

    // Edge Copying

    template <class U>
    void copy_edges(U data, unsigned long size)
    {
        T *stored_data = stored_data();

        for (int i = 0; i < pad_size; i++)
        {
            stored_data[-(i + 1)] = data(-(i + 1));
            stored_data[size + i] = data(size + i);
        }
    }
    
    // Peak Finding

    template <bool Func(const T *, double)>
    void find_peaks(uintptr_t size, T threshold)
    {
        const T *data = stored_data();
        
        auto peaks = get_peaks();
        
        for (uintptr_t i = 0; i < size; i++)
            if (Func(data++, threshold))
                peaks[m_num_peaks++] = { i, 0, i, *data };
    }

    template <int N>
    bool check_for_peak(const T *data, double threshold)
    {
        return check_for_peak<N-1>(data, threshold) && data[0] > data[-N] && data[0] > data[N];
    }

    template <>
    bool check_for_peak<1>(const T *data, double threshold)
    {
        return data[0] > threshold && data[0] > data[-1] && data[0] > data[1];
    }

    // Peak Refinement

    template <void Func(const T *, peak_data& peak)>
    void refine_peaks()
    {
        const T *data = stored_data();
        auto peaks = get_peaks();

        for (uintptr_t i = 0; i < num_peaks(); i++)
            Func(peaks[i]);
    }
    
    void refine_parabolic(const T *data, peak_data& peak)
    {
        uintptr_t idx = peak.m_position;
        parabolic_interp(peak.m_position, peak.m_value, idx, data[idx-1], data[idx], data[idx+1]);
    }

    void refine_parabolic_log(const T *data, peak_data& peak)
    {
        uintptr_t idx = peak.m_position;
        
        // Take log values (avoiding values that are too low) - doesn't work for negative values
        // N.B. we assume a max of -80dB difference between the samples either side to prevent extreme overshoot

        const double max_neighbour = std::max(data[idx-1], data[idx+1]);
        const double limit = std::max(max_neighbour * 0.0001, std::numeric_limits<double>::min());
        auto log_lim = [&](double x) { return log(std::max(x, limit)); };
        double value;
        
        parabolic_interp(value, peak.m_value, log_lim(data[idx-1]), log_lim(data[idx]), log_lim(data[idx+1]));
        peak.m_value = exp(value);
    }
    
    // Parabolic interpolator
    
    void parabolic_interp(double& position, double& value, double vm1, double v, double vp1)
    {
        const double divisor = vm1 + vp1 - (2.0 * v);
        const double correction = divisor ? (0.5 * (vm1 - vp1)) / divisor : 0.0;
        
        position += correction;
        value = v - (0.25 * (vm1 - vp1) * correction);
    }
    
    // Helpers
    
    uintptr_t padded_size(uintptr_t size) { return size - 2 * pad_size; }
    uintptr_t max_size() { return m_padded_data.size() - 2 * pad_size; }
    
    T *stored_data() { return m_padded_data.data() + pad_size; }
    peak_data *get_peaks() { return m_peaks.data(); };
    
    // Data

    uintptr_t m_num_peaks;

    std::vector<T> m_padded_data;
    std::vector<peak_data> m_peaks;
};

#endif /* _PEAKFINDER_HPP_ */
