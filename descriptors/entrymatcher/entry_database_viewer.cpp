
#include "entry_database_viewer.hpp"
#include "entry_database.hpp"
#include "sort.hpp"

#include <jpatcher_api.h>
#include <jgraphics.h>
#include <jdataview.h>

typedef struct _entry_database_viewer
{
    t_jbox              d_box;
    t_object            *d_dataview;
    t_object            *patcher;
    t_object            *database_object;
    EntryDatabase       *database;
    bool                visible;
    bool                in_edit;
    t_atom              edit_identifier;
    
    bool sort_direction;
    std::vector<long> indices;
    
} t_entry_database_viewer;


void *entry_database_viewer_new(t_symbol *s, short argc, t_atom *argv);
void entry_database_viewer_free(t_entry_database_viewer *x);

void entry_database_viewer_newpatcherview(t_entry_database_viewer *x, t_object *patcherview);
void entry_database_viewer_freepatcherview(t_entry_database_viewer *x, t_object *patcherview);

void entry_database_viewer_set_database(t_entry_database_viewer *x, t_object *database_object, EntryDatabase *database);
void entry_database_viewer_update(t_entry_database_viewer *x);
void entry_database_viewer_editstarted(t_entry_database_viewer *x, t_symbol *colname, t_rowref rr);
void entry_database_viewer_getcelltext(t_entry_database_viewer *x, t_symbol *colname, t_rowref rr, char *text, long maxlen);
void entry_database_viewer_getcellstyle(t_entry_database_viewer *x, t_symbol *colname, t_rowref rr, long *style, long *align);

void entry_database_viewer_sort(t_entry_database_viewer *x, t_symbol *colname, t_privatesortrec *record);

void entry_database_viewer_celledited(t_entry_database_viewer *x, t_symbol *colname, t_rowref rr, long argc, t_atom *argv);
void entry_database_viewer_component(t_entry_database_viewer *x, t_symbol *colname, t_rowref rr, long *component, long *options);

t_max_err entry_database_viewer_notify(t_entry_database_viewer *x, t_symbol *s, t_symbol *msg, void *sender, void *data);

long entry_database_viewer_dummycompare(t_rowref a, t_rowref b) { return 1; }

static t_class *entry_database_viewer_class = NULL;

void entry_database_viewer_init()
{
    t_class *c;
    long flags;
    
    common_symbols_init();
    
    c = class_new("__entry_database_view",
                  (method) entry_database_viewer_new,
                  (method) entry_database_viewer_free,
                  sizeof(t_entry_database_viewer),
                  (method) nullptr,
                  A_GIMME,
                  0);
    
    c->c_flags |= CLASS_FLAG_NEWDICTIONARY;
    
    flags = JBOX_COLOR;
    
    jbox_initclass(c, flags);
    
    class_addmethod(c, (method) entry_database_viewer_getcelltext, "getcelltext", A_CANT, 0);
    class_addmethod(c, (method) entry_database_viewer_getcellstyle, "getcellstyle", A_CANT, 0);

    class_addmethod(c, (method) entry_database_viewer_sort, "sortdata", A_CANT, 0);
    class_addmethod(c, (method) entry_database_viewer_dummycompare, "nevercalled", A_CANT, 0);
    class_addmethod(c, (method) entry_database_viewer_component, "component", A_CANT, 0);
    
    class_addmethod(c, (method) entry_database_viewer_editstarted, "editstarted", A_CANT, 0);
    class_addmethod(c, (method) entry_database_viewer_celledited, "editvalue", A_CANT, 0);
    
    class_addmethod(c, (method) entry_database_viewer_newpatcherview, "newpatcherview", A_CANT, 0);
    class_addmethod(c, (method) entry_database_viewer_freepatcherview, "freepatcherview", A_CANT, 0);
    
    class_addmethod(c, (method) entry_database_viewer_set_database, "__set_database", A_CANT, 0);
    class_addmethod(c, (method) entry_database_viewer_update, "__build_view", A_CANT, 0);
    
    class_addmethod(c, (method) entry_database_viewer_notify, "notify", A_CANT, 0);
  
    class_register(CLASS_BOX, c);
    entry_database_viewer_class = c;
}

void *entry_database_viewer_new(t_symbol *s, short argc, t_atom *argv)
{
    t_entry_database_viewer *x;
    t_dictionary *d = NULL;
    
    if (!(d=object_dictionaryarg(argc, argv)))
        return NULL;
    
    x = (t_entry_database_viewer *) object_alloc(entry_database_viewer_class);
    
    if (x)
    {
        new (x) t_entry_database_viewer;
        
        long flags =  JBOX_NODRAWBOX | JBOX_GROWBOTH | JBOX_NOFLOATINSPECTOR;
        
        jbox_new(&x->d_box, flags, argc, argv);
        x->d_box.b_firstin = (t_object *) x;
        
        x->d_dataview = (t_object *)jdataview_new();
        jdataview_setclient(x->d_dataview, (t_object *) x);
        jdataview_setcolumnheaderheight(x->d_dataview, 30);
        jdataview_setheight(x->d_dataview, 22.0);
        jdataview_setautosizeright(x->d_dataview, 1);
        jdataview_setautosizerightcolumn(x->d_dataview, 1);
        jdataview_setdragenabled(x->d_dataview, 0);
        jdataview_setscrollvisible(x->d_dataview, 1, 1);
     
        x->visible = false;
        x->database = NULL;
        x->patcher = gensym("#P")->s_thing;
        x->in_edit = false;
        atom_setobj(&x->edit_identifier, NULL);
        
        attr_dictionary_process(x, d);

        jbox_ready(&x->d_box);
    }
    
    return(x);
}

void entry_database_viewer_free(t_entry_database_viewer *x)
{
    object_method(x->database_object, gensym("__view_removed"));
    object_free(x->d_dataview);
    jbox_free(&x->d_box);
    x->~t_entry_database_viewer();
}

void entry_database_viewer_newpatcherview(t_entry_database_viewer *x, t_object *patcherview)
{
    object_attach_byptr(x, patcherview);
    jdataview_patchervis(x->d_dataview, patcherview, (t_object *) x);
}

void entry_database_viewer_freepatcherview(t_entry_database_viewer *x, t_object *patcherview)
{
    object_detach_byptr(x, patcherview);
    jdataview_patcherinvis(x->d_dataview, patcherview);
}

void entry_database_viewer_set_database(t_entry_database_viewer *x, t_object *database_object, EntryDatabase *database)
{
    x->database = database;
    x->database_object = database_object;
    entry_database_viewer_update(x);
}

void entry_database_viewer_update(t_entry_database_viewer *x)
{
    if (x->database && x->visible && !x->in_edit)
    {
        EntryDatabase::ReadPointer database(x->database);
        
        // Update columns
        
        long num_database_columns = database->numColumns() + 2;
        long num_columns = jdataview_getnumcolumns(x->d_dataview);
        
        if (num_database_columns != num_columns)
        {
            if (num_columns < num_database_columns)
            {
                long column_difference = num_database_columns - num_columns;
                for (long i = 0; i < column_difference; i++)
                {
                    t_object *column = jdataview_addcolumn(x->d_dataview, gensym(std::to_string(i).c_str()), NULL, true);
                    jcolumn_setminwidth(column, 50);
                    jcolumn_setwidth(column, 90);
                    jcolumn_setmaxwidth(column, 300);
                    jcolumn_setdraggable(column, 0);
                    jcolumn_setsortable(column, 1);
                    jcolumn_setcelltextstylemsg(column, gensym("getcellstyle"));
                    
                    // N.B. - custom sort must be set in order to override...
                    
                    jcolumn_setcustomsort(column, gensym("nevercalled"));
                    jcolumn_setoverridesort(column, 1);
                    
                    if ((num_columns + i) > 1)
                    {
                        jcolumn_sethideable(column, 1);
                        jcolumn_setrowcomponentmsg(column, gensym("component"));
                        jcolumn_setvaluemsg(column, gensym("editvalue"), gensym("editstarted"), NULL);
                    }
                }
            }
            else
            {
                long column_difference = num_columns - num_database_columns;
                for (long i = 0; i < column_difference; i++)
                    jdataview_deletecolumn(x->d_dataview, jdataview_getnthcolumn(x->d_dataview, i + num_database_columns));
            }
        }

        // Update columns labels

        t_object *column = jdataview_getnthcolumn(x->d_dataview, 0);
        jcolumn_setlabel(column, gensym("#"));

        column = jdataview_getnthcolumn(x->d_dataview, 1);
        jcolumn_setlabel(column, gensym("identifier"));
        
        for (long i = 0; i < database->numColumns(); i++)
        {
            t_object *column = jdataview_getnthcolumn(x->d_dataview,  i + 2);
            jcolumn_setlabel(column, database->getColumnName(i));
        }
        
        // Update rows
        
        long num_items = database->numItems();
        long num_rows = jdataview_getnumrows(x->d_dataview);
        
        if (num_rows != num_items)
        {
            if (num_items == 0)
            {
                jdataview_clear(x->d_dataview);
            }
            else if (num_rows < num_items)
            {
                long row_difference = num_items - num_rows;
                std::vector<t_rowref> rowrefs(row_difference);
                for (long i = 0; i < row_difference; i++)
                    rowrefs[i] = (t_rowref)(1 + i + num_rows);
                jdataview_addrows(x->d_dataview, row_difference, &rowrefs[0]);
            }
            else
            {
                long row_difference = num_rows - num_items;
                t_rowref *rowrefs = (t_rowref *)malloc(sizeof(t_rowref) * row_difference);
                for (long i = 0; i < row_difference; i++)
                    rowrefs[i] = (t_rowref *)(1 + i + num_items);
                jdataview_deleterows(x->d_dataview, row_difference, &rowrefs[0]);
            }
        }
        
        // FIX - doesn't work - why not?
        
        if (num_columns == 0)
            jdataview_sort(x->d_dataview, gensym("0"), 1);
        else
            jdataview_resort(x->d_dataview);
    }
}

t_max_err entry_database_viewer_notify(t_entry_database_viewer *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
    if (msg == gensym("attr_modified") && object_classname(sender) == gensym("patcherview"))
    {
        // Get attibute name
        
        t_symbol *attrname = (t_symbol *)object_method((t_object *)data, gensym("getname")); // ask attribute object for name
        
        if (attrname == gensym("rect"))
        {
            t_rect rect;
        
            patcherview_get_rect( (t_object *) sender, &rect);
            rect.x = 0;
            rect.y = 0;
            jbox_set_rect_for_view((t_object *) &x->d_box, (t_object *) sender, &rect);
        }
        else if (attrname == gensym("visible"))
        {
            if (patcherview_get_visible((t_object *) sender))
            {
                x->visible = true;
                entry_database_viewer_update(x);
            }
            else
                x->visible = false;
        }
    }
    
    return jbox_notify((t_jbox *) x, s, msg, sender, data);
}
// Convenience Mappers

t_ptr_int map_rowref(t_entry_database_viewer *x, t_rowref rr)
{
    t_ptr_int index = (t_ptr_int) rr - 1;
    
    if (index >= x->indices.size())
        return index;
    else if (x->sort_direction)
        return x->indices[index];
    else
        return x->indices[x->indices.size() - index - 1];
}

long map_colname(t_symbol *colname)
{
    return std::stol(colname->s_name) - 2;
}

// Cell behaviours

void entry_database_viewer_getcelltext(t_entry_database_viewer *x, t_symbol *colname, t_rowref rr, char *text, long maxlen)
{
    if (x->database)
    {
        EntryDatabase::ReadPointer database(x->database);
        long column = map_colname(colname);
        t_ptr_int index = map_rowref(x, rr);
        std::string str;

        if (column == -2)
            str = std::to_string(index + 1);
        else if (column == -1)
            str = database->getEntryIdentifier(index).getString();
        else
            str = database->getTypedData(index, column).getString();

        strncpy_zero(text, str.c_str(), maxlen-1);
    }
}

void entry_database_viewer_getcellstyle(t_entry_database_viewer *x, t_symbol *colname, t_rowref rr, long *style, long *align)
{
    *style = (colname == gensym("0")) ? JCOLUMN_STYLE_ITALIC : JCOLUMN_STYLE_PLAIN;
    *align = JCOLUMN_ALIGN_CENTER;
}

void entry_database_viewer_editstarted(t_entry_database_viewer *x, t_symbol *colname, t_rowref rr)
{
    EntryDatabase::ReadPointer database(x->database);
    t_ptr_int index = map_rowref(x, rr);
    
    x->in_edit = true;
    database->getEntryIdentifier(&x->edit_identifier, index);
}

void entry_database_viewer_celledited(t_entry_database_viewer *x, t_symbol *colname, t_rowref rr, long argc, t_atom *argv)
{
    long column = map_colname(colname);
    
    if (argc)
        x->database->replaceItem(&x->edit_identifier, column, argv);
    jdataview_redrawcell(x->d_dataview, colname, rr);
    atom_setobj(&x->edit_identifier, NULL);
    x->in_edit = false;
    
    entry_database_viewer_update(x);
}

void entry_database_viewer_component(t_entry_database_viewer *x, t_symbol *colname, t_rowref rr, long *component, long *options)
{
    EntryDatabase::ReadPointer database(x->database);
    
    *component = JCOLUMN_COMPONENT_TEXTEDITOR;
    *options = database->getColumnLabelMode(map_colname(colname)) ? JCOLUMN_TEXT_ONESYMBOL : JCOLUMN_TEXT_FLOAT;
}

void entry_database_viewer_sort(t_entry_database_viewer *x, t_symbol *colname, t_privatesortrec *record)
{
    x->sort_direction = record->p_fwd == JCOLUMN_SORTDIRECTION_FORWARD;
    long column = map_colname(colname);
    
    EntryDatabase::ReadPointer database = EntryDatabase::ReadPointer(x->database);
    
    if (x->indices.size() != database->numItems() || column == -2)
    {
        x->indices.resize(database->numItems());
    
        for (long i = 0; i < database->numItems(); i++)
            x->indices[i] = i;
    }
    
    if (column == -1)
    {
        struct Getter
        {
            Getter(const EntryDatabase *database) : mDatabase(database) {}
            CustomAtom operator()(long idx) { return mDatabase->getEntryIdentifier(idx); }
            const EntryDatabase *mDatabase;
        };
        
        Getter getter(x->database);
        sort(x->indices, database->numItems(), getter);
    }
    else if (column >= 0)
    {
        if (database->getColumnLabelMode(column))
        {
            struct Getter : public EntryDatabase::RawAccessor
            {
                Getter(long column, const EntryDatabase& database) : EntryDatabase::RawAccessor(database), mColumn(column) {}
                std::string operator()(long idx) { return getData(idx, mColumn).mSymbol->s_name; }
                long mColumn;
            };
            
            Getter getter(column, *x->database);
            sort(x->indices, database->numItems(), getter);
        }
        else
        {
            struct Getter : public EntryDatabase::RawAccessor
            {
                Getter(long column, const EntryDatabase& database) : EntryDatabase::RawAccessor(database), mColumn(column) {}
                double operator()(long idx) { return getData(idx, mColumn); }
                long mColumn;
            };
            
            Getter getter(column, *x->database);
            sort(x->indices, database->numItems(), getter);
        }
    }
}
