
#ifndef MATCHERS_H
#define MATCHERS_H

#include "EntryDatabase.h"

class Matchers
{
    
public:
    
    enum TestType { kTestMatch, kTestLess, kTestGreater, kTestLessEqual, kTestGreaterEqual, kTestDistance, kTestRatio };
    
private:
    
    class Matcher
    {
        
    public:
        
        Matcher(TestType type, long column, double scale = 1.0, bool reject = false)
        : mType(type), mColumn(column), mScale(scale), mReject(reject) {}
        
        long getColumn() const                  { return mColumn; }
        void addTarget(double value)            { mValues.push_back(value); }
        void addTarget(t_symbol *value)         { mValues.push_back(value); }
        
        void setTarget(double value)
        {
            mValues.resize(1);
            mValues[0] = value;
        }

        inline bool match(const EntryDatabase *database, long idx, double& overallDistance) const;
        
    private:
        
        template <typename Op> bool comparison(double value, Op op) const
        {
            for (std::vector<const FloatSym>::iterator it = mValues.begin(); it != mValues.end(); it++)
                if (op(value, (*it).mValue)) return true;
            
            return false;
        }
        
        TestType mType;
        long mColumn;
        double mScale;
        bool mReject;
        std::vector<FloatSym> mValues;
    };
    
public:
    
    long match(const EntryDatabase *database, std::vector<long>& indices, std::vector<double>& distances) const;
    
    void clear();
    
    void setTarget(long idx, double value)
    {
        if (idx >= 0 && idx < size())
            mMatchers[idx].setTarget(value);
    }
    
    void addTarget(double value);
    void addTarget(t_symbol *value);
    void addMatcher(TestType type, long column, double scale = 1.0, bool reject = false);
    
    size_t size() { return mMatchers.size(); }
    
private:
    
    std::vector<Matcher> mMatchers;
};


#endif