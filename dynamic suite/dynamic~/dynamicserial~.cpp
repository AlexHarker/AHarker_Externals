
/*
 *  dynamicserial~
 *
 *  dynamicserial~ is a serial audio processing object for dynamically loading / managing patches.
 *  Loading patches does not affect other audio patches that are processing.
 *  It is similar to dynamicdsp~, but is designed for serial processing (hence it isn't multithreaded).
 *
 *  There is an associated set of objects for audio input / output and querying and setting patch state:
 *  (dynamic.in~ / dynamic.out~ / dynamic.request~ / dynamic.this~ / dynamic.patch~).
 *  The standard in / out objects can be used for message IO.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>
#include <ext_wind.h>
#include <jpatcher_api.h>

#include <SIMDSupport.hpp>

#include "Common/patch_set.hpp"
#include "Common/dynamic_host.hpp"


// Global Variables

t_class *this_class;

t_symbol *ps_args = gensym("args");
t_symbol *ps_declareio = gensym("declareio");

static t_ptr_uint sig_size;

constexpr long max_args = patch_slot::max_args();

// Object Structure

struct t_dynamicserial
{
    t_pxobject x_obj;
    
    t_patcher *parent_patch;
    
    // Patch Data and Variables
    
    serial_patch_set *patch_set;
    
    long last_vec_size;
    long last_samp_rate;
    
    // IO Variables
    
    long num_sig_ins;
    long num_sig_outs;
    long num_ins;
    long num_outs;
    long num_temp_buffers;
    
    void **sig_object_ins;
    void **sig_outs;
    
    void **sig_ins;
    void **temp_buffers1;
    void **temp_buffers2;
    
    long num_proxies;                // number of proxies = MAX(num_sig_ins, num_ins)
};

// Function Prototypes

void *dynamicserial_new(t_symbol *s, long argc, t_atom *argv);
void dynamicserial_free(t_dynamicserial *x);
void dynamicserial_assist(t_dynamicserial *x, void *b, long m, long a, char *s);

void dynamicserial_loadpatch(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv);

void dynamicserial_perform_common(t_dynamicserial *x);
void dynamicserial_perform_denormal_handled(t_dynamicserial *x);
t_int *dynamicserial_perform(t_int *w);
void dynamicserial_perform64(t_dynamicserial *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

bool dynamicserial_dsp_common(t_dynamicserial *x, long vec_size, long sample_rate);
void dynamicserial_dsp(t_dynamicserial *x, t_signal **sp, short *count);
void dynamicserial_dsp64(t_dynamicserial *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags);

// Main

int C74_EXPORT main()
{
    using handler = dynamic_host<t_dynamicserial>;
    
    this_class = class_new("dynamicserial~",
                           (method) dynamicserial_new,
                           (method) dynamicserial_free,
                           sizeof(t_dynamicserial),
                           (method) nullptr,
                           A_GIMME,
                           0);
    
    class_addmethod(this_class, (method) dynamicserial_dsp, "dsp", A_CANT, 0);
    class_addmethod(this_class, (method) dynamicserial_dsp64, "dsp64", A_CANT, 0);
    
    class_addmethod(this_class, (method) dynamicserial_assist, "assist", A_CANT, 0);
    
    class_addmethod(this_class, (method) handler::open, "open", A_DEFLONG, 0);
    class_addmethod(this_class, (method) handler::dblclick, "dblclick", A_CANT, 0);
    class_addmethod(this_class, (method) handler::wclose, "wclose", A_DEFLONG, 0);
    
    class_addmethod(this_class, (method) handler::pupdate, "pupdate", A_CANT, 0);
    class_addmethod(this_class, (method) handler::subpatcher, "subpatcher", A_CANT, 0);
    class_addmethod(this_class, (method) handler::parentpatcher, "parentpatcher", A_CANT, 0);
    class_addmethod(this_class, (method) handler::getindex, "getindex", A_CANT, 0);
    
    class_addmethod(this_class, (method) handler::msgbang, "bang", 0);
    class_addmethod(this_class, (method) handler::msgint, "int", A_LONG, 0);
    class_addmethod(this_class, (method) handler::msgfloat, "float", A_FLOAT, 0);
    class_addmethod(this_class, (method) handler::msglist, "list", A_GIMME, 0);
    class_addmethod(this_class, (method) handler::msganything, "anything", A_GIMME, 0);
    
    class_addmethod(this_class, (method) dynamicserial_loadpatch, "loadpatch", A_GIMME, 0);
    class_addmethod(this_class, (method) handler::clear, "clear", 0);
    class_addmethod(this_class, (method) handler::deletepatch, "deletepatch", A_GIMME, 0);                        // MUST FIX TO GIMME FOR NOW

    class_addmethod(this_class, (method) handler::targetfree, "targetfree", A_GIMME, 0);                       // MUST FIX TO GIMME FOR NOW
    
    class_addmethod(this_class, (method) handler::loading_index, "loading_index", A_CANT, 0);
    class_addmethod(this_class, (method) handler::register_listener, "register_listener", A_CANT, 0);
    class_addmethod(this_class, (method) handler::unregister_listener, "unregister_listener", A_CANT, 0);
    class_addmethod(this_class, (method) handler::query_num_sigins, "query_num_sigins", A_CANT, 0);
    class_addmethod(this_class, (method) handler::query_num_sigouts, "query_num_sigouts", A_CANT, 0);
    class_addmethod(this_class, (method) handler::query_sigins, "query_sigins", A_CANT, 0);
    class_addmethod(this_class, (method) handler::query_sigouts, "query_sigouts", A_CANT, 0);
    class_addmethod(this_class, (method) handler::client_get_patch_on, "client_get_patch_on", A_CANT, 0);
    class_addmethod(this_class, (method) handler::client_get_patch_busy, "client_get_patch_busy", A_CANT, 0);
    class_addmethod(this_class, (method) handler::client_set_patch_on, "client_set_patch_on", A_CANT, 0);
    class_addmethod(this_class, (method) handler::client_set_patch_busy, "client_set_patch_busy", A_CANT, 0);
    
    CLASS_ATTR_OFFSET_DUMMY(this_class, "target", 0L, gensym("atom_long"));
    CLASS_ATTR_ACCESSORS(this_class, "target", handler::get_target, handler::target);
    CLASS_ATTR_LABEL(this_class, "target", 0L, "Target Patch Number");
    
    CLASS_ATTR_OFFSET_DUMMY(this_class, "ownsdspchain", ATTR_SET_OPAQUE | ATTR_SET_OPAQUE_USER, gensym("long"));
    CLASS_ATTR_ACCESSORS(this_class, "ownsdspchain", (method) patchset_get_ownsdspchain, (method) 0);
    CLASS_ATTR_INVISIBLE(this_class, "ownsdspchain", 0);
    
    class_dspinit(this_class);
    
    class_register(CLASS_BOX, this_class);
        
    sig_size = ((maxversion() & 0x3FFF) >= 0x600) ? sizeof(double) : sizeof(float);
    
    return 0;
}

// New / Free / Assist

void *dynamicserial_new(t_symbol *s, long argc, t_atom *argv)
{
    constexpr long max_io = 256;
    
    t_dynamicserial *x = (t_dynamicserial *) object_alloc(this_class);
    
    t_symbol *patch_name_entered = nullptr;
    t_symbol *tempsym;
    
    void *outs[max_io];
    long ac = 0;
    t_atom av[max_args];
    
    long num_sig_ins = 2;
    long num_sig_outs = 2;
    long num_ins = 2;
    long num_outs = 2;
    long num_temp_buffers = 0;
    
    auto set_io = [&](t_atom *arg)
    {
        t_atom_long N = atom_getlong(arg);
        t_atom_long result = (N < 0 ? 2 : (N > max_io ? max_io : N));
        
        return static_cast<long>(result);
    };
    
    // Check if there is a patch name given to load
    
    if (argc && atom_gettype(argv) == A_SYM && atom_getsym(argv) != ps_declareio)
    {
        patch_name_entered = atom_getsym(argv);
        argc--; argv++;
    }
    
    // Check for the declareio symbol and ignore to maintain old style functionality
    
    if (argc && atom_gettype(argv) == A_SYM && atom_getsym(argv) == ps_declareio)
    {
        argc--; argv++;
    }
    
    // Check if there is a declaration of the number of inputs and outs (message and signal)
    
    if (argc && atom_gettype(argv) == A_LONG)
    {
        num_sig_ins = set_io(argv);
        argc--; argv++;
    }
    if (argc && atom_gettype(argv) == A_LONG)
    {
        num_sig_outs = set_io(argv);
        argc--; argv++;
    }
    if (argc && atom_gettype(argv) == A_LONG)
    {
        num_ins = set_io(argv);
        argc--; argv++;
    }
    if (argc && atom_gettype(argv) == A_LONG)
    {
        num_outs = set_io(argv);
        argc--; argv++;
    }
    
    // Get arguments to patch that is being loaded if there are any
    
    if (argc && atom_gettype(argv) == A_SYM)
    {
        tempsym = atom_getsym(argv);
        argc--; argv++;
        if (tempsym == ps_args)
        {
            ac = std::min(argc, max_args);
            
            for (long i = 0; i < ac; i++, argv++)
                av[i] = *argv;
        }
    }
    
    // Set other variables to defaults
    
    x->num_sig_ins = num_sig_ins;
    x->num_sig_outs = num_sig_outs;
    
    if (num_sig_ins > num_sig_outs)
        num_temp_buffers = num_sig_ins;
    else
        num_temp_buffers = num_sig_outs;
    
    x->num_temp_buffers = num_temp_buffers;
    
    x->num_ins = num_ins;
    x->num_outs = num_outs;
    
    x->last_vec_size = 64;
    x->last_samp_rate = 44100;
    
    // Create signal in/out buffers and zero
    
    // N.B. here sig_ins are those used for the patches (which are dynamic) and sig_object_ins are the object ins
    
    x->sig_object_ins = (void **) malloc(num_sig_ins * sizeof(void *));
    x->sig_outs = (void **) malloc(num_sig_outs * sizeof(void *));
    
    x->sig_ins = (void **) malloc(num_temp_buffers * sizeof(void *));
    x->temp_buffers1 = (void **) malloc(num_temp_buffers * sizeof(void *));
    x->temp_buffers2 = (void **) malloc(num_temp_buffers * sizeof(void *));
    
    for (long i = 0; i < num_sig_ins; i++)
        x->sig_object_ins[i] = nullptr;
    for (long i = 0; i < num_sig_outs; i++)
        x->sig_outs[i] = nullptr;
    for (long i = 0; i < num_temp_buffers; i++)
        x->sig_ins[i] = x->temp_buffers1[i] = x->temp_buffers2[i] = nullptr;
    
    // Make non-signal outlets first
    
    for (long i = num_outs - 1; i >= 0; i--)
        outs[i] = outlet_new((t_object *) x, nullptr);
    
    // Make signal ins
    
    x->num_proxies = (num_sig_ins > num_ins) ? num_sig_ins : num_ins;
    
    // All proxies (signals or messages must be declared as if signal inlets
    
    dsp_setup((t_pxobject *) x, x->num_proxies);
    x->x_obj.z_misc = Z_NO_INPLACE;
    
    // Make signal outs
    
    for (long i = 0; i < num_sig_outs; i++)
        outlet_new((t_object *) x, "signal");
    
    // Get parent patcher
    
    x->parent_patch = (t_patcher *)gensym("#P")->s_thing;
    
    // Setup slots
    
    x->patch_set = new serial_patch_set((t_object *) x, x->parent_patch, num_ins, num_outs, outs);
    
    // Load patch
    
    if (patch_name_entered)
        x->patch_set->load(0, patch_name_entered, ac, av, x->last_vec_size, x->last_samp_rate);
    
    return x;
}

void dynamicserial_free(t_dynamicserial *x)
{
    dsp_free((t_pxobject *) x);
    
    // Free patches
    
    delete x->patch_set;
    
    // Free temp buffers
    
    if (x->num_temp_buffers)
    {
        for (long i = 0; i < x->num_temp_buffers; i++)
        {
            deallocate_aligned(x->temp_buffers1[i]);
            deallocate_aligned(x->temp_buffers2[i]);
        }
    }
    
    // Free buffer handles
    
    if (x->num_sig_ins)
        free(x->sig_object_ins);
    if (x->num_sig_outs)
        free(x->sig_outs);
    if (x->sig_ins)
        free(x->sig_ins);
    if (x->temp_buffers1)
        free(x->temp_buffers1);
    if (x->temp_buffers2)
        free(x->temp_buffers2);
}

void dynamicserial_assist(t_dynamicserial *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
    {
        if (a <    x->num_sig_outs)
            sprintf(s,"Signal Out %ld", a + 1);
        else
            sprintf(s,"Message Out %ld", a - x->num_sig_outs + 1);
    }
    else
        sprintf(s,"Signal / Message In %ld", a + 1);
}

// Patch Loading

void dynamicserial_loadpatch(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv)
{
    t_symbol *patch_name = nullptr;
    t_atom_long index = 0;
    
    // Get requested patch index if there is one
    
    if (argc && atom_gettype(argv) == A_LONG)
    {
        index = atom_getlong(argv);
        if (index < 1)
        {
            object_error((t_object *) x, "patch index out of range");
            return;
        }
        argc--; argv++;
    }
    
    // If there is a patch name then try to load the patch
    
    if (argc && atom_gettype(argv) == A_SYM)
    {
        patch_name = atom_getsym(argv);
        argc--; argv++;
        
        x->patch_set->load(index, patch_name, argc, argv, x->last_vec_size, x->last_samp_rate);
    }
    else
        object_error((t_object *) x, "no patch specified");
}

// Perform Routines

void dynamicserial_perform_common(t_dynamicserial *x)
{
    void **sig_object_ins = x->sig_object_ins;
    void **sig_ins = x->sig_ins;
    void **sig_outs = x->sig_outs;
    void **temp1 = x->temp_buffers1;
    void **temp2 = x->temp_buffers2;
    
    long num_ins = x->num_sig_ins;
    long num_outs = x->num_sig_outs;
    long num_temps = x->num_temp_buffers;
    long vec_size = x->last_vec_size;
        
    size_t buffer_size = sig_size * vec_size;
    
    // Process
    
    x->patch_set->process_serial(sig_outs, sig_object_ins, sig_ins, temp1, temp2, num_ins, num_outs, num_temps, buffer_size);
}

t_int *dynamicserial_perform(t_int *w)
{
    if (!((t_dynamicserial *) (w[1]))->x_obj.z_disabled)
        dynamicserial_perform_common((t_dynamicserial *) w[1]);
    
    return w + 2;
}

void dynamicserial_perform64(t_dynamicserial *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    memcpy(x->sig_object_ins, ins, x->num_sig_ins * sizeof(void *));
    memcpy(x->sig_outs, outs, x->num_sig_ins * sizeof(void *));
    
    dynamicserial_perform_common(x);
}

// DSP

bool dynamicserial_dsp_common(t_dynamicserial *x, long vec_size, long sample_rate)
{
    bool mem_fail = false;
    
    // Do internal dsp compile
    
    x->patch_set->compile_dsp(vec_size, sample_rate);
    
    for (long i = 0; i < x->num_temp_buffers; i++)
    {
        deallocate_aligned(x->temp_buffers1[i]);
        deallocate_aligned(x->temp_buffers2[i]);
        
        x->temp_buffers1[i] = allocate_aligned<uint8_t>(sig_size * vec_size);
        x->temp_buffers2[i] = allocate_aligned<uint8_t>(sig_size * vec_size);
        
        if (!x->temp_buffers1[i] || !x->temp_buffers2[i])
            mem_fail = true;
    }
    
    x->last_vec_size = vec_size;
    x->last_samp_rate = sample_rate;
    
    return mem_fail;
}

void dynamicserial_dsp(t_dynamicserial *x, t_signal **sp, short *count)
{
    // Copy in and out pointers (note that all inlets are declared as if signals)
    
    for (long i = 0; i < x->num_sig_ins; i++)
        x->sig_object_ins[i] = sp[i]->s_vec;
    for (long i = 0; i < x->num_sig_outs; i++)
        x->sig_outs[i] = sp[i + x->num_proxies]->s_vec;
    
    // Add to dsp if common routine successful
    
    if (!dynamicserial_dsp_common(x, sp[0]->s_n, static_cast<long>(sp[0]->s_sr)))
        dsp_add(dynamicserial_perform, 1, x);
}

void dynamicserial_dsp64(t_dynamicserial *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags)
{
    // Add to dsp if common routine successful
    
    if (!dynamicserial_dsp_common(x, max_vec, static_cast<long>(sample_rate)))
        object_method(dsp64, gensym("dsp_add64"), x, dynamicserial_perform64, 0, nullptr);
}
