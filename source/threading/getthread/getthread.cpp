
/*
 *  getthread
 *
 *  getthread is an object to report which thread incoming events are in.
 *  Output is 1 for the scheduler thread or 0 otherwise.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>


// Globals and Object Structure

t_class *this_class;

struct t_getthread
{    
    t_object a_obj;
    
    void *thread_out;
};

// Function Prototypes

void *getthread_new();

void getthread_doit(t_getthread *x);
void getthread_int(t_getthread *x, t_atom_long value);
void getthread_float(t_getthread *x, double float_in);
void getthread_list(t_getthread *x, t_symbol *msg, long argc, t_atom *argv);
void getthread_anything(t_getthread *x, t_symbol *msg, long argc, t_atom *argv);

void getthread_assist(t_getthread *x, void *b, long m, long a, char *s);

// Main

int C74_EXPORT main()
{
    this_class = class_new("getthread",
                          (method) getthread_new,
                          (method) nullptr,
                          sizeof(t_getthread),
                          (method) nullptr,
                          0);
    
    class_addmethod(this_class, (method) getthread_doit, "bang", 0);
    class_addmethod(this_class, (method) getthread_int, "int", A_LONG, 0);
    class_addmethod(this_class, (method) getthread_float, "float", A_FLOAT, 0);
    class_addmethod(this_class, (method) getthread_list, "list", A_GIMME, 0);
    class_addmethod(this_class, (method) getthread_anything, "anything", A_GIMME, 0);
    class_addmethod(this_class, (method) getthread_assist, "assist", A_CANT, 0);
    
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New

void *getthread_new()
{
    t_getthread *x = (t_getthread *) object_alloc(this_class);
    
    x->thread_out = intout(x);
    
    return x;
}

// Message Handlers

void getthread_doit(t_getthread *x)
{
    outlet_int(x->thread_out, isr());
}

void getthread_int(t_getthread *x, t_atom_long value)
{
    getthread_doit(x);
}

void getthread_float(t_getthread *x, double float_in)
{
    getthread_doit(x);
}

void getthread_list(t_getthread *x, t_symbol *msg, long argc, t_atom *argv)
{
    getthread_doit(x);
}

void getthread_anything(t_getthread *x, t_symbol *msg, long argc, t_atom *argv)
{
    getthread_doit(x);
}

// Assist

void getthread_assist(t_getthread *x, void *b, long m, long a, char *s)
{
    static constexpr int maxAssist = 256;

    if (m == ASSIST_OUTLET)
    {
        snprintf(s, maxAssist, "Thread ID Out");
    }
    else
    {
        snprintf(s, maxAssist, "Input (anything)");
    }
}
