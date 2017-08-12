
#include "Matchers.h"
#include <algorithm>
#include <functional>

inline bool Matchers::Matcher::match(const EntryDatabase::Accessor& accessor, long idx, double& overallDistance) const
{
    const CustomAtom data = accessor.getData(idx, mColumn);
    double distance = HUGE_VAL;
    
    switch (mType)
    {
        case kTestMatch:
            if (data.mType == CustomAtom::kSymbol)
            {
                t_symbol *sym = data.mSymbol;
                
                for (std::vector<const CustomAtom>::iterator it = mValues.begin(); it != mValues.end(); it++)
                    if (sym == (*it).mSymbol) return true;
                
                return false;
            }
            else
                return comparison(data.mValue, std::equal_to<double>());
            
        case kTestLess:             return comparison(data.mValue, std::less<double>());
        case kTestGreater:          return comparison(data.mValue, std::greater<double>());
        case kTestLessEqual:        return comparison(data.mValue, std::less_equal<double>());
        case kTestGreaterEqual:     return comparison(data.mValue, std::greater_equal<double>());
            
        case kTestDistance:
        case kTestDistanceReject:
            
            for (std::vector<const CustomAtom>::iterator it = mValues.begin(); it != mValues.end(); it++)
            {
                double currentDistance = ((*it).mValue - data.mValue) * mScale;
                distance = std::min(distance, currentDistance * currentDistance);
            }
            
            overallDistance += distance;
            return mType == kTestDistance || distance <= 1.0;
            
        case kTestRatio:
        case kTestRatioReject:
            
            // FIX - check this
            
            for (std::vector<const CustomAtom>::iterator it = mValues.begin(); it != mValues.end(); it++)
            {
                double currentDistance = ((*it).mValue > data.mValue) ? (*it).mValue / data.mValue : data.mValue / (*it).mValue;
                currentDistance = (currentDistance - 1.0) * mScale;
                distance = std::min(distance, currentDistance * currentDistance);
                
            }
            
            overallDistance += distance;
            return mType == kTestRatio || distance <= 1.0;
    }
}

long Matchers::match(const EntryDatabase *database) const
{
    long numItems = database->numItems();
    mNumMatches = 0;
    
    mIndices.resize(numItems);
    mDistances.resize(numItems);
    
    const EntryDatabase::Accessor accessor = database->accessor();
    
    for (long i = 0; i < numItems; i++)
    {
        // Assume a match for each entry (for the case of no matchers)
        
        bool matched = true;
        double distance = 0.0;
        
        for (std::vector<const Matcher>::iterator it = mMatchers.begin(); it != mMatchers.end(); it++)
            if (!(matched = it->match(accessor, i, distance)))
                break;
        
        // Store the entry if it is a valid match
        
        if (matched)
        {
            mIndices[mNumMatches++] = i;
            mDistances[i] = sqrt(distance);
        }
    }
    
    return mNumMatches;
}

long Matchers::getTopN(long N)
{
    N = std::min(N, mNumMatches);
    
    // Partial insertion sort (faster sorting for small numbers of n)...
    
    for (long i = 0; i < N; i++)
    {
        double minDistance = mDistances[mIndices[i]];
        long swap = i;
        
        for (long j = i + 1; j < mNumMatches; j++)
        {
            if (mDistances[mIndices[j]] < minDistance)
            {
                minDistance = mDistances[mIndices[j]];
                swap = j;
            }
        }

        std::swap(mIndices[swap], mIndices[i]);
   }
    
    return N;
}

void Matchers::sort()
{
    // An ascending order index sort (combsort11 algorithm)
    
    long gap = mNumMatches;
    bool swaps = 1;
    long i;
    
    while (gap > 1 || swaps)
    {
        if (gap > 1)
        {
            gap = (gap * 10) / 13;
            if (gap == 9 || gap == 10) gap = 11;
            if (gap < 1) gap = 1;
        }
        
        for (i = 0, swaps = 0; i + gap < mNumMatches; i++)
        {
            if (mDistances[mIndices[i]] > mDistances[mIndices[i + gap]])
            {
                std::swap(mIndices[i], mIndices[i + gap]);
                swaps = true;
            }
        }
    }
}

void Matchers::clear()
{
    mMatchers.clear();
}

void Matchers::addTarget(double value)
{
    if (mMatchers.size())
        mMatchers.back().addTarget(value);
}

void Matchers::addTarget(t_symbol *value)
{
    if (mMatchers.size())
        mMatchers.back().addTarget(value);
}

void Matchers::addMatcher(TestType type, long column, double scale)
{
    mMatchers.push_back(Matcher(type, column, scale));
}
