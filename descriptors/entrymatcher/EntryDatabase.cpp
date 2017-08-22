
#include "EntryDatabase.h"
#include "Matchers.h"
#include "Sort.h"
#include <algorithm>
#include <functional>
#include <cmath>

void EntryDatabase::reserve(long items)
{
    // FIX - do nothing....
}

void EntryDatabase::clear()
{
    HoldLock lock(&mWriteLock);
    clear(lock);
}

void EntryDatabase::clear(HoldLock &lock)
{
    mEntryList.clear();
    mMap.clear();
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
    std::list<Entry>::iterator it = searchIdentifiers(identifier);

    // Make a space for a new entry in the case that this identifier does *not* exist
    
    if (it == mEntryList.end())
    {
        CustomAtom id = CustomAtom(identifier, false);
        mEntryList.push_back(Entry(id, numColumns()));
        it = --mEntryList.end();
        mMap.insert(std::pair<CustomAtom, std::list<Entry>::iterator>(id, it));
    }

    // Store data of the correct data type but store null data for any unspecified columns / incorrect types
    
    for (long i = 0; i < numColumns(); i++, argv++)
    {
        CustomAtom data = argv;
        
        if (i < argc && mColumns[i].mLabel == (data.mType == CustomAtom::kSymbol))
            it->setData(i, data);
        else
        {
            if (i < argc)
                object_error((t_object *)x, "incorrect type in entry - column number %ld", i + 1);
            
            it->setData(i, mColumns[i].mLabel ? CustomAtom(gensym("")) : CustomAtom());
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
    std::vector<std::list<Entry>::const_iterator> iterators;
    long numMatches = 0;
    
    if (argc)
    {
        ReadPointer database(this);
    
        matchers.setMatchers(x, argc, argv, database);
        numMatches = matchers.match(database, true);
        iterators.resize(numMatches);
        
        for (long i = 0; i < numMatches; i++)
            iterators[i] = matchers.getIterator(i);
    }
    
    if (numMatches && matchers.size())
    {
        HoldLock lock(&mWriteLock);
        
        for (long i = 0; i < numMatches; i++)
        {
            CustomAtom id = iterators[i]->mIdentifier;
            mEntryList.erase(iterators[i]);
            mMap.erase(mMap.find(id));
        }
    }
}

void EntryDatabase::removeEntry(HoldLock &lock, void *x, t_atom *identifier)
{
    std::list<Entry>::iterator it = searchIdentifiers(identifier);
    
    if (it == mEntryList.end())
    {
        object_error((t_object *) x, "entry does not exist");
        return;
    }
    
    CustomAtom id = it->mIdentifier;
    mEntryList.erase(it);
    mMap.erase(mMap.find(id));
}

std::list<EntryDatabase::Entry>::const_iterator EntryDatabase::findItem(const t_atom *identifierAtom) const
{
    CustomAtom identifier(identifierAtom, false);
    std::map<CustomAtom, std::list<Entry>::iterator>::const_iterator it = mMap.find(identifier);
    
    return it == mMap.cend() ? mEntryList.end() : it->second;
}

std::list<EntryDatabase::Entry>::iterator EntryDatabase::searchIdentifiers(const t_atom *identifierAtom)
{
    CustomAtom identifier(identifierAtom, false);
    std::map<CustomAtom, std::list<Entry>::iterator>::const_iterator it = mMap.find(identifier);
    
    return it == mMap.cend() ? mEntryList.end() : it->second;
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
    long i = 0;
    
    for (std::list<Entry>::const_iterator it = mEntryList.cbegin(); it != mEntryList.cend(); it++)
    {
        indices[i] = i;
        values[i] = it->getData(column);
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

    for (std::list<Entry>::const_iterator it = mEntryList.cbegin(); it != mEntryList.cend(); it++)
    {
        str.insert(str.size(), it->mIdentifier.getString());
        
        for (long j = 0; j < numColumns(); j++)
        {
            str.insert(str.size(), " ");
            str.insert(str.size(), it->getTypedData(j).getString());
        }
        
        if (it != --mEntryList.end())
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

    long i = 0;
    for (std::list<Entry>::iterator it; it != mEntryList.end(); it++, i++)
    {
        std::string str("entry_" + std::to_string(i + 1));
        
        it->mIdentifier.getAtom(&args[0]);
        
        for (long j = 0; j < numColumns(); j++)
            it->getDataAtom(&args[j + 1], j);
        
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