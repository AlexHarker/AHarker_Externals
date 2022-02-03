
/*
 *  ibuffermulti~
 *
 *  ibuffermulti~ can be used to dynamically load audio files into multiple ibuffer~s.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

// N.B. - alterations to the object structure only happen in the low priority thread for threadsafety


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <vector>

#include <AH_Lifecycle.hpp>


// Globals and Object Structure

t_class *this_class;

t_symbol *ps_set = gensym("set");
t_symbol *ps_replace = gensym("replace");
t_symbol *ps_valid = gensym("valid");

struct t_ibuffermulti
{
    t_pxobject x_obj;
    
    std::vector<t_object *> buffers;
    
    void *number_out;
};

// Function Prototypes

void *ibuffermulti_new();
void ibuffermulti_free(t_ibuffermulti *x);
void ibuffermulti_assist(t_ibuffermulti *x, void *b, long m, long a, char *s);

void ibuffermulti_clear(t_ibuffermulti *x);
void ibuffermulti_clear_internal(t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv);
void ibuffermulti_load(t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv);
void ibuffermulti_load_internal(t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv);

// Main

int C74_EXPORT main()
{
    this_class = class_new("ibuffermulti~",
                           (method) ibuffermulti_new,
                           (method) ibuffermulti_free,
                           sizeof(t_ibuffermulti),
                           (method) nullptr,
                           0);
    
    class_addmethod(this_class, (method) ibuffermulti_clear, "clear", 0);
    class_addmethod(this_class, (method) ibuffermulti_load, "load", A_GIMME, 0);
    class_addmethod(this_class, (method) ibuffermulti_assist, "assist", A_CANT, 0);
    
    class_dspinit(this_class);
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / Free / Assist

void *ibuffermulti_new()
{
    t_ibuffermulti *x = (t_ibuffermulti *) object_alloc(this_class);
    
    dsp_setup((t_pxobject *) x, 0);
    
    x->number_out = intout(x);
        
    create_object(x->buffers);

    return x;
}

void ibuffermulti_free(t_ibuffermulti *x)
{
    dsp_free(&x->x_obj);
    ibuffermulti_clear(x);
    destroy_object(x->buffers);
}

void ibuffermulti_assist(t_ibuffermulti *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
    {
        sprintf(s,"Num of Buffers Loaded");
    }
    else
    {
        sprintf(s,"File Operations");
    }
}

// Clear

void ibuffermulti_clear(t_ibuffermulti *x)
{
    defer(x, (method) ibuffermulti_clear_internal, 0, 0, 0);
}

void ibuffermulti_clear_internal(t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv)
{
    for (long i = 0; i < x->buffers.size(); i++)
        object_free(x->buffers[i]);
    
    x->buffers.clear();
    
    outlet_int(x->number_out, 0);
}

// Load

void ibuffermulti_load(t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv)
{
    if (argc)
        defer(x, (method) ibuffermulti_load, s, argc, argv);
}

void ibuffermulti_load_internal(t_ibuffermulti *x, t_symbol *s, short argc, t_atom *argv)
{
    // First make sure the ibuffer object is loaded
    
    if (!class_findbyname(CLASS_BOX, gensym("ibuffer~")))
    {
        void *force_load = newinstance(gensym("ibuffer~"), 0, 0);
        
        if (force_load)
            freeobject(static_cast<t_object *>(force_load));
    }
    
    // Return a properly allocated object (may still return nullptr if the .mxo is not present)
    
    t_object *ibuffer = reinterpret_cast<t_object *>(object_new_typed(CLASS_BOX, gensym("ibuffer~"), 0, nullptr));
    
    bool buffer_loaded = false;
    
    // Now load the buffer
    
    if (ibuffer && argc > 1 && atom_gettype(argv) == A_SYM && atom_gettype(argv + 1) == A_SYM)
    {
        t_atom return_val;
        
        // Set name, then load file - finally check validity
        
        object_method_typed(ibuffer, ps_set, 1, argv, &return_val);
        object_method_typed(ibuffer, ps_replace, argc - 1, argv + 1, &return_val);
        long *valid_val = (long *) object_method(ibuffer, ps_valid);
        
        if (valid_val && *valid_val)
            buffer_loaded = true;
    }
    
    if (buffer_loaded)
        x->buffers.push_back(ibuffer);
    else
    {
        object_error(reinterpret_cast<t_object *>(x), "could not load ibuffer~");
        object_free(ibuffer);
    }
    
    outlet_int(x->number_out, x->buffers.size());
}
