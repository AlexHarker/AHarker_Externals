

#ifndef _PEAKFINDER_HPP_
#define _PEAKFINDER_HPP_

#include "descriptors_edges.hpp"
#include "descriptors_sort.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

template <class T>
class peak_finder;

template <class T>
class peak_set
{
    friend peak_finder<T>;
    
public:
    
    struct peak_data
    {
        uintptr_t m_index;
        uintptr_t m_begin;
        uintptr_t m_end;
        T m_position;
        T m_value;
        
        bool operator > (const peak_data& b) const { return m_value > b.m_value; }
        bool operator < (const peak_data& b) const { return m_value < b.m_value; }
    };
    
    peak_set(uintptr_t max_size)
    {
        m_peaks.resize(max_size);
        m_indices.resize(max_size);
    }

    uintptr_t num_peaks() const { return m_num_peaks; }
    
    const peak_data& by_value(size_t idx) const { return m_peaks[m_indices[idx]]; }
    const peak_data& operator [](size_t idx) const { return m_peaks[idx]; }

private:
    
    // Modifying contents

    void clear() { m_num_peaks = 0; }
    size_t capacity() { return m_peaks.size(); }

    peak_data& operator [](size_t idx) { return m_peaks[idx]; }

    void add_peak(const peak_data& peak)
    {
        if (num_peaks() < capacity())
            m_peaks[m_num_peaks++] = peak;
    }
    
    void complete()
    {
        sort_descending(m_indices, m_peaks.data(), m_num_peaks);
    };

    // Data
    
    uintptr_t m_num_peaks;
    std::vector<uintptr_t> m_indices;
    std::vector<peak_data> m_peaks;
};

template <class T>
class peak_finder
{
    enum constants : int { pad_size = 4 };
    
    using peak_data = typename peak_set<T>::peak_data;
    
public:
    
    // Enums for options
    
    enum class Edges { Pad, Extend, Wrap, Fold, Mirror };
    enum class Refine { None, Parabolic, ParabolicLog  };
    enum class Boundaries { Midpoint, Minimum };

    struct options
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
    }
    
    void operator()(peak_set<T>& peaks, const T *data, uintptr_t size, const options& params = options())
    {
        detect(peaks, data, nullptr, size, params);
    }
    
    void operator()(peak_set<T>& peaks, const T *data, const T *mask, uintptr_t size, const options& params = options())
    {
        detect(peaks, data, mask, size, params);
    }
    
private:
    
    void detect(peak_set<T>& peaks, const T *data, const T *mask, uintptr_t size, const options& params)
    {
        int neighbours = std::min(std::max(1, params.neighbours), static_cast<int>(pad_size));
        
        // Clear Peaks
        
        peaks.clear();
        
        // If there is no data or the size is too big there are no peaks
        
        if (!size || size > max_size() || peaks.capacity() < (size / (neighbours + 1)))
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
            case 1:  find_peaks<check_for_peak<1>>(peaks, size, mask, threshold);  break;
            case 2:  find_peaks<check_for_peak<2>>(peaks, size, mask, threshold);  break;
            case 3:  find_peaks<check_for_peak<3>>(peaks, size, mask, threshold);  break;
            case 4:  find_peaks<check_for_peak<4>>(peaks, size, mask, threshold);  break;
        }
        
        // Find boundaries (note by default all boundaries span the entire space)
        
        switch (params.bounds)
        {
            case Boundaries::Midpoint:
                for (uintptr_t i = 0; i < peaks.num_peaks() - 1; i++)
                    peaks[i + 1].m_begin = peaks[i].m_end = (peaks[i].m_index + peaks[i].m_index) >> 1;
                break;
                
            case Boundaries::Minimum:
                for (uintptr_t i = 0; i < peaks.num_peaks() - 1; i++)
                {
                    auto it = std::min_element(data + peaks[i].m_index + 1, data + peaks[i + 1].m_index);
                    peaks[i + 1].m_begin = peaks[i].m_end = iterator_delta(data, it);
                }
                break;
        }
                        
        // If required create a single peak at the maximum, (place central to multiple consecutive maxima)
        
        if (!peaks.num_peaks() && params.always_detect)
        {
            std::reverse_iterator<const T *> rev1(data + size);
            std::reverse_iterator<const T *> rev2(data);

            auto max_it = std::max_element(data, data + size);
            
            uintptr_t beg = iterator_delta(data, max_it);
            uintptr_t end = size - (iterator_delta(rev1, std::find(rev1, rev2, data[beg])) + 1UL);
            uintptr_t centre = (beg + end) >> 1;
            
            if (data[centre] == data[beg])
                peaks.add_peak({ centre, 0, size, static_cast<double>(centre), *max_it });
            else
                peaks.add_peak({ beg, 0, size, static_cast<double>(beg), *max_it });
        }

        // Refine peaks

        switch (params.refine)
        {
            case Refine::None:                                                              break;
            case Refine::Parabolic:         refine_peaks<refine_parabolic>(peaks);          break;
            case Refine::ParabolicLog:      refine_peaks<refine_parabolic_log>(peaks);      break;
        }
        
        peaks.complete();
    }
        
    // Iterator Helper
    
    template <typename U>
    uintptr_t iterator_delta(U a, U b)
    {
        return static_cast<uintptr_t>(std::distance(a, b));
    }
    
    // Edge Copying

    template <class U>
    void copy_edges(U data, uintptr_t size)
    {
        T *stored = stored_data();

        for (int i = 0; i < pad_size; i++)
        {
            stored[-(i + 1)] = data(-(i + 1));
            stored[size + i] = data(size + i);
        }
    }
    
    // Peak Finding

    template <bool Func(const T *, double)>
    void find_peaks(peak_set<T>& peaks, uintptr_t size, const T *mask, T threshold)
    {
        const T *data = stored_data();
                
        if (mask)
        {
            for (uintptr_t i = 0; i < size; i++)
                if (Func(data++, *mask++))
                    peaks.add_peak({ i, 0, size, static_cast<double>(i), *data });
        }
        else
        {
            for (uintptr_t i = 0; i < size; i++)
                if (Func(data++, threshold))
                    peaks.add_peak({ i, 0, size, static_cast<double>(i), *data });
        }
    }

    template <int N>
    static bool check_for_peak(const T *data, double threshold)
    {
        return check_for_peak<N-1>(data, threshold) && data[0] > data[-N] && data[0] > data[N];
    }

    template <>
    static bool check_for_peak<1>(const T *data, double threshold)
    {
        return data[0] > threshold && data[0] > data[-1] && data[0] > data[1];
    }

    // Peak Refinement

    template <void Func(peak_data& peak, const T *)>
    void refine_peaks(peak_set<T>& peaks)
    {
        const T *data = stored_data();

        for (uintptr_t i = 0; i < peaks.num_peaks(); i++)
            Func(peaks[i], data);
    }
    
    static void refine_parabolic(peak_data& peak, const T *data)
    {
        uintptr_t idx = peak.m_position;
        parabolic_interp(peak.m_position, peak.m_value, data[idx-1], data[idx], data[idx+1]);
    }

    static void refine_parabolic_log(peak_data& peak, const T *data)
    {
        uintptr_t idx = peak.m_position;
        
        // Take log values (avoiding values that are too low) - doesn't work for negative values
        // N.B. we assume a max of -80dB difference between the samples either side to prevent extreme overshoot

        const double max_neighbour = std::max(data[idx-1], data[idx+1]);
        const double limit = std::max(max_neighbour * 0.0001, std::numeric_limits<double>::min());
        auto log_lim = [&](double x) { return std::log(std::max(x, limit)); };
        double value;
        
        parabolic_interp(peak.m_position, value, log_lim(data[idx-1]), log_lim(data[idx]), log_lim(data[idx+1]));
        peak.m_value = std::exp(value);
    }
    
    // Parabolic interpolator
    
    static void parabolic_interp(double& position, double& value, double vm1, double v, double vp1)
    {
        const double divisor = vm1 + vp1 - (2.0 * v);
        const double correction = divisor ? (0.5 * (vm1 - vp1)) / divisor : 0.0;
        
        position += correction;
        value = v - (0.25 * (vm1 - vp1) * correction);
    }
    
    // Helpers
    
    uintptr_t padded_size(uintptr_t size) { return size + 2 * pad_size; }
    uintptr_t max_size() { return m_padded_data.size() - 2 * pad_size; }
    
    T *stored_data() { return m_padded_data.data() + pad_size; }
    
    // Data

    std::vector<T> m_padded_data;
};

#endif /* _PEAKFINDER_HPP_ */
