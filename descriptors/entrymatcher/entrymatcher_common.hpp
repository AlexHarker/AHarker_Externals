
/*
 *  entrymatcher_common.hpp
 *
 *  A header file with common functionaility for the entrymatcher(~) objects.
 *
 *  This class holds a number of useful static Max methods.
 *
 *  - The Max object structures are passed by template argument
 *  - The Max object structures mist contain several expected fields
 *  - The code for each object should call class_add in its main routine.
 *  - The code for each object should call object_init in its new routine.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#ifndef _ENTRYMATCHER_COMMON_HPP_
#define _ENTRYMATCHER_COMMON_HPP_

#include <ext.h>
#include <ext_obex.h>
#include <ext_dictobj.h>

#include <AH_Int_Handler.hpp>

#include "matchers.hpp"


template <class T>
struct entrymatcher_common
{
    using loop_order = matchers::loop_order;
    
    // Resize Columns
    
    static void entrymatcher_resize(T *x, t_atom_long columns)
    {
        database_get_write_access(x->database_object).resize_columns(limit_int<long>(columns));
    }
    
    // Entry Routines: Refer, Clear, Labelmodes, Names, Entry and Removal

    static void entrymatcher_refer(T *x, t_symbol *name)
    {
        x->database_object = database_change(x, name, x->database_object);
    }

    static void entrymatcher_clear(T *x)
    {
        database_get_write_access(x->database_object).clear();
    }

    static void entrymatcher_labelmodes(T *x, t_symbol *msg, long argc, t_atom *argv)
    {
        database_get_write_access(x->database_object).set_column_label_modes(x, argc, argv);
    }

    static void entrymatcher_names(T *x, t_symbol *msg, long argc, t_atom *argv)
    {
        database_get_write_access(x->database_object).set_column_names(x, argc, argv);
    }

    static void entrymatcher_entry(T *x, t_symbol *msg, long argc, t_atom *argv)
    {
        database_get_write_access(x->database_object).add_entry(x, argc, argv);
    }

    static void entrymatcher_remove(T *x, t_symbol *msg, long argc, t_atom *argv)
    {
        database_get_write_access(x->database_object).remove_entries(x, argc, argv);
    }

    static void entrymatcher_removeif(T *x, t_symbol *msg, long argc, t_atom *argv)
    {
        database_get_write_access(x->database_object).remove_matched_entries(x, argc, argv);
    }

    // View, User Save and User Load Routines

    static void entrymatcher_view(T *x)
    {
        database_view(x, x->database_object);
    }

    static void entrymatcher_save_file(T *x, t_symbol *file)
    {
        database_get_read_access(x->database_object).save_file((t_object *) x, file);
    }

    static void entrymatcher_load_file(T *x, t_symbol *file)
    {
        database_get_write_access(x->database_object).load_file((t_object *) x, file);
    }

    // Load and Save (pattr/embedding/parameters etc.)

    // Save and Load Dictionary

    static t_dictionary *entrymatcher_save_dict(T *x)
    {
        return database_get_read_access(x->database_object).save_dictionary(true);
    }

    static void entrymatcher_load_dict(T *x, t_dictionary *dict)
    {
        database_get_write_access(x->database_object).load_dictionary((t_object*) x, dict);
    }

    static void entrymatcher_dictionary(T *x, t_symbol *dictname)
    {
        t_dictionary *dict = dictobj_findregistered_retain(dictname);
        
        entrymatcher_load_dict(x, dict);
        
        if (dict)
            dictobj_release(dict);
    }

    // Save and Load Patcher

    static void entrymatcher_save_patcher(T *x, t_dictionary *dict)
    {
        if (x->embed)
            dictionary_appenddictionary(dict, gensym("entrymatcher_database"), (t_object *) entrymatcher_save_dict(x));
    }

    static void entrymatcher_load_patcher(T *x)
    {
        t_dictionary *dict = nullptr;
        
        if ((dict = (t_dictionary *) gensym("#D")->s_thing))
        {
            dictionary_getdictionary(dict, gensym("entrymatcher_database"), (t_object**) &dict);
            entrymatcher_load_dict(x, dict);
        }
    }

    // Attribute and pattr Support

    static t_max_err entrymatcher_getvalueof(T *x, long *argc, t_atom **argv)
    {
        char alloc;
        
        atom_alloc(argc, argv, &alloc);
        t_dictionary *dict = entrymatcher_save_dict(x);
        atom_setobj(*argv, dict);
        
        return MAX_ERR_GENERIC;
    }

    static t_max_err entrymatcher_setvalueof(T *x, long argc, t_atom *argv)
    {
        if (argc)
            entrymatcher_load_dict(x, (t_dictionary *) atom_getobj(argv));
        
        return MAX_ERR_NONE;
    }

    static void entrymatcher_notify(T *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
    {
        static t_symbol *modified = gensym("modified");
        static t_symbol *database_modified = gensym(private_strings::database_modified());

        if (msg == database_modified)
        {
            object_notify(x, modified, nullptr);
            if (x->embed)
                jpatcher_set_dirty(x->patcher, 1);
        }
    }

    // Set Loop Order

    static void entrymatcher_loop_order(T *x, t_atom_long style)
    {
        x->matchers.set_loop_order(style ? loop_order::by_matcher : loop_order::by_item);
    }

    // Add Common Routines

    static void class_add(t_class *class_pointer)
    {
        class_addmethod(class_pointer, (method) entrymatcher_resize, "resize", A_LONG, 0);

        class_addmethod(class_pointer, (method) entrymatcher_refer, "refer", A_SYM, 0);
        
        class_addmethod(class_pointer, (method) entrymatcher_clear, "clear", 0);
        class_addmethod(class_pointer, (method) entrymatcher_clear, "reset", 0);

        class_addmethod(class_pointer, (method) entrymatcher_entry, "entry", A_GIMME, 0);
        class_addmethod(class_pointer, (method) entrymatcher_remove, "remove", A_GIMME, 0);
        class_addmethod(class_pointer, (method) entrymatcher_removeif, "removeif", A_GIMME, 0);
        class_addmethod(class_pointer, (method) entrymatcher_labelmodes, "labelmodes", A_GIMME, 0);
        class_addmethod(class_pointer, (method) entrymatcher_names, "names", A_GIMME, 0);

        class_addmethod(class_pointer, (method) entrymatcher_dictionary, "dictionary", A_SYM, 0);

        class_addmethod(class_pointer, (method) entrymatcher_save_patcher, "appendtodictionary", A_CANT, 0);

        class_addmethod(class_pointer, (method) entrymatcher_getvalueof, "getvalueof", A_CANT, 0);
        class_addmethod(class_pointer, (method) entrymatcher_setvalueof, "setvalueof", A_CANT, 0);
        class_addmethod(class_pointer, (method) entrymatcher_notify,"notify", A_CANT, 0);

        class_addmethod(class_pointer, (method) entrymatcher_view, "dblclick", A_CANT, 0);
        class_addmethod(class_pointer, (method) entrymatcher_view, "open", 0);
        class_addmethod(class_pointer, (method) entrymatcher_save_file, "write", A_DEFSYM, 0);
        class_addmethod(class_pointer, (method) entrymatcher_load_file, "read", A_DEFSYM, 0);

        class_addmethod(class_pointer, (method) entrymatcher_loop_order, "looporder", A_DEFLONG, 0);
        
        CLASS_STICKY_CATEGORY(class_pointer, 0, "Storage");
        
        CLASS_ATTR_LONG(class_pointer, "embed", 0, T, embed);
        CLASS_ATTR_STYLE(class_pointer, "embed", 0, "onoff");
        CLASS_ATTR_SAVE(class_pointer, "embed", 0);
    }

    // New Common Routine

    static void object_init(T *x)
    {
        entrymatcher_load_patcher(x);
        x->patcher = gensym("#P")->s_thing;
    }
};

#endif /* _ENTRYMATCHER_COMMON_HPP_ */
