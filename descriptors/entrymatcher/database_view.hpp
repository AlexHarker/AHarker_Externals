
/*
 *  database_view.hpp
 *
 *  A header file for the database_view Max class for the entrymatcher(~) objects.
 *
 *  This Max class handles a dataview in an associated patcher for viewing a database.
 *  The init routine is called by entry_database.
 *  All other communication is handled via the Max API
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#ifndef _DATABASE_VIEW_HPP_
#define _DATABASE_VIEW_HPP_

#include <ext.h>
#include <ext_obex.h>

void database_view_init();

#endif /* _DATABASE_VIEW_HPP_ */
