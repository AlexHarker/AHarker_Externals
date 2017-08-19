
#ifndef MATCHERS_H
#define MATCHERS_H

#include "CustomAtom.h"
#include "EntryDatabase.h"
#include "utilities.h"

class Matchers
{
    
public:
    
    enum TestType { kTestMatch, kTestLess, kTestGreater, kTestLessEqual, kTestGreaterEqual, kTestDistance, kTestRatio, kTestDistanceReject, kTestRatioReject };
    
private:
    
    class Matcher
    {
        
    public:
        
        Matcher(TestType type, long column, double scale = 1.0)
        : mColumn(column), mType(type), mScale(scale) {}
        
        void addTarget(double value)            { mValues.push_back(value); }
        void addTarget(t_symbol *value)         { mValues.push_back(value); }
        
        void setTarget(double value)
        {
            mValues.resize(1);
            mValues[0] = value;
        }

        inline long match(std::vector<long>& indices, std::vector<double>& distancesSquared, long numMatches, const EntryDatabase::Accessor& accessor) const;
        
    private:
        
        template <typename type, typename Op> inline long comparison(std::vector<long>& indices, long numMatches, const EntryDatabase::Accessor& accessor, Op op) const
        {
            long matched = 0;
            
            if (mValues.size() == 1)
            {
                type comparisonValue = mValues[0];
                
                for (long i = 0; i < numMatches; i++)
                {
                    long idx = indices[i];
        
                    if (op(accessor.getData(idx, mColumn), comparisonValue))
                        indices[matched++] = idx;
                }
            }
            else
            {
                for (long i = 0; i < numMatches; i++)
                {
                    long idx = indices[i];
                    type value = accessor.getData(idx, mColumn);
                    
                    for (std::vector<const CustomAtom>::iterator it = mValues.begin(); it != mValues.end(); it++)
                    {
                        if (op(value, (*it)))
                        {
                            indices[matched++] = idx;
                            break;
                        }
                    }
                }
            }
            
            return matched;
        }
        
        template <typename Op> inline long distance(bool reject, std::vector<long>& indices, std::vector<double>& distancesSquared, long numMatches, const EntryDatabase::Accessor& accessor, Op op) const
        {
            long matched = 0;
            
            if (mValues.size() == 1)
            {
                double comparisonValue = mValues[0].mValue;
                
                for (long i = 0; i < numMatches; i++)
                {
                    long idx = indices[i];
                    double distance = op(comparisonValue, accessor.getData(idx, mColumn).mValue, mScale);
                    distance *= distance;
                    distancesSquared[idx] += distance;
                    
                    if (!reject || distance <= 1.0)
                        indices[matched++] = idx;
                }
            }
            else
            {
                for (long i = 0; i < numMatches; i++)
                {
                    long idx = indices[i];
                    double value = accessor.getData(idx, mColumn).mValue;
                    double distance = HUGE_VAL;
                    
                    for (std::vector<const CustomAtom>::iterator it = mValues.begin(); it != mValues.end(); it++)
                    {
                        double currentDistance = op((*it).mValue, value, mScale);
                        currentDistance *= currentDistance;
                        if (currentDistance < distance)
                            distance = currentDistance;
                    }
                    
                    if (!reject || distance <= 1.0)
                    {
                        distancesSquared[idx] += distance;
                        indices[matched++] = idx;
                    }
                }
            }
        
            return matched;
        }
        
        // Data
        
        long mColumn;
        TestType mType;
        std::vector<CustomAtom> mValues;
        double mScale;
    };

    
public:
    
    Matchers() : mNumMatches(0) {}
    
    long match(const EntryDatabase::ReadPointer database, double ratioMatched = 1.0, long maxMatches = 0, bool sortOnlyIfLimited = false) const;
    
    size_t size() const { return mMatchers.size(); }
    
    void clear();
    
    void setTarget(long idx, double value)
    {
        if (idx >= 0 && idx < size())
            mMatchers[idx].setTarget(value);
    }
    
    long getNumMatches() const              { return mNumMatches; }
    long getIndex(long idx) const           { return mIndices[idx]; }
    double getDistance(long idx) const      { return sqrt(mDistancesSquared[idx]); }
    
    void addTarget(double value);
    void addTarget(t_symbol *value);
    void addMatcher(TestType type, long column, double scale = 1.0);
    
    void setMatchers(void *x, long argc, t_atom *argv, const EntryDatabase::ReadPointer database);
    
private:
    
    long sortTopN(long N, long size) const;
    
    mutable long mNumMatches;
    
    mutable std::vector<long> mIndices;
    mutable std::vector<double> mDistancesSquared;
    
    std::vector<Matcher> mMatchers;
};


#endif