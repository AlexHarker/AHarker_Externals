
#include <jpatcher_api.h>
#include <jgraphics.h>
#include <jdataview.h>
#include <numeric>
#include <vector>

#include <AH_Lifecycle.hpp>

#include "database_view.hpp"
#include "entry_database.hpp"
#include "sort.hpp"

// Globals and Object Structure

t_class *database_view_class = nullptr;

struct t_database_view
{
    t_jbox d_box;
    
    t_object *dataview;
    t_object *database_object;
    t_object *patcherview;
    
    entries *database;
        
    t_atom edit_identifier;
    
    bool sort_direction;
    std::vector<long> row_map;
};

// Function Prototypes

void *database_view_new(t_symbol *s, short argc, t_atom *argv);
void database_view_free(t_database_view *x);

void database_view_newpatcherview(t_database_view *x, t_object *patcherview);
void database_view_freepatcherview(t_database_view *x, t_object *patcherview);

void database_view_set_database(t_database_view *x, t_object *database_object, entries *database);
void database_view_update(t_database_view *x);

t_max_err database_view_notify(t_database_view *x, t_symbol *s, t_symbol *msg, void *sender, void *data);

void database_view_getcellvalue(t_database_view *x, t_symbol *colname, t_rowref rr, long *argc, t_atom *argv);

void database_view_getcelltext(t_database_view *x, t_symbol *colname, t_rowref rr, char *text, long maxlen);
void database_view_getcellstyle(t_database_view *x, t_symbol *colname, t_rowref rr, long *style, long *align);

void database_view_editstarted(t_database_view *x, t_symbol *colname, t_rowref rr);
void database_view_celledited(t_database_view *x, t_symbol *colname, t_rowref rr, long argc, t_atom *argv);
void database_view_editended(t_database_view *x, t_symbol *colname, t_rowref rr);

void database_view_component(t_database_view *x, t_symbol *colname, t_rowref rr, long *component, long *options);
void database_view_sort(t_database_view *x, t_symbol *colname, t_privatesortrec *record);

// N.B. required for custom sorting

long database_view_dummycompare(t_rowref a, t_rowref b) { return 1; }

// Column + Row Mappers / Utilities

t_symbol *ps_first_colname = gensym("0");

enum header_column : long
{
    COLINDEX_ITEM_NUMBER = -2,
    COLINDEX_IDENTIFIER = -1,
};

t_symbol *get_colname_from_index(long idx)
{
    return gensym(std::to_string(idx).c_str());
}

long get_index_from_colname(t_symbol *colname)
{
    return std::stol(colname->s_name) - 2;
}

long map_rowref_to_index(t_database_view *x, t_rowref rr)
{
    long index = static_cast<long>(reinterpret_cast<t_ptr_int>(rr)) - 1;
    
    if (index >= x->row_map.size())
        return index;
    
    if (x->sort_direction)
        return x->row_map[index];
    else
        return x->row_map[x->row_map.size() - index - 1];
}

t_rowref map_index_to_rowref(t_database_view *x, long index)
{
    if (index >= x->row_map.size())
        return reinterpret_cast<t_rowref>(static_cast<t_ptr_int>(index + 1));
    
    auto it = std::find(x->row_map.begin(), x->row_map.end(), index);
    if (x->sort_direction)
        return reinterpret_cast<t_rowref>(static_cast<t_ptr_int>(it - x->row_map.begin() + 1));
    else
        return reinterpret_cast<t_rowref>(static_cast<t_ptr_int>(x->row_map.end() - it));
}

struct rowref_sequence : std::vector<t_rowref>
{
    rowref_sequence(long size, long start) : std::vector<t_rowref>(size)
    {
        for (auto it = begin(); it != end(); it++)
            (*it) = reinterpret_cast<t_rowref>(static_cast<t_ptr_int>(++start));
    }
    
    long size() { return static_cast<long>(std::vector<t_rowref>::size()); }
};

// Init (like main)

void database_view_init()
{
    t_class *c = class_new(private_strings::view_classname(),
                           (method) database_view_new,
                           (method) database_view_free,
                           sizeof(t_database_view),
                           (method) nullptr,
                           A_GIMME,
                           0);
    
    c->c_flags |= CLASS_FLAG_NEWDICTIONARY;
    
    long flags = JBOX_COLOR;
    
    jbox_initclass(c, flags);
    
    class_addmethod(c, (method) database_view_newpatcherview, "newpatcherview", A_CANT, 0);
    class_addmethod(c, (method) database_view_freepatcherview, "freepatcherview", A_CANT, 0);
      
    class_addmethod(c, (method) database_view_set_database, private_strings::set_database(), A_CANT, 0);
    class_addmethod(c, (method) database_view_update, private_strings::build_view(), A_CANT, 0);
    
    class_addmethod(c, (method) database_view_notify, "notify", A_CANT, 0);

    class_addmethod(c, (method) database_view_getcellvalue, "getcellvalue", A_CANT, 0);
    class_addmethod(c, (method) database_view_getcelltext, "getcelltext", A_CANT, 0);
    class_addmethod(c, (method) database_view_getcellstyle, "getcellstyle", A_CANT, 0);

    class_addmethod(c, (method) database_view_editstarted, "editstarted", A_CANT, 0);
    class_addmethod(c, (method) database_view_celledited, "editvalue", A_CANT, 0);
    class_addmethod(c, (method) database_view_editended, "editended", A_CANT, 0);
    
    class_addmethod(c, (method) database_view_dummycompare, "__never_called", A_CANT, 0);
    
    class_addmethod(c, (method) database_view_component, "component", A_CANT, 0);
    class_addmethod(c, (method) database_view_sort, "sortdata", A_CANT, 0);
    
    class_register(CLASS_BOX, c);
    database_view_class = c;
}

// New / Free

void *database_view_new(t_symbol *s, short argc, t_atom *argv)
{
    t_database_view *x;
    t_dictionary *d = nullptr;
    
    if (!(d = object_dictionaryarg(argc, argv)))
        return nullptr;
    
    x = (t_database_view *) object_alloc(database_view_class);
    
    if (x)
    {
        create_object(x->row_map);
     
        // Create and set up the view
        
        long flags =  JBOX_NODRAWBOX | JBOX_GROWBOTH | JBOX_NOFLOATINSPECTOR;
        
        jbox_new(&x->d_box, flags, argc, argv);
        x->d_box.b_firstin = (t_object *) x;
        
        x->dataview = (t_object *) jdataview_new();
        jdataview_setclient(x->dataview, (t_object *) x);
        jdataview_setcolumnheaderheight(x->dataview, 30);
        jdataview_setheight(x->dataview, 22);
        jdataview_setautosizeright(x->dataview, 1);
        jdataview_setautosizerightcolumn(x->dataview, 1);
        jdataview_setdragenabled(x->dataview, 0);
        jdataview_setscrollvisible(x->dataview, 1, 1);
     
        x->database = nullptr;

        atom_setobj(&x->edit_identifier, nullptr);
        
        attr_dictionary_process(x, d);

        jbox_ready(&x->d_box);
    }
    
    return x;
}

void database_view_free(t_database_view *x)
{
    // Notify the database object that the view has been removed
    
    object_method(x->database_object, gensym(private_strings::view_removed()));
    
    // Free resources
    
    object_free(x->dataview);
    jbox_free(&x->d_box);
    destroy_object(x->row_map);
}

// Patcher Views

void database_view_newpatcherview(t_database_view *x, t_object *patcherview)
{
    x->patcherview = patcherview;
    object_attach_byptr(x, patcherview);
    jdataview_patchervis(x->dataview, patcherview, (t_object *) x);
}

void database_view_freepatcherview(t_database_view *x, t_object *patcherview)
{
    x->patcherview = nullptr;
    object_detach_byptr(x, patcherview);
    jdataview_patcherinvis(x->dataview, patcherview);
}

// Set Database

void database_view_set_database(t_database_view *x, t_object *database_object, entries *database)
{
    x->database = database;
    x->database_object = database_object;
    database_view_update(x);
}

// Update

void database_view_update(t_database_view *x)
{
    // Don't update if there isn't a database
    
    if (!x->database)
        return;
    
    auto database = x->database->get_read_access();
        
    // Update columns
    
    const long num_view_columns = jdataview_getnumcolumns(x->dataview);
    const long required_columns = database.num_columns() + 2;
    
    if (required_columns != num_view_columns)
    {
        if (num_view_columns < required_columns)
        {
            // Add new columns and set their style
            
            for (long i = num_view_columns; i < required_columns; i++)
            {
                t_object *column = jdataview_addcolumn(x->dataview, get_colname_from_index(i), nullptr, 0);
                
                // N.B. - custom sort must be set in order to override...

                jcolumn_setminwidth(column, 50);
                jcolumn_setwidth(column, 90);
                jcolumn_setmaxwidth(column, 300);
                jcolumn_setdraggable(column, 0);
                jcolumn_setsortable(column, 1);
                jcolumn_setcelltextstylemsg(column, gensym("getcellstyle"));
                jcolumn_setcustomsort(column, gensym("__never_called"));
                jcolumn_setoverridesort(column, 1);
                
                if (i == 0)
                    jcolumn_setinitiallysorted(column, JCOLUMN_INITIALLYSORTED_FORWARDS);
                
                if (i >= 2)
                {
                    jcolumn_sethideable(column, 1);
                    jcolumn_setrowcomponentmsg(column, gensym("component"));
                    jcolumn_setvaluemsg(column, gensym("editvalue"), gensym("editstarted"), gensym("editended"));
                    jcolumn_setnumeric(column, !database.get_column_label_mode(i));
                }
            }
        }
        else
        {
            // Delete unused columns
            
            for (long i = required_columns; i < num_view_columns; i++)
                jdataview_deletecolumn(x->dataview, jdataview_getnthcolumn(x->dataview, i));
        }
    }

    // Update column labels (# and identifier columns / data)
    
    jcolumn_setlabel(jdataview_getnthcolumn(x->dataview, 0), gensym("#"));
    jcolumn_setlabel(jdataview_getnthcolumn(x->dataview, 1), gensym("identifier"));
        
    for (long i = 0; i < database.num_columns(); i++)
        jcolumn_setlabel(jdataview_getnthcolumn(x->dataview,  i + 2), database.get_column_name(i));

    // Update rows
    
    long num_view_rows = jdataview_getnumrows(x->dataview);
    long required_items = database.num_items();
    
    if (num_view_rows != required_items)
    {
        if (!required_items)
        {
            // No data - clear the view
            
            jdataview_clear(x->dataview);
        }
        else if (num_view_rows < required_items)
        {
            // Create additional rows
            
            rowref_sequence refs(required_items - num_view_rows, num_view_rows);
            jdataview_addrows(x->dataview, refs.size(), refs.data());
        }
        else
        {
            // Delete additional rows
            
            rowref_sequence refs(num_view_rows - required_items, required_items);
            jdataview_deleterows(x->dataview, refs.size(), refs.data());
        }
    }
    
    for (long i = 0; i < database.num_columns(); i++)
        jdataview_redrawcolumn(x->dataview, database.get_column_name(i));
}

// Notifications

t_max_err database_view_notify(t_database_view *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
    if (msg == gensym("attr_modified") && object_classname(sender) == gensym("patcherview"))
    {
        // Get attibute name
        
        t_symbol *attrname = (t_symbol *)object_method((t_object *)data, gensym("getname"));
        
        if (attrname == gensym("rect"))
        {
            t_rect rect;
        
            // Get rect and then set its position to (0, 0) to set the view size
            
            patcherview_get_rect((t_object *) sender, &rect);
            rect.x = rect.y = 0;
            jbox_set_rect_for_view((t_object *) &x->d_box, (t_object *) sender, &rect);
        }
    }
    
    return jbox_notify((t_jbox *) x, s, msg, sender, data);
}

// Cell Value

void database_view_getcellvalue(t_database_view *x, t_symbol *colname, t_rowref rr, long *argc, t_atom *argv)
{
    auto database = x->database->get_read_access();
    
    long column_index = get_index_from_colname(colname);
    long row_index = map_rowref_to_index(x, rr);

    *argc = 1;
    database.get_atom(argv, row_index, column_index);
}

// Cell Text and Style

void database_view_getcelltext(t_database_view *x, t_symbol *colname, t_rowref rr, char *text, long maxlen)
{
    if (x->database)
    {
        std::string str;

        auto database = x->database->get_read_access();
        
        long column_index = get_index_from_colname(colname);
        long row_index = map_rowref_to_index(x, rr);

        if (column_index == COLINDEX_ITEM_NUMBER)
            str = std::to_string(row_index + 1);
        else if (column_index == COLINDEX_IDENTIFIER)
            str = database.get_entry_identifier(row_index).get_string();
        else
            str = database.get_string(row_index, column_index);

        strncpy_zero(text, str.c_str(), maxlen - 1);
    }
}

void database_view_getcellstyle(t_database_view *x, t_symbol *colname, t_rowref rr, long *style, long *align)
{
    *style = (colname == ps_first_colname) ? JCOLUMN_STYLE_ITALIC : JCOLUMN_STYLE_PLAIN;
    *align = JCOLUMN_ALIGN_CENTER;
}

// Editing

void database_view_editstarted(t_database_view *x, t_symbol *colname, t_rowref rr)
{
    auto database = x->database->get_read_access();
    long row_index = map_rowref_to_index(x, rr);
        
    database.get_entry_identifier(&x->edit_identifier, row_index);
}

void database_view_celledited(t_database_view *x, t_symbol *colname, t_rowref rr, long argc, t_atom *argv)
{
    long column_index = get_index_from_colname(colname);
    
    if (argc)
    {
        x->database->get_write_access().replace_item(&x->edit_identifier, column_index, argv);
        jdataview_redrawcell(x->dataview, colname, rr);
    }
}

void database_view_editended(t_database_view *x, t_symbol *colname, t_rowref rr)
{
    jdataview_redrawcell(x->dataview, colname, rr);
    atom_setobj(&x->edit_identifier, nullptr);
}

// Component

void database_view_component(t_database_view *x, t_symbol *colname, t_rowref rr, long *component, long *options)
{
    auto database = x->database->get_read_access();
    long column_index = get_index_from_colname(colname);
    
    *component = JCOLUMN_COMPONENT_TEXTEDITOR;
    *options = database.get_column_label_mode(column_index) ? JCOLUMN_TEXT_ONESYMBOL : JCOLUMN_TEXT_FLOAT;
}

// Sorting

void database_view_sort(t_database_view *x, t_symbol *colname, t_privatesortrec *record)
{
    if (!record)
        return;
    
    // Sorting accessor functors
    
    struct identifier_getter : public entries::read_access
    {
        identifier_getter(const entries& database) : read_access(database) {}
        t_custom_atom operator()(long idx) const { return get_entry_identifier(idx); }
    };
    
    struct string_getter : public entries::read_access
    {
        string_getter(const entries& database, long column) : entries::read_access(database), m_column(column) {}
        std::string operator()(long idx) const { return get_untyped(idx, m_column).m_symbol->s_name; }
        long m_column;
    };
    
    struct data_getter : public entries::read_access
    {
        data_getter(const entries& database, long column) : entries::read_access(database), m_column(column) {}
        double operator()(long idx) const { return get_data(idx, m_column); }
        long m_column;
    };
    
    // Find any selection
    
    long selection_index = -1;
    
    if (jdataview_selectedrowcount(x->dataview))
        selection_index = map_rowref_to_index(x, *jdataview_getselectedrowsforview(x->dataview, x->patcherview));
    
    // Get the column and store the sorting direction
    
    long column_index = get_index_from_colname(colname);
    x->sort_direction = record->p_fwd == JCOLUMN_SORTDIRECTION_FORWARD;
    
    auto database = x->database->get_read_access();
    
    // If we are sorted in order, or the row map need resizing, intialise them here
    
    if (x->row_map.size() != database.num_items() || column_index == COLINDEX_ITEM_NUMBER)
    {
        x->row_map.resize(database.num_items());
        std::iota(x->row_map.begin(), x->row_map.end(), 0);
    }
    
    if (column_index == COLINDEX_IDENTIFIER)
    {
        // Sort row map on identifiers
        
        sort(x->row_map, database.num_items(), identifier_getter(*x->database));
    }
    else if (column_index >= 0)
    {
        // Sort row map on data (using labels or data as per the column)
        
        if (database.get_column_label_mode(column_index))
            sort(x->row_map, database.num_items(), string_getter(*x->database, column_index));
        else
            sort(x->row_map, database.num_items(), data_getter(*x->database, column_index));
    }
    
    // Move selection
    
    if (selection_index >= 0)
        jdataview_selectcell(x->dataview, get_colname_from_index(0), map_index_to_rowref(x, selection_index));
}
