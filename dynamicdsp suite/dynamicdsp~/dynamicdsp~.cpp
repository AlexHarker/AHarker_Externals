
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

#include <AH_Atomic.h>
#include <AH_Memory_Swap.hpp>

#include <dynamicdsp~.h>
#include "PatchSlot.h"
#include "PatchSet.h"
#include "ThreadSet.h"

// FIX - gen~ loading issue - workaround in place

// FIX - It seems I should clean up the threads better here / improve threading mechanisms further
// FIX - use an atomic counter for autoloadbalance to decrease thread sync costs??
// FIX - allow patch crossfading
// FIX - potential adc~ crashes / no audio - cannot get traction on this


// ======================================================================================================= //
// Global Varibles
// ======================================================================================================= //


t_class *dynamicdsp_class;

static t_ptr_uint sig_size;

#define MAX_ARGS 16
#define MAX_IO 256


// ======================================================================================================= //
// Structures and typedefs
// ======================================================================================================= //

// Patch Set Struct to Deal with Threads

struct ThreadedPatchSet : public PatchSet<ThreadedPatchSlot>
{
    ThreadedPatchSet(t_object *x, t_patcher *parent, long numIns, long numOuts, void **outs)
    : PatchSet(x, parent, numIns, numOuts, outs) {}
    
    bool processIfThreadMatches(long index, void *tempMem, void **outputs, t_ptr_uint tempMemSize, long thread, long nThreads)
    {
        if (!userSlotExists(index + 1))
            return false;
        
        return mSlots[index]->processIfThreadMatches(tempMem, outputs, tempMemSize, thread, nThreads);
    }
    
    bool processIfUnprocessed(long index, void *tempMem, void **outputs, t_ptr_uint tempMemSize)
    {
        if (!userSlotExists(index + 1))
            return false;
        
        return mSlots[index]->processIfUnprocessed(tempMem, outputs, tempMemSize);
    }
    
    void requestThread(long index, long thread)
    {
        if (userSlotExists(index + 1))
            mSlots[index]->requestThread(thread);
    }

    void resetProcessed()
    {
        for (auto it = mSlots.begin(); it != mSlots.end(); it++)
            if (*it) (*it)->resetProcessed();
    }
    
    void updateThreads()
    {
        for (auto it = mSlots.begin(); it != mSlots.end(); it++)
            if (*it) (*it)->updateThread();
    }
};

////////////////////////////////////// The object structure //////////////////////////////////////

typedef struct _dynamicdsp
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
	
    // Temporary Memory / Thread Data / Patches
    
	t_safe_mem_swap temp_mem;
    ThreadSet *threads;
    ThreadedPatchSet *slots;
    
} t_dynamicdsp;


// ======================================================================================================= //
// Function Prototypes
// ======================================================================================================= //


void *dynamicdsp_new(t_symbol *s, long argc, t_atom *argv);
void dynamicdsp_free(t_dynamicdsp *x);
void dynamicdsp_assist(t_dynamicdsp *x, void *b, long m, long a, char *s);

void dynamicdsp_deletepatch(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);
void dynamicdsp_clear(t_dynamicdsp *x);
void dynamicdsp_loadpatch(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv);

void dynamicdsp_bang(t_dynamicdsp *x);
void dynamicdsp_int(t_dynamicdsp *x, t_atom_long n);
void dynamicdsp_float(t_dynamicdsp *x, double f);
void dynamicdsp_list(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv);
void dynamicdsp_anything(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv);
void dynamicdsp_target(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);
void dynamicdsp_targetfree(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);

void dynamicdsp_autoloadbalance(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);
void dynamicdsp_multithread(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);
void dynamicdsp_activethreads(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);
void dynamicdsp_threadmap(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);

static __inline void dynamicdsp_multithread_perform(t_dynamicdsp *x, void **sig_outs, long vec_size, long num_active_threads);
void dynamicdsp_threadprocess(t_dynamicdsp *x, void **sig_outs, void *temp_mem_ptr, t_ptr_uint temp_mem_size, long vec_size, long thread_num, long num_active_threads);
void dynamicdsp_sum_float(ThreadSet *threads, void **sig_outs, long num_sig_outs, long vec_size, long num_active_threads);
void dynamicdsp_sum_double(ThreadSet *threads, void **sig_outs, long num_sig_outs, long vec_size, long num_active_threads);
void dynamicdsp_perform_common(t_dynamicdsp *x, void **sig_outs, long vec_size);
t_int *dynamicdsp_perform(t_int *w);
void dynamicdsp_perform64(t_dynamicdsp *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

bool dynamicdsp_dsp_common(t_dynamicdsp *x, long vec_size, long samp_rate);
void dynamicdsp_dsp(t_dynamicdsp *x, t_signal **sp, short *count);
void dynamicdsp_dsp64(t_dynamicdsp *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

void dynamicdsp_dblclick(t_dynamicdsp *x);
void dynamicdsp_open(t_dynamicdsp *x, t_atom_long index);
void dynamicdsp_pclose(t_dynamicdsp *x);
void dynamicdsp_wclose(t_dynamicdsp *x, t_atom_long index);

void dynamicdsp_pupdate(t_dynamicdsp *x, void *b, t_patcher *p);
void *dynamicdsp_subpatcher(t_dynamicdsp *x, long index, void *arg);
void dynamicdsp_parentpatcher(t_dynamicdsp *x, t_patcher **parent);

void *dynamicdsp_query_num_sigins(t_dynamicdsp *x);
void *dynamicdsp_query_num_sigouts(t_dynamicdsp *x);
void *dynamicdsp_query_sigins(t_dynamicdsp *x);
void *dynamicdsp_query_sigouts(t_dynamicdsp *x, long index);
void *dynamicdsp_client_get_patch_on(t_dynamicdsp *x, t_ptr_int index);
void *dynamicdsp_client_get_patch_busy(t_dynamicdsp *x, t_ptr_int index);
void dynamicdsp_client_set_patch_on(t_dynamicdsp *x, t_ptr_int index, t_ptr_int state);
void dynamicdsp_client_set_patch_busy(t_dynamicdsp *x, t_ptr_int index, t_ptr_int state);
void *dynamicdsp_query_temp_mem(t_dynamicdsp *x, t_ptr_int index);
void *dynamicdsp_client_temp_mem_resize(t_dynamicdsp *x, t_ptr_int index, t_ptr_uint size);


// ======================================================================================================= //
// Symbols
// ======================================================================================================= //


t_symbol *ps_args;
t_symbol *ps_declareio;


// ======================================================================================================= //
// Main
// ======================================================================================================= //

t_max_err patchset_get_patches(t_dynamicdsp *x, t_object *attr, long *argc, t_atom **argv)
{
    return x->slots->serialise(argc, argv);
}

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
    
    for (i = 0; i < x->slots->size(); i++)
    {
        const t_patcher *pp = x->slots->reportSubpatch(i, x);
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

t_atom_long dynamic_getindex(t_dynamicdsp *x, void *p)
{
    for (long i = 0; i < x->slots->size(); i++)
    {
        long index;
        const t_patcher *pp = x->slots->reportSubpatch(i, x, &index);
        if (pp == p)
            return index;
    }
    return -1;
}


int C74_EXPORT main()
{
	dynamicdsp_class = class_new("dynamicdsp~",
								 (method)dynamicdsp_new, 
								 (method)dynamicdsp_free, 
								 sizeof(t_dynamicdsp), 
								 NULL, 
								 A_GIMME, 
								 0);
	
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_dsp, "dsp", A_CANT, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_dsp64, "dsp64", A_CANT, 0);
	
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_assist, "assist", A_CANT, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_open, "open", A_DEFLONG, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_dblclick, "dblclick", A_CANT, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_wclose, "wclose", A_DEFLONG, 0);
	
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_pupdate, "pupdate", A_CANT, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_subpatcher, "subpatcher", A_CANT, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_parentpatcher, "parentpatcher", A_CANT, 0);
	
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_bang, "bang", 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_int, "int", A_LONG, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_float, "float", A_FLOAT, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_list, "list", A_GIMME, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_anything, "anything", A_GIMME, 0);
	
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_autoloadbalance, "autoloadbalance", A_GIMME, 0);				// MUST FIX TO GIMME FOR NOW
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_multithread, "multithread", A_GIMME, 0);						// MUST FIX TO GIMME FOR NOW
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_activethreads, "activethreads", A_GIMME, 0);					// MUST FIX TO GIMME FOR NOW
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_threadmap, "threadmap", A_GIMME, 0);							// MUST FIX TO GIMME FOR NOW
	
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_clear, "clear", 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_loadpatch, "loadpatch", A_GIMME, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_deletepatch, "deletepatch", A_GIMME, 0);						// MUST FIX TO GIMME FOR NOW
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_target, "target", A_GIMME, 0);                                 // MUST FIX TO GIMME FOR NOW
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_targetfree, "targetfree", A_GIMME, 0);                         // MUST FIX TO GIMME FOR NOW
	
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_query_num_sigins, "get_num_sigins", A_CANT, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_query_num_sigouts, "get_num_sigouts", A_CANT, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_query_sigins, "get_sigins", A_CANT, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_query_sigouts, "get_sigouts", A_CANT, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_client_get_patch_on, "get_patch_on", A_CANT, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_client_get_patch_busy, "get_patch_busy", A_CANT, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_client_set_patch_on, "set_patch_on", A_CANT, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_client_set_patch_busy, "set_patch_busy", A_CANT, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_query_temp_mem, "get_temp_mem", A_CANT, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_client_temp_mem_resize, "temp_mem_resize", A_CANT, 0);
	
    CLASS_ATTR_OBJ(dynamicdsp_class, "ownsdspchain", ATTR_SET_OPAQUE | ATTR_SET_OPAQUE_USER, t_dynamicdsp, x_obj);
    CLASS_ATTR_ACCESSORS(dynamicdsp_class, "ownsdspchain", (method) patchset_get_ownsdspchain, (method) 0);
    CLASS_ATTR_INVISIBLE(dynamicdsp_class, "ownsdspchain", 0);
    
    class_addmethod(dynamicdsp_class, (method)poly_titleassoc, "titleassoc", A_CANT, 0);
    class_addmethod(dynamicdsp_class, (method)dynamic_getindex, "getindex", A_CANT, 0);

/*
    CLASS_ATTR_OBJ(dynamicdsp_class, "patches", ATTR_SET_OPAQUE | ATTR_SET_OPAQUE_USER, t_dynamicdsp, x_obj);
    CLASS_ATTR_ACCESSORS(dynamicdsp_class, "patches", (method) patchset_get_patches, (method) 0);
    CLASS_ATTR_SAVE(dynamicdsp_class, "patches", 0);
    CLASS_ATTR_INVISIBLE(dynamicdsp_class, "patches", 0);*/

	class_dspinit(dynamicdsp_class);
	
	class_register(CLASS_BOX, dynamicdsp_class);
		
	ps_args = gensym("args");
	ps_declareio = gensym("declareio");
	
	sig_size = ((maxversion() & 0x3FFF) >= 0x600) ? sizeof(double) : sizeof(float);
	
	return 0;
}


// ======================================================================================================= //
// Object Creation / Freeing / Assisstance
// ======================================================================================================= //


void *dynamicdsp_new(t_symbol *s, long argc, t_atom *argv)
{	
	t_dynamicdsp *x = (t_dynamicdsp *)object_alloc(dynamicdsp_class);
	
	t_symbol *patch_name_entered = NULL;
	t_symbol *tempsym;
	
	long ac = 0;
	t_atom av[MAX_ARGS];						
	
    void *outs[MAX_IO];
    
	long num_sig_ins = 2;
	long num_sig_outs = 2;
	long num_ins = 2;
	long num_outs = 2;	
    long max_obj_threads = ThreadSet::getNumProcessors();
	
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
	
	if (Get_Dynamic_Object()) 
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
	
	alloc_mem_swap(&x->temp_mem, 0, 0);
    x->threads = new ThreadSet((t_object *) x, reinterpret_cast<ThreadSet::procFunc *>(&dynamicdsp_threadprocess), max_obj_threads, num_sig_outs);
    x->slots = new ThreadedPatchSet((t_object *)x, x->parent_patch, num_ins, num_outs, outs);
	
	// Load patch
	
	if (patch_name_entered)
        x->slots->load(0, patch_name_entered, ac, av, x->last_vec_size, x->last_samp_rate);
	
	return x;
}

void dynamicdsp_free(t_dynamicdsp *x)
{
	dsp_free((t_pxobject *)x);
	
	// Free temporary memory / threads / patches

    free_mem_swap(&x->temp_mem);
    delete x->threads;
	delete x->slots;
	
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


// ======================================================================================================= //
// Patcher Loading / Deleting
// ======================================================================================================= //


void dynamicdsp_deletepatch(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv)
{
    x->slots->remove(atom_getlong(argv));
}

void dynamicdsp_clear(t_dynamicdsp *x)
{
    x->slots->clear();
}

void dynamicdsp_loadpatch(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv)
{
    t_symbol *patch_name = NULL;
    t_atom_long index = -1;
	t_atom_long thread_request = -1;
	
	// Get requested patch index if there is one
	
	if (argc && atom_gettype(argv) == A_LONG)
	{
		index = atom_getlong(argv) - 1;
		if (index < 0)
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
        
        index = x->slots->load(index, patch_name, argc, argv, x->last_vec_size, x->last_samp_rate);
        
        // FIX - threading...
        // FIX - review
        
        if (thread_request && index >= 0)
        {
            if (thread_request > 0)
                x->slots->requestThread(index, thread_request);
            else
                x->slots->requestThread(index, index);
            
            x->update_thread_map = 1;
        }
	} 
	else 
		object_error((t_object *) x, "no patch specified");
}

// ======================================================================================================= //
// Messages in passed on to the patcher via the "in" objects / Voice targeting
// ======================================================================================================= //


void dynamicdsp_bang(t_dynamicdsp *x)
{	
    x->slots->objBang();
}

void dynamicdsp_int(t_dynamicdsp *x, t_atom_long n)
{
    x->slots->objInt(n);
}

void dynamicdsp_float(t_dynamicdsp *x, double f)
{
    x->slots->objFloat(f);
}

void dynamicdsp_list(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv)
{
    x->slots->objAnything(s, argc, argv);
}

void dynamicdsp_anything(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv)
{
    x->slots->objAnything(s, argc, argv);
}

void dynamicdsp_target(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv)
{
    x->slots->objTarget(argc, argv);
}

void dynamicdsp_targetfree(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv)
{
    x->slots->objTargetFree(argc, argv);
}


// ======================================================================================================= //
// Multithreading Messages
// ======================================================================================================= //


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
        x->slots->requestThread(index, thread_request);
    else
        x->slots->requestThread(index, index);
    
    x->update_thread_map = 1;
}


// ======================================================================================================= //
// Perform Routines
// ======================================================================================================= //


static __inline void dynamicdsp_multithread_perform(t_dynamicdsp *x, void **sig_outs, long vec_size, long num_active_threads)
{
    // Tick the threads and process in this thread (the main audio thread)
    
    x->threads->tick(vec_size, num_active_threads, sig_outs);
    
    if (num_active_threads > 1)
    {
        // Sum outputs
    
        if (sig_size == sizeof(float))
            dynamicdsp_sum_float(x->threads, sig_outs, x->num_sig_outs, vec_size, num_active_threads);
        else
            dynamicdsp_sum_double(x->threads, sig_outs, x->num_sig_outs, vec_size, num_active_threads);
    }
}

void dynamicdsp_threadprocess(t_dynamicdsp *x, void **sig_outs, void *temp_mem_ptr, t_ptr_uint temp_mem_size, long vec_size, long thread_num, long num_active_threads)
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
        for (long i = 0; i < x->slots->size(); i++)
            x->slots->processIfThreadMatches(i, temp_mem_ptr, sig_outs, temp_mem_size, thread_num, num_active_threads);
    }
    else
    {
        long size = x->slots->size();
        long index = (thread_num * (size / num_active_threads)) - 1;
        for (long i = 0; i < size; i++)
        {
            if (++index >= size)
                index -= size;
            
            x->slots->processIfUnprocessed(i, temp_mem_ptr, sig_outs, temp_mem_size);
        }
    }
    
    // return denormals to previous state 
    
#if defined( __i386__ ) || defined( __x86_64__ )	
    _mm_setcsr(oldMXCSR);	
#endif
}

void dynamicdsp_sum_float(ThreadSet *threads, void **sig_outs, long num_sig_outs, long vec_size, long num_active_threads)
{
    // Sum output of threads for each signal outlet
    
    for (long i = 0; i < num_sig_outs; i++)
    {
        for (long j = 0; j < num_active_threads; j++)
        {
            float *io_pointer = (float *)sig_outs[i];
            float *next_sig_pointer = (float *)threads->getThreadOut(j, i);
            
            if (next_sig_pointer)
                for (long k = 0; k < vec_size; k++)
                    *io_pointer++ += *next_sig_pointer++;
        }
    }
}

void dynamicdsp_sum_double(ThreadSet *threads, void **sig_outs, long num_sig_outs, long vec_size, long num_active_threads)
{
    // Sum output of threads for each signal outlet
    
    for (long i = 0; i < num_sig_outs; i++)
    {
        for (long j = 0; j < num_active_threads; j++)
        {
            double *io_pointer = (double *)sig_outs[i];
            double *next_sig_pointer = (double *)threads->getThreadOut(j, i);
            
            if (next_sig_pointer)
                for (long k = 0; k < vec_size; k++)
                    *io_pointer++ += *next_sig_pointer++;
        }	
    } 
}

void dynamicdsp_perform_common(t_dynamicdsp *x, void **sig_outs, long vec_size)
{
    void *new_temp_mem_ptr;
    t_ptr_uint new_temp_mem_size;
	
	long num_active_threads = x->request_num_active_threads;
    long multithread_flag = (x->slots->size() > 1) && x->multithread_flag;
	
	// Zero Outputs
	
	for (long i = 0; i < x->num_sig_outs; i++)
		memset(sig_outs[i], 0, sig_size * vec_size);
	
	// Update multithreading parameters (done in one thread and before processing to ensure uninterrupted audio processing
	
    x->num_active_threads = num_active_threads;
	x->manual_threading = x->request_manual_threading;		
    num_active_threads = !multithread_flag ? 1 : num_active_threads;
    
	if (!x->manual_threading)
        x->slots->resetProcessed();
	
	if (x->update_thread_map)
	{
		x->update_thread_map = 0;											
		x->slots->updateThreads();
	}
	
	// Update the temporary memory if relevant
	
	if (attempt_mem_swap(&x->temp_mem) == SWAP_DONE)
	{
		// Store the new pointers and size
		
		new_temp_mem_ptr = x->temp_mem.current_ptr;
		new_temp_mem_size = x->temp_mem.current_size;
		
		for (long i = 0; i < x->max_obj_threads; i++)
            x->threads->setTempMemory(i, (char *) new_temp_mem_ptr + (new_temp_mem_size * i), new_temp_mem_size);
	}
    
	// Do processing - the switching aims to get more speed out of inlining with a fixed loop size
	// Note that the signle threaded case is handled as a special case in the ThreadSet class
    
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

void dynamicdsp_perform64 (t_dynamicdsp *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
	for (long i = 0; i < x->num_sig_ins; i++)
		x->sig_ins[i] = ins[i];
		
	dynamicdsp_perform_common(x, (void **) outs, vec_size);
}


// ======================================================================================================= //
// DSP Routines
// ======================================================================================================= //


bool dynamicdsp_dsp_common(t_dynamicdsp *x, long vec_size, long samp_rate)
{	
    bool mem_fail = x->threads->resizeTempBuffers(vec_size * sig_size);
	
	// Do internal dsp compile (for each valid patch)
	
    x->slots->compileDSP(vec_size, samp_rate);
    
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


// ======================================================================================================= //
// Patcher Window stuff
// ======================================================================================================= //


void dynamicdsp_dblclick(t_dynamicdsp *x)
{
    for (long i = 1; i <= x->slots->size(); i++)
        if (x->slots->openWindow(i))
            break;
}

void dynamicdsp_open(t_dynamicdsp *x, t_atom_long index)
{
    x->slots->openWindow(index);
}

void dynamicdsp_pclose(t_dynamicdsp *x)
{
	// Do Nothing
}

void dynamicdsp_wclose(t_dynamicdsp *x, t_atom_long index)
{
    x->slots->closeWindow(index);
}


// ======================================================================================================= //
// Patcher Utilities (these deal with various updating and necessary behind the scenes state stuff)
// ======================================================================================================= //


void dynamicdsp_pupdate(t_dynamicdsp *x, void *b, t_patcher *p)
{
    x->slots->update(p, x->last_vec_size, x->last_samp_rate);
}

void *dynamicdsp_subpatcher(t_dynamicdsp *x, long index, void *arg)
{
    return (void *) x->slots->reportSubpatch(index, arg);
}

void dynamicdsp_parentpatcher(t_dynamicdsp *x, t_patcher **parent)
{
	*parent = x->parent_patch;
}


// ======================================================================================================= //
// Parent / Child Communication - Routines for owned objects to query the parent
// ======================================================================================================= //


// Note that objects wishing to query the parent dynamicdsp~ object should call the functions in dynamicdsp.h.
// These act as suitable wrappers to send the appropriate message to the parent object and returns values as appropriate

// Signals

void *dynamicdsp_query_num_sigins(t_dynamicdsp *x)
{
    return (void *)x->num_sig_ins;
}

void *dynamicdsp_query_num_sigouts(t_dynamicdsp *x)
{
    return (void *)x->num_sig_outs;
}

void *dynamicdsp_query_sigins(t_dynamicdsp *x)
{
    return (void *)x->sig_ins;
}

void *dynamicdsp_query_sigouts(t_dynamicdsp *x, long index)
{
    return x->slots->getOutputHandle(index);
}

// State

void *dynamicdsp_client_get_patch_on(t_dynamicdsp *x, t_ptr_int index)
{
    return (void *) (t_atom_long) x->slots->getOn(index);
}

void *dynamicdsp_client_get_patch_busy(t_dynamicdsp *x, t_ptr_int index)
{
    return (void *) (t_atom_long) x->slots->getBusy(index);
}

void dynamicdsp_client_set_patch_on(t_dynamicdsp *x, t_ptr_int index, t_ptr_int state)
{
    x->slots->setOn(index, state);
}

void dynamicdsp_client_set_patch_busy(t_dynamicdsp *x, t_ptr_int index, t_ptr_int state)
{
    x->slots->setBusy(index, state);
}

// Temporary memory

// dynamicdsp~ provides memory per audio thread for temporary calculations.
// Objects requiring temporary memory during their perform method request a minimum size during their dsp routine
// The pointer should be requested during the perform routine, and should not be stored
// This reduces memory allocation, and potentially increases speed by keeping temporary memory in the cache


void *dynamicdsp_query_temp_mem(t_dynamicdsp *x, t_ptr_int index)
{
    return x->slots->getTempMemHandle(index);
}

void *dynamicdsp_client_temp_mem_resize(t_dynamicdsp *x, t_ptr_int index, t_ptr_uint size)
{
    // FIX - better system for this...
    
    schedule_grow_mem_swap(&x->temp_mem, size * x->threads->getNumThreads(), size);
    
    x->slots->setTempMemSize(index, size);
    
    return (void *) 1;
}
