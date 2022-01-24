
#include <algorithm>

#include "entry_database_max.hpp"
#include "entry_database_viewer.hpp"

#include <AH_Locks.hpp>

/*****************************************/
// Entry Database Max Object Struture
/*****************************************/

struct t_entry_database
{
    t_object a_obj;
    
    EntryDatabase database;
    thread_lock lock;
    
    t_object *patch;
    t_object *viewer;
    
    long count;
    bool notify;
};

/*****************************************/
// Entry Database Max Object Definitions
/*****************************************/

t_class *database_class;
t_symbol *ps_database_class_name = gensym("__entry_database");
t_symbol *ps_name_space_name = gensym("__entry_database_private");

int entry_database_init();
void *entry_database_new(t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns);
void entry_database_free(t_entry_database *x);

void entry_database_view_removed(t_entry_database *database_object);

/*****************************************/
// Entry Database Max Object Routines
/*****************************************/

// Init (like main)

int entry_database_init()
{
    database_class = class_new("__entry_database",
                               (method) entry_database_new,
                               (method) entry_database_free,
                               sizeof(t_entry_database),
                               (method) nullptr,
                               A_SYM,
                               A_LONG,
                               A_LONG,
                               0);
    
    class_register(CLASS_NOBOX, database_class);
    
    class_addmethod(database_class, (method) entry_database_view_removed, "__view_removed", A_CANT, 0);

    entry_database_viewer_init();
    
    return 0;
}

// New

void *entry_database_new(t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns)
{
    t_entry_database *x = (t_entry_database *) object_alloc(database_class);
    
    num_reserved_entries = std::max(num_reserved_entries, t_atom_long(1));
    num_columns = std::max(num_columns, t_atom_long(1));
    
    // Construct the database and lock
    
    new(&x->lock) thread_lock();
    new(&x->database) EntryDatabase(name, num_columns);
    
    // Reserve entries, set count to one and set to notify
    
    x->database.reserve(num_reserved_entries);
    x->count = 1;
    x->notify = true;
    
    x->patch = nullptr;
    x->viewer = nullptr;
    
    return x;
}

// Free

void entry_database_free(t_entry_database *x)
{
    // Destruct C++ objects
    
    x->database.~EntryDatabase();
    x->lock.~thread_lock();
    
    // Destroy patch (which destroys viewer)
    
    object_free(x->patch);
}

// Modification Notification (in the low-priority thread)

void entry_database_modified(t_entry_database *x, t_symbol *msg, long argc, t_atom *argv)
{
    static t_symbol *database_modified = gensym("__database_modified");
    
    if (!x->notify)
    {
        if (x->viewer)
            object_method(x->viewer, gensym("__build_view"));
        object_notify(x, database_modified, nullptr);
        x->notify = true;
    }
}

// Delete (in the low-priority thread)

void entry_database_deferred_deletion(t_entry_database *x, t_symbol *msg, long argc, t_atom *argv)
{
    object_free(x);
}

// Release a Database Safely (deleting if necessary)

void entry_database_release(void *client, t_entry_database *x)
{
    bool last_client = false;
    
    if (x)
    {
        // Complete notifications before the object is released

        entry_database_modified(x, nullptr, 0, nullptr);
        
        object_detach(ps_name_space_name, x->database.getName(), client);
        
        spin_lock_hold(&x->lock);
        last_client = (--x->count <= 0);
    }
    
    // If this is the last client then free the object (deferlow in case the pointer is hanging around)
    
    if (last_client)
    {
        object_unregister(x);
        defer_low(x, (method) entry_database_deferred_deletion, nullptr, 0, nullptr);
    }
}

// Find a Entry Database Max Object and Attach (if it already exists)

t_entry_database *entry_database_findattach(void *client, t_symbol *name, t_entry_database *old_database_object)
{
    bool found_new = false;
    
    // Make sure the max database class exists
    
    if (!class_findbyname(CLASS_NOBOX, ps_database_class_name))
        entry_database_init();
    
    // See if an object is registered with this name that is still active nad if so increase the count
    
    t_entry_database *x = (t_entry_database *) object_findregistered(ps_name_space_name, name);
    
    if (x && x != old_database_object)
    {
        spin_lock_hold(&x->lock);

        if ((found_new = x->count > 0))
            x->count++;
    }
    
    // Otherwise return the old object
    
    if (!found_new)
        return old_database_object;
    
    // Release the old object and attach to the new
    
    if (old_database_object)
        entry_database_release(client, old_database_object);
    object_attach(ps_name_space_name, name, client);
    
    return x;
}

// Create a Max Database Object (or attach if it already exists)

t_entry_database *entry_database_create(void *client, t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns)
{
    t_atom argv[3];
    atom_setsym(argv + 0, name);
    atom_setlong(argv + 1, num_reserved_entries);
    atom_setlong(argv + 2, num_columns);
    
    // See if an object is registered (otherwise make object and register it)
    
    t_entry_database *x = entry_database_findattach(client, name, nullptr);
    
    if (!x)
    {
        x = (t_entry_database *) object_register(ps_name_space_name, name, object_new_typed(CLASS_NOBOX, ps_database_class_name, 3, argv));
        object_attach(ps_name_space_name, name, client);
    }
    
    return x;
}

// View

void entry_database_view(t_entry_database *database_object)
{
    // Create and/or bring to front
    
    if (!database_object->patch)
    {
        // Create patcher
        
        t_dictionary *d = dictionary_new();
        t_atom a;
        t_atom *av = nullptr;
        long ac = 0;
        
        // The patcher we create should not belong to any other patcher, so we need to set the #P symbol
        
        t_symbol *ps_parent_patcher = gensym("#P");
        t_patcher *parent = (t_patcher *) ps_parent_patcher->s_thing;
        ps_parent_patcher->s_thing = nullptr;
        
        atom_setparse(&ac, &av, "@defrect 0 0 600 600 @toolbarvisible 0 @enablehscroll 0 @enablevscroll 0 @noedit 1");
        attr_args_dictionary(d, ac, av);
        atom_setobj(&a, d);
        database_object->patch = (t_object *)object_new_typed(CLASS_NOBOX, gensym("jpatcher"),1, &a);
        ps_parent_patcher->s_thing = parent;
        
        // Must set after creating, because reasons...
        
        object_attr_setsym(database_object->patch, gensym("title"), database_object->database.getName());
        object_attr_setlong(database_object->patch, gensym("newviewdisabled"), 1);
        object_attr_setlong(database_object->patch, gensym("cansave"), 0);
        /*
        long attrcount = 0;
        t_symbol **names = nullptr;
        
        object_attr_getnames(database_object->patch, &attrcount, &names);
        
        for (long i = 0; i < attrcount; i++)
        {
            long argc = 0;
            t_atom *argv = nullptr;
            char *str = names[i]->s_name;
            
            object_attr_getvalueof(database_object->patch, names[i], &argc, &argv);
            post("attribute called %s", str);
        }
        */
        // Make internal object (and set database)
        
        database_object->viewer = newobject_sprintf(database_object->patch, "@maxclass newobj @text \"__entry_database_view\" @patching_rect 0 0 300 300");
        
        object_method(database_object->viewer, gensym("__set_database"), database_object, &database_object->database);
    }
    
    if (database_object->patch)
        object_method(database_object->patch, gensym("front"));
}

void entry_database_view_removed(t_entry_database *database_object)
{
    database_object->patch = nullptr;
    database_object->viewer = nullptr;
}

/*****************************************/
// Notify Pointer (notifies clients after write operation)
/*****************************************/

notify_pointer::~notify_pointer()
{
    t_entry_database *database = (t_entry_database *) m_max_database;
    
    if (database->notify)
    {
        database->notify = false;
        defer_low(m_max_database, (method) entry_database_modified, nullptr, 0, nullptr);
    }
}

/*****************************************/
// Client Routines
/*****************************************/

// Get / Change / Release Named Database

t_object *database_create(void *x, t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns)
{
    return (t_object *) entry_database_create(x, name, num_reserved_entries, num_columns);
}

t_object *database_change(void *x, t_symbol *name, t_object *old_object)
{
    return (t_object *) entry_database_findattach(x, name, (t_entry_database *) old_object);
}

void database_release(void *x, t_object *database_object)
{
    entry_database_release(x, (t_entry_database *) database_object);
}

// View

void database_view(void *x, t_object *database_object)
{
    entry_database_view((t_entry_database *) database_object);
}

// Retrieve Pointers for Reading or Writing

EntryDatabase::ReadPointer database_getptr_read(t_object *database_object)
{
    t_entry_database *obj = (t_entry_database *) database_object;
    return EntryDatabase::ReadPointer(&obj->database);
}

notify_pointer database_getptr_write(t_object *database_object)
{
    t_entry_database *obj = (t_entry_database *) database_object;
    return notify_pointer(&obj->database, (t_object *) obj);
}
