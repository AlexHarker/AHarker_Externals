
/*
 *  schedulemessage
 *
 *  schedulemessage has two potential uses:
 *
 *  1 - To move messages from the low priority thread to high priority scheduler thread (if it it exists).
 *  2 - To delay any message (like delay, or pipe but for any messages).
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>


// Globals and Object Structure

t_class *this_class;

t_symbol *ps_int = gensym("int");
t_symbol *ps_float = gensym("float");
t_symbol *ps_bang = gensym("bang");

struct t_schedulemessage
{
    t_object a_obj;
    
    double delay;
    
    void *message_out;
};

// Function Prototypes

void *schedulemessage_new(double delay);

void schedulemessage_output(t_schedulemessage *x, t_symbol *msg, long argc, t_atom *argv);
void schedulemessage_int(t_schedulemessage *x, t_atom_long value);
void schedulemessage_float(t_schedulemessage *x, double float_in);
void schedulemessage_bang(t_schedulemessage *x);
void schedulemessage_anything(t_schedulemessage *x, t_symbol *msg, long argc, t_atom *argv);
void schedulemessage_delay(t_schedulemessage *x, double delay);

void schedulemessage_assist(t_schedulemessage *x, void *b, long m, long a, char *s);

// Main

int C74_EXPORT main()
{
    this_class = class_new("schedulemessage",
                          (method) schedulemessage_new,
                          (method) nullptr,
                          sizeof(t_schedulemessage),
                          (method) nullptr,
                          A_DEFFLOAT,
                          0);

    class_addmethod(this_class, (method) schedulemessage_int, "int", A_LONG, 0);
    class_addmethod(this_class, (method) schedulemessage_float, "float", A_FLOAT, 0);
    class_addmethod(this_class, (method) schedulemessage_delay, "ft1", A_FLOAT, 0);
    class_addmethod(this_class, (method) schedulemessage_bang, "bang", 0);
    class_addmethod(this_class, (method) schedulemessage_anything, "list", A_GIMME, 0);
    class_addmethod(this_class, (method) schedulemessage_anything, "anything", A_GIMME, 0);
    class_addmethod(this_class, (method) schedulemessage_assist, "assist", A_CANT, 0);
    
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New

void *schedulemessage_new(double delay)
{
    t_schedulemessage *x = (t_schedulemessage *) object_alloc(this_class);
    
    floatin(x, 1);
    
    x->message_out = outlet_new(x, nullptr);
    x->delay = delay;
    
    return x;
}

// Message Handlers

void schedulemessage_output(t_schedulemessage *x, t_symbol *msg, long argc, t_atom *argv)
{
    if (msg == ps_bang)
    {
        outlet_bang(x->message_out);
        return;
    }
    
    if (msg == ps_int)
    {
        if (argc)
            outlet_int(x->message_out, atom_getlong(argv));
        return;
    }
    
    if (msg == ps_float)
    {
        if (argc)
            outlet_float(x->message_out, atom_getfloat(argv));
        return;
    }
    
    outlet_anything(x->message_out, msg, static_cast<short>(argc), argv);
}

void schedulemessage_int(t_schedulemessage *x, t_atom_long value)
{
    t_atom out_atom;
    
    atom_setlong(&out_atom, value);
    
    schedulemessage_anything(x, ps_int, 1, &out_atom);
}

void schedulemessage_float(t_schedulemessage *x, double float_in)
{
    t_atom out_atom;
    
    atom_setfloat(&out_atom, float_in);
    
    schedulemessage_anything(x, ps_float, 1, &out_atom);
}

void schedulemessage_bang(t_schedulemessage *x)
{
    schedulemessage_anything(x, ps_bang, 0, 0);
}

void schedulemessage_anything(t_schedulemessage *x, t_symbol *msg, long argc, t_atom *argv)
{
    double delay = x->delay;
    
    if (!isr() || delay > 0)
        schedule_fdelay(x, (method) schedulemessage_output, delay, msg, static_cast<short>(argc), argv);
    else
        schedulemessage_output(x, msg, argc, argv);
}

void schedulemessage_delay(t_schedulemessage *x, double delay)
{
    x->delay = delay;
}

// Assist

void schedulemessage_assist(t_schedulemessage *x, void *b, long m, long a, char *s)
{
    static constexpr int maxAssist = 256;

    if (m == ASSIST_OUTLET)
    {
        snprintf(s, maxAssist, "Scheduled Message");
    }
    else
    {
        switch (a)
        {
            case 0:
                snprintf(s, maxAssist, "Input (anything)");
                break;
                
            case 1:
                snprintf(s, maxAssist, "Set Delay Time In Milliseconds");
                break;
        }
    }
}
