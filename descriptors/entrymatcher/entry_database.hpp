
/*
 *  entry_database.hpp
 *
 *  A header file for the entry_database Max class for the entrymatcher(~) objects.
 *
 *  This Max class wraps an entries underlying database in a Max class.
 *  Facilities are provided to create, change, release or view the underlying database.
 *
 *  N.B. - Only client routines and structures are exposed here (other items are private)
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#ifndef _ENTRY_DATABASE_HPP_
#define _ENTRY_DATABASE_HPP_

#include <ext.h>
#include <ext_obex.h>

#include "entries.hpp"


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

struct notifying_write_access : public entries::write_access
{
    notifying_write_access(entries& data, t_entry_database *database)
    : entries::write_access(data), m_database(database) {}
    
    notifying_write_access(const notifying_write_access&) = delete;
    notifying_write_access& operator=(const notifying_write_access&) = delete;
    notifying_write_access(notifying_write_access&&) = default;
    ~notifying_write_access();
    
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
notifying_write_access database_get_write_access(t_entry_database *database);

#endif
