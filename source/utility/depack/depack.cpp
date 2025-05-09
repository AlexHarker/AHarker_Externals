
/*
 *  depack
 *
 *  depack is a non-typed version of unpack.
 *  depack is suitbale for situations in which you need to unpack a list or message of unknown, or varying types.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>


// Globals and Object Structure

constexpr int max_outlets = 256;

t_class *this_class;

struct t_depack
{
    
    t_object a_obj;
    
    long num_outlets;
    
    void *outlet_array[max_outlets];
};

// Function Prototypes

void *depack_new(t_atom_long num_outlets);
void depack_free(t_depack *x);

void depack_do_args(t_depack *x, long argc, t_atom *argv, long offset);

void depack_int(t_depack *x, t_atom_long value);
void depack_float(t_depack *x, double value);
void depack_list(t_depack *x, t_symbol *msg, long argc, t_atom *argv);
void depack_anything(t_depack *x, t_symbol *msg, long argc, t_atom *argv);

void depack_assist(t_depack *x, void *b, long m, long a, char *s);

// Main

int C74_EXPORT main()
{
    this_class = class_new("depack",
                           (method) depack_new,
                           (method) nullptr,
                           sizeof(t_depack),
                           (method) nullptr,
                           A_DEFLONG,
                           0);
    
    class_addmethod(this_class, (method) depack_int, "int", A_LONG, 0);
    class_addmethod(this_class, (method) depack_float, "float", A_FLOAT, 0);
    class_addmethod(this_class, (method) depack_list, "list", A_GIMME, 0);
    class_addmethod(this_class, (method) depack_anything, "anything", A_GIMME, 0);
    class_addmethod(this_class, (method) depack_assist, "assist", A_CANT, 0);
    
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New

void *depack_new(t_atom_long num_outlets)
{
    t_depack *x = (t_depack *) object_alloc(this_class);
    
    if (num_outlets < 1)
    {
        object_error((t_object *) x, "the minimum number of outlets is 1");
        num_outlets = 1;
    }
    
    if (num_outlets > max_outlets)
    {
        object_error((t_object *) x, "the maximum number of outlets is %ld", max_outlets);
        num_outlets = max_outlets;
    }
    
    x->num_outlets = static_cast<long>(num_outlets);
    
    for (long i = x->num_outlets - 1; i >= 0; i--)
        x->outlet_array[i] = outlet_new(x, nullptr);
    
    return x;
}

// Message Handling (generic)

void depack_do_args(t_depack *x, long argc, t_atom *argv, long offset)
{
    long num_outlets = x->num_outlets;
    void **outlet_array = x->outlet_array;
    
    if (argc > num_outlets - offset) argc = num_outlets - offset;
    
    for (long i = argc - 1; i >= 0; i--)
    {
        switch (atom_gettype(argv + i))
        {
            case A_SYM:
                outlet_anything(outlet_array[i + offset], atom_getsym(argv + i), 0, 0);
                break;
            case A_FLOAT:
                outlet_float(outlet_array[i + offset], atom_getfloat(argv + i));
                break;
            case A_LONG:
                outlet_int(outlet_array[i + offset], atom_getlong(argv + i));
                break;
        }
    }
}

// User Messages

void depack_float(t_depack *x, double value)
{
    outlet_float(x->outlet_array[0], value);
}

void depack_int(t_depack *x, t_atom_long value)
{
    outlet_int(x->outlet_array[0], value);
}

void depack_list(t_depack *x, t_symbol *msg, long argc, t_atom *argv)
{
    depack_do_args(x, argc, argv, 0);
}

void depack_anything(t_depack *x, t_symbol *msg, long argc, t_atom *argv)
{
    depack_do_args(x, argc, argv, 1);
    outlet_anything(x->outlet_array[0], msg, 0, 0);
}

// Assist

void depack_assist(t_depack *x, void *b, long m, long a, char *s)
{
    static constexpr int maxAssist = 256;

    if (m == ASSIST_OUTLET)
        snprintf(s, maxAssist, "Outlet %ld", a + 1);
    else
        snprintf(s, maxAssist, "Input (anything)");
}
