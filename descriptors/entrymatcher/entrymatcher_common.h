
#ifndef ENTRYMATCHER_COMMON_H
#define ENTRYMATCHER_COMMON_H

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
    database_getptr_write(x->database_object)->setLabelModes(x, argc, argv);
}

template <class T> void entrymatcher_names(T *x, t_symbol *msg, long argc, t_atom *argv)
{
    database_getptr_write(x->database_object)->setNames(x, argc, argv);
}

template <class T> void entrymatcher_entry(T *x, t_symbol *msg, long argc, t_atom *argv)
{
    database_getptr_write(x->database_object)->addEntry(x, argc, argv);
}

template <class T> void entrymatcher_remove(T *x, t_symbol *msg, long argc, t_atom *argv)
{
    database_getptr_write(x->database_object)->removeEntries(x, argc, argv);
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
// Add Common Routines
// ========================================================================================================================================== //

template <class T> void entrymatcher_add_common(t_class *class_pointer)
{
    class_addmethod(class_pointer, (method)entrymatcher_refer<T>,"refer", A_SYM, 0);
    
    class_addmethod(class_pointer, (method)entrymatcher_clear<T>,"clear", 0);
    class_addmethod(class_pointer, (method)entrymatcher_clear<T>,"reset", 0);
 
    class_addmethod(class_pointer, (method)entrymatcher_entry<T>,"entry", A_GIMME, 0);
    class_addmethod(class_pointer, (method)entrymatcher_remove<T>,"remove", A_GIMME, 0);
    class_addmethod(class_pointer, (method)entrymatcher_labelmodes<T>,"labelmodes", A_GIMME, 0);
    class_addmethod(class_pointer, (method)entrymatcher_names<T>,"names", A_GIMME, 0);
    
    class_addmethod(class_pointer, (method)entrymatcher_view<T>, "view", 0);
    class_addmethod(class_pointer, (method)entrymatcher_save<T>, "write", A_DEFSYM, 0);
    class_addmethod(class_pointer, (method)entrymatcher_load<T>, "read", A_DEFSYM, 0);
}

#endif