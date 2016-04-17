
/*
 *  dynamicdsp~
 *
 *	dynamicdsp~ is a multithreaded audio processing object for dynamically loading and managing audio (or non-audio) patches in realtime, without affecting other patches that are processing.
 *	
 *	It can be thought of as a poly~ alternative, with more extensive multithreading features than the Max 5 version (although dynamicdsp~ also runs in Max 4).
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

#ifdef __APPLE__
#include <pthread.h>
#include <mach/semaphore.h>
#include <mach/task.h>
#else
#include <Windows.h>
#define snprintf _snprintf
#endif 

#include <AH_Atomic.h>
#include <AH_Memory_Swap.h>
#include <AH_Types.h>
#include <AH_VectorOps.h>

#include <dynamicdsp~.h>


// FIX - hack for descending (could use traverse jpatcher system also..)

typedef int (*intfunc) (t_patcher *p, void *arg);

// FIX - It seems I should clean up the threads better here....
// FIX - use an atomic counter for autoloadbalance to decrease thread sync costs
// FIX - Rod Constanzo loading issue
// FIX - potential adc~ crashes / no audio - cannot get traction on this
// FIX - check email threads also

// ========================================================================================================================================== //
// Global Varibles
// ========================================================================================================================================== //


void *dynamicdsp_class;

static unsigned long processor_num_actual_threads;
static t_ptr_uint sig_size;

#define MAX_NUM_PATCHES 4096
#define MAX_ARGS 16
#define MAX_IO 256


// ========================================================================================================================================== //
// Structures and typedefs
// ========================================================================================================================================== //


/////////////////////////////////// Generic in/out void pointer //////////////////////////////////

typedef void *t_outvoid;

///////////////////////////// Generic stureture for an in/out object /////////////////////////////

typedef struct _inout
{
	t_object s_obj;
	
	long s_index;
	void *s_outlet;
	
} t_inout;

////////// Structure for passing arguments to patchers when targeting particular patches /////////

struct t_args_struct
{
	t_symbol *msg;
	long argc;
	t_atom *argv;
	
	long inlet_index;
};

////////////////////////////// Structure for patch and related data //////////////////////////////

typedef struct _patchspace
{
	// Patch and dspchain
	
	t_patcher *the_patch;
	struct _dspchain *the_dspchain;
	
	// Patch Variables 
	
	t_symbol *patch_name_in;
	char patch_name[256];
	short patch_path;
	
	// Arguments (stored in case of reload / update)
	
	long x_argc;
	t_atom x_argv[MAX_ARGS];
	
	// Pointer to Array of Audio Out Buffers (which are thread dependent)
	
	void **out_ptrs;
	
	// Flags
	
	AH_Boolean patch_valid;
	AH_Boolean patch_on;
	AH_Boolean patch_busy;
		
	// Threading Variables
	
    t_int32_atomic processed_flag;
	long thread_current;
	long thread_request;
	
	// Temporary Memory Variables
	
	t_ptr_uint required_temp_mem_size;
	void *temp_mem_ptr;
		
} t_patchspace;

////////////////////////////// Structure for thread and related data /////////////////////////////

typedef struct thread_space {
	
	// Thread and Semaphores
	
#ifdef __APPLE__
	pthread_t pth;
#else
	HANDLE pth;
	AH_Boolean exiting;
#endif

	t_int32_atomic processed;
	
	// Internal Buffer Pointer
	
	void **thread_temp_buffer;
	
	// Temporary Memory For Objects Within Patch
	
	t_ptr_uint temp_mem_size;
	void *temp_mem_ptr;
	
	void *first_thread_space;
	
	// Variables
	
	void *dynamicdsp_parent;
	
	long thread_num;
	long vec_size;
	
} t_threadspace;

////////////////////////////////////// The object structure //////////////////////////////////////

typedef struct _dynamicdsp
{
    t_pxobject x_obj;
    
	t_patcher *parent_patch;
	
	// Patch Data and Variables 
	
	t_patchspace *patch_space_ptrs[MAX_NUM_PATCHES];
	long patch_spaces_allocated;
	
	t_int32_atomic patch_is_loading;
	
	long target_index;
	
	long last_vec_size;
	long last_samp_rate;
	
	// IO Variables
	
	long num_sig_ins;
	long num_sig_outs;
	long num_ins;
	long num_outs;
	
	void **sig_ins;
	void **sig_outs;
	
	t_outvoid *in_table;			// table of non-signal inlets
	t_outvoid *out_table;			// table of non-signal outlets
	long num_proxies;				// number of proxies = MAX(num_sig_ins, num_ins)
	
	// Multithreading Variables
		
	long request_num_active_threads;
	long num_active_threads;
	
    long multithread_flag;
	long request_manual_threading;
	long manual_threading;
	long update_thread_map;
	
	long max_obj_threads;
	
#ifdef __APPLE__
	task_t the_task;
	semaphore_t tick_semaphore;	
#else
	HANDLE tick_semaphore;
#endif
	
	// Thread Data
	
	t_threadspace *thread_space_ptr;
	t_ptr_uint thread_temp_buffer_size;

	// Temporary Memory Variables
	
	t_safe_mem_swap temp_mem;
	
} t_dynamicdsp;


// ========================================================================================================================================== //
// Function Prototypes
// ========================================================================================================================================== //


void *dynamicdsp_new(t_symbol *s, long argc, t_atom *argv);
void dynamicdsp_free(t_dynamicdsp *x);
void dynamicdsp_assist(t_dynamicdsp *x, void *b, long m, long a, char *s);

void dynamicdsp_deletepatch(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);
void dynamicdsp_deletepatch_internal(t_dynamicdsp *x, t_atom_long index);
void dynamicdsp_cleanpatch(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv);
void dynamicdsp_load_exit(t_dynamicdsp *x, void *previous, void *previous_index);
void dynamicdsp_loadpatch(t_dynamicdsp *x, long index, long thread_request,  t_symbol *patch_name_in, long argc, t_atom *argv);
void dynamicdsp_user_clear(t_dynamicdsp *x);
void dynamicdsp_user_loadpatch(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv);

void dynamicdsp_bang(t_dynamicdsp *x);
void dynamicdsp_int(t_dynamicdsp *x, t_atom_long n);
void dynamicdsp_float(t_dynamicdsp *x, double f);
void dynamicdsp_list(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv);
void dynamicdsp_anything(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv);
void dynamicdsp_target(t_dynamicdsp *x, long target_index, long inlet, t_symbol *msg, long argc, t_atom *argv);
int dynamicdsp_targetinlets(t_patcher *p, struct t_args_struct *args);
void dynamicdsp_user_target(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);
void dynamicdsp_user_target_free(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);

void dynamicdsp_autoloadbalance(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);
void dynamicdsp_multithread(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);
void dynamicdsp_activethreads(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);
void dynamicdsp_threadmap(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv);

static __inline void dynamicdsp_multithread_perform(t_dynamicdsp *x, void **sig_outs, long num_sig_outs, long vec_size, long num_active_threads);
#ifdef __APPLE__
void *dynamicdsp_threadwait(void *arg);
#else
DWORD WINAPI dynamicdsp_threadwait(LPVOID arg);
#endif
static __inline void dynamicdsp_threadprocess(t_dynamicdsp *x, void **sig_outs, void *temp_mem_ptr, t_ptr_uint temp_mem_size, long vec_size, long thread_num, long threads_running);
void dynamicdsp_sum_float(t_threadspace *thread_space_ptr, void **sig_outs, long num_sig_outs, long vec_size, long num_active_threads);
void dynamicdsp_sum_double(t_threadspace *thread_space_ptr, void **sig_outs, long num_sig_outs, long vec_size, long num_active_threads);
void dynamicdsp_perform_common(t_dynamicdsp *x, void **sig_outs, long vec_size);
t_int *dynamicdsp_perform(t_int *w);
void dynamicdsp_perform64(t_dynamicdsp *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

AH_Boolean dynamicdsp_dsp_common(t_dynamicdsp *x, long vec_size, long samp_rate);
void dynamicdsp_dsp(t_dynamicdsp *x, t_signal **sp, short *count);
void dynamicdsp_dsp64(t_dynamicdsp *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void dynamicdsp_dsp_internal(t_patchspace *patch_space_ptrs, long vec_size, long samp_rate);

int dynamicdsp_linkinlets(t_patcher *p, t_dynamicdsp *x);
int dynamicdsp_linkoutlets(t_patcher *p, t_dynamicdsp *x);
int dynamicdsp_unlinkinlets(t_patcher *p, t_dynamicdsp *x);
int dynamicdsp_unlinkoutlets(t_patcher *p, t_dynamicdsp *x);

void dynamicdsp_dblclick(t_dynamicdsp *x);
void dynamicdsp_open(t_dynamicdsp *x, t_atom_long index);
void dynamicdsp_doopen(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv);
void dynamicdsp_pclose(t_dynamicdsp *x);
void dynamicdsp_wclose(t_dynamicdsp *x, t_atom_long index);
void dynamicdsp_dowclose(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv);

int dynamicdsp_patcher_descend(t_patcher *p, intfunc fn, void *arg, t_dynamicdsp *x);
int dynamicdsp_setsubassoc(t_patcher *p, t_dynamicdsp *x);
void dynamicdsp_pupdate(t_dynamicdsp *x, void *b, t_patcher *p);
void *dynamicdsp_subpatcher(t_dynamicdsp *x, long index, void *arg);
void dynamicdsp_parentpatcher(t_dynamicdsp *x, t_patcher **parent);

void dynamicdsp_init_patch_space(t_patchspace *patch_space_ptrs);
t_patchspace *dynamicdsp_new_patch_space(t_dynamicdsp *x,long index);
void dynamicdsp_free_patch_and_dsp(t_dynamicdsp *x, t_patchspace *patch_space_ptrs);

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


// ========================================================================================================================================== //
// Symbols
// ========================================================================================================================================== //


t_symbol *ps_dynamicdsp;
t_symbol *ps_DynamicPatchIndex;
t_symbol *ps_dspchain;
t_symbol *ps_sigin;
t_symbol *ps_sigout;
t_symbol *ps_bpatcher;
t_symbol *ps_patcher;
t_symbol *ps_front;
t_symbol *ps_in;
t_symbol *ps_out;
t_symbol *ps_bang;
t_symbol *ps_int;
t_symbol *ps_float;
t_symbol *ps_list;
t_symbol *ps_args;

t_symbol *ps_declareio;

t_symbol *ps_getassoc;
t_symbol *ps_setassoc;
t_symbol *ps_noedit;


// ========================================================================================================================================== //
// Main
// ========================================================================================================================================== //


int C74_EXPORT main(void)
{
#ifdef __APPLE__
	processor_num_actual_threads = MPProcessors();
#else
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	
	processor_num_actual_threads = sysinfo.dwNumberOfProcessors;
#endif

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
	
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_user_clear, "clear", 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_user_loadpatch, "loadpatch", A_GIMME, 0);
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_deletepatch, "deletepatch", A_GIMME, 0);						// MUST FIX TO GIMME FOR NOW
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_user_target, "target", A_GIMME, 0);							// MUST FIX TO GIMME FOR NOW
	class_addmethod(dynamicdsp_class, (method)dynamicdsp_user_target_free, "targetfree", A_GIMME, 0);					// MUST FIX TO GIMME FOR NOW
	
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
	
	class_dspinit(dynamicdsp_class);
	
	class_register(CLASS_BOX, dynamicdsp_class);
	
	ps_getassoc = gensym("getassoc");
	ps_setassoc = gensym("setassoc");
	ps_noedit = gensym("noedit");
	
	ps_dynamicdsp = gensym("___DynamicDSP~___");					// Capitals must stay here
	ps_DynamicPatchIndex = gensym("___DynamicPatchIndex___");		// Capitals must stay here
	ps_dspchain = gensym("dspchain");	
	ps_sigin = gensym("in~");
	ps_sigout = gensym("out~");
	ps_bpatcher = gensym("bpatcher");
	ps_patcher = gensym("patcher");
	ps_front = gensym("front");
	ps_in = gensym("in");
	ps_out = gensym("out");
	ps_bang = gensym("bang");
	ps_int = gensym("int");
	ps_float = gensym("float");
	ps_list = gensym("list");
	ps_args = gensym("args");
	ps_declareio = gensym("declareio");
	
	sig_size = ((maxversion() & 0x3FFF) >= 0x600) ? sizeof(double) : sizeof(float);
	
	return 0;
}


// ========================================================================================================================================== //
// Object Creation / Freeing / Assisstance
// ========================================================================================================================================== //


void *dynamicdsp_new(t_symbol *s, long argc, t_atom *argv)
{	
	t_dynamicdsp *x = object_alloc(dynamicdsp_class);
	
	t_symbol *patch_name_entered = NULL;
	t_symbol *tempsym;
	
	long ac = 0;
	t_atom av[MAX_ARGS];						
	
	long num_sig_ins = 2;
	long num_sig_outs = 2;
	long num_ins = 2;
	long num_outs = 2;	
	long max_obj_threads = processor_num_actual_threads;
	
#ifdef __APPLE__
	pthread_attr_t tattr;
	struct sched_param param;
	int newprio = 63;		
#endif
	
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
	
#ifdef __APPLE__
	x->the_task = mach_task_self();
#endif

	// Multithreading variables
	
	x->thread_space_ptr = (t_threadspace *) malloc(max_obj_threads * sizeof(t_threadspace));
	x->manual_threading = 1;
	x->request_manual_threading = 1;
	x->request_num_active_threads = max_obj_threads;
	x->thread_temp_buffer_size = 0;
	
	// Setup temporary memory 
	
	alloc_mem_swap(&x->temp_mem, 0, 0);
				
	// Create and eetup each threads variables
	
	for (long i = 0; i < max_obj_threads; i++)
	{
		x->thread_space_ptr[i].pth = NULL;
		x->thread_space_ptr[i].thread_temp_buffer = ALIGNED_MALLOC(num_sig_outs * sizeof(void *));
		x->thread_space_ptr[i].temp_mem_ptr = NULL;
        x->thread_space_ptr[i].temp_mem_size = 0;
		x->thread_space_ptr[i].dynamicdsp_parent = x;
		x->thread_space_ptr[i].thread_num = i;
		x->thread_space_ptr[i].first_thread_space = x->thread_space_ptr;
		
		for (long j = 0; j < num_sig_outs; j++)
			x->thread_space_ptr[i].thread_temp_buffer[j] = NULL;
	}
	
	// Create the extra threads and associated semaphores (thread zero is the main audio thread)

#ifdef __APPLE__	
	semaphore_create(x->the_task, &x->tick_semaphore, 0, 0);
#else
	//x->tick_semaphore  = CreateEvent(NULL, TRUE, FALSE, NULL);
	x->tick_semaphore  = CreateSemaphore(NULL, 0, max_obj_threads - 1, NULL);
#endif 

	for (long i = 1; i < max_obj_threads; i++)
	{
		x->thread_space_ptr[i].processed = 1;
		//x->thread_space_ptr[i].tick_semaphore = x->tick_semaphore;
#ifdef __APPLE__
		pthread_attr_init (&tattr);																// initialized with default attributes 
		pthread_attr_getschedparam (&tattr, &param);											// safe to get existing scheduling param 
		param.sched_priority = newprio;															// set the priority; others are unchanged 
		pthread_attr_setschedparam (&tattr, &param);											// setting the new scheduling param 
		pthread_create(&(x->thread_space_ptr[i].pth), &tattr, dynamicdsp_threadwait, x->thread_space_ptr + i);
#else
		//x->thread_space_ptr[i].tick_semaphore = CreateEvent(NULL, TRUE, FALSE, NULL);
		//x->thread_space_ptr[i].ret_semaphore = CreateEvent(NULL, TRUE, FALSE, NULL);

		x->thread_space_ptr[i].pth = CreateThread(NULL, 0, dynamicdsp_threadwait, x->thread_space_ptr + i, 0, NULL);
		SetThreadPriority(x->thread_space_ptr[i].pth, THREAD_PRIORITY_TIME_CRITICAL);

		x->thread_space_ptr[i].exiting = false;
#endif
	}
	
	// Set other variables to defaults
	
	x->num_sig_ins = num_sig_ins;
	x->num_sig_outs = num_sig_outs;
	x->num_ins = num_ins;
	x->num_outs = num_outs;
	
	x->patch_spaces_allocated = 0;
	x->update_thread_map = 0;
	x->target_index = 0;	
	
	x->last_vec_size = 64;
	x->last_samp_rate = 44100;
	
	x->in_table = 0;
	x->out_table = 0;
	
	x->patch_is_loading = 0;
	
	// Create signal in/out buffers and zero
	
	x->sig_ins = (void **) malloc(num_sig_ins * sizeof(void *));
	x->sig_outs = (void **) malloc(num_sig_outs * sizeof(void *));
	
	for (long i = 0; i < num_sig_ins; i++)
		x->sig_ins[i] = NULL;
	for (long i = 0; i < num_sig_outs; i++)
		x->sig_outs[i] = NULL;
	
	// Make non-signal outlets first
	
	if (num_outs) 
	{
		x->out_table = (t_outvoid *) t_getbytes(num_outs * sizeof(t_outvoid));
		for (long i = num_outs - 1; i >= 0; i--)
			x->out_table[i] = outlet_new((t_object *)x, NULL);
	}
	
	// Make non-signal inlets 
	
	if (num_ins) 
	{
		x->in_table = (t_outvoid *)t_getbytes(num_ins * sizeof(t_outvoid));
		for (long i = 0; i < num_ins; i++)
			x->in_table[i] = outlet_new(NULL, NULL);											// make generic unowned inlets
	}
	
	// Make signal ins
	
	x->num_proxies = (num_sig_ins > num_ins) ? num_sig_ins : num_ins;
	
	dsp_setup((t_pxobject *) x, x->num_proxies);
	x->x_obj.z_misc = Z_NO_INPLACE;                                                             // due to output zeroing!!
	
	// Make signal outs
	
	for (long i = 0; i < num_sig_outs; i++)
		outlet_new((t_object *)x, "signal");
	
	// Initialise patcher symbol
	
	x->parent_patch = (t_patcher *)gensym("#P")->s_thing;										// store reference to parent patcher
	
	// Load patch and initialise
	
	if (patch_name_entered) 
		dynamicdsp_loadpatch(x, 0, -1, patch_name_entered, ac, av);
	
	return x;
}

void dynamicdsp_free(t_dynamicdsp *x)
{
	dsp_free((t_pxobject *)x);
	
	// Free semaphores

#ifdef __APPLE__
	semaphore_destroy(x->the_task, x->tick_semaphore);
#else
	for (long i = 1; i < x->max_obj_threads; i++)
	{
		x->thread_space_ptr[i].exiting = true;
		//SetEvent(x->thread_space_ptr[i].tick_semaphore);
	}
	ReleaseSemaphore(x->tick_semaphore, x->max_obj_threads - 1, NULL);
	CloseHandle(x->tick_semaphore);
#endif
	
	// Free thread temporary buffers
	
	for (long i = 0; i < x->max_obj_threads; i++)
    {
        if (x->thread_space_ptr[i].thread_temp_buffer)
        {
            for (long j = 0; j < x->num_sig_outs; j ++)
            {
                if (x->thread_space_ptr[i].thread_temp_buffer[j])
                    ALIGNED_FREE(x->thread_space_ptr[i].thread_temp_buffer[j]);
            }
        
            free(x->thread_space_ptr[i].thread_temp_buffer);
        }
    }
	
	// Free patches
	
	for (long i = 0; i < x->patch_spaces_allocated; i++)
	{
		t_patchspace *patch_space_ptr = x->patch_space_ptrs[i];
        if (patch_space_ptr)
        {
            dynamicdsp_free_patch_and_dsp(x, patch_space_ptr);
            freebytes((char *) patch_space_ptr, sizeof(t_patchspace));
        }
	}
	
	// Free other resources
	
	free_mem_swap(&x->temp_mem);
	free(x->thread_space_ptr);
	
	if (x->num_sig_ins)
		free(x->sig_ins);
	
	if (x->num_sig_outs)
		free(x->sig_outs);
	
	for (long i = 0; i < x->num_ins; i++)
		freeobject(x->in_table[i]);
	
	if (x->in_table)
		freebytes(x->in_table, x->num_ins * sizeof(t_outvoid));
	
	if (x->out_table)
		freebytes(x->out_table, x->num_outs * sizeof(t_outvoid));
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


// ========================================================================================================================================== //
// Patcher Loading / Deleting
// ========================================================================================================================================== //


void dynamicdsp_deletepatch(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv)
{
	dynamicdsp_deletepatch_internal(x, argc ? atom_getlong(argv) - 1 : -1); 
}

void dynamicdsp_deletepatch_internal(t_dynamicdsp *x, t_atom_long index)
{	
	t_atom a;
	
	if (index < 0 || index >= x->patch_spaces_allocated)
	{
		object_error((t_object *) x, "patch index out of range");
		return;
	}
	
    atom_setlong(&a, index);
    x->patch_space_ptrs[index] ->patch_valid = false;
	defer(x,(method)dynamicdsp_cleanpatch, 0L, 1, &a);
}

void dynamicdsp_cleanpatch(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv)
{
	t_patchspace *patch_space_ptr;
	t_atom_long index = atom_getlong(argv);
	
    if (index < 0 || index >= x->patch_spaces_allocated)
        return;
    
	// First close the window
	
	dynamicdsp_dowclose(x, 0L, 1, argv);
	
	// Now free
	
	patch_space_ptr = x->patch_space_ptrs[index];
	dynamicdsp_free_patch_and_dsp(x, patch_space_ptr);
	dynamicdsp_init_patch_space(patch_space_ptr);
}

void dynamicdsp_load_exit(t_dynamicdsp *x, void *previous, void *previous_index)
{
	ps_dynamicdsp->s_thing = previous;
    ps_DynamicPatchIndex->s_thing = previous_index;
	ATOMIC_DECREMENT_BARRIER(&x->patch_is_loading);
}

void dynamicdsp_loadpatch(t_dynamicdsp *x, long index, long thread_request, t_symbol *patch_name_in, long argc, t_atom *argv)
{
	t_patchspace *patch_space_ptr = 0;
	t_object *previous;
	t_object *previous_index;
	
    t_fourcc type;
    t_fourcc valid_types[3];
	
	long patch_spaces_allocated = x->patch_spaces_allocated;

	short patch_path;
	short save_loadupdate;
	char patch_name[256];
	char window_name[280];
	void *p;
		
    t_object *save_inhibit_state;
    t_symbol *ps_inhibit_subpatcher_vis = gensym("inhibit_subpatcher_vis");
    t_symbol *ps_PAT = gensym("#P");
    t_patcher *saveparent;
    
    // Set the valid types to test for
    
	valid_types[0] = FOUR_CHAR_CODE('maxb');
	valid_types[1] = FOUR_CHAR_CODE('TEXT');
	valid_types[2] = FOUR_CHAR_CODE('JSON');

	// Check that this object is not loading in another thread
	
	if (ATOMIC_INCREMENT_BARRIER(&x->patch_is_loading) > 1)
	{
		object_error((t_object *) x, "patch is loading in another thread");
        ATOMIC_DECREMENT_BARRIER(&x->patch_is_loading);
		return;
	}
	
	// Find a free patch if no index is given
	
	if (index < 0)
	{
		for (index = 0; index < patch_spaces_allocated; index++)
			if (x->patch_space_ptrs[index]->the_patch == NULL)
				break;
	}
	
	// Check that the index is valid
	
	if (index >= MAX_NUM_PATCHES) 
	{
		object_error((t_object *) x, "slot out of range");
        ATOMIC_DECREMENT_BARRIER(&x->patch_is_loading);
		return;
	}
		
	// Create patchspaces up until the last allocated index (if necessary) and store the pointer
	
	for (long i = patch_spaces_allocated; i < index + 1; i++)
		dynamicdsp_new_patch_space(x, i);
	
	patch_space_ptr = x->patch_space_ptrs[index];
	
	// Free the old patch - the new patch is not yet valid, but we switch it on so it can be switched off at loadbang time
	
	patch_space_ptr->patch_valid = false;
	dynamicdsp_free_patch_and_dsp(x, patch_space_ptr);
	dynamicdsp_init_patch_space(patch_space_ptr);
	patch_space_ptr->patch_on = true;
	
	// Bind to the loading symbols and store the old symbols
	
	previous = ps_dynamicdsp->s_thing;
	previous_index = ps_DynamicPatchIndex->s_thing;
	
	ps_dynamicdsp->s_thing = (t_object *) x;										
	ps_DynamicPatchIndex->s_thing = (t_object *) (index + 1);						
	
	// Try to locate a file of the given name that is of the correct type
	
	strcpy(patch_name, patch_name_in->s_name);
	
	if (locatefile_extended(patch_name, &patch_path, &type, valid_types, 3))
	{
		object_error((t_object *) x, "no patcher %s", patch_name);
		dynamicdsp_load_exit(x, previous, previous_index);
		return;
	}
	
	// Check the number of rarguments (only up to 16 allowed right now)
	
	if (argc > MAX_ARGS)
		argc = MAX_ARGS;
	
	// Load the patch (don't interrupt dsp)
	
    save_loadupdate = dsp_setloadupdate(false);
    saveparent = (t_patcher *)ps_PAT->s_thing;
    ps_PAT->s_thing = (t_object *) x->parent_patch;
    save_inhibit_state = ps_inhibit_subpatcher_vis->s_thing;
    ps_inhibit_subpatcher_vis->s_thing = (t_object *) -1;
    loadbang_suspend();
    
	//save_loadupdate = dsp_setloadupdate(false);
	p = intload(patch_name, patch_path, 0 , argc, argv, false);
	//dsp_setloadupdate(save__loadupdate);
		
    ps_inhibit_subpatcher_vis->s_thing = save_inhibit_state;
    ps_PAT->s_thing = (t_object *) saveparent;
    
	// Check something has loaded
	
	if (!p) 
	{
		object_error((t_object *) x, "error loading %s", patch_name);
		dynamicdsp_load_exit(x, previous, previous_index);
		return;
	}
	
	// Check that it is a patcher that has loaded
	
	if (!ispatcher(p)) 
	{
		object_error((t_object *) x, "%s is not a patcher", patch_name);
		freeobject((t_object *)p);
		dynamicdsp_load_exit(x, previous, previous_index);
		return;
	}
	
	// Change the window name
	
	snprintf(window_name, 256, "%s %s%ld%s", patch_name, "(", index + 1, ")");
	jpatcher_set_title(p, gensym(window_name));
    
    object_method(p, gensym("setclass"));
    
	// Set the relevant associations (for Max 5 the dynamicdsp_setsubassoc call covers all of this)
	
	//dynamicdsp_patcher_descend((t_patcher *)p, (intfunc) dynamicdsp_setsubassoc, x, x);			// associate subpatches with this instance
    {
        long result = 0;
        object_method((t_patcher *)p,gensym("traverse"),dynamicdsp_setsubassoc, x, &result);
    }
    
	// Link inlets and outlets
	
	if (x->num_ins) 
		dynamicdsp_patcher_descend((t_patcher *)p, (intfunc) dynamicdsp_linkinlets, x, x);
	if (x->num_outs) 
		dynamicdsp_patcher_descend((t_patcher *)p, (intfunc) dynamicdsp_linkoutlets, x, x);
	
    dsp_setloadupdate(save_loadupdate);
    loadbang_resume();
    
	// Copy all the relevant data into the patch space
	
	patch_space_ptr->the_patch = p;
	patch_space_ptr->patch_name_in = patch_name_in;
	
	strcpy(patch_space_ptr->patch_name, patch_name);									
	patch_space_ptr->patch_path = patch_path;	
	
	patch_space_ptr->x_argc = argc;
    memcpy(patch_space_ptr->x_argv, argv, argc * sizeof(t_atom));
	
	// Set a request for a particular thread
    // FIX - review
    
	if (thread_request)
	{
		if (thread_request > 0) 
			patch_space_ptr->thread_request = thread_request - 1;
		else 
			patch_space_ptr->thread_request = index;
		x->update_thread_map = 1;
	}
	
	// Compile the dspchain in case dsp is on
	// FIX - do this twice as hack for gen~ patchers...
    
	dynamicdsp_dsp_internal(patch_space_ptr, x->last_vec_size, x->last_samp_rate);
	//dynamicdsp_dsp_internal(patch_space_ptr, x->last_vec_size, x->last_samp_rate);

	// The patch is valid and ready to go
	
	patch_space_ptr->patch_valid = true;
	
	// Return to previous state
		
	dynamicdsp_load_exit(x, previous, previous_index);
}

void dynamicdsp_user_clear(t_dynamicdsp *x)
{
	for (long i = 0; i < x->patch_spaces_allocated; i++)
		dynamicdsp_deletepatch_internal (x, i);
}

void dynamicdsp_user_loadpatch(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv)
{
    t_symbol *patch_name_entered = NULL;
    t_atom_long index = -1;
	t_atom_long thread_request = -1;
	
	// Get requested patch index if there is one
	
	if (argc && atom_gettype(argv) == A_LONG)
	{
		index = atom_getlong(argv) - 1;
		if (index < 0 || index >= MAX_NUM_PATCHES)
		{
			object_error((t_object *) x, "patch index out of range");
			return;
		}
		argc--; argv++;
	}
	
	// Get thread request if there is one
	
	if (argc && atom_gettype(argv) == A_LONG)
	{
		thread_request = atom_getlong(argv);
		argc--; argv++;
	}
	
	// If there is a patch name then try to load the patch
	
	if (argc && atom_gettype(argv) == A_SYM)			
	{
		patch_name_entered = atom_getsym(argv);
		argc--; argv++;
		
		dynamicdsp_loadpatch(x, index, thread_request, patch_name_entered, argc, argv);
	} 
	else 
		object_error((t_object *) x, "no patch specified");
}

// ========================================================================================================================================== //
// Messages in passed on to the patcher via the "in" objects / Voice targeting
// ========================================================================================================================================== //


void dynamicdsp_bang(t_dynamicdsp *x)
{	
	long inlet = proxy_getinlet((t_object *)x);
	long target_index = x->target_index;
	
	if (inlet >= x->num_ins)
		return;

	if (target_index)
		dynamicdsp_target(x, target_index, inlet, ps_bang, 0, 0);
	else
		outlet_bang(x->in_table[inlet]);
}

void dynamicdsp_int(t_dynamicdsp *x, t_atom_long n)
{
	long inlet = proxy_getinlet((t_object *)x);
	long target_index = x->target_index;
	
	if (inlet >= x->num_ins)
		return;		
	
	if (target_index)
	{
		t_atom n_atom;
        atom_setlong(&n_atom, n);
		dynamicdsp_target(x, target_index, inlet, ps_int, 1, &n_atom);
	}
	else
		outlet_int(x->in_table[inlet], n);
}

void dynamicdsp_float(t_dynamicdsp *x, double f)
{
    long inlet = proxy_getinlet((t_object *)x);
	long target_index = x->target_index;
	
	if (inlet >= x->num_ins)
		return;
	
	if (target_index)
	{
		t_atom f_atom; 
		atom_setfloat(&f_atom, f);
		dynamicdsp_target(x, target_index, inlet, ps_float, 1, &f_atom);
	}
	else
		outlet_float(x->in_table[inlet], f);
}

void dynamicdsp_list(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv)
{
    long inlet = proxy_getinlet((t_object *)x);
	long target_index = x->target_index;
	
	if (inlet >= x->num_ins)
		return;
	
	if (target_index)
		dynamicdsp_target(x, target_index, inlet, ps_list, argc, argv);
	else
		outlet_list(x->in_table[inlet], ps_list, argc, argv);
}

void dynamicdsp_anything(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv)
{
    long inlet = proxy_getinlet((t_object *)x);
	long target_index = x->target_index;
	
	if (inlet >= x->num_ins)
		return;		
	
	if (target_index)
		dynamicdsp_target(x, target_index, inlet, s, argc, argv);
	else
		outlet_anything(x->in_table[inlet], s, argc, argv);
}

void dynamicdsp_target(t_dynamicdsp *x, long target_index, long inlet, t_symbol *msg, long argc, t_atom *argv)
{	
	struct t_args_struct pass_args;
	
	pass_args.msg = msg;
	pass_args.argc = argc;
	pass_args.argv = argv;
	pass_args.inlet_index = inlet + 1;
	
	if (target_index >= 1 && target_index <= x->patch_spaces_allocated)
	{
		t_patcher *p = x->patch_space_ptrs[target_index - 1]->the_patch;
		
		if (x->patch_space_ptrs[target_index - 1]->patch_valid)
			dynamicdsp_patcher_descend(p, (intfunc) dynamicdsp_targetinlets, &pass_args, x);
	}
}

// - inlet and outlet linking using the in and out objects

int dynamicdsp_targetinlets(t_patcher *p, struct t_args_struct *args)
{
	for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
	{
		if (jbox_get_maxclass(b) == ps_in) 
		{
			t_inout *io = (t_inout *) jbox_get_object(b);
			if (io->s_index == args->inlet_index)
			{
				void *outletptr = io->s_obj.o_outlet;
				outlet_anything(outletptr, args->msg, args->argc, args->argv);
			}
		}
    }
    
	return 0;
}

void dynamicdsp_user_target(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv)
{
	t_atom_long target_index = argc ? atom_getlong(argv) : 0;
	
    if (target_index >= 0 || target_index <= x->patch_spaces_allocated)
        x->target_index = target_index;
    else
        x->target_index = -1;
}

void dynamicdsp_user_target_free(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv)
{
	t_patchspace **patch_space_ptrs = x->patch_space_ptrs;;
	
	long patch_spaces_allocated = x->patch_spaces_allocated;
    long lo = 0;
    long hi = patch_spaces_allocated;
    
    t_atom_long in1 = argc > 0 ? atom_getlong(argv + 0) : 0;
    t_atom_long in2 = argc > 1 ? atom_getlong(argv + 1) : 0;
    
    // Clip inputs
    
    in1 = (in1 < 1) ? 1 : ((in1 > patch_spaces_allocated) ? patch_spaces_allocated : in1);
    in2 = (in2 < 1) ? 1 : ((in2 > patch_spaces_allocated) ? patch_spaces_allocated : in2);
   
    // Load arguments
    
	if (argc)
	{
		if (argc > 1)
		{
            lo = ((in1 < in2) ? in1 : in2) - 1;
			hi = ((in1 > in2) ? in1 : in2);
		}
		else
            hi = in1;
    }
	
    // Search for a free voice
    
	for (long i = lo; i < hi; i++)
	{
		if (patch_space_ptrs[i]->patch_valid && !patch_space_ptrs[i]->patch_busy)
		{
			x->target_index = i + 1;
			return;
		}
	}
	
	x->target_index = -1;
}


// ========================================================================================================================================== //
// Multithreading Messages
// ========================================================================================================================================== //


void dynamicdsp_autoloadbalance(t_dynamicdsp *x, t_symbol *msg, long argc, t_atom *argv)
{
	x->request_manual_threading = (!argc || atom_getlong(argv)) ? 1 : 0;
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
    
	if (index >= 0 && index < x->patch_spaces_allocated)
	{
		if (thread_request >= 0)
			x->patch_space_ptrs[index]->thread_request = thread_request;
		else 
			x->patch_space_ptrs[index]->thread_request = index;
		x->update_thread_map = 1;
	}
}


// ========================================================================================================================================== //
// Perform Routines
// ========================================================================================================================================== //


static __inline void dynamicdsp_multithread_perform(t_dynamicdsp *x, void **sig_outs, long num_sig_outs, long vec_size, long num_active_threads)
{
    t_threadspace *thread_space_ptr = x->thread_space_ptr;
    
    for (long i = 1; i < num_active_threads; i++)
        thread_space_ptr[i].processed = 0;
    
    // Tick the worker threads and process in this thread (the main audio thread)
    
#ifdef __APPLE__
    
    OSMemoryBarrier();
    
    for (long i = 1; i < num_active_threads; i++)
        semaphore_signal(x->tick_semaphore);
#else
    
    MemoryBarrier();
    ReleaseSemaphore(x->tick_semaphore, num_active_threads - 1, NULL);
    
#endif
    
    // Process thread
    
    dynamicdsp_threadprocess(x, thread_space_ptr->thread_temp_buffer, thread_space_ptr->temp_mem_ptr, thread_space_ptr->temp_mem_size, vec_size, 0, num_active_threads);
    
    // Wait for all the other threads to return
    
    for (long i = 1; i < num_active_threads; i++)
        while (thread_space_ptr[i].processed != 1);
    
    // Sum outputs
    
    if (sig_size == sizeof(float))
        dynamicdsp_sum_float(thread_space_ptr, sig_outs, num_sig_outs, vec_size, num_active_threads);
    else
        dynamicdsp_sum_double(thread_space_ptr, sig_outs, num_sig_outs, vec_size, num_active_threads);
    
}

#ifdef __APPLE__
#define THREAD_RETURN_VALUE NULL
#else
#define THREAD_RETURN_VALUE 0;
#endif

#ifdef __APPLE__
void *dynamicdsp_threadwait(void *arg)
#else
DWORD WINAPI dynamicdsp_threadwait(LPVOID arg)
#endif
{
    t_dynamicdsp *x = (t_dynamicdsp *) ((t_threadspace *) arg)->dynamicdsp_parent;
    t_threadspace *thread_ptrs = ((t_threadspace *) arg)->first_thread_space;
    t_threadspace *this_thread;
    t_threadspace *constant_thread;
    long thread_num = ((t_threadspace *) arg)->thread_num;
    long current_thread_num;
    long num_active_threads;
    
    constant_thread = thread_ptrs + thread_num;
    
    while(1)
    {
#ifdef __APPLE__
        kern_return_t tick_return = semaphore_wait(x->tick_semaphore);
        if (tick_return == KERN_TERMINATED)
            break;
#else
        WaitForSingleObject(x->tick_semaphore, INFINITE);
        if (constant_thread->exiting)
            break;
#endif
        
        num_active_threads = x->num_active_threads;
        
        for (long i = thread_num; i < thread_num + num_active_threads - 1; i++)
        {
            // N.B. Get values from thread each time in case they have been changed
            
            current_thread_num = (i % (num_active_threads - 1)) + 1;
            this_thread = thread_ptrs + current_thread_num;
            
            if (Atomic_Compare_And_Swap(0, 2, (t_int32_atomic *) &this_thread->processed))
            {
                dynamicdsp_threadprocess(x, this_thread->thread_temp_buffer, this_thread->temp_mem_ptr, this_thread->temp_mem_size, this_thread->vec_size, current_thread_num, num_active_threads);
                this_thread->processed = 1;
            }
        }
    }
    
    return THREAD_RETURN_VALUE;
}

static __inline void dynamicdsp_threadprocess(t_dynamicdsp *x, void **sig_outs, void *temp_mem_ptr, t_ptr_uint temp_mem_size, long vec_size, long thread_num, long threads_running)
{
    t_patchspace **patch_space_ptrs = x->patch_space_ptrs;
    
    long num_sig_outs = x->num_sig_outs;
    long patch_spaces_allocated = x->patch_spaces_allocated;
    long index;
    
    // Turn off denormals
    
#if defined( __i386__ ) || defined( __x86_64__ )
    int oldMXCSR = _mm_getcsr();						// read the old MXCSR setting
    int newMXCSR = oldMXCSR | 0x8040;					// set DAZ and FZ bits
    _mm_setcsr( newMXCSR );								// write the new MXCSR setting to the MXCSR
#endif
    
    // Zero outputs
    
    for (long i = 0; i < num_sig_outs; i++)
        memset(sig_outs[i], 0, sig_size * vec_size);
    
    if (x->manual_threading)
    {
        // Here we run each patch in the requested thread
        
        for (long i = 0; i < patch_spaces_allocated; i++)
        {
            t_patchspace *next_patch_space_ptr = patch_space_ptrs[i];
            t_dspchain *next_dspchain = next_patch_space_ptr->the_dspchain;
            
            if (next_patch_space_ptr->patch_valid && next_patch_space_ptr->patch_on && next_dspchain && next_patch_space_ptr->required_temp_mem_size <= temp_mem_size &&
                (next_patch_space_ptr->thread_current % threads_running) == thread_num)
            {
                // Point to thread buffers and run DSP
                
                next_patch_space_ptr->out_ptrs = sig_outs;
                next_patch_space_ptr->temp_mem_ptr = temp_mem_ptr;
                
                dspchain_tick(next_dspchain);
            }
        }
    }
    else
    {
        // Here we start each thread at a different point in the cycle and whichever one reaches a patch first will process it
        
        index = (thread_num * (patch_spaces_allocated / threads_running)) - 1;
        for (long i = 0; i < patch_spaces_allocated; i++)
        {
            if (++index >= patch_spaces_allocated)
                index -= patch_spaces_allocated;
            
            t_patchspace *next_patch_space_ptr = patch_space_ptrs[index];
            t_dspchain *next_dspchain = next_patch_space_ptr->the_dspchain;
            
            if (next_patch_space_ptr->patch_valid && next_patch_space_ptr->patch_on && next_dspchain && next_patch_space_ptr->required_temp_mem_size  <= temp_mem_size && 
                Atomic_Compare_And_Swap(0, 1, &next_patch_space_ptr->processed_flag))
            {							
                // Point to thread buffers and run DSP
                
                next_patch_space_ptr->out_ptrs = sig_outs;
                next_patch_space_ptr->temp_mem_ptr = temp_mem_ptr;
                
                dspchain_tick(next_dspchain);
            }
        }
    }
    
    // return denormals to previous state 
    
#if defined( __i386__ ) || defined( __x86_64__ )	
    _mm_setcsr(oldMXCSR);	
#endif
}

void dynamicdsp_sum_float(t_threadspace *thread_space_ptr, void **sig_outs, long num_sig_outs, long vec_size, long num_active_threads)
{
    // Sum output of threads for each signal outlet
    
    for (long i = 0; i < num_sig_outs; i++)
    {
        for (long j = 0; j < num_active_threads; j++)
        {
            float *next_sig_pointer = thread_space_ptr[j].thread_temp_buffer[i];
            float *io_pointer = sig_outs[i];
            
            for (long k = 0; k < vec_size; k++)
                *io_pointer++ += *next_sig_pointer++;
        }
    }
}

void dynamicdsp_sum_double(t_threadspace *thread_space_ptr, void **sig_outs, long num_sig_outs, long vec_size, long num_active_threads)
{
    // Sum output of threads for each signal outlet
    
    for (long i = 0; i < num_sig_outs; i++)
    {
        for (long j = 0; j < num_active_threads; j++)
        {
            double *next_sig_pointer = thread_space_ptr[j].thread_temp_buffer[i];
            double *io_pointer = sig_outs[i];
            
            for (long k = 0; k < vec_size; k++)
                *io_pointer++ += *next_sig_pointer++;
        }	
    } 
}

void dynamicdsp_perform_common(t_dynamicdsp *x, void **sig_outs, long vec_size)
{
	t_threadspace *thread_space_ptr = x->thread_space_ptr;
	t_patchspace **patch_space_ptrs = x->patch_space_ptrs;
		
    void *new_temp_mem_ptr;
    t_ptr_uint new_temp_mem_size;
	
	long num_sig_outs = x->num_sig_outs;
	long num_active_threads = x->request_num_active_threads;	
    long multithread_flag = (x->patch_spaces_allocated > 1) && x->multithread_flag;
	
	// Zero Outputs
	
	for (long i = 0; i < num_sig_outs; i++)
		memset(sig_outs[i], 0, sig_size * vec_size);
	
	if (x->x_obj.z_disabled)
		return;

	// Update multithreading parameters (this is done in one thread and before all threads process to ensure uninterrupted audio processing
	
    x->num_active_threads = num_active_threads;
	x->manual_threading = x->request_manual_threading;		
    num_active_threads = !multithread_flag ? 1 : num_active_threads;
    
	if (!x->manual_threading)
	{
		for (long i = 0; i < x->patch_spaces_allocated; i++)
			patch_space_ptrs[i]->processed_flag = 0;
	}
	
	if (x->update_thread_map)
	{
		x->update_thread_map = 0;											
		
		for (long i = 0; i < x->patch_spaces_allocated; i++)
            patch_space_ptrs[i]->thread_current = patch_space_ptrs[i]->thread_request;
	}
	
	// Update the temporary memory if relevant
	
	if (attempt_mem_swap(&x->temp_mem) == SWAP_DONE)
	{
		// Store the new pointers and size
		
		new_temp_mem_ptr = x->temp_mem.current_ptr;
		new_temp_mem_size = x->temp_mem.current_size;
		
		for (long i = 0; i < x->max_obj_threads; i++)
		{
			thread_space_ptr[i].temp_mem_ptr = (char *) new_temp_mem_ptr + (new_temp_mem_size * i);
			thread_space_ptr[i].temp_mem_size = new_temp_mem_size;
		}
	}
	
	// Do multithreaded or non-multithread processing - the former case is switched to try to get more speed out of inlining with a fixed loop size
	
	switch (num_active_threads)
    {
        case 1:
            dynamicdsp_threadprocess(x, (void **) sig_outs, x->thread_space_ptr[0].temp_mem_ptr, x->thread_space_ptr[0].temp_mem_size, vec_size, 0, 1);
            break;
                
        case 2:
            dynamicdsp_multithread_perform(x, sig_outs, num_sig_outs, vec_size, 2);
            break;
				
        case 3:
            dynamicdsp_multithread_perform(x, sig_outs, num_sig_outs, vec_size, 3);
            break;
				
        case 4:
            dynamicdsp_multithread_perform(x, sig_outs, num_sig_outs, vec_size, 4);
            break;
				
        case 5:
            dynamicdsp_multithread_perform(x, sig_outs, num_sig_outs, vec_size, 5);
            break;
				
        case 6:
            dynamicdsp_multithread_perform(x, sig_outs, num_sig_outs, vec_size, 6);
            break;
				
        case 7:
            dynamicdsp_multithread_perform(x, sig_outs, num_sig_outs, vec_size, 7);
            break;
				
        case 8:
            dynamicdsp_multithread_perform(x, sig_outs, num_sig_outs, vec_size, 8);
            break;
				
        default:
            dynamicdsp_multithread_perform(x, sig_outs, num_sig_outs, vec_size, num_active_threads);
            break;
	}
}

t_int *dynamicdsp_perform(t_int *w)
{	
	t_dynamicdsp *x = (t_dynamicdsp *) (w[1]);
	void **sig_outs = (void **) x->sig_outs;
	long vec_size = x->last_vec_size;
	
	dynamicdsp_perform_common(x, sig_outs, vec_size);
	
	return w + 2;	
}

void dynamicdsp_perform64 (t_dynamicdsp *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
	for (long i = 0; i < x->num_sig_ins; i++)
		x->sig_ins[i] = ins[i];
		
	dynamicdsp_perform_common(x, (void **) outs, vec_size);
}


// ========================================================================================================================================== //
// DSP Routines
// ========================================================================================================================================== //


AH_Boolean dynamicdsp_dsp_common(t_dynamicdsp *x, long vec_size, long samp_rate)
{	
    AH_Boolean mem_fail = false;
	t_ptr_uint thread_temp_buffer_size;
	
	for (long i = 0; i < x->max_obj_threads; i++)
		x->thread_space_ptr[i].vec_size = vec_size;
	
	// Memory allocation for temporary thread buffers 
	
	thread_temp_buffer_size = vec_size * sig_size;
	
	if (thread_temp_buffer_size != x->thread_temp_buffer_size)
	{
		for (long i = 0; i < x->max_obj_threads; i++)
		{
			for (long j = 0; j < x->num_sig_outs; j++)
			{
				ALIGNED_FREE(x->thread_space_ptr[i].thread_temp_buffer[j]);
				x->thread_space_ptr[i].thread_temp_buffer[j] = ALIGNED_MALLOC(thread_temp_buffer_size);
				if (!x->thread_space_ptr[i].thread_temp_buffer[j])
				{		
					object_error((t_object *) x, "not enough memory");
					mem_fail = true;
					x->x_obj.z_disabled = TRUE;	
				}
			}
		}
	}
	
	// Do internal dsp compile (for each valid patch)
	
	for (long i = 0; i < x->patch_spaces_allocated; i++)
	{
		t_patchspace * patch_space_ptr = x->patch_space_ptrs[i];
		if (patch_space_ptr->patch_valid)
			dynamicdsp_dsp_internal(patch_space_ptr, vec_size, samp_rate);
	}
	
	x->last_vec_size = vec_size;
	x->last_samp_rate = samp_rate;
	
	if (!mem_fail)
		x->thread_temp_buffer_size = thread_temp_buffer_size;
    else
        x->thread_temp_buffer_size = 0;
	
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
/*
t_signal **poly_allocsignals(long numsignals, long vs, long sr)
{
	t_signal **sigs = (t_signal **) sysmem_newptr(sizeof(t_signal *) * numsignals);
	long i;
	for (i=0; i < numsignals; i++) {
		t_signal *ps = (t_signal *) sysmem_newptrclear(sizeof(t_signal));
		ps->s_n = vs;
		ps->s_size = sizeof(t_sample);   // correct based on MSP64 define
		ps->s_sr = sr;
		ps->s_ptr = (char *) sysmem_newptrclear(ps->s_n * ps->s_size + 32);
		ps->s_vec = (t_sample *) ps->s_ptr;
		sigs[i] = ps;
	}
	return sigs;
}*/

void dynamicdsp_dsp_internal(t_patchspace *patch_space_ptr, long vec_size, long samp_rate)
{
	t_dspchain *c;
    
    // Free the old dspchain
		
	if (patch_space_ptr->the_dspchain)
		freeobject((t_object *)patch_space_ptr->the_dspchain);
	
	// Recompile
    
    c = dspchain_start(vec_size, samp_rate);
    //c->c_inputs = poly_allocsignals(1, vec_size, samp_rate);
    //c->c_intype = 1;
    patch_space_ptr->the_dspchain = dspchain_compile2(patch_space_ptr->the_patch, c);

    //patch_space_ptr->the_dspchain = dspchain_compile(patch_space_ptr->the_patch, vec_size, samp_rate);
}


// ========================================================================================================================================== //
// Patcher Link Inlets / Outlets
// ========================================================================================================================================== //

// - inlet and outlet linking using the in and out objects

int dynamicdsp_linkinlets(t_patcher *p, t_dynamicdsp *x)
{
	for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
	{
		if (jbox_get_maxclass(b) == ps_in) 
		{
			t_inout *io = (t_inout *) jbox_get_object(b);
			if (io->s_index <= x->num_ins)
                outlet_add(x->in_table[io->s_index - 1], (struct inlet *)io->s_obj.o_outlet);
		}
    }
    
	return 0;
}

int dynamicdsp_linkoutlets(t_patcher *p, t_dynamicdsp *x)
{
	for (t_box * b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
	{
		if (jbox_get_maxclass(b) == ps_out) 
		{
			t_inout *io = (t_inout *) jbox_get_object(b);
			if (io->s_index <= x->num_outs)
                outlet_add(io->s_outlet, x->out_table[io->s_index - 1]);
		}
    }
    
	return 0;
}

// - inlet and outlet removal using the in and out objects

int dynamicdsp_unlinkinlets(t_patcher *p, t_dynamicdsp *x)
{
	for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
	{
		if (jbox_get_maxclass(b) == ps_in) 
		{
			t_inout *io = (t_inout *) jbox_get_object(b);
			if (io->s_index <= x->num_ins)
                outlet_rm(x->in_table[io->s_index - 1], (struct inlet *)io->s_obj.o_outlet);
		}
    }
    
	return 0;
}

int dynamicdsp_unlinkoutlets(t_patcher *p, t_dynamicdsp *x)
{
	for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
	{
		if (jbox_get_maxclass(b)  == ps_out) 
		{
			t_inout *io = (t_inout *) jbox_get_object(b);
			if (io->s_index <= x->num_outs)
                outlet_rm(io->s_outlet, x->out_table[io->s_index - 1]);
		}
    }
    
	return 0;
}

// ========================================================================================================================================== //
// Patcher Window stuff
// ========================================================================================================================================== //


void dynamicdsp_dblclick(t_dynamicdsp *x)
{
	for (long i = 0; i < x->patch_spaces_allocated; i++)
	{
		if (x->patch_space_ptrs[i]->the_patch)
		{
			dynamicdsp_open(x, i + 1);
			break;
		}
	}
}

void dynamicdsp_open(t_dynamicdsp *x, t_atom_long index)
{
	t_atom a;
    atom_setlong(&a, index - 1);
	defer(x,(method)dynamicdsp_doopen, 0L, 1, &a);
}

void dynamicdsp_doopen(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom_long index = atom_getlong(argv);
	
    if ((index < 0) || (index >= x->patch_spaces_allocated) || (!x->patch_space_ptrs[index]->patch_valid))
        return;
    
	if (x->patch_space_ptrs[index]->the_patch) 
		mess0((t_object *)x->patch_space_ptrs[index]->the_patch, ps_front);		// this will always do the right thing
}

void dynamicdsp_pclose(t_dynamicdsp *x)
{
	// Do Nothing
}

void dynamicdsp_wclose(t_dynamicdsp *x, t_atom_long index)
{
	t_atom a;
    atom_setlong(&a, index - 1);
	defer(x,(method)dynamicdsp_dowclose, 0L, 1, &a);
}

void dynamicdsp_dowclose(t_dynamicdsp *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom_long index = atom_getlong(argv);
	
    if ((index < 0) || (index >= x->patch_spaces_allocated) || (!x->patch_space_ptrs[index]->patch_valid))
        return;
	
	if (x->patch_space_ptrs[index]->the_patch)
        object_method(x->patch_space_ptrs[index]->the_patch, gensym("wclose"), x);
}


// ========================================================================================================================================== //
// Patcher Utilities (these deal with various updating and necessary behind the scenes state stuff)
// ========================================================================================================================================== //


int dynamicdsp_patcher_descend(t_patcher *p, intfunc fn, void *arg, t_dynamicdsp *x)
{
	t_patcher *p2;
    t_object *assoc = 0;
	
    object_method(p, ps_getassoc, &assoc);				// Avoid recursion into a poly / pfft / dynamicdsp~
	if (assoc && (t_dynamicdsp *) assoc != x) 
		return 0;

	// CHANGED - DO NOT PASS x AS ARG
	if ((*fn)(p, arg))
		return (1);
	
	for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
	{
		if (b) 
		{
			long index = 0;
			while ((p2 = object_subpatcher(jbox_get_object(b), &index, arg)))
				if (dynamicdsp_patcher_descend(p2, fn, arg, x))
					return 1;
		}
	}

	return 0;
}

int dynamicdsp_setsubassoc(t_patcher *p, t_dynamicdsp *x)
{
	t_object *assoc;
	
	object_method(p, ps_getassoc, &assoc);
	if (!assoc) 
		object_method(p, ps_setassoc, x);
    // FIX - why?
	//object_method(p, ps_noedit, 1);
	
	return 0;
}

void dynamicdsp_pupdate(t_dynamicdsp *x, void *b, t_patcher *p)				// broken in Max 4 due to renaming......
{
	// Reload the patcher when it's updated
	
	for (long i = 0; i < x->patch_spaces_allocated; i++)
	{
		t_patchspace *patch_space_ptr = x->patch_space_ptrs[i];
		if (patch_space_ptr->the_patch == p) 
			dynamicdsp_loadpatch(x, i, 0, patch_space_ptr->patch_name_in, patch_space_ptr->x_argc, patch_space_ptr->x_argv);
	}
}

void *dynamicdsp_subpatcher(t_dynamicdsp *x, long index, void *arg)
{		
	 if (arg && (t_ptr_uint) arg != 1)
		if (!NOGOOD(arg))								// arg might be good but not a valid object pointer
			if (ob_sym(arg) == ps_dspchain)				// don't report subpatchers to dspchain
				return NULL;

	if (index < x->patch_spaces_allocated)
		if (x->patch_space_ptrs[index]->patch_valid) return x->patch_space_ptrs[index]->the_patch;		// the indexed patcher
            return NULL;
}

void dynamicdsp_parentpatcher(t_dynamicdsp *x, t_patcher **parent)
{
	*parent = x->parent_patch;
}


// ========================================================================================================================================== //
// Patchspace Utilities
// ========================================================================================================================================== //


// Make a new patchspace

t_patchspace *dynamicdsp_new_patch_space(t_dynamicdsp *x,long index)
{
	t_patchspace *patch_space_ptr;
	
	x->patch_space_ptrs[index] = patch_space_ptr = (t_patchspace *)t_getbytes(sizeof(t_patchspace));
	
	dynamicdsp_init_patch_space(patch_space_ptr);
	x->patch_spaces_allocated++;
	
	return patch_space_ptr;
}


// Initialise a patchspace 

void dynamicdsp_init_patch_space(t_patchspace *patch_space_ptr)
{		
	patch_space_ptr->the_patch = NULL;
    patch_space_ptr->the_dspchain = NULL;
	patch_space_ptr->patch_name_in = NULL;
	patch_space_ptr->patch_path = 0;
	patch_space_ptr->patch_valid = false;
	patch_space_ptr->patch_on = false;
	patch_space_ptr->patch_busy = false;
	patch_space_ptr->x_argc = 0;
	patch_space_ptr->out_ptrs = NULL;
		
	patch_space_ptr->processed_flag = 0;
	patch_space_ptr->required_temp_mem_size = 0;
}

// Free the patch and dspchain

void dynamicdsp_free_patch_and_dsp(t_dynamicdsp *x, t_patchspace *patch_space_ptr)
{
	// free old patch and dspchain
	
	if (patch_space_ptr->the_dspchain)
		freeobject((t_object *)patch_space_ptr->the_dspchain);
	
	if (patch_space_ptr->the_patch)
	{
		if (x->num_ins)
            dynamicdsp_patcher_descend(patch_space_ptr->the_patch, (intfunc) dynamicdsp_unlinkinlets, x, x);
		if (x->num_outs)
            dynamicdsp_patcher_descend(patch_space_ptr->the_patch, (intfunc) dynamicdsp_unlinkoutlets, x, x);
		freeobject((t_object *)patch_space_ptr->the_patch);
	}
}


// ========================================================================================================================================== //
// Parent / Child Communication - Routines for owned objects to query the parent
// ========================================================================================================================================== //


// Note that objects wishing to query the parent dynamicdsp~ object should call the functions in dynamicdsp.h.
// These act as suitable wrappers to send the appropriate message to the parent object and returns values as appropriate


////////////////////////////////////////////////// Signal IO Queries //////////////////////////////////////////////////


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
    if (index <= x->patch_spaces_allocated)
        return &x->patch_space_ptrs[index - 1]->out_ptrs;
    else
        return NULL;
}


//////////////////////////////////////////////////// State Queries ////////////////////////////////////////////////////


void *dynamicdsp_client_get_patch_on(t_dynamicdsp *x, t_ptr_int index)
{
    if (index <= x->patch_spaces_allocated)
        return (void *) (t_atom_long) x->patch_space_ptrs[index - 1]->patch_on;
    
    return 0;
}

void *dynamicdsp_client_get_patch_busy(t_dynamicdsp *x, t_ptr_int index)
{
    if (index <= x->patch_spaces_allocated)
        return (void *) (t_atom_long) x->patch_space_ptrs[index - 1]->patch_busy;
    
    return 0;
}

void dynamicdsp_client_set_patch_on(t_dynamicdsp *x, t_ptr_int index, t_ptr_int state)
{
    if (index <= x->patch_spaces_allocated)
        x->patch_space_ptrs[index - 1]->patch_on = state ? true : false;
}

void dynamicdsp_client_set_patch_busy(t_dynamicdsp *x, t_ptr_int index, t_ptr_int state)
{
    if (index <= x->patch_spaces_allocated)
        x->patch_space_ptrs[index - 1]->patch_busy =  state ? true : false;
}


//////////////////////////////////////////////// Temporary Memory Queries ///////////////////////////////////////////////


// dynamicdsp~ provides memory per audio thread for temporary calculations.
// Objects requiring temporary memory during their perform method request a minimum size during their dsp routine
// The pointer should be requested during the perform routine, and should not be stored
// This reduces memory alloaction, and potentially increases speed by keeping temporary memory in the cache


void *dynamicdsp_query_temp_mem(t_dynamicdsp *x, t_ptr_int index)
{
    if (index <= x->patch_spaces_allocated)
        return &x->patch_space_ptrs[index - 1]->temp_mem_ptr;
    else
        return NULL;
}

void *dynamicdsp_client_temp_mem_resize(t_dynamicdsp *x, t_ptr_int index, t_ptr_uint size)
{
    schedule_grow_mem_swap(&x->temp_mem, size, size);
    
    if (index > 0 && index <= x->patch_spaces_allocated)
        x->patch_space_ptrs[index - 1]->required_temp_mem_size = size;
    
    return (void *) 1;
}