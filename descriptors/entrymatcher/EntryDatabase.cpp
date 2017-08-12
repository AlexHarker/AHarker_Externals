
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
    long order;
    
    if (!argc--)
        return;
    
    // Get the identifier, order position and find any prexisting entry with this identifier
    
    FloatSym identifier = FloatSym(argv++, false);
    long idx = itemFromIdentifier(identifier, order);

    // Make a space for a new entry in the case that this identifier does *not* exist
    
    if (idx < 0)
    {
        idx = numItems();
        mEntries.resize((idx + 1) * numColumns());
        mOrder.insert(mOrder.begin() + order, idx);
        mIdentifiers.push_back(identifier);
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
    long order;
    
    if (idx < 0 || idx >= numItems())
        return;
    
    itemFromIdentifier(getIdentifierInternal(idx), order);
    mOrder.erase(mOrder.begin() + order);
    mIdentifiers.erase(mIdentifiers.begin() + idx);
    mEntries.erase(mEntries.begin() + (idx * numColumns()), mEntries.begin() + ((idx + 1) * numColumns()));
}

long EntryDatabase::itemFromIdentifier(const FloatSym& identifier, long& idx) const
{
    long gap = idx = numItems() / 2;

    while (gap && idx < numItems())
    {
        gap /= 2;
        gap = gap < 1 ? 1 : gap;
    
        switch (compare(identifier, getIdentifierInternal(mOrder[idx])))
        {
            case FloatSym::kEqual:
                return mOrder[idx];
                
            case FloatSym::kGreater:
                idx += gap;
                break;
                
            case FloatSym::kLess:
                if (gap == 1 && (!idx || compare(identifier, getIdentifierInternal(mOrder[idx - 1])) == FloatSym::kGreater))
                    gap = 0;
                else
                    idx -= gap;
                break;
        }
    }
  
    return -1;
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
