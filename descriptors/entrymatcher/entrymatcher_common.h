
#ifndef ENTRYMATCHER_COMMON_H
#define ENTRYMATCHER_COMMON_H

#include <ext.h>
#include <ext_obex.h>

// ========================================================================================================================================== //
// Entry routines: refer, clear, labelmodes, names, entry and removal
// ========================================================================================================================================== //

template <class T> void entrymatcher_refer(T *x, t_symbol *name)
{
    x->database_object = database_change(name, x->database_object);
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

template <class T> void entrymatcher_audiostyle(T *x, t_atom_long style)
{
    x->matchers->setAudioStyle(style ? true : false);
}

template <class T> t_max_err entrymatcher_save_dict(T *x, long *argc, t_atom **argv)
{
    char alloc;
    
    atom_alloc(argc, argv, &alloc);
    t_dictionary *dict = database_getptr_read(x->database_object)->saveDictionary(true);
    atom_setobj(*argv, dict);
    
    return MAX_ERR_NONE;
}

template <class T> t_max_err entrymatcher_load_dict(T *x, long argc, t_atom *argv)
{
    t_dictionary *dict = NULL;
    
    if (argc)
        dict = (t_dictionary *) atom_getobj(argv);
    
    if (dict)
        database_getptr_write(x->database_object)->loadDictionary((t_object*) x, dict);
    
    return MAX_ERR_NONE;
}

template <class T> t_max_err entrymatcher_save_dict_attr(T *x, t_object *attr, long *argc, t_atom **argv)
{
    if (x->embed)
        return entrymatcher_save_dict<T>(x, argc, argv);
    else
        return MAX_ERR_GENERIC;
}

template <class T> t_max_err entrymatcher_load_dict_attr(T *x, t_object *attr, long argc, t_atom *argv)
{
    return entrymatcher_load_dict<T>(x, argc, argv);
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
    
    class_addmethod(class_pointer, (method)entrymatcher_save_dict_attr<T>, "getvalueof", A_CANT, 0);
    class_addmethod(class_pointer, (method)entrymatcher_load_dict_attr<T>, "setvalueof", A_CANT, 0);
    
    class_addmethod(class_pointer, (method)entrymatcher_view<T>, "view", 0);
    class_addmethod(class_pointer, (method)entrymatcher_save<T>, "write", A_DEFSYM, 0);
    class_addmethod(class_pointer, (method)entrymatcher_load<T>, "read", A_DEFSYM, 0);


    class_addmethod(class_pointer, (method)entrymatcher_audiostyle<T>, "audiostyle", A_DEFLONG, 0);
    
    CLASS_STICKY_CATEGORY(class_pointer, 0, "Storage");
    
    CLASS_ATTR_LONG(class_pointer, "embed", 0, T, embed);
    CLASS_ATTR_STYLE(class_pointer, "embed", 0, "onoff");
    CLASS_ATTR_SAVE(class_pointer, "embed", 0);

    CLASS_ATTR_OBJ(class_pointer, "database", ATTR_SET_OPAQUE_USER | ATTR_GET_OPAQUE_USER, T, x_obj);
    CLASS_ATTR_ACCESSORS(class_pointer, "database", (method) entrymatcher_save_dict_attr<T>, (method) entrymatcher_load_dict_attr<T>); 
    // this should be set based on the embed value, but for now...
    CLASS_ATTR_SELFSAVE(class_pointer, "database", 0);
}

#endif
