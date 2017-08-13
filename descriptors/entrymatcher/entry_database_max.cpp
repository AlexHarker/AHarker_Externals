
#include "entry_database_max.h"
#include <algorithm>

t_class *database_class;
const char database_class_name[] = "__entry_database";
t_symbol *name_space_name = gensym("__entry_database_private");

int entry_database_init();
void *entry_database_new(t_atom_long num_reserved_entries, t_atom_long num_columns);
void entry_database_free(t_entry_database *x);

t_entry_database *entry_database_get_database_object(t_symbol *name);

int entry_database_init()
{
    database_class = class_new ("__entry_database",
                             (method)entry_database_new,
                             (method)entry_database_free,
                             (short)sizeof(t_entry_database),
                             NULL,
                             A_LONG,
                             A_LONG,
                             0);
    
    class_register(CLASS_NOBOX, database_class);
    
    return 0;
}

void *entry_database_new(t_atom_long num_reserved_entries, t_atom_long num_columns)
{
    t_entry_database *x = (t_entry_database *)object_alloc(database_class);
    
    num_reserved_entries = std::max(num_reserved_entries, t_atom_long(1));
    num_columns = std::max(num_columns, t_atom_long(1));
    
    x->database = new EntryDatabase(num_columns);
    x->database->reserve(num_reserved_entries);
    x->count = 0;
    
    return (x);
}

void entry_database_free(t_entry_database *x)
{
    delete x->database;
}

void entry_database_release(t_entry_database *x)
{
    if (x && --x->count == 0)
    {
        object_unregister(x);
        object_free(x);
    }
}

t_entry_database *entry_database_get_database_object(t_symbol *name)
{
    // Make sure the max database class exists
    
    if (!class_findbyname(CLASS_NOBOX, gensym(database_class_name)))
        entry_database_init();
    
    // See if an object is registered
    
    return (t_entry_database *) object_findregistered(name_space_name, name);
}

t_entry_database *entry_database_get_database_object(EntryDatabase **database_ptr, t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns)
{
    t_atom argv[2];
    atom_setlong(argv + 0, num_reserved_entries);
    atom_setlong(argv + 1, num_columns);
    
    // See if an object is registered (otherwise make object and register it...)
    
    t_entry_database *x = entry_database_get_database_object(name);
    
    if (!x)
        x = (t_entry_database *) object_register(name_space_name, name, object_new_typed(CLASS_NOBOX, gensym(database_class_name), 2, argv));
    
    x->count++;
    *database_ptr = x->database;
    
    return x;
}

t_entry_database *entry_database_get_database_object(t_entry_database *old_object, EntryDatabase **database_ptr, t_symbol *name)
{
    bool free_unused_old_object = false;
    
    // See if an object is registered (otherwise make object and register it...)
    
    if (old_object && --old_object->count == 0)
    {
        object_unregister(old_object);
        free_unused_old_object = true;
    }
    
    // See if an object is registered with this name
    
    t_entry_database *x = entry_database_get_database_object(name);
    
    // Register the old object with the new name
    
    if (!x)
        x = (t_entry_database *) object_register(name_space_name, name, old_object);
    else if (free_unused_old_object)
        object_free(old_object);
    
    x->count++;
    *database_ptr = x->database;
    
    return x;
}



    