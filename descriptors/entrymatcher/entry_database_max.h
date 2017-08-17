
#ifndef ENTRY_DATABASE_MAX_H
#define ENTRY_DATABASE_MAX_H

#include <ext.h>
#include <ext_obex.h>

#include "EntryDatabase.h"

template <class T> class ManagedPtr
{
public:
    
    ManagedPtr(T *ptr) : mPtr(ptr) {}
    
    T *operator->() { return mPtr; }
    T *get()  { return mPtr; }
    
private:
    
    T *mPtr;
};

typedef ManagedPtr<EntryDatabase> WritableDatabasePtr;
typedef ManagedPtr<const EntryDatabase> ReadableDatabasePtr;

t_object *database_create(t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns);
t_object *database_change(t_symbol *name, t_object *old_object);
void database_release(t_object *x);

ReadableDatabasePtr database_getptr_read(t_object *x);
WritableDatabasePtr database_getptr_write(t_object *x);

#endif
