
#include "Matchers.h"
#include "Sort.h"
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

void Matchers::setMatchers(void *x, long argc, t_atom *argv, EntryDatabase *database)
{
    // Empty the matchers
    
    clear();
    
    // Loop over arguments
    
    while (argc)
    {
        if (argc < 2)
        {
            object_error((t_object *)x, "insufficient items in matchers message for unparsed test");
            break;
        }
        
        // Get the column and test type
        
        long column = database->columnFromSpecifier(argv++);
        ::TestType type = entrymatcher_test_types(argv++);
        argc -= 2;
        
        // Test for issues
        
        if (type == TEST_NONE)
        {
            object_error((t_object *) x, "invalid test / no test specified in unparsed segment of matchers message");
            break;
        }
        else if (column < 0 || column >= database->numColumns())
        {
            object_error((t_object *) x, "specified column in matchers message does not exist");
            continue;
        }
        else if (database->getLabelMode(column) && type != TEST_MATCH)
        {
            object_error((t_object *) x, "incorrect matcher for label type column (should be equals or ==)  column number %ld", column + 1);
            continue;
        }
        
        bool hasTarget = false;
        
        // Parse values
        
        if (database->getLabelMode(column))
        {
            // If this column is for labels store details of a valid match test (other tests are not valid)
            
            addMatcher(Matchers::kTestMatch, column);
            
            for ( ; argc; argc--, argv++)
            {
                if (entrymatcher_test_types(argv) != TEST_NONE)
                    break;
                addTarget(atom_getsym(argv));
                hasTarget = true;
            }
        }
        else
        {
            // If this column is for values store the parameters for any valid test
            
            double scale = 1.0;
            
            if (argc && (type == TEST_SCALE || type == TEST_WITHIN))
            {
                scale = fabs(1.0 / atom_getfloat(argv++));
                argc--;
            }
            else if (argc && (type == TEST_SCALE_RATIO || type == TEST_WITHIN_RATIO))
            {
                scale = fabs(atom_getfloat(argv++));
                scale = (scale < 1.0) ? 1.0 / scale : scale;
                scale = 1.0 / (scale - 1.0);
                argc--;
            }
            
            switch (type)
            {
                case TEST_NONE:                 break;
                case TEST_MATCH:                addMatcher(Matchers::kTestMatch, column);                      break;
                case TEST_LESS_THAN:            addMatcher(Matchers::kTestLess, column);                       break;
                case TEST_GREATER_THAN:         addMatcher(Matchers::kTestGreater, column);                    break;
                case TEST_LESS_THAN_EQ:         addMatcher(Matchers::kTestLessEqual, column);                  break;
                case TEST_GREATER_THAN_EQ:      addMatcher(Matchers::kTestGreaterEqual, column);               break;
                case TEST_DISTANCE:             addMatcher(Matchers::kTestDistance, column);                   break;
                case TEST_SCALE:                addMatcher(Matchers::kTestDistance, column, scale);            break;
                case TEST_WITHIN:               addMatcher(Matchers::kTestDistanceReject, column, scale);      break;
                case TEST_DISTANCE_RATIO:       addMatcher(Matchers::kTestRatio, column);                      break;
                case TEST_SCALE_RATIO:          addMatcher(Matchers::kTestRatio, column, scale);               break;
                case TEST_WITHIN_RATIO:         addMatcher(Matchers::kTestRatioReject, column, scale);         break;
            }
            
            for ( ; argc; argc--, argv++)
            {
                if (argc > 1 && entrymatcher_test_types(argv + 1) != TEST_NONE)
                    break;
                addTarget(atom_getfloat(argv));
                hasTarget = true;
            }
        }
        
        if (!hasTarget)
            object_error((t_object *) x, "no target values given for specified test in matchers message");
    }
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
    ::sort(mIndices, mDistances, mNumMatches);
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
