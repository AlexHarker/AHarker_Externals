
#include "entry_database_viewer.h"

/**
	@file
	dbviewer - demonstrate use of database views for sqlite and jdataview
 
	@ingroup	examples
 
	Copyright 2009 - Cycling '74
	Timothy Place, tim@cycling74.com
 */

#include <jpatcher_api.h>
#include <jgraphics.h>
#include <jdataview.h>


typedef struct _entry_database_viewer
{
    t_jbox		d_box;
    t_object	*d_dataview;	///< The dataview object
    //t_hashtab	*d_columns;		///< The dataview columns:  column name -> column index
    //t_object	*d_view;		///< The dbview object that we need to display in a dataview

} t_entry_database_viewer;



// general prototypes
void		*entry_database_viewer_new(t_symbol *s, short argc, t_atom *argv);
void		entry_database_viewer_free(t_entry_database_viewer *x);

void		entry_database_viewer_newpatcherview(t_entry_database_viewer *x, t_object *patcherview);
void		entry_database_viewer_freepatcherview(t_entry_database_viewer *x, t_object *patcherview);

//t_max_err	dbviewer_notify(t_dbviewer *x, t_symbol *s, t_symbol *msg, void *sender, void *data);

//void		dbviewer_getcelltext(t_dbviewer *x, t_symbol *colname, long index, char *text, long maxlen);
//t_max_err	dbviewer_set_query(t_dbviewer *x, void *attr, long argc, t_atom *argv);
//t_max_err	dbviewer_set_database(t_dbviewer *x, void *attr, long argc, t_atom *argv);

static t_class	*s_dbviewer_class = NULL;
static t_symbol	*ps_dbview_update = NULL;
static t_symbol	*ps_dbview_query_changed = NULL;

/************************************************************************************/

void entry_database_viewer_init()
{
    t_class	*c;
    long	flags;
    
    common_symbols_init();
    c = class_new("__entry_database_view", (method)entry_database_viewer_new, (method)entry_database_viewer_free, sizeof(t_entry_database_viewer), (method)0L, A_GIMME, 0);
    c->c_flags |= CLASS_FLAG_NEWDICTIONARY;
    flags = JBOX_COLOR;
    jbox_initclass(c, flags);
    
 //   class_addmethod(c, (method)dbviewer_getcelltext,		"getcelltext",		A_CANT, 0);
    class_addmethod(c, (method)entry_database_viewer_newpatcherview,		"newpatcherview",	A_CANT, 0);
    class_addmethod(c, (method)entry_database_viewer_freepatcherview,	"freepatcherview",	A_CANT, 0);
    //class_addmethod(c, (method)dbviewer_notify,				"notify",			A_CANT,	0);
  
    class_register(CLASS_BOX, c);
    s_dbviewer_class = c;
    
    ps_dbview_update = gensym("dbview_update");
    ps_dbview_query_changed = gensym("dbview_query_changed");
}


/************************************************************************************/
// Object Creation Method

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
        
        //x->d_columns = hashtab_new(13);
        //hashtab_flags(x->d_columns, OBJ_FLAG_DATA);
        
        x->d_dataview = (t_object *)jdataview_new();
        jdataview_setclient(x->d_dataview, (t_object *)x);
        jdataview_setcolumnheaderheight(x->d_dataview, 40);
        jdataview_setheight(x->d_dataview, 16.0);
        
        attr_dictionary_process(x, d);
        
        jbox_ready(&x->d_box);
        //		object_notify(x->d_view, ps_dbview_update, NULL);
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


/************************************************************************************/
/*

void dbviewer_getcelltext(t_dbviewer *x, t_symbol *colname, long index, char *text, long maxlen)
{
    t_object	*result = (t_object *)object_method(x->d_view, gensym("getresult"));
    char		*itemtext;
    t_atom_long	column_index;
    t_max_err	err;
    
    if (!result)
        return;
    
    err = hashtab_lookuplong(x->d_columns, colname, &column_index);
    if (!err) {
        itemtext = (char *)object_method(result, _sym_valuebyindex, index-1, column_index);
        
        if (itemtext && itemtext[0]) {
            if (strstr(colname->s_name, "date")) {
                t_datetime td;
                sysdateformat_strftimetodatetime(itemtext, &td);
                sysdateformat_formatdatetime(&td, SYSDATEFORMAT_RELATIVE, 0, text, maxlen-1);
            }
            else
                strncpy_zero(text, itemtext, maxlen-1);
        }
    }
}


t_max_err dbviewer_set_query(t_dbviewer *x, void *attr, long argc, t_atom *argv)
{
    if (argc && argv) {
        x->d_query = atom_getsym(argv);
        object_attr_setsym(x->d_view, _sym_query, x->d_query);
        if (x->d_db && x->d_query) {
            db_view_create(x->d_db, x->d_query->s_name, &x->d_view);
            object_attach_byptr_register(x, x->d_view, _sym_nobox);
        }
    }
    return MAX_ERR_NONE;
}


t_max_err dbviewer_set_database(t_dbviewer *x, void *attr, long argc, t_atom *argv)
{
    t_max_err err;
    
    if (argc && argv) {
        db_view_remove(x->d_db, &x->d_view);
        db_close(&x->d_db);
        
        x->d_database = atom_getsym(argv);
        err = db_open(x->d_database, NULL, &x->d_db);
        if (!err && x->d_db && x->d_query) {
            db_view_create(x->d_db, x->d_query->s_name, &x->d_view);
            object_attach_byptr_register(x, x->d_view, _sym_nobox);
        }
    }
    return MAX_ERR_NONE;
}*/

