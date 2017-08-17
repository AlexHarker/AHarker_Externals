
#ifndef ENTRY_DATABASE_MAX_H
#define ENTRY_DATABASE_MAX_H

#include <ext.h>
#include <ext_obex.h>

#include "EntryDatabase.h"

template <class T> class DatabasePtr
{
public:
    
    DatabasePtr(T *database) : mDatabase(database) {}
    
    T *operator->() { return mDatabase; }
    T *operator*()  { return mDatabase; }
    
private:
    
    T *mDatabase;
};

typedef DatabasePtr<EntryDatabase> WritableDatabase;
typedef DatabasePtr<const EntryDatabase> ReadableDatabase;


typedef struct entry_database{
    
    t_object a_obj;
    
    EntryDatabase *database;
    
    long count;
    
} t_entry_database;

t_entry_database *entry_database_get_database_object(t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns);
t_entry_database *entry_database_get_database_object(t_entry_database *old_object, t_symbol *name);
void entry_database_release(t_entry_database *x);

ReadableDatabase entry_database_get_database_read(t_entry_database *x);
WritableDatabase entry_database_get_database_write(t_entry_database *x);

#endif
