
#include "EntryDatabase.h"
#include "Matchers.h"
#include "Sort.h"
#include <algorithm>
#include <functional>
#include <cmath>

void EntryDatabase::reserve(long items)
{
    HoldLock lock(&mWriteLock);
    
    mIdentifiers.reserve(items);
    mOrder.reserve(items);
    mEntries.reserve(items * numColumns());
    mTypes.reserve(items * numColumns());
}

void EntryDatabase::clear()
{
    HoldLock lock(&mWriteLock);
    clear(lock);
}

void EntryDatabase::clear(HoldLock &lock)
{
    mEntries.clear();
    mIdentifiers.clear();
    mOrder.clear();
    mTypes.clear();
}

void EntryDatabase::setColumnLabelModes(void *x, long argc, t_atom *argv)
{
    HoldLock lock(&mWriteLock);
    setColumnLabelModes(lock, x, argc, argv);
}

void EntryDatabase::setColumnLabelModes(HoldLock &lock, void *x, long argc, t_atom *argv)
{
    bool labelsModesChanged = false;
    
    if (argc > numColumns())
        object_error((t_object *) x, "more label modes than columns");
    
    argc = (argc > numColumns()) ? numColumns() : argc;
    
    for (long i = 0; i < argc; i++)
    {
        bool label = atom_getlong(argv++) ? true : false;
        labelsModesChanged |= label != mColumns[i].mLabel;
        mColumns[i].mLabel = label;
    }
    
    if (labelsModesChanged)
        clear(lock);
}

void EntryDatabase::setColumnNames(void *x, long argc, t_atom *argv)
{
    HoldLock lock(&mWriteLock);
    setColumnNames(lock, x, argc, argv);
}

void EntryDatabase::setColumnNames(HoldLock &lock, void *x, long argc, t_atom *argv)
{
    if (argc > numColumns())
        object_error((t_object *) x, "more names than columns");
    
    argc = (argc > numColumns()) ? numColumns() : argc;
    
    for (long i = 0; i < argc; i++)
        mColumns[i].mName = atom_getsym(argv++);
}

void EntryDatabase::addEntry(void *x, long argc, t_atom *argv)
{
    HoldLock lock(&mWriteLock);
    addEntry(lock, x, argc, argv);
}

void EntryDatabase::addEntry(HoldLock &lock, void *x, long argc, t_atom *argv)
{
    if (!argc--)
    {
        object_error((t_object *)x, "no arguments for entry");
        return;
    }
    
    // Get the identifier, order position and find any prexisting entry with this identifier
    
    t_atom *identifier = argv++;
    long order;
    long idx = searchIdentifiers(identifier, order);

    // Make a space for a new entry in the case that this identifier does *not* exist
    
    if (idx < 0)
    {
        idx = numItems();
        mEntries.resize((idx + 1) * numColumns());
        mTypes.resize((idx + 1) * numColumns());
        mOrder.insert(mOrder.begin() + order, idx);
        mIdentifiers.push_back(CustomAtom(identifier, false));
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

void EntryDatabase::removeEntries(void *x, long argc, t_atom *argv)
{
    if (!argc)
        object_error((t_object *)x, "no identifiers given for remove message");
    else
    {
        HoldLock lock(&mWriteLock);
        
        while (argc--)
             removeEntry(lock, x, argv++);
    }
}

void EntryDatabase::removeMatchedEntries(void *x, long argc, t_atom *argv)
{
    Matchers matchers;
    std::vector<long> indices;
    long numMatches = 0;
    
    if (argc)
    {
        ReadPointer database(this);
    
        matchers.setMatchers(x, argc, argv, database);
        numMatches = matchers.match(database, true);
        indices.resize(numMatches);
        
        for (long i = 0; i < numMatches; i++)
            indices[i] = matchers.getIndex(i);
    }
    
    if (numMatches && matchers.size())
    {
        HoldLock lock(&mWriteLock);
        
        removeEntries(lock, indices);
    }
}

template <class T> void copyRange(std::vector<T>& data, long from, long to, long size, long itemSize = 1)
{
    std::copy(data.begin() + (from * itemSize), data.begin() + (from + size) * itemSize, data.begin() + to * itemSize);
}

long EntryDatabase::getOrder(long idx)
{
    t_atom identifier;
    long order;
    mIdentifiers[idx].getAtom(&identifier);
    searchIdentifiers(&identifier, order);
    
    return order;
}

void EntryDatabase::removeEntries(HoldLock &lock, const std::vector<long>& indices)
{
    long orderStart = getOrder(indices[0]);
    long offset = indices[0];
    long next = indices[0];
    long size;
    long end;
    
    // Setup new order vector
    
    std::vector<long> newOrder(numItems());
    std::copy(mOrder.begin(), mOrder.begin() + offset, newOrder.begin());
    
    for (long i = 0; i < indices.size(); offset += (next - end))
    {
        long start = next;
        
        for (++i; i < indices.size(); i++)
            if (indices[i] > indices[i - 1] + 1)
                break;
        
        end = indices[i - 1] + 1;
        next = i < indices.size() ? indices[i] : numItems();
        
        // Mark new order array for deletion
        
        for (long j = start; j < end; j++)
            newOrder[getOrder(j)] = -1;
        
        // Alter order indices
        
        for (long j = end; j < next; j++)
                newOrder[getOrder(j)] = (j - end) + offset;
    }
    
    // Remove data

    offset = indices[0];
    
    for (long i = 0; i < indices.size(); offset += size)
    {
        for (++i; i < indices.size(); i++)
            if (indices[i] > indices[i - 1] + 1)
                break;
        
        end = indices[i - 1] + 1;
        size = (i < indices.size() ? indices[i] : numItems()) - end;
        
        // Move data
        
        copyRange(mIdentifiers, end, offset, size);
        copyRange(mTypes, end, offset, size, numColumns());
        copyRange(mEntries, end, offset, size, numColumns());
    }

    // Swap order vectors and do deletion
    
    std::swap(mOrder, newOrder);
    offset = orderStart;
    
    for (long i = orderStart; i < mOrder.size(); offset += size)
    {
        for (end = ++i; end < mOrder.size(); end++)
            if (mOrder[end] >= 0)
                break;
        
        for (i = end, size = 0; i < mOrder.size(); i++, size++)
            if (mOrder[i] < 0)
                break;
        
        copyRange(mOrder, end, offset, size);
    }
    
    // Resize storage
    
    long newSize = numItems() - indices.size();
    
    mIdentifiers.resize(newSize);
    mEntries.resize(newSize * numColumns());
    mTypes.resize(newSize * numColumns());
    mOrder.resize(newSize);
}

void EntryDatabase::removeEntry(HoldLock &lock, void *x, t_atom *identifier)
{
    long order;
    long idx = searchIdentifiers(identifier, order);
    
    if (idx < 0)
    {
        object_error((t_object *) x, "entry does not exist");
        return;
    }
    
    mIdentifiers.erase(mIdentifiers.begin() + idx);
    mOrder.erase(mOrder.begin() + order);
    mEntries.erase(mEntries.begin() + (idx * numColumns()), mEntries.begin() + ((idx + 1) * numColumns()));
    mTypes.erase(mTypes.begin() + (idx * numColumns()), mTypes.begin() + ((idx + 1) * numColumns()));
 
    std::vector<long>::iterator it = mOrder.begin();
    
    // Unrolled order updating for speed
    
    for ( ; it < (mOrder.end() - 3); it += 4)
    {
        if (it[0] > idx) it[0]--;
        if (it[1] > idx) it[1]--;
        if (it[2] > idx) it[2]--;
        if (it[3] > idx) it[3]--;
    }
    
    for ( ; it != mOrder.end(); it++)
        if (*it > idx)
            (*it)--;
}

long EntryDatabase::searchIdentifiers(const t_atom *identifierAtom, long& idx) const
{
    CustomAtom identifier(identifierAtom, false);
    
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
    return columnCalculate(specifier, HUGE_VAL, BinaryFunctor<std::min<double> >());
}

double EntryDatabase::columnMax(const t_atom *specifier) const
{
    return columnCalculate(specifier, -HUGE_VAL, BinaryFunctor<std::max<double> >());
}

double EntryDatabase::columnMean(const t_atom *specifier) const
{
    return columnCalculate(specifier, 0.0, std::plus<double>()) / numItems();
}

double EntryDatabase::columnStandardDeviation(const t_atom *specifier) const
{
    struct Variance
    {
        Variance(double mean) : mMean(mean) {}
        double operator()(double sum, double data) { return sum + ((data - mMean) * (data - mMean)); }
        double mMean;
    };

    return sqrt(columnCalculate(specifier, 0.0, Variance(columnMean(specifier))) / numItems());
}

double EntryDatabase::columnPercentile(const t_atom *specifier, double percentile) const
{
    long nItems = numItems();
    std::vector<long> indices(nItems);
    std::vector<double> values(nItems);
    
    if (!nItems)
        return std::numeric_limits<double>::quiet_NaN();
        
    long idx = std::max(0L, std::min((long) round(nItems * (percentile / 100.0)), nItems - 1));
    long column = columnFromSpecifier(specifier);
    
    for (long i = 0; i < numItems(); i++)
    {
        indices[i] = i;
        values[i] = getData(i, column);
    }

    sort(indices, values, nItems);
    
    return values[indices[idx]];
}

double EntryDatabase::columnMedian(const t_atom *specifier) const
{
    return columnPercentile(specifier, 50.0);
}

void EntryDatabase::view(t_object *database_object) const
{
    t_object *editor = (t_object *)object_new(CLASS_NOBOX, gensym("jed"), database_object, 0);
    std::string str;

    for (long i = 0; i < numItems(); i++)
    {
        str.insert(str.size(), getIdentifierInternal(i).getString());
        
        for (long j = 0; j < numColumns(); j++)
        {
            str.insert(str.size(), " ");
            str.insert(str.size(), getTypedData(i, j).getString());
        }
        
        if (i != (numItems() - 1))
            str.insert(str.size(), "\n");
    }
    
    object_method(editor, gensym("settext"), str.c_str(), gensym("utf-8"));
    object_attr_setsym(editor, gensym("title"), mName);
}

void EntryDatabase::save(t_object *x, t_symbol *fileSpecifier) const
{
    char filepath[MAX_PATH_CHARS];
    char filename[MAX_FILENAME_CHARS];
    short path;
    
    t_fourcc type;
    t_fourcc types = 'JSON';
    
    if (fileSpecifier && fileSpecifier != gensym(""))
    {
        strncpy_zero(filepath, fileSpecifier->s_name, MAX_PATH_CHARS);
        if (path_frompotentialpathname(filepath, &path, filename))
            return;
    }
    else
    {
        strcpy(filename, mName->s_name);
        if (saveasdialog_extended(filename, &path, &type, &types, 1))
            return;
    }
    
    std::vector<t_atom> args(numColumns() + 1);
    t_dictionary *dict = dictionary_new();

    dictionary_appendlong(dict, gensym("numcolumns"), numColumns());

    for (long i = 0; i < numColumns(); i++)
        atom_setsym(&args[i], mColumns[i].mName);
    
    dictionary_appendatoms(dict, gensym("names"), numColumns(), &args[0]);

    for (long i = 0; i < numColumns(); i++)
        atom_setlong(&args[i], mColumns[i].mLabel);

    dictionary_appendatoms(dict, gensym("labelmodes"), numColumns(), &args[0]);

    for (long i = 0; i < numItems(); i++)
    {
        std::string str("entry_" + std::to_string(i + 1));
        
        getEntryIdentifier(&args[0], i);
        
        for (long j = 0; j < numColumns(); j++)
            getDataAtom(&args[j + 1], i, j);
        
        dictionary_appendatoms(dict, gensym(str.c_str()), numColumns() + 1, &args[0]);
    }
    
    dictionary_write(dict, filename, path);
    object_free(dict);
}

void EntryDatabase::load(t_object *x, t_symbol *fileSpecifier)
{
    char filename[MAX_PATH_CHARS];
    short path;
    t_max_err err = MAX_ERR_NONE;

    t_fourcc type;
    t_fourcc types = 'JSON';
    
    if (fileSpecifier && fileSpecifier != gensym(""))
    {
        strncpy_zero(filename, fileSpecifier->s_name, MAX_PATH_CHARS);
        if (locatefile_extended(filename, &path, &type, &types, 1))
            return;
    }
    else
    {
        strcpy(filename, "");
        if (open_dialog(filename, &path, &type, &types, 1))
            return;
    }
    
    t_dictionary *dict;
    dictionary_read(filename, path, &dict);
 
    HoldLock lock(&mWriteLock);
    clear(lock);
    
    t_atom_long newNumColumns;
    dictionary_getlong(dict, gensym("numcolumns"), &newNumColumns);
        
    if (newNumColumns != numColumns())
    {
        mColumns.clear();
        mColumns.resize(newNumColumns);
    }
    
    t_atom *argv;
    long argc;
    
    dictionary_getatoms(dict, gensym("names"), &argc, &argv);
    setColumnNames(lock, x, argc, argv);
    
    dictionary_getatoms(dict, gensym("labelmodes"), &argc, &argv);
    setColumnLabelModes(lock, x, argc, argv);
    
    for (long i = 0; err == MAX_ERR_NONE; i++)
    {
        std::string str("entry_" + std::to_string(i + 1));
        if ((err = dictionary_getatoms(dict, gensym(str.c_str()), &argc, &argv)) == MAX_ERR_NONE)
            addEntry(lock, x, argc, argv);
    }
    
    object_free(dict);
}