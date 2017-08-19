
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
        
        long getColumn() const                  { return mColumn; }
        void addTarget(double value)            { mValues.push_back(value); }
        void addTarget(t_symbol *value)         { mValues.push_back(value); }
        
        void setTarget(double value)
        {
            mValues.resize(1);
            mValues[0] = value;
        }

        inline bool match(const EntryDatabase::Accessor& accessor, long idx, double& overallDistance) const;
        
    private:
        
        template <typename Op> bool comparison(double value, Op op) const
        {
            for (std::vector<const CustomAtom>::iterator it = mValues.begin(); it != mValues.end(); it++)
                if (op(value, (*it).mValue)) return true;
            
            return false;
        }
        
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
    double getDistance(long idx) const      { return mDistances[idx]; }
    
    void addTarget(double value);
    void addTarget(t_symbol *value);
    void addMatcher(TestType type, long column, double scale = 1.0);
    
    void setMatchers(void *x, long argc, t_atom *argv, const EntryDatabase::ReadPointer database);
    
private:
    
    long sortTopN(long N) const;
    
    mutable long mNumMatches;
    
    mutable std::vector<long> mIndices;
    mutable std::vector<double> mDistances;
    
    std::vector<Matcher> mMatchers;
};


#endif