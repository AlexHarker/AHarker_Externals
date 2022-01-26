
#include "database_view.hpp"
#include "entry_database.hpp"
#include "sort.hpp"

#include "AH_Lifecycle.hpp"

#include <jpatcher_api.h>
#include <jgraphics.h>
#include <jdataview.h>

#include <numeric>
#include <vector>

static t_class *database_view_class = nullptr;

struct t_database_view
{
    t_jbox d_box;
    
    t_object *d_dataview;
    t_object *database_object;
    
    entries *database;
    
    bool visible;
    bool in_edit;
    
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

void database_view_getcelltext(t_database_view *x, t_symbol *colname, t_rowref rr, char *text, long maxlen);
void database_view_getcellstyle(t_database_view *x, t_symbol *colname, t_rowref rr, long *style, long *align);

void database_view_editstarted(t_database_view *x, t_symbol *colname, t_rowref rr);
void database_view_celledited(t_database_view *x, t_symbol *colname, t_rowref rr, long argc, t_atom *argv);

void database_view_component(t_database_view *x, t_symbol *colname, t_rowref rr, long *component, long *options);
void database_view_sort(t_database_view *x, t_symbol *colname, t_privatesortrec *record);

// N.B. required for custom sorting

long database_view_dummycompare(t_rowref a, t_rowref b) { return 1; }

// Column and Row Mappers and Utilities

enum header_column : long
{
    COLINDEX_ITEM_NUMBER = -2,
    COLINDEX_IDENTIFIER = -1,
};

t_symbol *ps_first_colname = gensym("0");

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
    long index = reinterpret_cast<long>(rr) - 1;
    
    if (index >= x->row_map.size())
        return index;
    
    if (x->sort_direction)
        return x->row_map[index];
    else
        return x->row_map[x->row_map.size() - index - 1];
}

struct rowref_sequence : std::vector<t_rowref>
{
    rowref_sequence(long size, long start) : std::vector<t_rowref>(size)
    {
        for (auto it = begin(); it != end(); it++)
            (*it) = reinterpret_cast<t_rowref>(++start);
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

    class_addmethod(c, (method) database_view_getcelltext, "getcelltext", A_CANT, 0);
    class_addmethod(c, (method) database_view_getcellstyle, "getcellstyle", A_CANT, 0);

    class_addmethod(c, (method) database_view_editstarted, "editstarted", A_CANT, 0);
    class_addmethod(c, (method) database_view_celledited, "editvalue", A_CANT, 0);
    
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
        create_object(x);
     
        // Create and set up the view
        
        long flags =  JBOX_NODRAWBOX | JBOX_GROWBOTH | JBOX_NOFLOATINSPECTOR;
        
        jbox_new(&x->d_box, flags, argc, argv);
        x->d_box.b_firstin = (t_object *) x;
        
        x->d_dataview = (t_object *) jdataview_new();
        jdataview_setclient(x->d_dataview, (t_object *) x);
        jdataview_setcolumnheaderheight(x->d_dataview, 30);
        jdataview_setheight(x->d_dataview, 22);
        jdataview_setautosizeright(x->d_dataview, 1);
        jdataview_setautosizerightcolumn(x->d_dataview, 1);
        jdataview_setdragenabled(x->d_dataview, 0);
        jdataview_setscrollvisible(x->d_dataview, 1, 1);
     
        x->visible = false;
        x->database = nullptr;
        x->in_edit = false;
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
    
    object_free(x->d_dataview);
    jbox_free(&x->d_box);
    destroy_object(x);
}

// Patcher Views

void database_view_newpatcherview(t_database_view *x, t_object *patcherview)
{
    object_attach_byptr(x, patcherview);
    jdataview_patchervis(x->d_dataview, patcherview, (t_object *) x);
}

void database_view_freepatcherview(t_database_view *x, t_object *patcherview)
{
    object_detach_byptr(x, patcherview);
    jdataview_patcherinvis(x->d_dataview, patcherview);
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
    // Don't update if there isn't a database, it's not visible, or we are editing
    
    if (!x->database || !x->visible || x->in_edit)
        return;
    
    entries::read_pointer database(x->database);
        
    // Update columns
    
    const long num_view_columns = jdataview_getnumcolumns(x->d_dataview);
    const long required_columns = database->num_columns() + 2;
    
    if (required_columns != num_view_columns)
    {
        if (num_view_columns < required_columns)
        {
            // Add new columns and set their style
            
            for (long i = num_view_columns; i < required_columns; i++)
            {
                t_object *column = jdataview_addcolumn(x->d_dataview, get_colname_from_index(i), nullptr, 0);
                
                // N.B. - custom sort must be set in order to override...

                jcolumn_setminwidth(column, 50);
                jcolumn_setwidth(column, 90);
                jcolumn_setmaxwidth(column, 300);
                jcolumn_setdraggable(column, 0);
                jcolumn_setsortable(column, 1);
                jcolumn_setcelltextstylemsg(column, gensym("getcellstyle"));
                jcolumn_setcustomsort(column, gensym("__never_called"));
                jcolumn_setoverridesort(column, 1);
                
                if (i >= 2)
                {
                    jcolumn_sethideable(column, 1);
                    jcolumn_setrowcomponentmsg(column, gensym("component"));
                    jcolumn_setvaluemsg(column, gensym("editvalue"), gensym("editstarted"), nullptr);
                }
            }
        }
        else
        {
            // Delete unused columns
            
            for (long i = required_columns; i < num_view_columns; i++)
                jdataview_deletecolumn(x->d_dataview, jdataview_getnthcolumn(x->d_dataview, i));
        }
    }

    // Update column labels (# and identifier columns / data)
    
    jcolumn_setlabel(jdataview_getnthcolumn(x->d_dataview, 0), gensym("#"));
    jcolumn_setlabel(jdataview_getnthcolumn(x->d_dataview, 1), gensym("identifier"));
        
    for (long i = 0; i < database->num_columns(); i++)
        jcolumn_setlabel(jdataview_getnthcolumn(x->d_dataview,  i + 2), database->get_column_name(i));

    // Update rows
    
    long num_view_rows = jdataview_getnumrows(x->d_dataview);
    long required_items = database->num_items();
    
    if (num_view_rows != required_items)
    {
        if (!required_items)
        {
            // No data - clear the view
            
            jdataview_clear(x->d_dataview);
        }
        else if (num_view_rows < required_items)
        {
            // Create additional rows
            
            rowref_sequence refs(required_items - num_view_rows, num_view_rows);
            jdataview_addrows(x->d_dataview, refs.size(), refs.data());
        }
        else
        {
            // Delte additional rows
            
            rowref_sequence refs(num_view_rows - required_items, required_items);
            jdataview_deleterows(x->d_dataview, refs.size(), refs.data());
        }
    }
    
    // Sort or resort
    
    // FIX - doesn't work - why not?
    
    if (num_view_columns == 0)
        jdataview_sort(x->d_dataview, ps_first_colname, 1);
    else
        jdataview_resort(x->d_dataview);
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
        else if (attrname == gensym("visible"))
        {
            if (patcherview_get_visible((t_object *) sender))
            {
                x->visible = true;
                database_view_update(x);
            }
            else
                x->visible = false;
        }
    }
    
    return jbox_notify((t_jbox *) x, s, msg, sender, data);
}

// Cell Text and Style

void database_view_getcelltext(t_database_view *x, t_symbol *colname, t_rowref rr, char *text, long maxlen)
{
    if (x->database)
    {
        std::string str;

        entries::read_pointer database(x->database);

        long column_index = get_index_from_colname(colname);
        long row_index = map_rowref_to_index(x, rr);

        if (column_index == COLINDEX_ITEM_NUMBER)
            str = std::to_string(row_index + 1);
        else if (column_index == COLINDEX_IDENTIFIER)
            str = database->get_entry_identifier(row_index).get_string();
        else
            str = database->get_string(row_index, column_index);

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
    entries::read_pointer database(x->database);
    long row_index = map_rowref_to_index(x, rr);
    
    x->in_edit = true;
    database->get_entry_identifier(&x->edit_identifier, row_index);
}

void database_view_celledited(t_database_view *x, t_symbol *colname, t_rowref rr, long argc, t_atom *argv)
{
    long column_index = get_index_from_colname(colname);
    
    if (argc)
        x->database->replace_item(&x->edit_identifier, column_index, argv);
    jdataview_redrawcell(x->d_dataview, colname, rr);
    atom_setobj(&x->edit_identifier, nullptr);
    x->in_edit = false;
    
    database_view_update(x);
}

// Component

void database_view_component(t_database_view *x, t_symbol *colname, t_rowref rr, long *component, long *options)
{
    entries::read_pointer database(x->database);
    
    long column_index = get_index_from_colname(colname);
    
    *component = JCOLUMN_COMPONENT_TEXTEDITOR;
    *options = database->get_column_label_mode(column_index) ? JCOLUMN_TEXT_ONESYMBOL : JCOLUMN_TEXT_FLOAT;
}

// Sorting

void database_view_sort(t_database_view *x, t_symbol *colname, t_privatesortrec *record)
{
    // Sorting accessor functors
    
    struct identifier_getter
    {
        identifier_getter(const entries *database) : m_database(database) {}
        t_custom_atom operator()(long idx) const { return m_database->get_entry_identifier(idx); }
        const entries *m_database;
    };
    
    struct string_getter : public entries::accessor
    {
        string_getter(long column, const entries& database) : entries::accessor(database), m_column(column) {}
        std::string operator()(long idx) const { return get_untyped(idx, m_column).m_symbol->s_name; }
        long m_column;
    };
    
    struct data_getter : public entries::accessor
    {
        data_getter(long column, const entries& database) : entries::accessor(database), m_column(column) {}
        double operator()(long idx) const { return get_data(idx, m_column); }
        long m_column;
    };
    
    // Get the column and store the sorting direction
    
    long column_index = get_index_from_colname(colname);
    x->sort_direction = record->p_fwd == JCOLUMN_SORTDIRECTION_FORWARD;
    
    entries::read_pointer database(x->database);
    
    // If we are sorted in order, or the row map need resizing, intialise them here
    
    if (x->row_map.size() != database->num_items() || column_index == COLINDEX_ITEM_NUMBER)
    {
        x->row_map.resize(database->num_items());
        std::iota(x->row_map.begin(), x->row_map.end(), 0);
    }
    
    if (column_index == COLINDEX_IDENTIFIER)
    {
        // Sort row map on identifiers
        
        sort(x->row_map, database->num_items(), identifier_getter(x->database));
    }
    else if (column_index >= 0)
    {
        // Sort row map on data (using labels or data as per the column)
        
        if (database->get_column_label_mode(column_index))
            sort(x->row_map, database->num_items(), string_getter(column_index, *x->database));
        else
            sort(x->row_map, database->num_items(), data_getter(column_index, *x->database));
    }
}
