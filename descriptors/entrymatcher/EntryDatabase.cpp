
#include "EntryDatabase.h"
#include <algorithm>

void EntryDatabase::reserve(long items)
{
    mIdentifiers.reserve(items);
    mOrder.reserve(items);
    mEntries.reserve(items * numColumns());
}

void EntryDatabase::clear()
{
    mEntries.clear();
    mIdentifiers.clear();
    mOrder.clear();
}

void EntryDatabase::setLabelModes(void *x, long argc, t_atom *argv)
{
    if (argc > numColumns())
        object_error((t_object *) x, "more label modes than columns");
    
    argc = (argc > numColumns()) ? numColumns() : argc;
    
    clear();
    
    for (long i = 0; i < argc; i++)
        mColumns[i].mLabel = atom_getlong(argv++) ? true : false;
}

void EntryDatabase::setNames(void *x, long argc, t_atom *argv)
{
    if (argc > numColumns())
        object_error((t_object *) x, "more names than columns");
    
    argc = (argc > numColumns()) ? numColumns() : argc;
    
    for (long i = 0; i < argc; i++)
        mColumns[i].mName = atom_getsym(argv++);
}

void EntryDatabase::addEntry(void *x, long argc, t_atom *argv)
{
    long order;
    
    if (!argc--)
    {
        object_error((t_object *)x, "no arguments for entry");
        return;
    }
    
    // Get the identifier, order position and find any prexisting entry with this identifier
    
    CustomAtom identifier = CustomAtom(argv++, false);
    long idx = searchIdentifiers(identifier, order);

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
        CustomAtom data = argv;
        
        if (i < argc && mColumns[i].mLabel == (data.mType == CustomAtom::kSymbol))
            setData(idx, i, data);
        else
        {
            if (i < argc)
                object_error((t_object *)x, "incorrect type in entry - column number %ld", i + 1);
            
            setData(idx, i, mColumns[i].mLabel ? CustomAtom(gensym("")) : CustomAtom());
        }
    }
}

void EntryDatabase::removeEntry(void *x, t_atom *identifier)
{
    long order;
    long idx = searchIdentifiers(identifier, order);
    
    if (idx < 0 || idx >= numItems())
    {
        object_error((t_object *) x, "entry does not exist");
        return;
    }
    
    mIdentifiers.erase(mIdentifiers.begin() + idx);
    mOrder.erase(mOrder.begin() + order);
    mEntries.erase(mEntries.begin() + (idx * numColumns()), mEntries.begin() + ((idx + 1) * numColumns()));
}

long EntryDatabase::searchIdentifiers(const CustomAtom& identifier, long& idx) const
{
    long gap = idx = numItems() / 2;
    gap = gap < 1 ? 1 : gap;
    
    while (gap && idx < numItems())
    {
        gap /= 2;
        gap = gap < 1 ? 1 : gap;
    
        switch (compare(identifier, getIdentifierInternal(mOrder[idx])))
        {
            case CustomAtom::kEqual:
                return mOrder[idx];
                
            case CustomAtom::kGreater:
                idx += gap;
                break;
                
            case CustomAtom::kLess:
                if (gap == 1 && (!idx || compare(identifier, getIdentifierInternal(mOrder[idx - 1])) == CustomAtom::kGreater))
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

double EntryDatabase::columnMin(const t_atom *specifier) const
{
    double minVal = HUGE_VAL;
    long column = columnFromSpecifier(specifier);
    
    if (column >= 0 && !mColumns[column].mLabel)
    {
        for (long i = 0; i < numItems(); i++)
            minVal = std::min(minVal, getData(i, column).mValue);
    }
    
    return minVal;
}

double EntryDatabase::columnMax(const t_atom *specifier) const
{
    double maxVal = -HUGE_VAL;
    long column = columnFromSpecifier(specifier);
    
    if (column >= 0 && !mColumns[column].mLabel)
    {
        for (long i = 0; i < numItems(); i++)
            maxVal = std::max(maxVal, getData(i, column).mValue);
    }
    
    return maxVal;
}

double EntryDatabase::columnMean(const t_atom *specifier) const
{
    double meanVal = 0.0;
    long column = columnFromSpecifier(specifier);
    
    if (column >= 0 && !mColumns[column].mLabel)
    {
        for (long i = 0; i < numItems(); i++)
            meanVal += getData(i, column).mValue;
    }
    
    return meanVal / numItems();
}

void EntryDatabase::view() const
{
    t_object *editor = (t_object *)object_new(CLASS_NOBOX, gensym("jed"));
    std::string str;
    
    long nItems = numItems();
    long nColumns = numColumns();
    
    for (long i = 0; i < nItems; i++)
    {
        str.insert(str.size(), getIdentifierInternal(i).getString());
        
        for (long j = 0; j < nColumns; j++)
        {
            str.insert(str.size(), " ");
            str.insert(str.size(), getData(i, j).getString());
        }
        
        if (i != (nItems - 1))
            str.insert(str.size(), "\n");
    }
    
    object_method(editor, gensym("settext"), str.c_str(), gensym("utf-8"));
    object_attr_setsym(editor, gensym("title"), gensym("cool"));
}
