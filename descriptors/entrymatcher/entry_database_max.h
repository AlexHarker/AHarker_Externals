
#ifndef ENTRY_DATABASE_MAX_H
#define ENTRY_DATABASE_MAX_H

#include <ext.h>
#include <ext_obex.h>

#include "EntryDatabase.h"

struct NotifyPointer : public EntryDatabase::WritePointer
{
    NotifyPointer(EntryDatabase *ptr, t_object *maxDatabase, t_object *client) : EntryDatabase::WritePointer(ptr), mMaxDatabase(maxDatabase), mClient(client) {}
    
    ~NotifyPointer();
    
private:
    
    t_object *mMaxDatabase;
    t_object *mClient;
};

t_object *database_create(void *x, t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns);
t_object *database_change(void *x, t_symbol *name, t_object *old_object);
void database_release(void *x, t_object *database_object);

EntryDatabase::ReadPointer database_getptr_read(t_object *x);
NotifyPointer database_getptr_write(t_object *x, t_object *client);

#endif
