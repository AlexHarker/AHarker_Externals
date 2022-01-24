
#include <algorithm>
#include <functional>
#include <cmath>

#include "entry_database.hpp"
#include "matchers.hpp"
#include "sort.hpp"

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
    write_lock_hold lock(&m_lock);
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
        labelsModesChanged |= label != m_columns[i].m_label;
        m_columns[i].m_label = label;
    }
    
    if (labelsModesChanged)
        clear(lock);
}

// Set Column Names (with lock)

void EntryDatabase::setColumnNames(void *x, long argc, t_atom *argv)
{
    write_lock_hold lock(&m_lock);
    setColumnNames(lock, x, argc, argv);
}

// Set Column Names (with pre-held lock)

void EntryDatabase::setColumnNames(write_lock_hold &lock, void *x, long argc, t_atom *argv)
{
    if (argc > numColumns())
        object_error((t_object *) x, "more names than columns");
    
    argc = (argc > numColumns()) ? numColumns() : argc;
    
    for (long i = 0; i < argc; i++)
        m_columns[i].m_name = atom_getsym(argv++);
}

/*****************************************/
// Find Entries / Columns
/*****************************************/

// Search Identifiers

long EntryDatabase::search_identifiers(const t_atom *identifierAtom, long& idx) const
{
    t_custom_atom identifier(identifierAtom, false);
    
    long gap = idx = numItems() / 2;
    gap = gap < 1 ? 1 : gap;
    
    while (gap && idx < numItems())
    {
        gap /= 2;
        gap = gap < 1 ? 1 : gap;
        
        switch (compare(identifier, getEntryIdentifier(m_order[idx])))
        {
            case t_custom_atom::kEqual:
                return m_order[idx];
                
            case t_custom_atom::kGreater:
                idx += gap;
                break;
                
            case t_custom_atom::kLess:
                if (gap == 1 && (!idx || compare(identifier, getEntryIdentifier(m_order[idx - 1])) == t_custom_atom::kGreater))
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
        if (columnName == m_columns[i].m_name)
            return i;
    
    return -2;
}

/*****************************************/
// Saving and Loading
/*****************************************/

// Reserve (with lock)

void EntryDatabase::reserve(long items)
{
    write_lock_hold lock(&m_lock);
    
    m_identifiers.reserve(items);
    m_order.reserve(items);
    m_entries.reserve(items * numColumns());
    m_types.reserve(items * numColumns());
}

// Clear (with lock)

void EntryDatabase::clear()
{
    write_lock_hold lock(&m_lock);
    clear(lock);
}

// Clear (with pre-held lock)

void EntryDatabase::clear(write_lock_hold &lock)
{
    m_entries.clear();
    m_identifiers.clear();
    m_order.clear();
    m_types.clear();
}

/*****************************************/
// Adding / Removing Entries
/*****************************************/

// Add (with lock)

void EntryDatabase::addEntry(void *x, long argc, t_atom *argv)
{
    write_lock_hold lock(&m_lock);
    add_entry(lock, x, argc, argv);
}

// Add (with pre-held lock)

void EntryDatabase::add_entry(write_lock_hold &lock, void *x, long argc, t_atom *argv)
{
    if (!argc--)
    {
        object_error((t_object *) x, "no arguments for entry");
        return;
    }
    
    // Get the identifier, order position and find any prexisting entry with this identifier
    
    t_atom *identifier = argv++;
    long order;
    long idx = search_identifiers(identifier, order);

    // Make a space for a new entry in the case that this identifier does *not* exist
    
    if (idx < 0)
    {
        idx = numItems();
        m_entries.resize((idx + 1) * numColumns());
        m_types.resize((idx + 1) * numColumns());
        m_order.insert(m_order.begin() + order, idx);
        m_identifiers.push_back(t_custom_atom(identifier, false));
    }

    // Store data of the correct data type but store null data for any unspecified columns / incorrect types
    
    for (long i = 0; i < numColumns(); i++, argv++)
    {
        t_custom_atom data = argv;
        
        if (i < argc && m_columns[i].m_label == (data.m_type == t_custom_atom::kSymbol))
            set_data(idx, i, data);
        else
        {
            if (i < argc)
                object_error((t_object *) x, "incorrect type in entry - column number %ld", i + 1);
            
            set_data(idx, i, m_columns[i].m_label ? t_custom_atom(gensym("")) : t_custom_atom());
        }
    }
}

// Replace one item

void EntryDatabase::replaceItem(t_atom *identifier, long column, t_atom *item)
{
    write_lock_hold lock(&m_lock);

    long order;
    long idx = search_identifiers(identifier, order);
    
    if (idx < 0)
        return;
    
    t_custom_atom data = item;
    
    if (m_columns[column].m_label == (data.m_type == t_custom_atom::kSymbol))
        set_data(idx, column, data);
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
        remove_entry(x, argv);
    }
    else
    {
        ReadWritePointer database(this);
    
        std::vector<long> indices(argc);
        long size = 0;
        
        for (long i = 0; i < argc; i++)
        {
            long idx;
            long index = search_identifiers(argv +i, idx);
            
            if (index > -1)
                indices[size++] = index;
        }
        
        indices.resize(size);
        sort(indices, size);
        
        if (size)
            remove_entries(database, indices);
    }
}

// Remove entries from matching (with read pointer - ugraded on next method call to write)

void EntryDatabase::removeMatchedEntries(void *x, long argc, t_atom *argv)
{
    if (!argc)
        return;
    
    ReadWritePointer database(this);
    matchers matchers;
    std::vector<long> indices;
    long numMatches = 0;
    
    matchers.set_matchers(x, argc, argv, database);
    numMatches = matchers.match(database, true);
    indices.resize(numMatches);
        
    for (long i = 0; i < numMatches; i++)
        indices[i] = matchers.get_index(i);
        
    if (numMatches && matchers.size())
        remove_entries(database, indices);
}

// Helpers for mass removal

template <class T> void copyRange(std::vector<T>& data, long from, long to, long size, long itemSize = 1)
{
    std::copy(data.begin() + (from * itemSize), data.begin() + (from + size) * itemSize, data.begin() + to * itemSize);
}

long EntryDatabase::get_order(long idx)
{
    t_atom identifier;
    long order;
    m_identifiers[idx].get_atom(&identifier);
    search_identifiers(&identifier, order);
    
    return order;
}

// Remove multiple entries form sorted indices (upgrading a read pointer to a write lock)

void EntryDatabase::remove_entries(ReadWritePointer& readLockedDatabase, const std::vector<long>& sortedIndices)
{
    readLockedDatabase.promoteToWrite();
    
    long orderStart = get_order(sortedIndices[0]);
    long offset = sortedIndices[0];
    long next = sortedIndices[0];
    long size;
    long end;
    
    // Setup new order vector
    
    std::vector<long> newOrder(numItems());
    std::copy(m_order.begin(), m_order.begin() + offset, newOrder.begin());
    
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
            newOrder[get_order(j)] = -1;
        
        // Alter order indices
        
        for (long j = end; j < next; j++)
            newOrder[get_order(j)] = (j - end) + offset;
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
        
        copyRange(m_identifiers, end, offset, size);
        copyRange(m_types, end, offset, size, numColumns());
        copyRange(m_entries, end, offset, size, numColumns());
    }

    // Swap order vectors and do deletion
    
    std::swap(m_order, newOrder);
    offset = orderStart;
    
    for (long i = orderStart; i < m_order.size(); offset += size)
    {
        for (end = ++i; end < m_order.size(); end++)
            if (m_order[end] >= 0)
                break;
        
        for (i = end, size = 0; i < m_order.size(); i++, size++)
            if (m_order[i] < 0)
                break;
        
        copyRange(m_order, end, offset, size);
    }
    
    // Resize storage
    
    m_identifiers.resize(newSize);
    m_entries.resize(newSize * numColumns());
    m_types.resize(newSize * numColumns());
    m_order.resize(newSize);
}

// Remove a single entry (with lock)

void EntryDatabase::remove_entry(void *x, t_atom *identifier)
{
    write_lock_hold lock(&m_lock);
    
    long order;
    long idx = search_identifiers(identifier, order);
    
    if (idx < 0)
    {
        object_error((t_object *) x, "entry does not exist");
        return;
    }
    
    m_identifiers.erase(m_identifiers.begin() + idx);
    m_order.erase(m_order.begin() + order);
    m_entries.erase(m_entries.begin() + (idx * numColumns()), m_entries.begin() + ((idx + 1) * numColumns()));
    m_types.erase(m_types.begin() + (idx * numColumns()), m_types.begin() + ((idx + 1) * numColumns()));
 
    std::vector<long>::iterator it = m_order.begin();
    
    // Unrolled order updating for speed
    
    for ( ; it < (m_order.end() - 3); it += 4)
    {
        if (it[0] > idx) it[0]--;
        if (it[1] > idx) it[1]--;
        if (it[2] > idx) it[2]--;
        if (it[3] > idx) it[3]--;
    }
    
    for ( ; it != m_order.end(); it++)
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
    bool mean_calculated = false;
    bool sorted = false;
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
            if (!mean_calculated)
                mean = column_mean(column);
            if (test == ps_mean)
                atom_setfloat(&output[i], mean);
            else
                atom_setfloat(&output[i], column_standard_deviation(column, mean));
            mean_calculated = true;
        }
        else if (test == ps_median || test == ps_centile || test == ps_percentile)
        {
            double percentile = 50.0;
            
            if (!sorted)
                column_sort_values(column, values);
            if (test != ps_median)
            {
                if (argc--)
                    percentile = atom_getfloat(argv++);
                else
                    object_error((t_object *) x, "no percentile given for percentile stat");
            }
            atom_setfloat(&output[i], find_percentile(values, percentile));
            sorted = true;
        }
        else if (test == ps_min || test == ps_minimum)
            atom_setfloat(&output[i], column_min(column));
        else if (test == ps_max || test == ps_maximum)
            atom_setfloat(&output[i], column_max(column));
        else
            object_error((t_object *) x, "unknown stat type");
    }
    
    output.resize(i);
}

double EntryDatabase::column_min(long column) const
{
    return column_calculate(column, HUGE_VAL, BinaryFunctor<std::min<double> >());
}

double EntryDatabase::column_max(long column) const
{
    return column_calculate(column, -HUGE_VAL, BinaryFunctor<std::max<double> >());
}

double EntryDatabase::column_mean(long column) const
{
    return column_calculate(column, 0.0, std::plus<double>()) / numItems();
}

double EntryDatabase::column_standard_deviation(long column, double mean) const
{
    struct variance
    {
        variance(double mean) : m_mean(mean) {}
        double operator()(double sum, double data) { return sum + ((data - m_mean) * (data - m_mean)); }
        double m_mean;
    };
    
    return sqrt(column_calculate(column, 0.0, variance(mean)) / numItems());
}

double EntryDatabase::column_standard_deviation(long column) const
{
    return column_standard_deviation(column, column_mean(column));
}

double EntryDatabase::column_percentile(long column, double percentile) const
{
    std::vector<double> values;
    
    column_sort_values(column, values);
    return find_percentile(values, percentile);
}

double EntryDatabase::column_median(long column) const
{
    return column_percentile(column, 50.0);
}

void EntryDatabase::column_sort_values(long column, std::vector<double>& sortedValues) const
{
    sortedValues.resize(numItems());

    for (long i = 0; i < numItems(); i++)
        sortedValues[i] = getData(i, column);
    
    sort(sortedValues, numItems());
}

double EntryDatabase::find_percentile(std::vector<double>& sortedValues, double percentile) const
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
        str.insert(str.size(), getEntryIdentifier(i).get_string());
        
        for (long j = 0; j < numColumns(); j++)
        {
            str.insert(str.size(), " ");
            str.insert(str.size(), getTypedData(i, j).get_string());
        }
        
        if (i != (numItems() - 1))
            str.insert(str.size(), "\n");
    }
    
    object_method(editor, gensym("settext"), str.c_str(), gensym("utf-8"));
    object_attr_setsym(editor, gensym("title"), m_name);
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
        strcpy(filename, m_name->s_name);
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
    
    dictionary_appendlong(dictMeta, gensym("numColumns"), numColumns());
    
    for (long i = 0; i < numColumns(); i++)
        atom_setsym(&args[i], m_columns[i].m_name);
    
    dictionary_appendatoms(dictMeta, gensym("names"), numColumns(), &args[0]);
    
    for (long i = 0; i < numColumns(); i++)
        atom_setlong(&args[i], m_columns[i].m_label);
    
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
    t_dictionary *dictMeta = nullptr;
    t_dictionary *dictData = nullptr;
    
    if (!dict)
        return;
    
    dictionary_getdictionary(dict, gensym("metadata"), (t_object **) &dictMeta);
    dictionary_getdictionary(dict, gensym("data"), (t_object **) &dictData);
    
    if (dictMeta && dictData)
    {
        write_lock_hold lock(&m_lock);
        clear(lock);
        
        t_atom_long newnumColumns;
        dictionary_getlong(dictMeta, gensym("numColumns"), &newnumColumns);
        
        if (newnumColumns != numColumns())
        {
            m_columns.clear();
            m_columns.resize(newnumColumns);
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
                    add_entry(lock, x, entryArgc, entryArgv);
            }
        }
        else
        {
            for (long i = 0; err == MAX_ERR_NONE; i++)
            {
                std::string str("entry_" + std::to_string(i + 1));
                if ((err = dictionary_getatoms(dictData, gensym(str.c_str()), &argc, &argv)) == MAX_ERR_NONE)
                    add_entry(lock, x, argc, argv);
            }
        }
    }
}
