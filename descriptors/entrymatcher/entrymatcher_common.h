
#ifndef ENTRYMATCHER_COMMON_H
#define ENTRYMATCHER_COMMON_H

#include <ext.h>
#include <ext_obex.h>

// ========================================================================================================================================== //
// Entry routines: refer, clear, labelmodes, names, entry and removal
// ========================================================================================================================================== //

template <class T> void entrymatcher_refer(T *x, t_symbol *name)
{
    x->database_object = database_change(x, name, x->database_object);
}

template <class T> void entrymatcher_clear(T *x)
{
    database_getptr_write(x->database_object)->clear();
}

template <class T> void entrymatcher_labelmodes(T *x, t_symbol *msg, long argc, t_atom *argv)
{
    database_getptr_write(x->database_object)->setColumnLabelModes(x, argc, argv);
}

template <class T> void entrymatcher_names(T *x, t_symbol *msg, long argc, t_atom *argv)
{
    database_getptr_write(x->database_object)->setColumnNames(x, argc, argv);
}

template <class T> void entrymatcher_entry(T *x, t_symbol *msg, long argc, t_atom *argv)
{
    database_getptr_write(x->database_object)->addEntry(x, argc, argv);
}

template <class T> void entrymatcher_remove(T *x, t_symbol *msg, long argc, t_atom *argv)
{
    database_getptr_write(x->database_object)->removeEntries(x, argc, argv);
}

template <class T> void entrymatcher_removeif(T *x, t_symbol *msg, long argc, t_atom *argv)
{
    database_getptr_write(x->database_object)->removeMatchedEntries(x, argc, argv);
}

// ========================================================================================================================================== //
// View, Save and Load Routines
// ========================================================================================================================================== //

template <class T> void entrymatcher_view(T *x)
{
    database_getptr_read(x->database_object)->view(x->database_object);
}

template <class T> void entrymatcher_save(T *x, t_symbol *file)
{
    database_getptr_read(x->database_object)->save((t_object *)x, file);
}

template <class T> void entrymatcher_load(T *x, t_symbol *file)
{
    database_getptr_write(x->database_object)->load((t_object *)x, file);
}

// ========================================================================================================================================== //
// Load and Save
// ========================================================================================================================================== //

template <class T> t_dictionary *entrymatcher_save_dict(T *x)
{
    return database_getptr_read(x->database_object)->saveDictionary(true);
}

template <class T> void entrymatcher_load_dict(T *x, t_dictionary *dict)
{
    if (dict)
        database_getptr_write(x->database_object)->loadDictionary((t_object*) x, dict);
}

template <class T> t_max_err entrymatcher_save_patcher(T *x, t_dictionary *dict)
{
    if (x->embed)
        return dictionary_appenddictionary(dict, gensym("database"), (t_object *) entrymatcher_save_dict(x));
   
    return MAX_ERR_NONE;
}

template <class T> void entrymatcher_load_patcher(T *x)
{
    t_dictionary *dict = NULL;
    
    if ((dict = (t_dictionary *) gensym("#D")->s_thing))
    {
        dictionary_getdictionary(dict, gensym("database"), (t_object**) &dict);
        entrymatcher_load_dict(x, dict);
    }
}

template <class T> t_max_err entrymatcher_getvalueof(T *x, long *argc, t_atom **argv)
{
    char alloc;
    
    atom_alloc(argc, argv, &alloc);
    t_dictionary *dict = entrymatcher_save_dict<T>(x);
    atom_setobj(*argv, dict);
    
    return MAX_ERR_GENERIC;
}

template <class T> t_max_err entrymatcher_setvalueof(T *x, long argc, t_atom *argv)
{
    if (argc)
        entrymatcher_load_dict<T>(x, (t_dictionary *) atom_getobj(argv));
    
    return MAX_ERR_NONE;
}

template <class T> void entrymatcher_notify(T *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
    static t_symbol *database_modified = gensym("database_modified");

    if (s == database_modified)
    {
        object_notify(x, _sym_modified, NULL);
        jpatcher_set_dirty(x->patcher, 1);
    }
}

template <class T> void entrymatcher_audiostyle(T *x, t_atom_long style)
{
    x->matchers->setAudioStyle(style ? true : false);
}

// ========================================================================================================================================== //
// Add Common Routines
// ========================================================================================================================================== //

template <class T> void entrymatcher_add_common(t_class *class_pointer)
{
    class_addmethod(class_pointer, (method)entrymatcher_refer<T>,"refer", A_SYM, 0);
    
    class_addmethod(class_pointer, (method)entrymatcher_clear<T>,"clear", 0);
    class_addmethod(class_pointer, (method)entrymatcher_clear<T>,"reset", 0);
 
    class_addmethod(class_pointer, (method)entrymatcher_entry<T>,"entry", A_GIMME, 0);
    class_addmethod(class_pointer, (method)entrymatcher_remove<T>,"remove", A_GIMME, 0);
    class_addmethod(class_pointer, (method)entrymatcher_removeif<T>,"removeif", A_GIMME, 0);
    class_addmethod(class_pointer, (method)entrymatcher_labelmodes<T>,"labelmodes", A_GIMME, 0);
    class_addmethod(class_pointer, (method)entrymatcher_names<T>,"names", A_GIMME, 0);
    
    class_addmethod(class_pointer, (method)entrymatcher_save_patcher<T>, "appendtodictionary", A_CANT, 0);

    class_addmethod(class_pointer, (method)entrymatcher_getvalueof<T>, "getvalueof", A_CANT, 0);
    class_addmethod(class_pointer, (method)entrymatcher_setvalueof<T>, "setvalueof", A_CANT, 0);
    class_addmethod(class_pointer, (method)entrymatcher_notify<T>,"notify", A_CANT, 0);

    class_addmethod(class_pointer, (method)entrymatcher_view<T>, "view", 0);
    class_addmethod(class_pointer, (method)entrymatcher_save<T>, "write", A_DEFSYM, 0);
    class_addmethod(class_pointer, (method)entrymatcher_load<T>, "read", A_DEFSYM, 0);

    class_addmethod(class_pointer, (method)entrymatcher_audiostyle<T>, "audiostyle", A_DEFLONG, 0);
    
    CLASS_STICKY_CATEGORY(class_pointer, 0, "Storage");
    
    CLASS_ATTR_LONG(class_pointer, "embed", 0, T, embed);
    CLASS_ATTR_STYLE(class_pointer, "embed", 0, "onoff");
    CLASS_ATTR_SAVE(class_pointer, "embed", 0);
}

// ========================================================================================================================================== //
// New Common Routine
// ========================================================================================================================================== //

template <class T> void entrymatcher_new_common(T *x)
{
    entrymatcher_load_patcher(x);
    x->patcher = gensym("#P")->s_thing;
}

#endif
