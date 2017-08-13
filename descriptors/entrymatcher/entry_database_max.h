
#ifndef ENTRY_DATABASE_MAX_H
#define ENTRY_DATABASE_MAX_H

#include <ext.h>
#include <ext_obex.h>

#include "EntryDatabase.h"

typedef struct entry_database{
    
    t_object a_obj;
    
    EntryDatabase *mDatabase;
    
    long count;
    
} t_entry_database;

t_entry_database *entry_database_get_database_object(t_symbol *name, t_atom_long num_reserved_entries, t_atom_long num_columns);
void entry_database_release(t_entry_database *x);
EntryDatabase *entry_database_get_database(t_entry_database *x);

#endif 
