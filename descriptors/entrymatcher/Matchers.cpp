
#include "Matchers.h"
#include "Sort.h"
#include <algorithm>
#include <functional>


inline long Matchers::Matcher::match(std::vector<long>& indices, std::vector<double>& distancesSquared, long numMatches, const EntryDatabase::Accessor& accessor) const
{
    struct Distance { double operator()(double a, double b, double scale) { return (a - b) * scale; } };
    struct Ratio { double operator()(double a, double b, double scale) { return (((a > b) ? a / b : b / a) - 1.0) * scale; }};
    
    if (!numMatches)
        return 0;
    
    switch (mType)
    {
        case kTestMatch:
            if (accessor.getData(indices[0], mColumn).mType == CustomAtom::kSymbol)
                return comparison<t_symbol *>(indices, numMatches, accessor, std::equal_to<t_symbol *>());
            else
                return comparison<double>(indices, numMatches, accessor, std::equal_to<double>());
        case kTestLess:             return comparison<double>(indices, numMatches, accessor, std::less<double>());
        case kTestGreater:          return comparison<double>(indices, numMatches, accessor, std::greater<double>());
        case kTestLessEqual:        return comparison<double>(indices, numMatches, accessor, std::less_equal<double>());
        case kTestGreaterEqual:     return comparison<double>(indices, numMatches, accessor, std::greater_equal<double>());
        case kTestDistance:         return distance(false, indices, distancesSquared, numMatches, accessor, Distance());
        case kTestDistanceReject:   return distance(true, indices, distancesSquared, numMatches, accessor, Distance());
        case kTestRatio:            return distance(false, indices, distancesSquared, numMatches, accessor, Ratio());
        case kTestRatioReject:      return distance(true, indices, distancesSquared, numMatches, accessor, Ratio());
    }
}

long Matchers::match(const EntryDatabase::ReadPointer database, double ratioMatched, long maxMatches, bool sortOnlyIfLimited) const
{
    long numItems = database->numItems();
    mNumMatches = 0;
    
    mIndices.resize(numItems);
    mDistancesSquared.resize(numItems);
    
    const EntryDatabase::Accessor accessor = database->accessor();
    
    for (long i = 0; i < numItems; i++)
    {
        mIndices[i] = i;
        mDistancesSquared[i] = 0.0;
    }
    
    mNumMatches = numItems;
    
    for (std::vector<const Matcher>::iterator it = mMatchers.begin(); it != mMatchers.end(); it++)
        mNumMatches = it->match(mIndices, mDistancesSquared, mNumMatches, accessor);
    
    if (size())
    {
        ratioMatched = std::min(std::max(ratioMatched, 0.0), 1.0);
        maxMatches = std::max(maxMatches, 0L);
        long numMatches = round(mNumMatches * ratioMatched);
        numMatches = (maxMatches && mNumMatches > maxMatches) ? maxMatches : mNumMatches;
        
        // FIX - better heuristics and more info on what has been sorted...
        
        if (numMatches != mNumMatches && !sortOnlyIfLimited)
        {
            if (numMatches < (database->numItems() / 8))
                numMatches = sortTopN(numMatches, mNumMatches);
            else
                sort(mIndices, mDistancesSquared, mNumMatches);
        }
        
        mNumMatches = numMatches;
    }
    
    return mNumMatches;
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
        double minDistance = mDistancesSquared[mIndices[i]];
        long swap = i;
        
        for (long j = i + 1; j < size; j++)
        {
            if (mDistancesSquared[mIndices[j]] < minDistance)
            {
                minDistance = mDistancesSquared[mIndices[j]];
                swap = j;
            }
        }
        
        std::swap(mIndices[swap], mIndices[i]);
    }
    
    return N;
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
