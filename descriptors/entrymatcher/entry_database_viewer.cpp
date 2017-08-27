
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
void entry_database_viewer_resize(t_entry_database_viewer *x);

void entry_database_viewer_set_database(t_entry_database_viewer *x, EntryDatabase *database);
void entry_database_viewer_buildview(t_entry_database_viewer *x);
void entry_database_viewer_getcelltext(t_entry_database_viewer *x, t_symbol *colname, long index, char *text, long maxlen);

//t_max_err	dbviewer_notify(t_dbviewer *x, t_symbol *s, t_symbol *msg, void *sender, void *data);

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
    
    //class_addmethod(c, (method)dbviewer_notify,				"notify",			A_CANT,	0);
  
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
        
        x->d_dataview = NULL;
        x->database = NULL;
        x->patcher = gensym("#P")->s_thing;
        
        attr_dictionary_process(x, d);
        
        jbox_ready(&x->d_box);
        
        entry_database_viewer_resize(x);
    }
    return(x);
}

void entry_database_viewer_free(t_entry_database_viewer *x)
{
    jbox_free(&x->d_box);
}

void entry_database_viewer_newpatcherview(t_entry_database_viewer *x, t_object *patcherview)
{
    jdataview_patchervis(x->d_dataview, patcherview, (t_object *)x);
}

void entry_database_viewer_freepatcherview(t_entry_database_viewer *x, t_object *patcherview)
{
    jdataview_patcherinvis(x->d_dataview, patcherview);
}

void entry_database_viewer_resize(t_entry_database_viewer *x)
{
    t_rect rect;
    
    jpatcher_get_rect(x->patcher, &rect);
    rect.x = 0;
    rect.y = 0;
    jbox_set_rect((t_object *) &x->d_box, &rect);
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
        
        if (x->d_dataview)
            object_free(x->d_dataview);
    
        x->d_dataview = (t_object *)jdataview_new();
        jdataview_setclient(x->d_dataview, (t_object *)x);
        jdataview_setcolumnheaderheight(x->d_dataview, 40);
        jdataview_setheight(x->d_dataview, 16.0);
        
        t_object *column = jdataview_addcolumn(x->d_dataview, gensym("__identifier"), NULL, true);
        jcolumn_setlabel(column, gensym("identifier"));
        jcolumn_setwidth(column, 110);
        jcolumn_setmaxwidth(column, 300);
        //jcolumn_setcelltextstylemsg(column, gensym("justifyhcenter"));
        
        for (long i = 0; i < database->numColumns(); i++)
        {
            t_object *column = jdataview_addcolumn(x->d_dataview, database->getColumnName(i), NULL, true);
            jcolumn_setlabel(column, database->getColumnName(i));
            jcolumn_setwidth(column, 110);
            jcolumn_setmaxwidth(column, 300);
        }
        
        long numItems = database->numItems();
        
        t_rowref *rowrefs = (t_rowref *)malloc(sizeof(t_rowref) * numItems);
        for (long i = 0; i < numItems; i++)
            rowrefs[i] = (t_rowref *)(i+1);
        jdataview_addrows(x->d_dataview, numItems, rowrefs);
        free(rowrefs);
    }
}
/*
t_max_err dbviewer_notify(t_dbviewer *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
    if (sender == x->d_view) {
        if (msg == ps_dbview_update) {
            dbviewer_bang(x);
        }
        else if (msg == ps_dbview_query_changed) {	// dump all of the columns
            t_object	*column = NULL;
            t_symbol	**column_names = NULL;
            long		numcolumns = 0;
            long		i;
            
            hashtab_getkeys(x->d_columns, &numcolumns, &column_names);
            if (column_names) {
                for (i=0; i<numcolumns; i++) {
                    column = jdataview_getnamedcolumn(x->d_dataview, column_names[i]);
                    if (column)
                        jdataview_deletecolumn(x->d_dataview, column);
                }
                sysmem_freeptr(column_names);
            }
            hashtab_clear(x->d_columns);
        }
        else if (msg == _sym_free) {
            object_detach_byptr((t_object *)x, x->d_view);
            x->d_view = NULL;
        }
    }
    return jbox_notify((t_jbox *)x, s, msg, sender, data);
}*/

void entry_database_viewer_getcelltext(t_entry_database_viewer *x, t_symbol *colname, long index, char *text, long maxlen)
{
    if (x->database)
    {
        EntryDatabase::ReadPointer database(x->database);
        std::string str;
        
        t_atom a;
        atom_setsym(&a, colname);
        
        if (colname == gensym("__identifier"))
            str = database->getEntryIdentifier(index - 1).getString();
        else
            str = database->getTypedData(index - 1, database->columnFromSpecifier(&a)).getString();

        strncpy_zero(text, str.c_str(), maxlen-1);
    }
}
