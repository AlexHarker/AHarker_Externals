
/*
 *  dynamicdsp~
 *
 *	dynamicdsp~ is a multithreaded audio processing object for dynamically loading and managing audio (or non-audio) patches in realtime, without affecting other patches that are processing.
 *	
 *	It can be thought of as a poly~ alternative, with more extensive multithreading features than the Max version.
 *	There is an associated set of objects for audio input / output and querying and setting patch state (dynamic.in~ / dynamic.out~ / dynamic.request~ / dynamic.this~ / dynamic.patch~).
 *	The voice allocation system is somewhat different to poly~.
 *
 *	See the helpfile documentation for further details of functionality.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>
#include <ext_wind.h>
#include <jpatcher_api.h>

#include <dynamic~.hpp>
#include "PatchSlot.hpp"
#include "PatchSet.hpp"
#include "ThreadSet.hpp"

#include "dynamic_host.hpp"

// TODO - check all poly CANT methods
// TODO - change some items to attributes

// FIX - threadsafety around alterations to the patch set vector

// TODO - Share threads between objects
// TODO - use an atomic counter for autoloadbalance to decrease thread sync costs??
// FIX - It seems I should clean up the threads better here / improve threading mechanisms further

// FIX - potential adc~ crashes / no audio - cannot get traction on this

// TODO - allow patch crossfading
// TODO - patch serialisation

// Global Varibles

t_class *this_class;

t_symbol *ps_args;
t_symbol *ps_declareio;

static t_ptr_uint sig_size;

constexpr int MAX_ARGS = 16;
constexpr int MAX_IO = 256;

// Object Structure

struct t_dynamicdsp
{
    t_pxobject x_obj;
    
	t_patcher *parent_patch;
	
	long last_vec_size;
	long last_samp_rate;
	
	// IO Variables
	
	long num_sig_ins;
	long num_sig_outs;
	long num_ins;
	long num_outs;
	
	void **sig_ins;
	void **sig_outs;
	
	long num_proxies;				// number of proxies = MAX(num_sig_ins, num_ins)
	
	// Multithreading Variables
		
	long request_num_active_threads;
	long num_active_threads;
	
    long multithread_flag;
	long request_manual_threading;
	long manual_threading;
	long update_thread_map;
	
	long max_obj_threads;
	
    // Thread Data / Patches
    
    ThreadSet *threads;
    ThreadedPatchSet *patch_set;
};

// Function Prototypes

void *dynamicdsp_new(t_symbol *s, long argc, t_atom *argv);
void dynamicdsp_free(t_dynamicdsp *x);
void dynamicdsp_assist(t_dynamicdsp *x, void *b, long m, long a, char *s);

void dynamicdsp_loadpatch(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv);

void dynamicdsp_autoloadbalance(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);
void dynamicdsp_multithread(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);
void dynamicdsp_activethreads(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);
void dynamicdsp_threadmap(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);

static __inline void dynamicdsp_multithread_perform(t_dynamicdsp *x, void **sig_outs, long vec_size, long num_active_threads);
void dynamicdsp_threadprocess(t_dynamicdsp *x, void **sig_outs, long vec_size, long thread_num, long num_active_threads);
void dynamicdsp_perform_common(t_dynamicdsp *x, void **sig_outs, long vec_size);
t_int *dynamicdsp_perform(t_int *w);
void dynamicdsp_perform64(t_dynamicdsp *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

bool dynamicdsp_dsp_common(t_dynamicdsp *x, long vec_size, long samp_rate);
void dynamicdsp_dsp(t_dynamicdsp *x, t_signal **sp, short *count);
void dynamicdsp_dsp64(t_dynamicdsp *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

// Main

long poly_isparent(t_object *p, t_object *mightbeparent)
{
    t_object *pp = p;
    
    do {
        object_method(pp, gensym("getvnewobjparent"), &pp);
        if (pp == mightbeparent)
            return true;
    } while (pp);
    
    return false;
}

void poly_appendinstanceifneeded(char *buf, char *name, long instance)
{
    long len;
    char seeninstance = false;
    
    len = strlen(name);
    // is last character of name a right paren?
    if (len > 3 && name[len - 1] == ')') {
        // look for pattern, digits until left paren
        char seendigit = true;
        long i;
        
        for (i = len - 2; i >= 0; i--) {
            if (isdigit(name[i])) {
                seendigit = true;
            } else {
                if (!seendigit)
                    break;
                if (name[i] == '(') {
                    seeninstance = true;
                    break;
                }
            }
        }
    }
    if (seeninstance)
        strcpy(buf, name);
    else
        sprintf(buf, "%s (%ld)", name, instance);
}

void poly_titleassoc(t_dynamicdsp *x, t_object *p, char **title)
{
    long i;
    t_symbol *name;
    char buf[1024];
    char subpatcher = false;
    
    *title = NULL;
    
    for (i = 0; i < x->patch_set->size(); i++)
    {
        const t_patcher *pp = x->patch_set->subpatch(i, x);
        if (p == pp || (subpatcher = poly_isparent(p, (t_object*)pp)))
        {
            object_method(p, gensym("getname"), &name);
            poly_appendinstanceifneeded(buf, name->s_name, i + 1);
            *title = sysmem_newptr(strlen(buf)+1);
            strcpy(*title,buf);
            return;
        }
    }
    // got here? it's ok, conventional title will be used
}


int C74_EXPORT main()
{
    using handler = DynamicHost<t_dynamicdsp>;

	this_class = class_new("dynamicdsp~",
								 (method)dynamicdsp_new, 
								 (method)dynamicdsp_free, 
								 sizeof(t_dynamicdsp), 
								 NULL, 
								 A_GIMME, 
								 0);
	
	class_addmethod(this_class, (method)dynamicdsp_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)dynamicdsp_dsp64, "dsp64", A_CANT, 0);
	
	class_addmethod(this_class, (method)dynamicdsp_assist, "assist", A_CANT, 0);
    
    class_addmethod(this_class, (method)handler::open, "open", A_DEFLONG, 0);
	class_addmethod(this_class, (method)handler::dblclick, "dblclick", A_CANT, 0);
	class_addmethod(this_class, (method)handler::wclose, "wclose", A_DEFLONG, 0);
	
	class_addmethod(this_class, (method)handler::pupdate, "pupdate", A_CANT, 0);
	class_addmethod(this_class, (method)handler::subpatcher, "subpatcher", A_CANT, 0);
	class_addmethod(this_class, (method)handler::parentpatcher, "parentpatcher", A_CANT, 0);
    class_addmethod(this_class, (method)handler::getindex, "getindex", A_CANT, 0);

	class_addmethod(this_class, (method)handler::msgbang, "bang", 0);
	class_addmethod(this_class, (method)handler::msgint, "int", A_LONG, 0);
	class_addmethod(this_class, (method)handler::msgfloat, "float", A_FLOAT, 0);
	class_addmethod(this_class, (method)handler::msglist, "list", A_GIMME, 0);
	class_addmethod(this_class, (method)handler::msganything, "anything", A_GIMME, 0);
	
	class_addmethod(this_class, (method)dynamicdsp_autoloadbalance, "autoloadbalance", A_GIMME, 0);				// MUST FIX TO GIMME FOR NOW
	class_addmethod(this_class, (method)dynamicdsp_multithread, "multithread", A_GIMME, 0);						// MUST FIX TO GIMME FOR NOW
	class_addmethod(this_class, (method)dynamicdsp_activethreads, "activethreads", A_GIMME, 0);					// MUST FIX TO GIMME FOR NOW
	class_addmethod(this_class, (method)dynamicdsp_threadmap, "threadmap", A_GIMME, 0);							// MUST FIX TO GIMME FOR NOW
	
	class_addmethod(this_class, (method)handler::clear, "clear", 0);
	class_addmethod(this_class, (method)dynamicdsp_loadpatch, "loadpatch", A_GIMME, 0);
	class_addmethod(this_class, (method)handler::deletepatch, "deletepatch", A_GIMME, 0);						// MUST FIX TO GIMME FOR NOW
    
	class_addmethod(this_class, (method)handler::target, "target", A_GIMME, 0);                                 // MUST FIX TO GIMME FOR NOW
	class_addmethod(this_class, (method)handler::targetfree, "targetfree", A_GIMME, 0);                         // MUST FIX TO GIMME FOR NOW
	
    class_addmethod(this_class, (method)handler::loading_index, "loading_index", A_CANT, 0);
    class_addmethod(this_class, (method)handler::register_listener, "register_listener", A_CANT, 0);
    class_addmethod(this_class, (method)handler::unregister_listener, "unregister_listener", A_CANT, 0);
    class_addmethod(this_class, (method)handler::query_num_sigins, "query_num_sigins", A_CANT, 0);
	class_addmethod(this_class, (method)handler::query_num_sigouts, "query_num_sigouts", A_CANT, 0);
	class_addmethod(this_class, (method)handler::query_sigins, "query_sigins", A_CANT, 0);
	class_addmethod(this_class, (method)handler::query_sigouts, "query_sigouts", A_CANT, 0);
	class_addmethod(this_class, (method)handler::client_get_patch_on, "client_get_patch_on", A_CANT, 0);
	class_addmethod(this_class, (method)handler::client_get_patch_busy, "client_get_patch_busy", A_CANT, 0);
	class_addmethod(this_class, (method)handler::client_set_patch_on, "client_set_patch_on", A_CANT, 0);
	class_addmethod(this_class, (method)handler::client_set_patch_busy, "client_set_patch_busy", A_CANT, 0);
	
    CLASS_ATTR_OBJ(this_class, "ownsdspchain", ATTR_SET_OPAQUE | ATTR_SET_OPAQUE_USER, t_dynamicdsp, x_obj);
    CLASS_ATTR_ACCESSORS(this_class, "ownsdspchain", (method) patchset_get_ownsdspchain, (method) 0);
    CLASS_ATTR_INVISIBLE(this_class, "ownsdspchain", 0);
    
    class_addmethod(this_class, (method)poly_titleassoc, "titleassoc", A_CANT, 0);

	class_dspinit(this_class);
	
	class_register(CLASS_BOX, this_class);
		
	ps_args = gensym("args");
	ps_declareio = gensym("declareio");
	
	sig_size = ((maxversion() & 0x3FFF) >= 0x600) ? sizeof(double) : sizeof(float);
	
	return 0;
}

// New / Free / Assisstance

void *dynamicdsp_new(t_symbol *s, long argc, t_atom *argv)
{	
	t_dynamicdsp *x = (t_dynamicdsp *)object_alloc(this_class);
	
	t_symbol *patch_name_entered = NULL;
	t_symbol *tempsym;
	
	long ac = 0;
	t_atom av[MAX_ARGS];						
	
    void *outs[MAX_IO];
    
	long num_sig_ins = 2;
	long num_sig_outs = 2;
	long num_ins = 2;
	long num_outs = 2;	
    long max_obj_threads = NumProcessors()();
	
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
		if (atom_getlong(argv) >= 0 && atom_getlong(argv) < MAX_IO)
			num_sig_ins = atom_getlong(argv);
		argc--; argv++;
	}
	if (argc && atom_gettype(argv) == A_LONG)
	{
		if (atom_getlong(argv) >= 0 && atom_getlong(argv) < MAX_IO)
			num_sig_outs = atom_getlong(argv);
		argc--; argv++;
	}
	if (argc && atom_gettype(argv) == A_LONG)
	{
		if (atom_getlong(argv) >= 0 && atom_getlong(argv) < MAX_IO)
			num_ins = atom_getlong(argv);
		argc--; argv++;
	}
	if (argc && atom_gettype(argv) == A_LONG)
	{
		if (atom_getlong(argv) >= 0 && atom_getlong(argv) < MAX_IO)
			num_outs = atom_getlong(argv);
		argc--; argv++;
	}
	if (argc && atom_gettype(argv) == A_LONG)
	{
        if (atom_getlong(argv) < max_obj_threads)
            max_obj_threads = atom_getlong(argv);
        if (max_obj_threads < 1)
			max_obj_threads = 1;
		argc--; argv++;
	}
	
	x->max_obj_threads = max_obj_threads;
	
	// Get arguments to patch that is being loaded if there are any
	
	if (argc && atom_gettype(argv) == A_SYM) 
	{
		tempsym = atom_getsym(argv);
		argc--; argv++;
		if (tempsym == ps_args) 
		{				
			ac = argc;
			if (ac > MAX_ARGS)
				ac = MAX_ARGS;
			for (long i = 0; i < ac; i++, argv++)
				av[i] = *argv;
		}
	}
	
	// Multithreading Setup - defaults to multi-threading off for nested objects, on for non-nested
	
	if (dynamic_get_parent()) 
		x->multithread_flag = 0;									
	else 
		x->multithread_flag = 1;

	// Multithreading variables
	
	x->manual_threading = 1;
	x->request_manual_threading = 1;
	x->request_num_active_threads = max_obj_threads;
	
	// Set other variables to defaults
	
	x->num_sig_ins = num_sig_ins;
	x->num_sig_outs = num_sig_outs;
	x->num_ins = num_ins;
	x->num_outs = num_outs;
	
	x->update_thread_map = 0;
	
	x->last_vec_size = 64;
	x->last_samp_rate = 44100;
	
	// Create signal in/out buffers and zero
	
	x->sig_ins = (void **) malloc(num_sig_ins * sizeof(void *));
	x->sig_outs = (void **) malloc(num_sig_outs * sizeof(void *));
	
	for (long i = 0; i < num_sig_ins; i++)
		x->sig_ins[i] = NULL;
	for (long i = 0; i < num_sig_outs; i++)
		x->sig_outs[i] = NULL;
	
	// Make non-signal outlets first
	
    for (long i = num_outs - 1; i >= 0; i--)
        outs[i] = outlet_new((t_object *)x, NULL);
    
	// Make signal ins
	
	x->num_proxies = (num_sig_ins > num_ins) ? num_sig_ins : num_ins;
	
	dsp_setup((t_pxobject *) x, x->num_proxies);
	x->x_obj.z_misc = Z_NO_INPLACE;                                                             // due to output zeroing!!
	
	// Make signal outs
	
	for (long i = 0; i < num_sig_outs; i++)
		outlet_new((t_object *)x, "signal");
	
    // Get parent patcher
    
    x->parent_patch = (t_patcher *)gensym("#P")->s_thing;										// store reference to parent patcher
    
    // Setup temporary memory / threads / slots
	
    x->threads = new ThreadSet((t_object *) x, reinterpret_cast<ThreadSet::procFunc *>(&dynamicdsp_threadprocess), max_obj_threads, num_sig_outs);
    x->patch_set = new ThreadedPatchSet((t_object *)x, x->parent_patch, num_ins, num_outs, outs);
	
	// Load patch
	
	if (patch_name_entered)
        x->patch_set->load(0, patch_name_entered, ac, av, x->last_vec_size, x->last_samp_rate);
	
	return x;
}

void dynamicdsp_free(t_dynamicdsp *x)
{
	dsp_free((t_pxobject *)x);
	
	// Free threads / patches

    delete x->threads;
	delete x->patch_set;
	
	// Free other resources
	
	if (x->num_sig_ins)
		free(x->sig_ins);
	if (x->num_sig_outs)
		free(x->sig_outs);
}

void dynamicdsp_assist(t_dynamicdsp *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_OUTLET)
	{
		if (a <	x->num_sig_outs)
			sprintf(s,"Signal Out %ld", a + 1); 
		else
			sprintf(s,"Message Out %ld", a - x->num_sig_outs + 1); 
	}
	else 
		sprintf(s,"Signal / Message In %ld", a + 1);
}

// Patcher Loading

void dynamicdsp_loadpatch(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv)
{
    t_symbol *patch_name = NULL;
    t_atom_long index = 0;
	t_atom_long thread_request = -1;
	
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
	
	// Get thread request if there is one
	
	if (argc && atom_gettype(argv) == A_LONG)
	{
		thread_request = atom_getlong(argv) - 1;
		argc--; argv++;
	}
	
	// If there is a patch name then try to load the patch
	
	if (argc && atom_gettype(argv) == A_SYM)			
	{
		patch_name = atom_getsym(argv);
		argc--; argv++;
        
        index = x->patch_set->load(index, patch_name, argc, argv, x->last_vec_size, x->last_samp_rate);
        
        // FIX - threading...
        // FIX - review
        
        if (thread_request && index >= 0)
        {
            if (thread_request > 0)
                x->patch_set->requestThread(index, thread_request);
            else
                x->patch_set->requestThread(index, index);
            
            x->update_thread_map = 1;
        }
	} 
	else 
		object_error((t_object *) x, "no patch specified");
}

// Multithreading Messages

void dynamicdsp_autoloadbalance(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv)
{
	x->request_manual_threading = !(!argc || atom_getlong(argv)) ? 1 : 0;
}

void dynamicdsp_multithread(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv)
{
    x->multithread_flag = (!argc || atom_getlong(argv)) ? 1 : 0;
}

void dynamicdsp_activethreads(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv)
{
	t_atom_long n = x->max_obj_threads;
	
	if (argc)
    {
		n = atom_getlong(argv);
        n = (n < 1) ? 1 : ((n > x->max_obj_threads) ? x->max_obj_threads : n);
    }
	
	x->request_num_active_threads = n;
}

void dynamicdsp_threadmap(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv)
{
	t_atom_long index = -1;
	t_atom_long thread_request = -1;
	
	// Load arguments (and alter index by one to change counting point)
	
	if (argc > 0)
		index = atom_getlong(argv + 0) - 1;
		
    // FIX - how to check the range of this value (N.B. different hardware may exhibit different numbers of max threads)
    
	if (argc > 1)
		thread_request = atom_getlong(argv + 1) - 1;
    
	if (thread_request >= 0)
        x->patch_set->requestThread(index, thread_request);
    else
        x->patch_set->requestThread(index, index);
    
    x->update_thread_map = 1;
}

// Perform Routines

template <typename T>
void dynamicdsp_sum(ThreadSet *threads, void **sig_outs, long num_sig_outs, long vec_size, long num_active_threads)
{
    const long max_simd_size = SIMDLimits<T>::max_size;
    
    // Sum output of threads for each signal outlet
    
    for (long i = 0; i < num_sig_outs; i++)
    {
        for (long j = 0; j < num_active_threads; j++)
        {
            T *io_pointer = (T *) sig_outs[i];
            T *next_sig_pointer = threads->getThreadBuffer<T>(j, i);
            
            if (next_sig_pointer)
            {
                if (vec_size < max_simd_size || t_ptr_uint(io_pointer) % 16)
                {
                    for (long k = 0; k < vec_size; k++)
                        *io_pointer++ += *next_sig_pointer++;
                }
                else
                {
                    using SIMD = SIMDType<T, max_simd_size>;
                    const long num_vecs = vec_size / max_simd_size;
                    SIMD *a = (SIMD *) io_pointer;
                    SIMD *b = (SIMD *) next_sig_pointer;
                    
                    for (long k = 0; k < num_vecs; k++)
                        *a++ += *b++;
                }
            }
        }
    }
}

static inline void dynamicdsp_multithread_perform(t_dynamicdsp *x, void **sig_outs, long vec_size, long num_active_threads)
{
    // Tick the threads and process in this thread (the main audio thread)
    
    x->threads->tick(vec_size, num_active_threads, sig_outs);
    
    if (num_active_threads > 1)
    {
        // Sum outputs
    
        if (sig_size == sizeof(float))
            dynamicdsp_sum<float>(x->threads, sig_outs, x->num_sig_outs, vec_size, num_active_threads);
        else
            dynamicdsp_sum<double>(x->threads, sig_outs, x->num_sig_outs, vec_size, num_active_threads);
    }
}

void dynamicdsp_threadprocess(t_dynamicdsp *x, void **sig_outs, long vec_size, long thread_num, long num_active_threads)
{
    long num_sig_outs = x->num_sig_outs;
    
    // Turn off denormals
    
#if defined( __i386__ ) || defined( __x86_64__ )
    int oldMXCSR = _mm_getcsr();						// read the old MXCSR setting
    _mm_setcsr(oldMXCSR | 0x8040);						// write the new MXCSR setting setting DAZ and FZ bits
#endif
    
    // Zero outputs
    
    for (long i = 0; i < num_sig_outs; i++)
        memset(sig_outs[i], 0, sig_size * vec_size);
    
    if (x->manual_threading)
    {
        for (long i = 1; i <= x->patch_set->size(); i++)
            x->patch_set->processIfThreadMatches(i, sig_outs, thread_num, num_active_threads);
    }
    else
    {
        long size = x->patch_set->size();
        long index = (thread_num * (size / num_active_threads));
        for (long i = 1; i <= size; i++)
        {
            if (++index > size)
                index -= size;
            
            x->patch_set->processIfUnprocessed(i, sig_outs);
        }
    }
    
    // return denormals to previous state 
    
#if defined( __i386__ ) || defined( __x86_64__ )	
    _mm_setcsr(oldMXCSR);	
#endif
}

void dynamicdsp_perform_common(t_dynamicdsp *x, void **sig_outs, long vec_size)
{
	long num_active_threads = x->request_num_active_threads;
    long multithread_flag = (x->patch_set->size() > 1) && x->multithread_flag;
	
	// Zero Outputs
	
	for (long i = 0; i < x->num_sig_outs; i++)
		memset(sig_outs[i], 0, sig_size * vec_size);
	
	// Update multithreading parameters (done in one thread and before processing to ensure uninterrupted audio processing
	
    x->num_active_threads = num_active_threads;
	x->manual_threading = x->request_manual_threading;		
    num_active_threads = !multithread_flag ? 1 : num_active_threads;
    
	if (!x->manual_threading)
        x->patch_set->resetProcessed();
	
	if (x->update_thread_map)
	{
		x->update_thread_map = 0;											
		x->patch_set->updateThreads();
	}
    
	// Do processing - the switch aims to get more speed from inlining a fixed loop size
	// N.B. - the single threaded case is handled as a special case in the ThreadSet class
    
	switch (num_active_threads)
    {                
        case 2:
            dynamicdsp_multithread_perform(x, sig_outs, vec_size, 2);
            break;
				
        case 3:
            dynamicdsp_multithread_perform(x, sig_outs, vec_size, 3);
            break;
				
        case 4:
            dynamicdsp_multithread_perform(x, sig_outs, vec_size, 4);
            break;
				
        case 5:
            dynamicdsp_multithread_perform(x, sig_outs, vec_size, 5);
            break;
				
        case 6:
            dynamicdsp_multithread_perform(x, sig_outs, vec_size, 6);
            break;
				
        case 7:
            dynamicdsp_multithread_perform(x, sig_outs, vec_size, 7);
            break;
				
        case 8:
            dynamicdsp_multithread_perform(x, sig_outs, vec_size, 8);
            break;
				
        default:
            dynamicdsp_multithread_perform(x, sig_outs, vec_size, num_active_threads);
            break;
	}
}

t_int *dynamicdsp_perform(t_int *w)
{	
	t_dynamicdsp *x = (t_dynamicdsp *) (w[1]);
	void **sig_outs = (void **) x->sig_outs;
	long vec_size = x->last_vec_size;
	
    if (!x->x_obj.z_disabled)
        dynamicdsp_perform_common(x, sig_outs, vec_size);
	
	return w + 2;	
}

void dynamicdsp_perform64(t_dynamicdsp *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
	for (long i = 0; i < x->num_sig_ins; i++)
		x->sig_ins[i] = ins[i];
		
	dynamicdsp_perform_common(x, (void **) outs, vec_size);
}

// DSP Routines

bool dynamicdsp_dsp_common(t_dynamicdsp *x, long vec_size, long samp_rate)
{	
    bool mem_fail = x->threads->resizeBuffers(vec_size * sig_size);
	
	// Do internal dsp compile (for each valid patch)
	
    x->patch_set->compileDSP(vec_size, samp_rate);
    
	x->last_vec_size = vec_size;
	x->last_samp_rate = samp_rate;
	
	return mem_fail;
}

void dynamicdsp_dsp(t_dynamicdsp *x, t_signal **sp, short *count)
{
	// Copy in and out pointers
	
	for (long i = 0; i < x->num_sig_ins; i++)
		x->sig_ins[i] = sp[i]->s_vec;
	for (long i = 0; i < x->num_sig_outs; i++)
		x->sig_outs[i] = sp[i + x->num_proxies]->s_vec;
	
	// Add to dsp if common routine successful
	
	if (!dynamicdsp_dsp_common(x, sp[0]->s_n, sp[0]->s_sr))
		dsp_add(dynamicdsp_perform, 1, x);
}

void dynamicdsp_dsp64(t_dynamicdsp *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	// Add to dsp if common routine successful
	
	if (!dynamicdsp_dsp_common(x, maxvectorsize, samplerate))
		object_method(dsp64, gensym("dsp_add64"), x, dynamicdsp_perform64, 0, NULL);
}
