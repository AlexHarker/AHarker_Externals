
#include "EntryDatabase.h"
#include <algorithm>
#include <functional>

inline bool Matcher::match(const FloatSym& data, double& overallDistance) const
{    
    double distance = HUGE_VAL;
    double value;
    
    switch (mType)
    {
        case kTestMatch:
            if (data.mType == FloatSym::kSymbol)
            {
                t_symbol *sym = data.mSymbol;
                
                for (std::vector<const FloatSym>::iterator it = mValues.begin(); it != mValues.end(); it++)
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
            
            value = data.mValue;
                
            for (std::vector<const FloatSym>::iterator it = mValues.begin(); it != mValues.end(); it++)
            {
                double currentDistance = ((*it).mValue - value) * mScale;
                distance = std::min(distance, currentDistance * currentDistance);
            }
            
            overallDistance += distance;
            return !mReject || distance <= 1.0;
            
        case kTestRatio:
            
            // FIX - check this
            
            value = data.mValue;
                
            for (std::vector<const FloatSym>::iterator it = mValues.begin(); it != mValues.end(); it++)
            {
                double currentDistance = ((*it).mValue > value) ? (*it).mValue / value : value / (*it).mValue;
                currentDistance = (currentDistance - 1.0) * mScale;
                distance = std::min(distance, currentDistance * currentDistance);
                
            }
            
            overallDistance += distance;
            return !mReject || distance <= 1.0;
    }
}

void EntryDatabase::labelModes(long argc, t_atom *argv)
{
    argc = (argc > numColumns()) ? numColumns() : argc;
    
    clear();
    
    for (long i = 0; i < argc; i++)
        mColumns[i].mLabel = atom_getlong(argv++) ? true : false;
}

void EntryDatabase::names(long argc, t_atom *argv)
{
    argc = (argc > numColumns()) ? numColumns() : argc;
    
    for (long i = 0; i < argc; i++)
        mColumns[i].mName = atom_getsym(argv++);
}

void EntryDatabase::entry(long argc, t_atom *argv)
{
    if (!argc--)
        return;
    
    // Get the identifier and find any prexisting entry with this identifier
    
    t_atom identifier = *argv++;
    long idx = itemFromIdentifier(identifier);

    // Make a space for a new entry in the case that this identifier does *not* exist
    
    if (idx < 0)
    {
        idx = numItems();
        addItem(identifier);
    }

    // Store data of the correct data type but store null data for any unspecified columns / incorrect types
    
    for (long i = 0; i < numColumns(); i++, argv++)
    {
        FloatSym data = argv;
        
        if (i < argc && mColumns[i].mLabel == (data.mType == FloatSym::kSymbol))
            setData(idx, i, data);
        else
        {
            if (i < argc)
                error("entrymatcher: incorrect type in entry - column number %ld", i + 1);
            
            setData(idx, i, mColumns[i].mLabel ? FloatSym(gensym("")) : FloatSym());
        }
    }
}

void EntryDatabase::lookup(std::vector<t_atom>& output, long idx, long argc, t_atom *argv) const
{
    if (idx < 0 || idx >= numItems())
    {
        output.resize(0);
    }
    else if (!argc)
    {
        output.resize(numColumns());
        
        // If no columns are specified construct a list of all colums for that entry
        
        for (long i = 0; i < numColumns(); i++)
            output[i] = getDataAtom(idx, i);
    }
    else
    {
        // Construct a list of the data in the specified columns (in the specified order)
        
        output.resize(argc);

        for (long i = 0; i < argc; i++)
        {
            // Get column - if not valid output from the first column
            
            long columnIndex = columnFromSpecifier(*argv++);
            columnIndex = (columnIndex < -1 || columnIndex >= numColumns()) ? 0 : columnIndex;
            
            output[i] = (columnIndex == -1) ? getIdentifier(idx) : getDataAtom(idx, columnIndex);
        }
    }
}

long EntryDatabase::calculate(std::vector<Matcher>& matchers, std::vector<long>& indices, std::vector<double>& distances) const
{
    long numMatches = 0;
    
    indices.resize(numItems());
    distances.resize(numItems());
    
    for (long i = 0; i < numItems(); i++)
    {
        // Assume a match for each entry (for the case of no matchers)
        
        bool matched = true;
        double distance = 0.0;
        
        for (std::vector<Matcher>::iterator it = matchers.begin(); it != matchers.end(); it++)
            if (!(matched = it->match(getData(i, it->getcolumn()), distance)))
                break;
        
        // Store the entry if it is a valid match
        
        if (matched)
        {
            indices[numMatches++] = i;
            distances[i] = sqrt(distance);
        }
    }
    
    return numMatches;
}

long EntryDatabase::itemFromIdentifier(t_atom& identifier) const
{
    for (long i = 0; i < numItems(); i++)
        if (compareIdentifiers(identifier, getIdentifier(i)))
            return i;

    return -1;
}

bool EntryDatabase::compareIdentifiers(const t_atom& identifier1, const t_atom& identifier2) const
{
    if (identifier1.a_type != identifier2.a_type)
        return false;
    
    switch (identifier1.a_type)
    {
        case A_LONG:        return identifier1.a_w.w_long == identifier2.a_w.w_long;
        case A_FLOAT:       return identifier1.a_w.w_float == identifier2.a_w.w_float;
        case A_SYM:         return identifier1.a_w.w_sym == identifier2.a_w.w_sym;
        default:
            return false;
    }
    
    /*
    if (atom_gettype(&identifier1) != atom_gettype(&identifier2))
        return false;
    
    switch (atom_gettype(&identifier1))
    {
        case A_LONG:    return atom_getlong(&identifier1) == atom_getlong(&identifier2);
        case A_FLOAT:   return atom_getfloat(&identifier1) == atom_getfloat(&identifier2);
        case A_SYM:     return atom_getsym(&identifier1) == atom_getsym(&identifier2);
            
        default:
            return false;
    }*/
}

long EntryDatabase::columnFromSpecifier(const t_atom& specifier) const
{
    if (atom_gettype(&specifier) != A_SYM)
        return atom_getlong(&specifier) - 1;
    
    t_symbol *columnName = atom_getsym(&specifier);
    
    if (columnName == gensym("identifier"))
        return -1;
    
    for (long i = 0; i < numColumns(); i++)
        if (columnName == mColumns[i].mName)
            return i;
    
    return -2;
}
