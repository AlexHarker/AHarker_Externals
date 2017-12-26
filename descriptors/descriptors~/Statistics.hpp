
#ifndef STATISTICS_H
#define STATISTICS_H

#include <algorithm>
#include <numeric>
#include <cmath>

// Helper functors

struct Pow2      { template <class T> T operator()(T a) { return a * a; } };
struct Pow3      { template <class T> T operator()(T a) { return a * a * a; } };
struct Pow4      { template <class T> T operator()(T a) { return Pow2()(Pow2()(a)); } };
struct Absolute  { template <class T> T operator()(T a) { return std::abs(a); } };
struct Logarithm { template <class T> T operator()(T a) { return log(a); } };

struct Index     { template <class T> double operator[](T a) { return static_cast<double>(a); } };
struct LogIndex  { template <class T> double operator[](T a) { return a ? log2(static_cast<double>(a)) : -12.0; } };

template <class T, typename Op> struct ModifiedData
{
    ModifiedData(T& data) : mData(data) {}
    double operator[](size_t i) { return Op()(mData[i]); }
    T mData;
};

template <class T, typename Op> struct ModifiedDiffData
{
    ModifiedDiffData(T& data, double value) : mData(data), mValue(value) {}
    double operator[](size_t i) { return Op()(mData[i] - mValue); }
    T mData;
    double mValue;
};

template <typename Op> struct IndexDiffOp
{
    IndexDiffOp(double value) : mValue(value) {}
    double operator[](size_t i) { return Op()(static_cast<double>(i) - mValue); }
    double mValue;
};

template <typename Op> struct LogIndexDiffOp
{
    LogIndexDiffOp(double value) : mValue(value) {}
    double operator[](size_t i) { return Op()(LogIndex()[i] - mValue); }
    double mValue;
};

// Length

template <class T> double statLength(T input, size_t size)
{
    return static_cast<double>(size);
}

// Min / Max Values

template <class T> double statMin(T input, size_t size)
{
    return *(std::min_element(input, input + size));
}

template <class T> double statMax(T input, size_t size)
{
    return *(std::max_element(input, input + size));
}

// Counts

template <class T, typename CountOp> double statCount(T input, size_t size, CountOp op)
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

template <class T, class U> double statCountAbove(T input, U threshold, size_t size)
{
    return statCount(input, size, FixedCompare<U, std::greater<U> >(threshold));
}

template <class T, class U> double statCountBelow(T input, U threshold, size_t size)
{
    return statCount(input, size, FixedCompare<U, std::less<U> >(threshold));
}

// Ratios

template <class T, class U> double statRatioAbove(T input, U threshold, size_t size)
{
    return statCountAbove(input, threshold, size) / statLength(input, size);
}

template <class T, class U> double statRatioBelow(T input, U threshold, size_t size)
{
    return statCountBelow(input, threshold, size) / statLength(input, size);
}

// Sums

template <class T> double statSum(T input, size_t size)
{
    double sum = 0.0;
    
    for (size_t i = 0; i < size; i++)
        sum += input[i];
    
    return sum;
}

template <class T> double statSumAbs(T input, size_t size)
{
    return statSum(ModifiedData<T, Absolute>(input), size);
}

template <class T> double statSumSquares(T input, size_t size)
{
    return statSum(ModifiedData<T, Pow2>(input), size);
}

template <class T> double statSumLogs(T input, size_t size)
{
    return statSum(ModifiedData<T, Logarithm>(input), size);
}

// Weighted Sums

template <class T, class U> double statWeightedSum(T data, U weights, size_t size)
{
    double sum = 0.0;
    
    for (size_t i = 0; i < size; i++)
        sum += weights[i] * data[i];
    
    return sum;
}

template <class T> double statWeightedSum(T input, size_t size)
{
    return statWeightedSum(Index(), input, size);
}

template <class T> double statWeightedSumAbs(T input, size_t size)
{
    return statWeightedSum(Index(), ModifiedData<T, Absolute>(input), size);
}

template <class T> double statWeightedSumSquares(T input, size_t size)
{
    return statWeightedSum(Index(), ModifiedData<T, Pow2>(input), size);
}

template <class T> double statWeightedSumLogs(T input, size_t size)
{
    return statWeightedSum(Index(), ModifiedData<T, Logarithm>(input), size);
}

// Weighted Sums (by weights)

template <class T> double statWeightedSum(T input, T weights, size_t size)
{
    return statWeightedSum(input, weights, size);
}

template <class T> double statWeightedSumAbs(T input, T weights, size_t size)
{
    return statWeightedSum(ModifiedData<T, Absolute>(input), weights, size);
}

template <class T> double statWeightedSumSquares(T input, T weights, size_t size)
{
    return statWeightedSum(ModifiedData<T, Pow2>(input), weights, size);
}

template <class T> double statWeightedSumLogs(T input, T weights, size_t size)
{
    return statWeightedSum(ModifiedData<T, Logarithm>(input), weights, size);
}

// Product

template <class T> double statProduct(T input, size_t size)
{
    double product = 1.0;
    
    for (size_t i = 0; i < size; i++)
        product *= input[i];
    
    return product;
}

// Means

template <class T> double statMean(T input, size_t size)
{
    return statSum(input, size) / statLength(input, size);
}

template <class T> double statMeanSquares(T input, size_t size)
{
    return statSumSquares(input, size) / statLength(input, size);
}

template <class T> double statGeometricMean(T input, size_t size)
{
    return exp(statSumLogs(input, size) / statLength(input, size));
}

// Variance

template <class T> double statVariance(T input, size_t size)
{
    double mean = statMean(input, size);
    return statSum(ModifiedDiffData<T, Pow2>(input, mean), size);
}
                           
// Standard Deviation

template <class T> double statStandardDeviation(T input, size_t size)
{
    return sqrt(statVariance(input, size));
}

// Shape

template <class T> double statCentroid(T input, size_t size)
{
    return statWeightedSum(input, size) / statSum(input, size);
}
                        
template <class T> double statSpread(T input, size_t size)
{
    double centroid = statCentroid(input, size);
    return statWeightedSum(IndexDiffOp<Pow2>(centroid), input, size) / statSum(input, size);
}

template <class T> double statSkewness(T input, size_t size)
{
    double centroid = statCentroid(input, size);
    double spreadNorm = Pow3()(sqrt(statSpread(input, size)));
    return statWeightedSum(IndexDiffOp<Pow3>(centroid), input, size) / (spreadNorm * statSum(input, size));
}

template <class T> double statKurtosis(T input, size_t size)
{
    double centroid = statCentroid(input, size);
    double spreadNorm = Pow2()(statSpread(input, size));
    return statWeightedSum(IndexDiffOp<Pow4>(centroid), input, size) / (spreadNorm * statSum(input, size));
}

// Log Shape

template <class T> double statLogCentroid(T input, size_t size)
{
    return exp2(statWeightedSum(LogIndex(), input, size) / statSum(input, size));
}

template <class T> double statLogSpread(T input, size_t size)
{
    double centroid = statLogCentroid(input, size);
    return statWeightedSum(LogIndexDiffOp<Pow2>(log2(centroid)), input, size) / statSum(input, size);
}

template <class T> double statLogSkewness(T input, size_t size)
{
    double centroid = statLogCentroid(input, size);
    double spreadNorm = Pow3()(sqrt(statLogSpread(input, size)));
    return statWeightedSum(LogIndexDiffOp<Pow3>(log2(centroid)), input, size) / (spreadNorm * statSum(input, size));
}

template <class T> double statLogKurtosis(T input, size_t size)
{
    double centroid = statCentroid(input, size);
    double spreadNorm = Pow2()(statLogSpread(input, size));
    return statWeightedSum(LogIndexDiffOp<Pow4>(log2(centroid)), input, size) / (spreadNorm * statSum(input, size));
}

// Flatness

template <class T> double statFlatness(T input, size_t size)
{
    return statGeometricMean(input, size) / statMean(input, size);
}

// RMS

template <class T> double statRMS(T input, size_t size)
{
    return sqrt(statMeanSquares(input, size));
}

// Crest

template <class T> double statCrest(T input, size_t size)
{
    return statMax(input, size) / statRMS(input, size);
}

#endif /* Statistics_h */
