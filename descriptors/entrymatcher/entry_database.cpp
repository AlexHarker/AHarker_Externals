
/*
 *  entry_database.cpp
 *
 *  This Max class wraps an entries underlying database in a Max class.
 *  Facilities are provided to create, change, release or view the underlying database.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <algorithm>
#include <string>

#include <AH_Int_Handler.hpp>
#include <AH_Lifecycle.hpp>
#include <AH_Private_Object.hpp>

#include "entry_database.hpp"
#include "database_view.hpp"


// Entry Database Object Structure

struct t_entry_database
{
    t_object a_obj;
    
    entries database;
    
    t_object *view_patch;
    t_object *view;
    
    t_private_count count;
    
    bool notify_in_progress;
};

// Private Object Helper

static t_private_count& get_count(t_entry_database& x)
{
    return x.count;
}

// Entry Database Definitions

t_class *database_class;
const char *database_classname = "__entry_database";
t_symbol *ps_database_classname = gensym(database_classname);
t_symbol *ps_private_namespace = gensym("__entry_database_private");

// Function Prototypes

int entry_database_init();
void *entry_database_new(t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns);
void entry_database_free(t_entry_database *x);

void entry_database_view_removed(t_entry_database *database);

// Init (main)

int entry_database_init()
{
    database_class = class_new(database_classname,
                               (method) entry_database_new,
                               (method) entry_database_free,
                               sizeof(t_entry_database),
                               (method) nullptr,
                               A_SYM,
                               A_LONG,
                               A_LONG,
                               0);
    
    class_register(CLASS_NOBOX, database_class);
    
    class_addmethod(database_class, (method) entry_database_view_removed, private_strings::view_removed(), A_CANT, 0);

    database_view_init();
    
    return 0;
}

// New / Free

void *entry_database_new(t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns)
{
    t_entry_database *x = (t_entry_database *) object_alloc(database_class);
    
    num_reserved_entries = std::max(num_reserved_entries, t_atom_long(1));
    num_columns = std::max(num_columns, t_atom_long(1));
    
    // Construct the database and lock
    
    create_object(x->count);
    create_object(x->database, name, limit_int<long>(num_columns));
    
    // Reserve entries, set count to one and set to notify
    
    x->database.get_write_access().reserve(limit_int<long>(num_reserved_entries));
    x->notify_in_progress = false;
    
    // Don't create the view yet
    
    x->view_patch = nullptr;
    x->view = nullptr;
    
    return x;
}

void entry_database_free(t_entry_database *x)
{
    destroy_object(x->database);
    destroy_object(x->count);
   
    // Destroy view patch (which destroys the view)
    
    object_free(x->view_patch);
}

// Modification Notification (in the low-priority thread)

void entry_database_modified(t_entry_database *x, t_symbol *msg, long argc, t_atom *argv)
{
    static t_symbol *database_modified = gensym(private_strings::database_modified());
    static t_symbol *build_view = gensym(private_strings::build_view());

    if (x->notify_in_progress)
    {
        if (x->view)
            object_method(x->view, build_view);
        object_notify(x, database_modified, nullptr);
        x->notify_in_progress = false;
    }
}

// Delete (in the low-priority thread)

void entry_database_deferred_deletion(t_entry_database *x, t_symbol *msg, long argc, t_atom *argv)
{
    object_free(x);
}

// Release a Database (deleting if necessary)

void entry_database_release(t_entry_database *x, void *client)
{
    if (x)
    {
        // Complete notifications before the object is released

        entry_database_modified(x, nullptr, 0, nullptr);
        object_detach(ps_private_namespace, x->database.get_name(), client);
        
        private_object_release(x, &get_count);
    }
}

// Find a Database and Attach (if it already exists)

t_entry_database *entry_database_find_and_attach(t_entry_database *prev, t_symbol *name, void *client)
{
    // Make sure the max database class exists
    
    if (!class_findbyname(CLASS_NOBOX, ps_database_classname))
        entry_database_init();
    
    // See if an object is registered with this name that is still active nad if so increase the count
    
    t_entry_database *x = private_object_find_retain(prev, name, ps_private_namespace, &get_count);

    // If the two objects are the same or no object is found do nothing
    
    if (!x || prev == x)
        return prev;
    
    // Release the old object and attach to the new
    
    if (prev)
        entry_database_release(prev, client);
    object_attach(ps_private_namespace, name, client);
    
    return x;
}

// Create a Database Object (or attach if it already exists)

t_entry_database *entry_database_create(t_symbol *name, t_atom_long num_entries, t_atom_long num_columns, void *client)
{
    // Note that the number of entries is not fixed (this is just the number that are reserved)
    
    t_atom argv[3];
    atom_setsym(argv + 0, name);
    atom_setlong(argv + 1, num_entries);
    atom_setlong(argv + 2, num_columns);
    
    // See if an object is registered (otherwise make object and register it)
    
    t_entry_database *x = entry_database_find_and_attach(nullptr, name, client);
    
    if (!x)
    {
        x = private_object_create<t_entry_database>(ps_database_classname, name, ps_private_namespace, 3, argv);
        object_attach(ps_private_namespace, name, client);
    }
    else
    {
        auto access = x->database.get_write_access();
        access.ensure_columns(limit_int<long>(num_columns));
        access.reserve(limit_int<long>(num_entries));
    }
    
    return x;
}

// Open View

void entry_database_view(t_entry_database *x)
{
    // Create if it doesn't yet exist
    
    if (!x->view_patch)
    {
        // Create patcher
        
        t_dictionary *d = dictionary_new();
        t_atom a;
        t_atom *av = nullptr;
        long ac = 0;
        
        // The patcher we create should not belong to any other patcher, so we need to set #P s_thing
        
        t_symbol *ps_parent_patcher = gensym("#P");
        t_patcher *parent = (t_patcher *) ps_parent_patcher->s_thing;
        ps_parent_patcher->s_thing = nullptr;
        
        atom_setparse(&ac, &av, "@defrect 0 0 600 600 @toolbarvisible 0 @enablehscroll 0 @enablevscroll 0 @noedit 1");
        attr_args_dictionary(d, static_cast<short>(ac), av);
        atom_setobj(&a, d);
        x->view_patch = (t_object *) object_new_typed(CLASS_NOBOX, gensym("jpatcher"),1, &a);
        ps_parent_patcher->s_thing = parent;
        
        // These must all be set after creating
        
        object_attr_setsym(x->view_patch, gensym("title"), x->database.get_name());
        object_attr_setlong(x->view_patch, gensym("newviewdisabled"), 1);
        object_attr_setlong(x->view_patch, gensym("cansave"), 0);

        // Make view object (and set database)
        
        std::string view_text("@maxclass newobj @text \"");
        view_text.append(private_strings::view_classname());
        view_text.append("\" @patching_rect 0 0 300 300");
        x->view = newobject_sprintf(x->view_patch, view_text.c_str());
        object_method(x->view, gensym(private_strings::set_database()), x, &x->database);
    }
    
    // Bring to front
    
    if (x->view_patch)
        object_method(x->view_patch, gensym("front"));
}

void entry_database_view_removed(t_entry_database *x)
{
    x->view_patch = nullptr;
    x->view = nullptr;
}

// Notifing Access Class (notifies clients after write operation when it destructs)

notifying_write_access::~notifying_write_access()
{    
    if (!m_database->notify_in_progress)
    {
        m_database->notify_in_progress = true;
        defer_low(m_database, (method) entry_database_modified, nullptr, 0, nullptr);
    }
}

// Client Routines

// Get / Change / Release

t_entry_database *database_create(void *x, t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns)
{
    return entry_database_create(name, num_reserved_entries, num_columns, x);
}

t_entry_database *database_change(void *x, t_symbol *name, t_entry_database *prev_database)
{
    t_entry_database *database = entry_database_find_and_attach(prev_database, name,  x);
    
    if (database == prev_database && database->database.get_name() != name)
        object_error((t_object *) x, "no database %s found!", name->s_name);
    
    return database;
}

void database_release(void *x, t_entry_database *database)
{
    entry_database_release(database, x);
}

// View

void database_view(void *x, t_entry_database *database)
{
    entry_database_view(database);
}

// Retrieve Pointers for Reading or Writing

entries::read_access database_get_read_access(t_entry_database *database)
{
    return database->database.get_read_access();
}

notifying_write_access database_get_write_access(t_entry_database *database)
{
    return notifying_write_access(database->database, database);
}
