
#include "entry_database_max.h"
#include <algorithm>

t_class *the_database_class;

int entry_database_init();
void *entry_database_new(t_atom_long num_reserved_entries, t_atom_long num_columns);
void entry_database_free(t_entry_database *x);

int entry_database_init()
{
    the_database_class = class_new ("__entry_database",
                             (method)entry_database_new,
                             (method)entry_database_free,
                             (short)sizeof(t_entry_database),
                             NULL,
                             A_LONG,
                             A_LONG,
                             0);
    
    class_register(CLASS_NOBOX, the_database_class);
    
    return 0;
}

void *entry_database_new(t_atom_long num_reserved_entries, t_atom_long num_columns)
{
    t_entry_database *x = (t_entry_database *)object_alloc(the_database_class);
    
    num_reserved_entries = std::max(num_reserved_entries, t_atom_long(1));
    num_columns = std::max(num_columns, t_atom_long(1));
    
    x->mDatabase = new EntryDatabase(num_columns);
    x->mDatabase->reserve(num_reserved_entries);
    x->count = 1;
    
    return (x);
}

void entry_database_free(t_entry_database *x)
{
    delete x->mDatabase;
}

void entry_database_release(t_entry_database *x)
{
    if (--x->count == 0)
    {
        object_unregister(x);
        object_free(x);
    }
}

t_entry_database *entry_database_get_database_object(t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns)
{
    t_atom argv[2];
    atom_setlong(argv + 0, num_reserved_entries);
    atom_setlong(argv + 1, num_columns);
    
    const char database_class[] = "__entry_database";
    t_symbol *name_space = gensym("__entry_database_private");
    
    // Make sure the max globals class exists
    
    if (!class_findbyname(CLASS_NOBOX, gensym(database_class)))
        entry_database_init();
    
    // See if an object is registered (otherwise make object and register it...)
    
    t_entry_database *x = (t_entry_database *) object_findregistered(name_space, name);
    
    if (!x)
        x = (t_entry_database *) object_register(name_space, name, object_new_typed(CLASS_NOBOX, gensym(database_class), 2, argv));
    
    x->count++;
    
    return x;
}

EntryDatabase *entry_database_get_database(t_entry_database *x)
{
    return x->mDatabase;
}


    