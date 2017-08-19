
#include "Matchers.h"
#include "Sort.h"
#include <algorithm>
#include <functional>


inline bool Matchers::match(std::vector<const CustomAtom>::iterator& params, const EntryDatabase::Accessor& accessor, long idx, double& overallDistance) const
{
    TestType type = (TestType) (*params++).mInt;
    long column = (*params++).mInt;
    double scale = (*params++).mValue;
    std::vector<const CustomAtom>::iterator to = params + (*params++).mInt;
    
    const CustomAtom data = accessor.getData(idx, column);
    double distance = HUGE_VAL;
    
    switch (type)
    {
        case kTestMatch:
            if (data.mType == CustomAtom::kSymbol)
            {
                t_symbol *sym = data.mSymbol;
                
                for ( ; params != to; params++)
                    if (sym == (*params).mSymbol) return true;
                
                return false;
            }
            else
                return comparison(params, to, data.mValue, std::equal_to<double>());
            
        case kTestLess:             return comparison(params, to, data.mValue, std::less<double>());
        case kTestGreater:          return comparison(params, to, data.mValue, std::greater<double>());
        case kTestLessEqual:        return comparison(params, to, data.mValue, std::less_equal<double>());
        case kTestGreaterEqual:     return comparison(params, to, data.mValue, std::greater_equal<double>());
            
        case kTestDistance:
        case kTestDistanceReject:
            
            for ( ; params != to; params++)
            {
                double currentDistance = ((*params).mValue - data.mValue) * scale;
                distance = std::min(distance, currentDistance * currentDistance);
            }
            
            overallDistance += distance;
            return type == kTestDistance || distance <= 1.0;
            
        case kTestRatio:
        case kTestRatioReject:
            
            // FIX - check this
            
            for ( ; params != to; params++)
            {
                double currentDistance = ((*params).mValue > data.mValue) ? (*params).mValue / data.mValue : data.mValue / (*params).mValue;
                currentDistance = (currentDistance - 1.0) * scale;
                distance = std::min(distance, currentDistance * currentDistance);
                
            }
            
            overallDistance += distance;
            return type == kTestRatio || distance <= 1.0;
    }
}

long Matchers::match(const EntryDatabase::ReadPointer database, double ratioMatched, long maxMatches, bool sortOnlyIfLimited) const
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
        std::vector<const CustomAtom>::iterator it = mMatchingParams.begin();
        
        while (it != mMatchingParams.end())
            if (!(matched = match(it, accessor, i, distance)))
                break;
        
        // Store the entry if it is a valid match
        
        if (matched)
        {
            mIndices[mNumMatches++] = i;
            mDistances[i] = sqrt(distance);
        }
    }
    
    long numMatches = mNumMatches;
    
    if (size())
    {
        ratioMatched = std::min(std::max(ratioMatched, 0.0), 1.0);
        maxMatches = std::max(maxMatches, 0L);
        numMatches = round(mNumMatches * ratioMatched);
        numMatches = (maxMatches && mNumMatches > maxMatches) ? maxMatches : mNumMatches;

        // FIX - better heuristics and more info on what has been sorted...
        
        if (numMatches != mNumMatches && !sortOnlyIfLimited)
        {
            if (numMatches < (database->numItems() / 8))
                numMatches = sortTopN(numMatches, mNumMatches);
            else
                sort(mIndices, mDistances, mNumMatches);
        }
    }
    
    return mNumMatches = numMatches;
}

void Matchers::setMatchers(void *x, long argc, t_atom *argv, const EntryDatabase::ReadPointer database)
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
        else if (database->getColumnLabelMode(column) && type != TEST_MATCH)
        {
            object_error((t_object *) x, "incorrect matcher for label type column (should be equals or ==)  column number %ld", column + 1);
            continue;
        }
        
        bool hasTarget = false;
        
        // Parse values
        
        if (database->getColumnLabelMode(column))
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

long Matchers::sortTopN(long N, long size) const
{
    N = std::min(N, size);
    
    // Partial insertion sort (faster sorting for small numbers of n)...
    
    for (long i = 0; i < N; i++)
    {
        double minDistance = mDistances[mIndices[i]];
        long swap = i;
        
        for (long j = i + 1; j < size; j++)
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

void Matchers::clear()
{
    mMatchingParams.clear();
    mOffsets.clear();
}

void Matchers::addTarget(double value)
{
    if (mOffsets.size())
    {
        mMatchingParams[mOffsets.back()].mInt++;
        mMatchingParams.push_back(value);
    }
}

void Matchers::addTarget(t_symbol *value)
{
    if (mOffsets.size())
    {
        mMatchingParams[mOffsets.back()].mInt++;
        mMatchingParams.push_back(value);
    }
}

void Matchers::addMatcher(TestType type, long column, double scale)
{
    mMatchingParams.push_back(CustomAtom((long) type, false));
    mMatchingParams.push_back(CustomAtom(column, false));
    mMatchingParams.push_back(scale);
    mMatchingParams.push_back(CustomAtom((long) 0, false));
    
    mOffsets.push_back(mMatchingParams.size() - 1);
}
