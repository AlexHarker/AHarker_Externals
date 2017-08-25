
#ifndef ENTRY_DATABASE_MAX_H
#define ENTRY_DATABASE_MAX_H

#include <ext.h>
#include <ext_obex.h>

#include "EntryDatabase.h"

struct NotifyPointer : public EntryDatabase::WritePointer
{
    NotifyPointer(EntryDatabase *ptr, t_object *maxDatabase) : EntryDatabase::WritePointer(ptr), mMaxDatabase(maxDatabase) {}
    
    ~NotifyPointer();
    
private:
    
    t_object *mMaxDatabase;
};

t_object *database_create(void *x, t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns);
t_object *database_change(void *x, t_symbol *name, t_object *old_database_object);
void database_release(void *x, t_object *database_object);

EntryDatabase::ReadPointer database_getptr_read(t_object *database_object);
NotifyPointer database_getptr_write(t_object *database_object);

#endif
