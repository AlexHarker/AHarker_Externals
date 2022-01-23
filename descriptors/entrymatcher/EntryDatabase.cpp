
#include <algorithm>
#include <functional>
#include <cmath>

#include "EntryDatabase.h"
#include "Matchers.h"
#include "Sort.h"

t_symbol *ps_mean = gensym("mean");
t_symbol *ps_min = gensym("min");
t_symbol *ps_minimum = gensym("minimum");
t_symbol *ps_max = gensym("max");
t_symbol *ps_maximum = gensym("maximum");
t_symbol *ps_median = gensym("median");
t_symbol *ps_stddev = gensym("stddev");
t_symbol *ps_deviation = gensym("deviation");
t_symbol *ps_standard_dev = gensym("standard_dev");
t_symbol *ps_standard_deviation= gensym("standard_deviation");
t_symbol *ps_centile = gensym("centile");
t_symbol *ps_percentile = gensym("percentile");

t_symbol *ps_identfier = gensym("identifier");

/*****************************************/
// Column Setup
/*****************************************/

// Set Column Labels (with lock)

void EntryDatabase::setColumnLabelModes(void *x, long argc, t_atom *argv)
{
    write_lock_hold lock(&mLock);
    setColumnLabelModes(lock, x, argc, argv);
}

// Set Column Labels (with pre-held lock)

void EntryDatabase::setColumnLabelModes(write_lock_hold &lock, void *x, long argc, t_atom *argv)
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

// Set Column Names (with lock)

void EntryDatabase::setColumnNames(void *x, long argc, t_atom *argv)
{
    write_lock_hold lock(&mLock);
    setColumnNames(lock, x, argc, argv);
}

// Set Column Names (with pre-held lock)

void EntryDatabase::setColumnNames(write_lock_hold &lock, void *x, long argc, t_atom *argv)
{
    if (argc > numColumns())
        object_error((t_object *) x, "more names than columns");
    
    argc = (argc > numColumns()) ? numColumns() : argc;
    
    for (long i = 0; i < argc; i++)
        mColumns[i].mName = atom_getsym(argv++);
}

/*****************************************/
// Find Entries / Columns
/*****************************************/

// Search Identifiers

long EntryDatabase::searchIdentifiers(const t_atom *identifierAtom, long& idx) const
{
    CustomAtom identifier(identifierAtom, false);
    
    long gap = idx = numItems() / 2;
    gap = gap < 1 ? 1 : gap;
    
    while (gap && idx < numItems())
    {
        gap /= 2;
        gap = gap < 1 ? 1 : gap;
        
        switch (compare(identifier, getEntryIdentifier(mOrder[idx])))
        {
            case CustomAtom::kEqual:
                return mOrder[idx];
                
            case CustomAtom::kGreater:
                idx += gap;
                break;
                
            case CustomAtom::kLess:
                if (gap == 1 && (!idx || compare(identifier, getEntryIdentifier(mOrder[idx - 1])) == CustomAtom::kGreater))
                    gap = 0;
                else
                    idx -= gap;
                break;
        }
    }
    
    return -1;
}

// Find a Column from Specifier

long EntryDatabase::columnFromSpecifier(const t_atom *specifier) const
{
    if (atom_gettype(specifier) != A_SYM)
        return atom_getlong(specifier) - 1;
    
    t_symbol *columnName = atom_getsym(specifier);
    
    if (columnName == ps_identfier)
        return -1;
    
    for (long i = 0; i < numColumns(); i++)
        if (columnName == mColumns[i].mName)
            return i;
    
    return -2;
}

/*****************************************/
// Saving and Loading
/*****************************************/

// Reserve (with lock)

void EntryDatabase::reserve(long items)
{
    write_lock_hold lock(&mLock);
    
    mIdentifiers.reserve(items);
    mOrder.reserve(items);
    mEntries.reserve(items * numColumns());
    mTypes.reserve(items * numColumns());
}

// Clear (with lock)

void EntryDatabase::clear()
{
    write_lock_hold lock(&mLock);
    clear(lock);
}

// Clear (with pre-held lock)

void EntryDatabase::clear(write_lock_hold &lock)
{
    mEntries.clear();
    mIdentifiers.clear();
    mOrder.clear();
    mTypes.clear();
}

/*****************************************/
// Adding / Removing Entries
/*****************************************/

// Add (with lock)

void EntryDatabase::addEntry(void *x, long argc, t_atom *argv)
{
    write_lock_hold lock(&mLock);
    addEntry(lock, x, argc, argv);
}

// Add (with pre-held lock)

void EntryDatabase::addEntry(write_lock_hold &lock, void *x, long argc, t_atom *argv)
{
    if (!argc--)
    {
        object_error((t_object *) x, "no arguments for entry");
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
                object_error((t_object *) x, "incorrect type in entry - column number %ld", i + 1);
            
            setData(idx, i, mColumns[i].mLabel ? CustomAtom(gensym("")) : CustomAtom());
        }
    }
}

// Replace one item

void EntryDatabase::replaceItem(t_atom *identifier, long column, t_atom *item)
{
    write_lock_hold lock(&mLock);

    long order;
    long idx = searchIdentifiers(identifier, order);
    
    if (idx < 0)
        return;
    
    CustomAtom data = item;
    
    if (mColumns[column].mLabel == (data.mType == CustomAtom::kSymbol))
        setData(idx, column, data);
    else
        return;
}

// Remove entries from identifiers (with read pointer - ugraded on next method call to write)

void EntryDatabase::removeEntries(void *x, long argc, t_atom *argv)
{
    if (!argc)
    {
        object_error((t_object *) x, "no identifiers given for remove message");
        return;
    }
    
    if (argc == 1)
    {
        removeEntry(x, argv);
    }
    else
    {
        ReadWritePointer database(this);
    
        std::vector<long> indices(argc);
        long size = 0;
        
        for (long i = 0; i < argc; i++)
        {
            long idx;
            long index = searchIdentifiers(argv +i, idx);
            
            if (index > -1)
                indices[size++] = index;
        }
        
        indices.resize(size);
        sort(indices, size);
        
        if (size)
            removeEntries(database, indices);
    }
}

// Remove entries from matching (with read pointer - ugraded on next method call to write)

void EntryDatabase::removeMatchedEntries(void *x, long argc, t_atom *argv)
{
    if (!argc)
        return;
    
    ReadWritePointer database(this);
    Matchers matchers;
    std::vector<long> indices;
    long numMatches = 0;
    
    matchers.setMatchers(x, argc, argv, database);
    numMatches = matchers.match(database, true);
    indices.resize(numMatches);
        
    for (long i = 0; i < numMatches; i++)
        indices[i] = matchers.getIndex(i);
        
    if (numMatches && matchers.size())
        removeEntries(database, indices);
}

// Helpers for mass removal

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

// Remove multiple entries form sorted indices (upgrading a read pointer to a write lock)

void EntryDatabase::removeEntries(ReadWritePointer& readLockedDatabase, const std::vector<long>& sortedIndices)
{
    readLockedDatabase.promoteToWrite();
    
    long orderStart = getOrder(sortedIndices[0]);
    long offset = sortedIndices[0];
    long next = sortedIndices[0];
    long size;
    long end;
    
    // Setup new order vector
    
    std::vector<long> newOrder(numItems());
    std::copy(mOrder.begin(), mOrder.begin() + offset, newOrder.begin());
    
    for (long i = 0; i < sortedIndices.size(); offset += (next - end))
    {
        long start = next;
        
        for (++i; i < sortedIndices.size(); i++)
            if (sortedIndices[i] > sortedIndices[i - 1] + 1)
                break;
        
        end = sortedIndices[i - 1] + 1;
        next = i < sortedIndices.size() ? sortedIndices[i] : numItems();
        
        // Mark new order array for deletion
        
        for (long j = start; j < end; j++)
            newOrder[getOrder(j)] = -1;
        
        // Alter order indices
        
        for (long j = end; j < next; j++)
            newOrder[getOrder(j)] = (j - end) + offset;
    }
    
    long newSize = offset;
    
    // Remove data

    offset = sortedIndices[0];
    
    for (long i = 0; i < sortedIndices.size(); offset += size)
    {
        for (++i; i < sortedIndices.size(); i++)
            if (sortedIndices[i] > sortedIndices[i - 1] + 1)
                break;
        
        end = sortedIndices[i - 1] + 1;
        size = (i < sortedIndices.size() ? sortedIndices[i] : numItems()) - end;
        
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
    
    mIdentifiers.resize(newSize);
    mEntries.resize(newSize * numColumns());
    mTypes.resize(newSize * numColumns());
    mOrder.resize(newSize);
}

// Remove a single entry (with lock)

void EntryDatabase::removeEntry(void *x, t_atom *identifier)
{
    write_lock_hold lock(&mLock);
    
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

/*****************************************/
// Stats Calculations
/*****************************************/

void EntryDatabase::stats(void *x, std::vector<t_atom>& output, long argc, t_atom *argv) const
{
    if (argc < 2)
        return;
    
    std::vector<double> values;
    double mean = 0.0;
    bool meanCalculated = false;
    bool valuesSorted = false;
    long i;

    long column = columnFromSpecifier(argv++);
    argc--;

    output.resize(argc);

    for (i = 0; argc > 0; i++)
    {
        t_symbol *test = atom_getsym(argv++);
        argc--;
        
        if (test == ps_mean || test == ps_stddev || test == ps_deviation || test == ps_standard_dev || test == ps_standard_deviation)
        {
            if (!meanCalculated)
                mean = columnMean(column);
            if (test == ps_mean)
                atom_setfloat(&output[i], mean);
            else
                atom_setfloat(&output[i], columnStandardDeviation(column, mean));
            meanCalculated = true;
        }
        else if (test == ps_median || test == ps_centile || test == ps_percentile)
        {
            double percentile = 50.0;
            
            if (!valuesSorted)
                columnSortValues(column, values);
            if (test != ps_median)
            {
                if (argc--)
                    percentile = atom_getfloat(argv++);
                else
                    object_error((t_object *) x, "no percentile given for percentile stat");
            }
            atom_setfloat(&output[i], findPercentile(values, percentile));
            valuesSorted = true;
        }
        else if (test == ps_min || test == ps_minimum)
            atom_setfloat(&output[i], columnMin(column));
        else if (test == ps_max || test == ps_maximum)
            atom_setfloat(&output[i], columnMax(column));
        else
            object_error((t_object *) x, "unknown stat type");
    }
    
    output.resize(i);
}

double EntryDatabase::columnMin(long column) const
{
    return columnCalculate(column, HUGE_VAL, BinaryFunctor<std::min<double> >());
}

double EntryDatabase::columnMax(long column) const
{
    return columnCalculate(column, -HUGE_VAL, BinaryFunctor<std::max<double> >());
}

double EntryDatabase::columnMean(long column) const
{
    return columnCalculate(column, 0.0, std::plus<double>()) / numItems();
}

double EntryDatabase::columnStandardDeviation(long column, double mean) const
{
    struct Variance
    {
        Variance(double mean) : mMean(mean) {}
        double operator()(double sum, double data) { return sum + ((data - mMean) * (data - mMean)); }
        double mMean;
    };
    
    return sqrt(columnCalculate(column, 0.0, Variance(mean)) / numItems());
}

double EntryDatabase::columnStandardDeviation(long column) const
{
    return columnStandardDeviation(column, columnMean(column));
}

double EntryDatabase::columnPercentile(long column, double percentile) const
{
    std::vector<double> values;
    
    columnSortValues(column, values);
    return findPercentile(values, percentile);
}

double EntryDatabase::columnMedian(long column) const
{
    return columnPercentile(column, 50.0);
}

void EntryDatabase::columnSortValues(long column, std::vector<double>& sortedValues) const
{
    sortedValues.resize(numItems());

    for (long i = 0; i < numItems(); i++)
        sortedValues[i] = getData(i, column);
    
    sort(sortedValues, numItems());
}

double EntryDatabase::findPercentile(std::vector<double>& sortedValues, double percentile) const
{
    long nItems = sortedValues.size();
    
    if (!nItems)
        return std::numeric_limits<double>::quiet_NaN();
    
    long idx = std::max(0L, std::min((long) round(nItems * (percentile / 100.0)), nItems - 1));
    
    return sortedValues[idx];
}

/*****************************************/
// View
/*****************************************/

void EntryDatabase::view(t_object *database_object) const
{
    t_object *editor = (t_object *)object_new(CLASS_NOBOX, gensym("jed"), database_object, 0);
    std::string str;

    for (long i = 0; i < numItems(); i++)
    {
        str.insert(str.size(), getEntryIdentifier(i).getString());
        
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

/*****************************************/
// Saving and Loading
/*****************************************/

// File Save

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
    
    t_dictionary *dict = saveDictionary(true);
    dictionary_write(dict, filename, path);
    object_free(dict);
}

// File Load

void EntryDatabase::load(t_object *x, t_symbol *fileSpecifier)
{
    char filename[MAX_PATH_CHARS];
    short path;

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
    loadDictionary(x, dict);
    object_free(dict);
}

// Dictionary Save

t_dictionary *EntryDatabase::saveDictionary(bool entriesAsOneKey) const
{
    std::vector<t_atom> args(numColumns() + 1);
    t_dictionary *dict = dictionary_new();
    t_dictionary *dictMeta = dictionary_new();
    t_dictionary *dictData = dictionary_new();
    
    // Metadata
    
    dictionary_appendlong(dictMeta, gensym("numcolumns"), numColumns());
    
    for (long i = 0; i < numColumns(); i++)
        atom_setsym(&args[i], mColumns[i].mName);
    
    dictionary_appendatoms(dictMeta, gensym("names"), numColumns(), &args[0]);
    
    for (long i = 0; i < numColumns(); i++)
        atom_setlong(&args[i], mColumns[i].mLabel);
    
    dictionary_appendatoms(dictMeta, gensym("labelmodes"), numColumns(), &args[0]);
    dictionary_appenddictionary(dict, gensym("metadata"), (t_object *) dictMeta);
    
    // Data
    
    if (entriesAsOneKey)
    {
        std::vector<t_atom> entries(numItems());

        for (long i = 0; i < numItems(); i++)
        {
            t_dictionary *entryDict = dictionary_new();
            
            getEntryIdentifier(&args[0], i);
            
            for (long j = 0; j < numColumns(); j++)
                getDataAtom(&args[j + 1], i, j);
            
            dictionary_appendatoms(entryDict, gensym("entry"), numColumns() + 1, &args[0]);
            atom_setobj(&entries[i], entryDict);
        }
        
        dictionary_appendatoms(dictData, gensym("all_entries"), numItems(), &entries[0]);
    }
    else
    {
        for (long i = 0; i < numItems(); i++)
        {
            std::string str("entry_" + std::to_string(i + 1));
            
            getEntryIdentifier(&args[0], i);
            
            for (long j = 0; j < numColumns(); j++)
                getDataAtom(&args[j + 1], i, j);
            
            dictionary_appendatoms(dictData, gensym(str.c_str()), numColumns() + 1, &args[0]);
        }
    }
    dictionary_appenddictionary(dict, gensym("data"), (t_object *) dictData);
    
    return dict;
}

// Dictionary Load (with lock)

void EntryDatabase::loadDictionary(t_object *x, t_dictionary *dict)
{
    t_max_err err = MAX_ERR_NONE;
    t_dictionary *dictMeta = NULL;
    t_dictionary *dictData = NULL;
    
    if (!dict)
        return;
    
    dictionary_getdictionary(dict, gensym("metadata"), (t_object **) &dictMeta);
    dictionary_getdictionary(dict, gensym("data"), (t_object **) &dictData);
    
    if (dictMeta && dictData)
    {
        write_lock_hold lock(&mLock);
        clear(lock);
        
        t_atom_long newNumColumns;
        dictionary_getlong(dictMeta, gensym("numcolumns"), &newNumColumns);
        
        if (newNumColumns != numColumns())
        {
            mColumns.clear();
            mColumns.resize(newNumColumns);
        }
        
        t_atom *argv;
        long argc;
        
        dictionary_getatoms(dictMeta, gensym("names"), &argc, &argv);
        setColumnNames(lock, x, argc, argv);
        
        dictionary_getatoms(dictMeta, gensym("labelmodes"), &argc, &argv);
        setColumnLabelModes(lock, x, argc, argv);
        
        // Data
        
        if (dictionary_getatoms(dictData, gensym("all_entries"), &argc, &argv) == MAX_ERR_NONE)
        {
            for (long i = 0; i < argc; i++)
            {
                t_atom *entryArgv;
                long entryArgc;
                
                t_dictionary *entryDict = (t_dictionary *) atom_getobj(argv + i);
                if ((err = dictionary_getatoms(entryDict, gensym("entry"), &entryArgc, &entryArgv)) == MAX_ERR_NONE)
                    addEntry(lock, x, entryArgc, entryArgv);
            }
        }
        else
        {
            for (long i = 0; err == MAX_ERR_NONE; i++)
            {
                std::string str("entry_" + std::to_string(i + 1));
                if ((err = dictionary_getatoms(dictData, gensym(str.c_str()), &argc, &argv)) == MAX_ERR_NONE)
                    addEntry(lock, x, argc, argv);
            }
        }
    }
}
