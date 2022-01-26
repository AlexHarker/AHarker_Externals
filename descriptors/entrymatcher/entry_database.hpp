
#ifndef ENTRY_DATABASE_HPP
#define ENTRY_DATABASE_HPP

#include <ext.h>
#include <ext_obex.h>

#include "entries.hpp"

// N.B. - Only Client Routines and Structures are ezposed (other items are private)

// This structure is opaque from other files

struct t_entry_database;

// Pointer that Notifies Max Database Object of Write Operations (notifying all clients)

struct notify_pointer : public entries::write_pointer
{
    notify_pointer(entries *ptr, t_entry_database *database)
    : entries::write_pointer(ptr), m_database(database) {}
    
    notify_pointer(const notify_pointer&) = delete;
    notify_pointer& operator=(const notify_pointer&) = delete;
    notify_pointer(notify_pointer&&) = default;
    ~notify_pointer();
    
protected:

    t_entry_database *m_database;
};

// Get / Change / Release Database

t_entry_database *database_create(void *x, t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns);
t_entry_database *database_change(void *x, t_symbol *name, t_entry_database *prev_database);
void database_release(void *x, t_entry_database *database);
void database_view(void *x, t_entry_database *database);

// Retrieve Pointers for Reading or Writing

entries::read_pointer database_getptr_read(t_entry_database *database);
notify_pointer database_getptr_write(t_entry_database *database);

#endif
