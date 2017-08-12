
#include "EntryDatabase.h"

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
    
    t_atom *identifier = argv++;
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
            
            long columnIndex = columnFromSpecifier(argv++);
            columnIndex = (columnIndex < -1 || columnIndex >= numColumns()) ? 0 : columnIndex;
            
            output[i] = (columnIndex == -1) ? getIdentifier(idx) : getDataAtom(idx, columnIndex);
        }
    }
}

void EntryDatabase::remove(long idx)
{
    if (idx < 0 || idx >= numItems())
        return;
    
    mIdentifiers.erase(mIdentifiers.begin() + idx);
    mEntries.erase(mEntries.begin() + (idx * numColumns()), mEntries.begin() + ((idx + 1) * numColumns()));
}

long EntryDatabase::itemFromIdentifier(t_atom *identifier) const
{
    for (long i = 0; i < numItems(); i++)
        if (compareIdentifiers(FloatSym(identifier, false), getIdentifierInternal(i)))
            return i;

    return -1;
}

bool EntryDatabase::compareIdentifiers(const FloatSym& identifier1, const FloatSym& identifier2) const
{
    if (identifier1.mType != identifier2.mType)
        return false;
    
    switch (identifier1.mType)
    {
        case FloatSym::kInt:        return identifier1.mInt == identifier2.mInt;
        case FloatSym::kDouble:     return identifier1.mValue == identifier2.mValue;
        case FloatSym::kSymbol:     return identifier1.mSymbol == identifier2.mSymbol;
        default:
            return false;
    }
}

long EntryDatabase::columnFromSpecifier(const t_atom *specifier) const
{
    if (atom_gettype(specifier) != A_SYM)
        return atom_getlong(specifier) - 1;
    
    t_symbol *columnName = atom_getsym(specifier);
    
    if (columnName == gensym("identifier"))
        return -1;
    
    for (long i = 0; i < numColumns(); i++)
        if (columnName == mColumns[i].mName)
            return i;
    
    return -2;
}
