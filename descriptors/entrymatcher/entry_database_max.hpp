
#ifndef ENTRY_DATABASE_MAX_HPP
#define ENTRY_DATABASE_MAX_HPP

#include <ext.h>
#include <ext_obex.h>

#include "entry_database.hpp"

// Client Routines and Structures ONLY (other items are private)

// Pointer that Notifies Max Database Object of Write Operations (notifying all clients)

struct notify_pointer : public EntryDatabase::WritePointer
{
    notify_pointer(EntryDatabase *ptr, t_object *maxDatabase) : EntryDatabase::WritePointer(ptr), mMaxDatabase(maxDatabase) {}
    notify_pointer(const notify_pointer&) = delete;
    notify_pointer& operator=(const notify_pointer&) = delete;
    notify_pointer(notify_pointer&&) = default;
    ~notify_pointer();
    
protected:

    t_object *mMaxDatabase;
};

// Get / Change / Release Named Database

t_object *database_create(void *x, t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns);
t_object *database_change(void *x, t_symbol *name, t_object *old_database_object);
void database_release(void *x, t_object *database_object);
void database_view(void *x, t_object *database_object);

// Retrieve Pointers for Reading or Writing

EntryDatabase::ReadPointer database_getptr_read(t_object *database_object);
notify_pointer database_getptr_write(t_object *database_object);

#endif
