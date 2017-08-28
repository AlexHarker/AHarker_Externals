
#include "entry_database_viewer.h"
#include "EntryDatabase.h"

#include <jpatcher_api.h>
#include <jgraphics.h>
#include <jdataview.h>

typedef struct _entry_database_viewer
{
    t_jbox              d_box;
    t_object            *d_dataview;
    t_object            *patcher;
    EntryDatabase       *database;
    bool                visible;
} t_entry_database_viewer;


void *entry_database_viewer_new(t_symbol *s, short argc, t_atom *argv);
void entry_database_viewer_free(t_entry_database_viewer *x);

void entry_database_viewer_newpatcherview(t_entry_database_viewer *x, t_object *patcherview);
void entry_database_viewer_freepatcherview(t_entry_database_viewer *x, t_object *patcherview);

void entry_database_viewer_set_database(t_entry_database_viewer *x, EntryDatabase *database);
void entry_database_viewer_buildview(t_entry_database_viewer *x);
void entry_database_viewer_getcelltext(t_entry_database_viewer *x, t_symbol *colname, long index, char *text, long maxlen);
void entry_database_viewer_getcellstyle(t_entry_database_viewer *x, t_symbol *colname, long index, long *style, long *align);

void entry_database_viewer_sortdata(t_entry_database_viewer *x, t_symbol *colname, t_privatesortrec *record);
void entry_database_viewer_editstart(t_entry_database_viewer *x, t_symbol *colname, t_rowref rr);
void entry_database_viewer_editenter(t_entry_database_viewer *x, t_symbol *colname, t_rowref rr, char *newtext, char *newtext2);
void entry_database_viewer_component(t_entry_database_viewer *x, t_symbol *colname, t_rowref rr, long *component);

long entry_database_viewer_idxsort(t_rowref a, t_rowref b);
long entry_database_viewer_identifiersort(t_rowref a, t_rowref b);
long entry_database_viewer_datasort(t_rowref a, t_rowref b);

t_max_err entry_database_viewer_notify(t_entry_database_viewer *x, t_symbol *s, t_symbol *msg, void *sender, void *data);

static t_class	*entry_database_viewer_class = NULL;

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
    class_addmethod(c, (method)entry_database_viewer_getcellstyle, "getcellstyle", A_CANT, 0);

    class_addmethod(c, (method)entry_database_viewer_sortdata, "sortdata", A_CANT, 0);
    class_addmethod(c, (method)entry_database_viewer_idxsort, "idxsort", A_CANT, 0);
    class_addmethod(c, (method)entry_database_viewer_identifiersort, "identifiersort", A_CANT, 0);
    class_addmethod(c, (method)entry_database_viewer_datasort, "datasort", A_CANT, 0);
    class_addmethod(c, (method)entry_database_viewer_component, "component", A_CANT, 0);
    
    class_addmethod(c, (method)entry_database_viewer_editstart, "editstarted", A_CANT, 0);
    class_addmethod(c, (method)entry_database_viewer_editenter, "editentered", A_CANT, 0);
    
    class_addmethod(c, (method)entry_database_viewer_newpatcherview, "newpatcherview", A_CANT, 0);
    class_addmethod(c, (method)entry_database_viewer_freepatcherview, "freepatcherview", A_CANT, 0);
    
    class_addmethod(c, (method)entry_database_viewer_set_database, "__set_database", A_CANT, 0);
    class_addmethod(c, (method)entry_database_viewer_buildview, "__build_view", A_CANT, 0);
    
    class_addmethod(c, (method)entry_database_viewer_notify, "notify", A_CANT, 0);
  
    class_register(CLASS_BOX, c);
    entry_database_viewer_class = c;
}

void *entry_database_viewer_new(t_symbol *s, short argc, t_atom *argv)
{
    t_entry_database_viewer *x;
    t_dictionary *d = NULL;
    
    if (!(d=object_dictionaryarg(argc, argv)))
        return NULL;
    
    x = (t_entry_database_viewer *)object_alloc(entry_database_viewer_class);
    
    if (x)
    {
        long flags =  JBOX_NODRAWBOX | JBOX_GROWBOTH | JBOX_NOFLOATINSPECTOR;
        
        jbox_new(&x->d_box, flags, argc, argv);
        x->d_box.b_firstin = (t_object *)x;
        
        x->d_dataview = (t_object *)jdataview_new();
        jdataview_setclient(x->d_dataview, (t_object *)x);
        jdataview_setcolumnheaderheight(x->d_dataview, 30);
        jdataview_setheight(x->d_dataview, 22.0);
        jdataview_setautosizeright(x->d_dataview, 1);
        jdataview_setautosizerightcolumn(x->d_dataview, 1);
        jdataview_setdragenabled(x->d_dataview, 0);
        
        x->visible = false;
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
    if (x->database && x->visible)
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
                    
                    if (num_columns == 0 && i == 0)
                        jcolumn_setcustomsort(column, gensym("idxsort"));
                    else if (num_columns == 0 && i == 1)
                        jcolumn_setcustomsort(column, gensym("identifiersort"));
                    else
                        jcolumn_setcustomsort(column, gensym("datasort"));
                    
                    if ((num_columns + i) != 0)
                    {
                        jcolumn_setrowcomponentmsg(column, gensym("component"));
                        t_jcolumn *col = (t_jcolumn*)column;
                        //col->c_endmsg = gensym("editentered");
                        col->c_editable = 1;
                        col->c_endmsg = gensym("editentered");
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
        // Get attibute name
        
        t_symbol *attrname = (t_symbol *)object_method((t_object *)data, gensym("getname"));	// ask attribute object for name
        
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
                entry_database_viewer_buildview(x);
            }
            else
                x->visible = false;
        }
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
        
        if (column == 0)
            str = std::to_string(index);
        else if (column == 1)
            str = database->getEntryIdentifier(index - 1).getString();
        else
            str = database->getTypedData(index - 1, column - 2).getString();

        strncpy_zero(text, str.c_str(), maxlen-1);
    }
}

void entry_database_viewer_getcellstyle(t_entry_database_viewer *x, t_symbol *colname, long index, long *style, long *align)
{
    *style = (colname == gensym("0")) ? JCOLUMN_STYLE_ITALIC : JCOLUMN_STYLE_PLAIN;
    *align = JCOLUMN_ALIGN_CENTER;
}

// Custom Sorting

EntryDatabase *sort_database;
static bool sort_direction;
static bool sort_symbols;
static long sort_column;

long entry_database_viewer_idxsort(t_rowref a, t_rowref b)
{
    if (sort_direction)
        return a > b;
    else
        return b > a;
}

long entry_database_viewer_identifiersort(t_rowref a, t_rowref b)
{
    EntryDatabase::ReadPointer database(sort_database);

    long item1 = (long) a - 1;
    long item2 = (long) b - 1;
        
    if (sort_direction)
        return database->getEntryIdentifier(item1) > database->getEntryIdentifier(item2);
    else
        return database->getEntryIdentifier(item2) > database->getEntryIdentifier(item1);
}

long entry_database_viewer_datasort(t_rowref a, t_rowref b)
{
    EntryDatabase::ReadPointer database(sort_database);
    
    long item1 = (long) a - 1;
    long item2 = (long) b - 1;
    
    if (sort_symbols)
    {
        if (sort_direction)
            return strcmp(database->getData(item1, sort_column).mSymbol->s_name, database->getData(item2, sort_column).mSymbol->s_name) > 0;
        else
            return strcmp(database->getData(item1, sort_column).mSymbol->s_name, database->getData(item2, sort_column).mSymbol->s_name) < 0;
    }
    else
    {
        if (sort_direction)
            return (database->getData(item1, sort_column).mValue > database->getData(item2, sort_column).mValue);
        else
            return database->getData(item2, sort_column).mValue > database->getData(item1, sort_column).mValue;
    }
}

void entry_database_viewer_sortdata(t_entry_database_viewer *x, t_symbol *colname, t_privatesortrec *record)
{
    sort_column = std::stol(colname->s_name) - 2;
    sort_database = x->database;
    sort_direction = record->p_fwd == JCOLUMN_SORTDIRECTION_FORWARD;
    sort_symbols = sort_column >= 0 ? sort_database->getColumnLabelMode(sort_column) : false;
}

void entry_database_viewer_editstart(t_entry_database_viewer *x, t_symbol *colname, t_rowref rr)
{
    jdataview_selectcell(x->d_dataview, colname, rr);
}

void entry_database_viewer_editenter(t_entry_database_viewer *x, t_symbol *colname, t_rowref rr, char *newtext, char *newtext2)
{
    
}

void entry_database_viewer_component(t_entry_database_viewer *x, t_symbol *colname, t_rowref rr, long *component)
{
    *component = JCOLUMN_COMPONENT_TEXTEDITOR;
}
