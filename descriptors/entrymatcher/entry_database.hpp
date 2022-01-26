
#ifndef ENTRY_DATABASE_HPP
#define ENTRY_DATABASE_HPP

#include <ext.h>
#include <ext_obex.h>

#include "entries.hpp"

// N.B. - Only Client Routines and Structures are ezposed (other items are private)

// Pointer that Notifies Max Database Object of Write Operations (notifying all clients)

struct notify_pointer : public entries::write_pointer
{
    notify_pointer(entries *ptr, t_object *max_database)
    : entries::write_pointer(ptr), m_max_database(max_database) {}
    
    notify_pointer(const notify_pointer&) = delete;
    notify_pointer& operator=(const notify_pointer&) = delete;
    notify_pointer(notify_pointer&&) = default;
    ~notify_pointer();
    
protected:

    t_object *m_max_database;
};

// Get / Change / Release Database

t_object *database_create(void *x, t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns);
t_object *database_change(void *x, t_symbol *name, t_object *old_database_object);
void database_release(void *x, t_object *database_object);
void database_view(void *x, t_object *database_object);

// Retrieve Pointers for Reading or Writing

entries::read_pointer database_getptr_read(t_object *database_object);
notify_pointer database_getptr_write(t_object *database_object);

#endif
