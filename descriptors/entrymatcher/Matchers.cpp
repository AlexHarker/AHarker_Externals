
#include <algorithm>
#include <functional>

#include "Matchers.h"
#include "Sort.h"

long Matchers::match(const EntryDatabase::ReadPointer& database, double ratioMatched, long maxMatches, bool mustSort) const
{
    struct Distance { double operator()(double a, double b, double scale) { return (a - b) * scale; } };
    struct Ratio { double operator()(double a, double b, double scale) { return (((a > b) ? a / b : b / a) - 1.0) * scale; }};
    
    long numItems = database->numItems();
    mNumMatches = 0;
    
    mResults.resize(numItems);
    
    const EntryDatabase::RawAccessor accessor = database->rawAccessor();
    
    if (!size() || mAudioStyle)
    {
        for (long i = 0; i < numItems; i++)
            mResults[i] = Result(i, 0.0);
        
        mNumMatches = numItems;
    }

    if (!size())
        return numItems;
    
    if (mAudioStyle)
    {
        for (std::vector<Matcher>::const_iterator it = mMatchers.begin(); it != mMatchers.end(); it++)
        {
           if (!mNumMatches)
               break;
            
            switch (it->mType)
            {
                case kTestMatch:
                    if (database->getColumnLabelMode(it->mColumn))
                        mNumMatches = it->comparisonTest<t_symbol *>(mResults, mNumMatches, accessor, std::equal_to<t_symbol *>());
                    else
                        mNumMatches = it->comparisonTest<double>(mResults, mNumMatches, accessor, std::equal_to<double>());
                    break;
                    
                case kTestLess:             mNumMatches = it->comparisonTest<double>(mResults, mNumMatches, accessor, std::less<double>()); break;
                case kTestGreater:          mNumMatches = it->comparisonTest<double>(mResults, mNumMatches, accessor, std::greater<double>()); break;
                case kTestLessEqual:        mNumMatches = it->comparisonTest<double>(mResults, mNumMatches, accessor, std::less_equal<double>()); break;
                case kTestGreaterEqual:     mNumMatches = it->comparisonTest<double>(mResults, mNumMatches, accessor, std::greater_equal<double>()); break;
                case kTestDistance:         mNumMatches = it->distanceTest(false, mResults, mNumMatches, accessor, Distance()); break;
                case kTestDistanceReject:   mNumMatches = it->distanceTest(true, mResults, mNumMatches, accessor, Distance()); break;
                case kTestRatio:            mNumMatches = it->distanceTest(false, mResults, mNumMatches, accessor, Ratio()); break;
                case kTestRatioReject:      mNumMatches = it->distanceTest(true, mResults, mNumMatches, accessor, Ratio()); break;
            }
        }
    }
    else
    {
        for (long i = 0; i < numItems; i++)
        {
            // Assume a match for each entry (for the case of no matchers)
            
            bool matched = true;
            double distanceSquared = 0.0;
            
            for (std::vector<Matcher>::const_iterator it = mMatchers.begin(); it != mMatchers.end(); it++)
            {
                switch (it->mType)
                {
                    case kTestMatch:
                        if (database->getColumnLabelMode(it->mColumn))
                            matched = it->comparisonTest(accessor.getData(i, it->mColumn), std::equal_to<t_symbol *>());
                        else
                            matched = it->comparisonTest(accessor.getData(i, it->mColumn), std::equal_to<double>());
                        break;
                        
                    case kTestLess:             matched = it->comparisonTest(accessor.getData(i, it->mColumn), std::less<double>()); break;
                    case kTestGreater:          matched = it->comparisonTest(accessor.getData(i, it->mColumn), std::greater<double>()); break;
                    case kTestLessEqual:        matched = it->comparisonTest(accessor.getData(i, it->mColumn), std::less_equal<double>()); break;
                    case kTestGreaterEqual:     matched = it->comparisonTest(accessor.getData(i, it->mColumn), std::greater_equal<double>()); break;
                    case kTestDistance:         matched = it->distanceTest(false, accessor.getData(i, it->mColumn), distanceSquared, Distance()); break;
                    case kTestDistanceReject:   matched = it->distanceTest(true, accessor.getData(i, it->mColumn), distanceSquared, Distance()); break;
                    case kTestRatio:            matched = it->distanceTest(false, accessor.getData(i, it->mColumn), distanceSquared, Ratio()); break;
                    case kTestRatioReject:      matched = it->distanceTest(true, accessor.getData(i, it->mColumn), distanceSquared, Ratio()); break;
                }

                if (!matched)
                    break;
            }
            
            // Store the entry if it is a valid match
            
            if (matched)
                mResults[mNumMatches++] = Result(i, distanceSquared);
        }
    }
    
    ratioMatched = std::min(std::max(ratioMatched, 0.0), 1.0);
    maxMatches = std::max(maxMatches, 0L);
    long numMatches = round(mNumMatches * ratioMatched);
    numMatches = (maxMatches && mNumMatches > maxMatches) ? maxMatches : mNumMatches;

    // FIX - better heuristics and more info on what has been sorted...
    
    if (size() && (mustSort || numMatches < mNumMatches))
    {
        if (numMatches < (database->numItems() / 8))
            numMatches = sortTopN(numMatches, mNumMatches);
        else
            sort(mResults, mNumMatches);
    }
    
    return mNumMatches = numMatches;
}

void Matchers::setMatchers(void *x, long argc, t_atom *argv, const EntryDatabase::ReadPointer& database)
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
                if (argc > 1 && entrymatcher_test_types(argv + 1) != TEST_NONE)
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
        double minDistance = mResults[i].mDistance;
        long swap = i;
        
        for (long j = i + 1; j < size; j++)
        {
            if (mResults[j].mDistance < minDistance)
            {
                minDistance = mResults[j].mDistance;
                swap = j;
            }
        }
        
        std::swap(mResults[swap], mResults[i]);
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
        mMatchers.back().mValues.push_back(value);
}

void Matchers::addTarget(t_symbol *value)
{
    if (mMatchers.size())
        mMatchers.back().mValues.push_back(value);;
}

void Matchers::addMatcher(TestType type, long column, double scale)
{
    mMatchers.push_back(Matcher(type, column, scale));
}
