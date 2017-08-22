
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
 
    struct Result
    {
        Result() {}
        Result(const std::list<EntryDatabase::Entry>::const_iterator it, double distance) : mIterator(it), mDistance(distance) {}
        
        friend bool operator > (const Result& a, const Result& b) { return a.mDistance > b.mDistance; }
        
        std::list<EntryDatabase::Entry>::const_iterator mIterator;
        double mDistance;
    };
    
    struct Matcher
    {
        Matcher(TestType type, long column, double scale = 1.0) : mType(type), mColumn(column), mScale(scale) {}

        template <typename Op> inline bool comparisonTest(const UntypedAtom value, Op op) const
        {
            for (std::vector<const CustomAtom>::iterator it = mValues.begin(); it != mValues.end(); it++)
                if (op(value, (*it))) return true;
            
            return false;
        }
        
        template <typename Op> inline bool distanceTest(bool reject, const double value, double& overallDistance, Op op) const
        {
            double distance = HUGE_VAL;

            for (std::vector<const CustomAtom>::iterator it = mValues.begin(); it != mValues.end(); it++)
            {
                double currentDistance = op(value, *it, mScale);
                currentDistance *= currentDistance;
                if (currentDistance < distance)
                    distance = currentDistance;
            }
            
            overallDistance += distance;
            return !reject || distance <= 1.0;
        }
        
        template <typename T, typename Op> inline long comparisonTest(std::vector<Result>& results, long numMatches, const EntryDatabase::Accessor& accessor, Op op) const
        {
            long matched = 0;
            
            if (mValues.size() == 1)
            {
                T comparisonValue = mValues[0];
                
                for (long i = 0; i < numMatches; i++)
                {
                    const std::list<EntryDatabase::Entry>::const_iterator item = results[i].mIterator;
                    
                    if (op(item->getData(mColumn), comparisonValue))
                        results[matched++] = Result(item, results[i].mDistance);
                }
            }
            else
            {
                for (long i = 0; i < numMatches; i++)
                {
                    const std::list<EntryDatabase::Entry>::const_iterator item = results[i].mIterator;
                    UntypedAtom value = item->getData(mColumn);
                    
                    for (std::vector<const CustomAtom>::iterator it = mValues.begin(); it != mValues.end(); it++)
                    {
                        if (op(value, (*it)))
                        {
                            results[matched++] = Result(item, results[i].mDistance);
                            break;
                        }
                    }
                }
            }
            
            return matched;
        }
        
        template <typename Op> inline long distanceTest(bool reject, std::vector<Result>& results, long numMatches, const EntryDatabase::Accessor& accessor, Op op) const
        {
            long matched = 0;
            
            if (mValues.size() == 1)
            {
                double comparisonValue = mValues[0];
                
                for (long i = 0; i < numMatches; i++)
                {
                    const std::list<EntryDatabase::Entry>::const_iterator item = results[i].mIterator;
                    double distance = op(comparisonValue, item->getData(mColumn).mValue, mScale);
                    distance *= distance;
                    
                    if (!reject || distance <= 1.0)
                        results[matched++] = Result(item, results[i].mDistance + distance);
                }
            }
            else
            {
                for (long i = 0; i < numMatches; i++)
                {
                    const std::list<EntryDatabase::Entry>::const_iterator item = results[i].mIterator;
                    double value = item->getData(mColumn).mValue;
                    double distance = HUGE_VAL;
                    
                    for (std::vector<const CustomAtom>::iterator it = mValues.begin(); it != mValues.end(); it++)
                    {
                        double currentDistance = op((*it), value, mScale);
                        currentDistance *= currentDistance;
                        if (currentDistance < distance)
                            distance = currentDistance;
                    }
                    
                    if (!reject || distance <= 1.0)
                        results[matched++] = Result(item, results[i].mDistance + distance);
                }
            }
            
            return matched;
        }

        TestType mType;
        long mColumn;
        std::vector<CustomAtom> mValues;
        double mScale;
    };
    
public:
    
    Matchers() : mNumMatches(0), mAudioStyle(false) {}
    
    long match(const EntryDatabase::ReadPointer database, double ratioMatched = 1.0, long maxMatches = 0, bool sortOnlyIfLimited = false);
    
    size_t size() const { return mMatchers.size(); }
    
    void clear();
    
    void setTarget(long idx, double value)
    {
        if (idx >= 0 && idx < size())
        {
            mMatchers[idx].mValues.resize(1);
            mMatchers[idx].mValues[0] = value;
        }
    }
    
    long getNumMatches() const                                                          { return mNumMatches; }
    std::list<EntryDatabase::Entry>::const_iterator getIterator(long idx) const         { return mResults[idx].mIterator; }
    long getIndex(long idx) const                                                       { return 0; }
    double getDistance(long idx) const                                                  { return sqrt(mResults[idx].mDistance); }
    
    void addTarget(double value);
    void addTarget(t_symbol *value);
    void addMatcher(TestType type, long column, double scale = 1.0);
    
    void setMatchers(void *x, long argc, t_atom *argv, const EntryDatabase::ReadPointer database);
    void setAudioStyle(bool style) { mAudioStyle = style; }
    
private:
    
    long sortTopN(long N, long size) const;
    
    mutable long mNumMatches;
    
    mutable std::vector<Result> mResults;
    
    std::vector<Matcher> mMatchers;
    bool mAudioStyle;
};


#endif