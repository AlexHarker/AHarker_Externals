
#include "entry_database_viewer.h"
#include "EntryDatabase.h"

#include <jpatcher_api.h>
#include <jgraphics.h>
#include <jdataview.h>

typedef struct _entry_database_viewer
{
    t_jbox		d_box;
    t_object	*d_dataview;
    t_object    *patcher;
    EntryDatabase *database;

} t_entry_database_viewer;

void *entry_database_viewer_new(t_symbol *s, short argc, t_atom *argv);
void entry_database_viewer_free(t_entry_database_viewer *x);

void entry_database_viewer_newpatcherview(t_entry_database_viewer *x, t_object *patcherview);
void entry_database_viewer_freepatcherview(t_entry_database_viewer *x, t_object *patcherview);

void entry_database_viewer_set_database(t_entry_database_viewer *x, EntryDatabase *database);
void entry_database_viewer_buildview(t_entry_database_viewer *x);
void entry_database_viewer_getcelltext(t_entry_database_viewer *x, t_symbol *colname, long index, char *text, long maxlen);

t_max_err entry_database_viewer_notify(t_entry_database_viewer *x, t_symbol *s, t_symbol *msg, void *sender, void *data);

static t_class	*s_dbviewer_class = NULL;
static t_symbol	*ps_dbview_query_changed = NULL;

void entry_database_viewer_init()
{
    t_class	*c;
    long	flags;
    
    common_symbols_init();
    c = class_new("__entry_database_view", (method)entry_database_viewer_new, (method)entry_database_viewer_free, sizeof(t_entry_database_viewer), (method)0L, A_GIMME, 0);
    c->c_flags |= CLASS_FLAG_NEWDICTIONARY;
    flags = JBOX_COLOR;
    jbox_initclass(c, flags);
    
    class_addmethod(c, (method)entry_database_viewer_getcelltext, "getcelltext", A_CANT, 0);
    
    class_addmethod(c, (method)entry_database_viewer_newpatcherview, "newpatcherview", A_CANT, 0);
    class_addmethod(c, (method)entry_database_viewer_freepatcherview, "freepatcherview", A_CANT, 0);
    
    class_addmethod(c, (method)entry_database_viewer_set_database, "__set_database", A_CANT, 0);
    class_addmethod(c, (method)entry_database_viewer_buildview, "__build_view", A_CANT, 0);
    
    class_addmethod(c, (method)entry_database_viewer_notify, "notify", A_CANT, 0);
  
    class_register(CLASS_BOX, c);
    s_dbviewer_class = c;
    
    ps_dbview_query_changed = gensym("dbview_query_changed");
}

void *entry_database_viewer_new(t_symbol *s, short argc, t_atom *argv)
{
    t_entry_database_viewer *x;
    t_dictionary *d = NULL;
    
    if (!(d=object_dictionaryarg(argc, argv)))
        return NULL;
    
    x = (t_entry_database_viewer *)object_alloc(s_dbviewer_class);
    
    if (x)
    {
        long flags =  JBOX_NODRAWBOX | JBOX_GROWBOTH | JBOX_NOFLOATINSPECTOR;
        
        jbox_new(&x->d_box, flags, argc, argv);
        x->d_box.b_firstin = (t_object *)x;
        
        x->d_dataview = (t_object *)jdataview_new();
        jdataview_setclient(x->d_dataview, (t_object *)x);
        jdataview_setcolumnheaderheight(x->d_dataview, 40);
        jdataview_setheight(x->d_dataview, 16.0);
        
        x->database = NULL;
        x->patcher = gensym("#P")->s_thing;
        
        attr_dictionary_process(x, d);
        
        jbox_ready(&x->d_box);
    }
    return(x);
}

void entry_database_viewer_free(t_entry_database_viewer *x)
{
    jbox_free(&x->d_box);
}

void entry_database_viewer_newpatcherview(t_entry_database_viewer *x, t_object *patcherview)
{
    object_attach_byptr(x, patcherview);
    jdataview_patchervis(x->d_dataview, patcherview, (t_object *)x);
}

void entry_database_viewer_freepatcherview(t_entry_database_viewer *x, t_object *patcherview)
{
    object_detach_byptr(x, patcherview);
    jdataview_patcherinvis(x->d_dataview, patcherview);
}

void entry_database_viewer_set_database(t_entry_database_viewer *x, EntryDatabase *database)
{
    x->database = database;
    entry_database_viewer_buildview(x);
}

void entry_database_viewer_buildview(t_entry_database_viewer *x)
{
    if (x->database)
    {
        EntryDatabase::ReadPointer database(x->database);
        
        // Update columns
        
        long num_database_columns = database->numColumns() + 1;
        long num_columns = jdataview_getnumcolumns(x->d_dataview);
        
        if (num_database_columns != num_columns)
        {
            if (num_columns < num_database_columns)
            {
                long column_difference = num_database_columns - num_columns;
                for (long i = 0; i < column_difference; i++)
                    jdataview_addcolumn(x->d_dataview, gensym(std::to_string(i).c_str()), NULL, true);
            }
            else
            {
                long column_difference = num_columns - num_database_columns;
                for (long i = 0; i < column_difference; i++)
                    jdataview_deletecolumn(x->d_dataview, jdataview_getnthcolumn(x->d_dataview, i + num_database_columns));
            }
        }

        // Update columns labels etc.

        t_object *column = jdataview_getnthcolumn(x->d_dataview, 0);
        jcolumn_setlabel(column, gensym("identifier"));
        jcolumn_setwidth(column, 110);
        jcolumn_setmaxwidth(column, 300);
        //jcolumn_setcelltextstylemsg(column, gensym("justifyhcenter"));
        
        for (long i = 0; i < database->numColumns(); i++)
        {
            t_object *column = jdataview_getnthcolumn(x->d_dataview,  i + 1);
            jcolumn_setlabel(column, database->getColumnName(i));
            jcolumn_setwidth(column, 110);
            jcolumn_setmaxwidth(column, 300);
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
                    rowrefs[i] = (t_rowref)(i+1 + num_rows);
                jdataview_addrows(x->d_dataview, row_difference, &rowrefs[0]);
            }
            else
            {
                long row_difference = num_rows - num_items;
                t_rowref *rowrefs = (t_rowref *)malloc(sizeof(t_rowref) * row_difference);
                for (long i = 0; i < row_difference; i++)
                    rowrefs[i] = (t_rowref *)(i+1 + num_items);
                jdataview_deleterows(x->d_dataview, row_difference, &rowrefs[0]);
            }
        }
    }
}

t_max_err entry_database_viewer_notify(t_entry_database_viewer *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
    if (msg == gensym("attr_modified") && object_classname(sender) == gensym("patcherview"))
    {
        t_rect rect;
        
        patcherview_get_rect( (t_object *) sender, &rect);
        rect.x = 0;
        rect.y = 0;
        jbox_set_rect_for_view((t_object *) &x->d_box, (t_object *) sender, &rect);
    }
    
    return jbox_notify((t_jbox *)x, s, msg, sender, data);
}

void entry_database_viewer_getcelltext(t_entry_database_viewer *x, t_symbol *colname, long index, char *text, long maxlen)
{
    if (x->database)
    {
        EntryDatabase::ReadPointer database(x->database);
        std::string str;
        
        long column = std::stol(colname->s_name);
        
        if (colname == gensym("0"))
            str = database->getEntryIdentifier(index - 1).getString();
        else
            str = database->getTypedData(index - 1, column - 1).getString();

        strncpy_zero(text, str.c_str(), maxlen-1);
    }
}
