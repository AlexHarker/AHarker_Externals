
#ifndef ENTRYMATCHER_COMMON_HPP
#define ENTRYMATCHER_COMMON_HPP

#include <ext.h>
#include <ext_obex.h>
#include <ext_dictobj.h>

template <class T>
struct entrymatcher_common
{
    // Entry Routines: Refer, Clear, Labelmodes, Names, Entry and Removal

    static void entrymatcher_refer(T *x, t_symbol *name)
    {
        x->database_object = database_change(x, name, x->database_object);
    }

    static void entrymatcher_clear(T *x)
    {
        database_getptr_write(x->database_object)->clear();
    }

    static void entrymatcher_labelmodes(T *x, t_symbol *msg, long argc, t_atom *argv)
    {
        database_getptr_write(x->database_object)->set_column_label_modes(x, argc, argv);
    }

    static void entrymatcher_names(T *x, t_symbol *msg, long argc, t_atom *argv)
    {
        database_getptr_write(x->database_object)->set_column_names(x, argc, argv);
    }

    static void entrymatcher_entry(T *x, t_symbol *msg, long argc, t_atom *argv)
    {
        database_getptr_write(x->database_object)->add_entry(x, argc, argv);
    }

    static void entrymatcher_remove(T *x, t_symbol *msg, long argc, t_atom *argv)
    {
        database_getptr_write(x->database_object)->remove_entries(x, argc, argv);
    }

    static void entrymatcher_removeif(T *x, t_symbol *msg, long argc, t_atom *argv)
    {
        database_getptr_write(x->database_object)->remove_matched_entries(x, argc, argv);
    }

    // View, User Save and User Load Routines

    static void entrymatcher_view(T *x)
    {
        database_view(x, x->database_object);
    }

    static void entrymatcher_save(T *x, t_symbol *file)
    {
        database_getptr_read(x->database_object)->save((t_object *) x, file);
    }

    static void entrymatcher_load(T *x, t_symbol *file)
    {
        database_getptr_write(x->database_object)->load((t_object *) x, file);
    }

    // Load and Save (pattr/embedding/parameters etc.)

    // Save and Load Dictionary

    static t_dictionary *entrymatcher_save_dict(T *x)
    {
        return database_getptr_read(x->database_object)->save_dictionary(true);
    }

    static void entrymatcher_load_dict(T *x, t_dictionary *dict)
    {
        database_getptr_write(x->database_object)->load_dictionary((t_object*) x, dict);
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
            dictionary_appenddictionary(dict, gensym("database"), (t_object *) entrymatcher_save_dict(x));
    }

    static void entrymatcher_load_patcher(T *x)
    {
        t_dictionary *dict = nullptr;
        
        if ((dict = (t_dictionary *) gensym("#D")->s_thing))
        {
            dictionary_getdictionary(dict, gensym("database"), (t_object**) &dict);
            entrymatcher_load_dict(x, dict);
        }
    }

    // Parameter and pattr Support

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
        static t_symbol *database_modified = gensym("__database_modified");

        if (msg == database_modified)
        {
            object_notify(x, _sym_modified, nullptr);
            if (x->embed)
                jpatcher_set_dirty(x->patcher, 1);
        }
    }

    // Audiostyle Temporary Routine

    static void entrymatcher_audiostyle(T *x, t_atom_long style)
    {
        x->matchers->set_audio_style(style ? true : false);
    }

    // Add Common Routines

    static void class_add(t_class *class_pointer)
    {
        class_addmethod(class_pointer, (method) entrymatcher_refer,"refer", A_SYM, 0);
        
        class_addmethod(class_pointer, (method) entrymatcher_clear,"clear", 0);
        class_addmethod(class_pointer, (method) entrymatcher_clear,"reset", 0);
     
        class_addmethod(class_pointer, (method) entrymatcher_entry,"entry", A_GIMME, 0);
        class_addmethod(class_pointer, (method) entrymatcher_remove,"remove", A_GIMME, 0);
        class_addmethod(class_pointer, (method) entrymatcher_removeif,"removeif", A_GIMME, 0);
        class_addmethod(class_pointer, (method) entrymatcher_labelmodes,"labelmodes", A_GIMME, 0);
        class_addmethod(class_pointer, (method) entrymatcher_names,"names", A_GIMME, 0);

        class_addmethod(class_pointer, (method) entrymatcher_dictionary,"dictionary", A_SYM, 0);

        class_addmethod(class_pointer, (method) entrymatcher_save_patcher, "appendtodictionary", A_CANT, 0);

        class_addmethod(class_pointer, (method) entrymatcher_getvalueof, "getvalueof", A_CANT, 0);
        class_addmethod(class_pointer, (method) entrymatcher_setvalueof, "setvalueof", A_CANT, 0);
        class_addmethod(class_pointer, (method) entrymatcher_notify,"notify", A_CANT, 0);

        class_addmethod(class_pointer, (method) entrymatcher_view, "dblclick", A_CANT, 0);
        class_addmethod(class_pointer, (method) entrymatcher_view, "open", 0);
        class_addmethod(class_pointer, (method) entrymatcher_save, "write", A_DEFSYM, 0);
        class_addmethod(class_pointer, (method) entrymatcher_load, "read", A_DEFSYM, 0);

        class_addmethod(class_pointer, (method) entrymatcher_audiostyle, "audiostyle", A_DEFLONG, 0);
        
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

#endif
