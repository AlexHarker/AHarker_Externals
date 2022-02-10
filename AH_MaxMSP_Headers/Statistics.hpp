
#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <algorithm>
#include <numeric>
#include <cmath>

// Helper functors

struct Pow2      { template <class T> T operator()(T a) const { return a * a; } };
struct Pow3      { template <class T> T operator()(T a) const { return a * a * a; } };
struct Pow4      { template <class T> T operator()(T a) const { return Pow2()(Pow2()(a)); } };
struct Absolute  { template <class T> T operator()(T a) const { return std::abs(a); } };
struct Logarithm { template <class T> T operator()(T a) const { return log(a); } };

struct Index     { template <class T> double operator[](T a) const { return static_cast<double>(a); } };
struct LogIndex  { template <class T> double operator[](T a) const { return a ? log2(static_cast<double>(a)) : 0.0; } };

template <class T>
struct LogWidth
{
    LogWidth(T& data) : mData(data) {}
    double operator[](size_t i) const { return i ? mData[i] * (log2(i + 0.5) - log2(i - 0.5)) : mData[i] * 0.0; }
    const T mData;
};


template <class T, typename Op>
struct ModifiedData
{
    ModifiedData(T& data) : mData(data) {}
    double operator[](size_t i) const { return Op()(mData[i]); }
    const T mData;
};

template <class T, typename Op>
struct ModifiedDiffData
{
    ModifiedDiffData(T& data, double value) : mData(data), mValue(value) {}
    double operator[](size_t i) const { return Op()(mData[i] - mValue); }
    const T mData;
    double mValue;
};

template <typename Op>
struct IndexDiffOp
{
    IndexDiffOp(double value) : mValue(value) {}
    double operator[](size_t i) const { return Op()(static_cast<double>(i) - mValue); }
    double mValue;
};

template <typename Op>
struct LogIndexDiffOp
{
    LogIndexDiffOp(double value) : mValue(value) {}
    double operator[](size_t i) const { return Op()(LogIndex()[i] - mValue); }
    double mValue;
};

// Length

template <class T>
double statLength(const T input, size_t size)
{
    return static_cast<double>(size);
}

// Min / Max Values

template <class T>
double statMin(const T input, size_t size)
{
    return size ? *(std::min_element(input, input + size)) : std::numeric_limits<double>::infinity();
}

template <class T>
double statMax(const T input, size_t size)
{
    return size ? *(std::max_element(input, input + size)) : -std::numeric_limits<double>::infinity();
}

// Counts

template <class T, typename CountOp>
double statCount(const T input, size_t size, CountOp op)
{
    size_t count = 0;
    
    for (size_t i; i < size; i++)
        if (op(input[i]))
            count++;
    
    return count;
}

template <class T, typename Op>
struct FixedCompare
{
    FixedCompare(T value, Op op) : mValue(value) {}
    bool operator()(T a) { return Op()(a, mValue); }
    T mValue;
};

template <class T, class U>
double statCountAbove(const T input, U threshold, size_t size)
{
    return statCount(input, size, FixedCompare<U, std::greater<U>>(threshold));
}

template <class T, class U>
double statCountBelow(const T input, U threshold, size_t size)
{
    return statCount(input, size, FixedCompare<U, std::less<U>>(threshold));
}

// Ratios

template <class T, class U>
double statRatioAbove(const T input, U threshold, size_t size)
{
    return statCountAbove(input, threshold, size) / statLength(input, size);
}

template <class T, class U>
double statRatioBelow(const T input, U threshold, size_t size)
{
    return statCountBelow(input, threshold, size) / statLength(input, size);
}

// Sums

template <class T>
double statSum(const T input, size_t size)
{
    double sum = 0.0;
    
    for (size_t i = 0; i < size; i++)
        sum += input[i];
    
    return sum;
}

template <class T>
double statSumAbs(const T input, size_t size)
{
    return statSum(ModifiedData<const T, Absolute>(input), size);
}

template <class T>
double statSumSquares(const T input, size_t size)
{
    return statSum(ModifiedData<const T, Pow2>(input), size);
}

template <class T>
double statSumLogs(const T input, size_t size)
{
    return statSum(ModifiedData<const T, Logarithm>(input), size);
}

// Weighted Sums

template <class T, class U>
double statWeightedSum(const T data, const U weights, size_t size)
{
    double sum = 0.0;
    
    for (size_t i = 0; i < size; i++)
        sum += weights[i] * data[i];
    
    return sum;
}

template <class T>
double statWeightedSum(const T input, size_t size)
{
    return statWeightedSum(Index(), input, size);
}

template <class T>
double statWeightedSumAbs(const T input, size_t size)
{
    return statWeightedSum(Index(), ModifiedData<T, Absolute>(input), size);
}

template <class T>
double statWeightedSumSquares(const T input, size_t size)
{
    return statWeightedSum(Index(), ModifiedData<T, Pow2>(input), size);
}

template <class T>
double statWeightedSumLogs(const T input, size_t size)
{
    return statWeightedSum(Index(), ModifiedData<T, Logarithm>(input), size);
}

// Weighted Sums (by weights)

template <class T>
double statWeightedSum(const T input, const T weights, size_t size)
{
    return statWeightedSum(input, weights, size);
}

template <class T>
double statWeightedSumAbs(const T input, const T weights, size_t size)
{
    return statWeightedSum(ModifiedData<const T, Absolute>(input), weights, size);
}

template <class T>
double statWeightedSumSquares(const T input, const T weights, size_t size)
{
    return statWeightedSum(ModifiedData<const T, Pow2>(input), weights, size);
}

template <class T>
double statWeightedSumLogs(const T input, const T weights, size_t size)
{
    return statWeightedSum(ModifiedData<const T, Logarithm>(input), weights, size);
}

// Product

template <class T>
double statProduct(const T input, size_t size)
{
    double product = 1.0;
    
    for (size_t i = 0; i < size; i++)
        product *= input[i];
    
    return product;
}

// Means

template <class T>
double statMean(const T input, size_t size)
{
    return statSum(input, size) / statLength(input, size);
}

template <class T>
double statMeanSquares(const T input, size_t size)
{
    return statSumSquares(input, size) / statLength(input, size);
}

template <class T>
double statGeometricMean(const T input, size_t size)
{
    return exp(statSumLogs(input, size) / statLength(input, size));
}

// Variance

template <class T>
double statVariance(const T input, size_t size)
{
    double mean = statMean(input, size);
    return statSum(ModifiedDiffData<const T, Pow2>(input, mean), size) / statLength(input, size);
}
                           
// Standard Deviation

template <class T>
double statStandardDeviation(const T input, size_t size)
{
    return sqrt(statVariance(input, size));
}

// PDF Percentile

template <class T>
double statPDFPercentile(const T input, double centile, size_t size)
{
    double target = statSum(input, size) * std::min(100.0, std::max(centile, 0.0)) / 100.0;
    
    double sum = 0.0;
    
    for (size_t i = 0; i < size; i++)
    {
        sum += input[i];
        if (sum >= target)
            return static_cast<double>(1 + i - ((sum - target) / input[i]));
    }
    
    return static_cast<double>(size - 1);
}

// Shape

template <class T>
double statCentroid(const T input, size_t size)
{
    return statWeightedSum(input, size) / statSum(input, size);
}
                        
template <class T>
double statSpread(const T input, size_t size)
{
    double centroid = statCentroid(input, size);
    return statWeightedSum(IndexDiffOp<Pow2>(centroid), input, size) / statSum(input, size);
}

template <class T>
double statSkewness(const T input, size_t size)
{
    double centroid = statCentroid(input, size);
    double spreadNorm = Pow3()(sqrt(statSpread(input, size)));
    return statWeightedSum(IndexDiffOp<Pow3>(centroid), input, size) / (spreadNorm * statSum(input, size));
}

template <class T>
double statKurtosis(const T input, size_t size)
{
    double centroid = statCentroid(input, size);
    double spreadNorm = Pow2()(statSpread(input, size));
    return statWeightedSum(IndexDiffOp<Pow4>(centroid), input, size) / (spreadNorm * statSum(input, size));
}

// Log Shape

template <class T>
double statLogCentroid(const T input, size_t size)
{
    return exp2(statWeightedSum(LogIndex(), LogWidth<T>(input), size) / (statSum(LogWidth<T>(input), size)));
}

template <class T>
double statLogSpread(const T input, size_t size)
{
    double centroid = statLogCentroid(input, size);
    return statWeightedSum(LogIndexDiffOp<Pow2>(log2(centroid)), LogWidth<T>(input), size) / (statSum(LogWidth<T>(input), size));
}

template <class T>
double statLogSkewness(const T input, size_t size)
{
    double centroid = statLogCentroid(input, size);
    double spreadNorm = Pow3()(sqrt(statLogSpread(input, size)));
    return statWeightedSum(LogIndexDiffOp<Pow3>(log2(centroid)), LogWidth<T>(input), size) / (spreadNorm * statSum(LogWidth<T>(input), size));
}

template <class T>
double statLogKurtosis(const T input, size_t size)
{
    double centroid = statCentroid(input, size);
    double spreadNorm = Pow2()(statLogSpread(input, size));
    return statWeightedSum(LogIndexDiffOp<Pow4>(log2(centroid)), LogWidth<T>(input), size) / (spreadNorm * statSum(LogWidth<T>(input), size));
}

// Flatness

template <class T>
double statFlatness(const T input, size_t size)
{
    return statGeometricMean(input, size) / statMean(input, size);
}

// RMS

template <class T>
double statRMS(const T input, size_t size)
{
    return sqrt(statMeanSquares(input, size));
}

// Crest

template <class T>
double statCrest(const T input, size_t size)
{
    return statMax(input, size) / statRMS(input, size);
}

// Arg Min and Max

template <class T>
double statMaxPosition(const T input, size_t size)
{
    return size ? std::distance(input, std::max_element(input, input + size)) : -std::numeric_limits<double>::infinity();
}

template <class T>
double statMinPosition(const T input, size_t size)
{
    return size ? std::distance(input, std::min_element(input, input + size)) : -std::numeric_limits<double>::infinity();
}

#endif /* Statistics_h */
