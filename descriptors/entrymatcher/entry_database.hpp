
#ifndef ENTRY_DATABASE_HPP
#define ENTRY_DATABASE_HPP

#include <ext.h>
#include <ext_obex.h>

#include "entries.hpp"

// N.B. - Only Client Routines and Structures are ezposed (other items are private)

// Private Strings (for inter max-object communication)

struct private_strings
{
    static const char *view_classname() { return "__entry_database_view"; }
    static const char *database_modified() { return "__database_modified"; }
    static const char *view_removed() { return "__view_removed"; }
    static const char *build_view() { return "__build_view"; }
    static const char *set_database() { return "__set_database"; }
};

// This structure is opaque from other files

struct t_entry_database;

// Access that Notifies Max Database Object of Write Operations (notifying all clients)

struct notifying_modify_access : public entries::modify_access
{
    notifying_modify_access(entries& data, t_entry_database *database)
    : entries::modify_access(data), m_database(database) {}
    
    notifying_modify_access(const notifying_modify_access&) = delete;
    notifying_modify_access& operator=(const notifying_modify_access&) = delete;
    notifying_modify_access(notifying_modify_access&&) = default;
    ~notifying_modify_access();
    
protected:

    t_entry_database *m_database;
};

// Get / Change / Release Database

t_entry_database *database_create(void *x, t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns);
t_entry_database *database_change(void *x, t_symbol *name, t_entry_database *prev_database);
void database_release(void *x, t_entry_database *database);
void database_view(void *x, t_entry_database *database);

// Retrieve Pointers for Reading or Writing

entries::read_access database_get_read_access(t_entry_database *database);
notifying_modify_access database_get_write_access(t_entry_database *database);

#endif
