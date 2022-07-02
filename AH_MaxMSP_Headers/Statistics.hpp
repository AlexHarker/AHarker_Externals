
#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <algorithm>
#include <limits>
#include <numeric>
#include <cmath>

namespace impl
{
    // Helper functors

    struct pow2         { template <class T> T operator()(T a) const { return a * a; } };
    struct pow3         { template <class T> T operator()(T a) const { return a * a * a; } };
    struct pow4         { template <class T> T operator()(T a) const { return pow2()(pow2()(a)); } };
    struct absolute     { template <class T> T operator()(T a) const { return std::abs(a); } };
    struct logarithm    { template <class T> T operator()(T a) const { return log(a); } };

    struct indices      { template <class T> double operator[](T a) const { return static_cast<double>(a); } };
    struct log_indices  { template <class T> double operator[](T a) const { return a ? log2(static_cast<double>(a)) : 0.0; } };

    template <class T, typename Op>
    struct modified_data
    {
        modified_data(T& data) : m_data(data) {}
        double operator[](size_t i) const { return Op()(m_data[i]); }
        const T m_data;
    };

    template <class T, typename Op>
    struct modified_diff_data
    {
        modified_diff_data(T& data, double value) : m_data(data), m_value(value) {}
        double operator[](size_t i) const { return Op()(m_data[i] - m_value); }
        const T m_data;
        double m_value;
    };

    template <typename Op>
    struct indices_diff_op
    {
        indices_diff_op(double value) : m_value(value) {}
        double operator[](size_t i) const { return Op()(static_cast<double>(i) - m_value); }
        double m_value;
    };

    template <typename Op>
    struct log_indices_diff_op
    {
        log_indices_diff_op(double value) : m_value(value) {}
        double operator[](size_t i) const { return Op()(log_indices()[i] - m_value); }
        double m_value;
    };

    template <class T, typename Op>
    struct fixed_compare
    {
        fixed_compare(T value) : m_value(value) {}
        bool operator()(T a) { return Op()(a, m_value); }
        T m_value;
    };
}

// Length

template <class T>
double stat_length(const T input, size_t size)
{
    return static_cast<double>(size);
}

// Min / Max Values

template <class T>
double stat_min(const T input, size_t size)
{
    return size ? *(std::min_element(input, input + size)) : std::numeric_limits<double>::infinity();
}

template <class T>
double stat_max(const T input, size_t size)
{
    return size ? *(std::max_element(input, input + size)) : -std::numeric_limits<double>::infinity();
}

// Min and Max Positions

template <class T>
double stat_max_position(const T input, size_t size)
{
    return size ? std::distance(input, std::max_element(input, input + size)) : -std::numeric_limits<double>::infinity();
}

template <class T>
double stat_min_position(const T input, size_t size)
{
    return size ? std::distance(input, std::min_element(input, input + size)) : -std::numeric_limits<double>::infinity();
}

// Counts

template <class T, typename CountOp>
double stat_count(const T input, size_t size, CountOp op)
{
    size_t count = 0;
    
    for (size_t i; i < size; i++)
        if (op(input[i]))
            count++;
    
    return count;
}

template <class T, class U>
double stat_count_above(const T input, U threshold, size_t size)
{
    return stat_count(input, size, impl::fixed_compare<U, std::greater<U>>(threshold));
}

template <class T, class U>
double stat_count_below(const T input, U threshold, size_t size)
{
    return stat_count(input, size, impl::fixed_compare<U, std::less<U>>(threshold));
}

// Ratios

template <class T, class U>
double stat_ratio_above(const T input, U threshold, size_t size)
{
    return stat_count_above(input, threshold, size) / stat_length(input, size);
}

template <class T, class U>
double stat_ratio_below(const T input, U threshold, size_t size)
{
    return stat_count_below(input, threshold, size) / stat_length(input, size);
}

// Sums

template <class T>
double stat_sum(const T input, size_t size)
{
    double sum = 0.0;
    
    for (size_t i = 0; i < size; i++)
        sum += input[i];
    
    return sum;
}

template <class T>
double stat_sum_abs(const T input, size_t size)
{
    return stat_sum(impl::modified_data<const T, impl::absolute>(input), size);
}

template <class T>
double stat_sum_squares(const T input, size_t size)
{
    return stat_sum(impl::modified_data<const T, impl::pow2>(input), size);
}

template <class T>
double stat_sum_logs(const T input, size_t size)
{
    return stat_sum(impl::modified_data<const T, impl::logarithm>(input), size);
}

// Weighted Sums

template <class T, class U>
double stat_weighted_sum(const T data, const U weights, size_t size)
{
    double sum = 0.0;

    for (size_t i = 0; i < size; i++)
        sum += weights[i] * data[i];

    return sum;
}

template <class T>
double stat_weighted_sum(const T input, size_t size)
{
    return stat_weighted_sum(impl::indices(), input, size);
}

template <class T>
double stat_weighted_sum_abs(const T input, size_t size)
{
    return stat_weighted_sum(impl::indices(), impl::modified_data<T, impl::absolute>(input), size);
}

template <class T>
double stat_weighted_sum_squares(const T input, size_t size)
{
    return stat_weighted_sum(impl::indices(), impl::modified_data<T, impl::pow2>(input), size);
}

template <class T>
double stat_weighted_sum_logs(const T input, size_t size)
{
    return stat_weighted_sum(impl::indices(), impl::modified_data<T, impl::logarithm>(input), size);
}

// Weighted Sums (by weights)

template <class T>
double stat_weighted_sum_abs(const T input, const T weights, size_t size)
{
    return stat_weighted_sum(impl::modified_data<const T, impl::absolute>(input), weights, size);
}

template <class T>
double stat_weighted_sum_squares(const T input, const T weights, size_t size)
{
    return stat_weighted_sum(impl::modified_data<const T, impl::pow2>(input), weights, size);
}

template <class T>
double stat_weighted_sum_logs(const T input, const T weights, size_t size)
{
    return stat_weighted_sum(impl::modified_data<const T, impl::logarithm>(input), weights, size);
}

// Product

template <class T>
double stat_product(const T input, size_t size)
{
    double product = 1.0;
    
    for (size_t i = 0; i < size; i++)
        product *= input[i];
    
    return product;
}

// Means

template <class T>
double stat_mean(const T input, size_t size)
{
    return stat_sum(input, size) / stat_length(input, size);
}

template <class T>
double stat_mean_squares(const T input, size_t size)
{
    return stat_sum_squares(input, size) / stat_length(input, size);
}

template <class T>
double stat_geometric_mean(const T input, size_t size)
{
    return exp(stat_sum_logs(input, size) / stat_length(input, size));
}

// Variance

template <class T>
double stat_variance(const T input, size_t size)
{
    double mean = stat_mean(input, size);
    return stat_sum(impl::modified_diff_data<const T, impl::pow2>(input, mean), size) / stat_length(input, size);
}
                           
// Standard Deviation

template <class T>
double stat_standard_deviation(const T input, size_t size)
{
    return sqrt(stat_variance(input, size));
}

// PDF Percentile

template <class T>
double stat_pdf_percentile(const T input, double centile, size_t size)
{
    double target = stat_sum(input, size) * std::min(100.0, std::max(centile, 0.0)) / 100.0;
    
    double sum = 0.0;
    
    for (size_t i = 0; i < size; i++)
    {
        sum += input[i];
        if (sum >= target)
            return static_cast<double>(i - ((sum - target) / input[i]));
    }
    
    return static_cast<double>(size - 1);
}

// Shape

template <class T>
double stat_centroid(const T input, size_t size)
{
    return stat_weighted_sum(input, size) / stat_sum(input, size);
}

template <class T>
double stat_spread(const T input, size_t size)
{
    double centroid = stat_centroid(input, size);
    return sqrt(stat_weighted_sum(impl::indices_diff_op<impl::pow2>(centroid), input, size) / stat_sum(input, size));
}

template <class T>
double stat_skewness(const T input, size_t size)
{
    double centroid = stat_centroid(input, size);
    double denominator = impl::pow3()(stat_spread(input, size)) * stat_sum(input, size);
    return denominator ? stat_weighted_sum(impl::indices_diff_op<impl::pow3>(centroid), input, size) / denominator : 0.0;
}

template <class T>
double stat_kurtosis(const T input, size_t size)
{
    double centroid = stat_centroid(input, size);
    double denominator = impl::pow4()(stat_spread(input, size)) * stat_sum(input, size);
    return denominator ? stat_weighted_sum(impl::indices_diff_op<impl::pow4>(centroid), input, size) / denominator : std::numeric_limits<double>::infinity();
}

// Log Shape

template <class T>
double stat_log_centroid(const T input, size_t size)
{
    return exp2(stat_weighted_sum(impl::log_indices(), input, size) / (stat_sum(input, size)));
}

template <class T>
double stat_log_spread(const T input, size_t size)
{
    double centroid = stat_log_centroid(input, size);
    return sqrt(stat_weighted_sum(impl::log_indices_diff_op<impl::pow2>(log2(centroid)), input, size) / stat_sum(input, size));
}

template <class T>
double stat_log_skewness(const T input, size_t size)
{
    double centroid = stat_log_centroid(input, size);
    double denominator = impl::pow3()(stat_log_spread(input, size)) * stat_sum(input, size);
    return denominator ? stat_weighted_sum(impl::log_indices_diff_op<impl::pow3>(log2(centroid)), input, size) / denominator : 0.0;
}

template <class T>
double stat_log_kurtosis(const T input, size_t size)
{
    double centroid = stat_log_centroid(input, size);
    double denominator = impl::pow4()(stat_log_spread(input, size)) * stat_sum(input, size);
    return denominator ? stat_weighted_sum(impl::log_indices_diff_op<impl::pow4>(log2(centroid)), input, size) / denominator : std::numeric_limits<double>::infinity();
}

// Flatness

template <class T>
double stat_flatness(const T input, size_t size)
{
    return stat_geometric_mean(input, size) / stat_mean(input, size);
}

// RMS

template <class T>
double stat_rms(const T input, size_t size)
{
    return sqrt(stat_mean_squares(input, size));
}

// Crest

template <class T>
double stat_crest(const T input, size_t size)
{
    return stat_max(input, size) / stat_rms(input, size);
}

#endif /* STATISTICS_HPP */
