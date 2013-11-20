
/*
 *  dynamicserial~
 *
 *	dynamicserial~ is an audio processing object for dynamically loading and managing serially processing audio (or non-audio) patches in realtime, without affecting other patches that are processing.
 *	
 *	It is related to the dynamicdsp~ object and has a similar functionality but is designed for serial rather than parallel processing (and hence is not multithreaded).
 *	There is an associated set of objects for audio input / output and querying and setting patch state (dynamic.in~ / dynamic.out~ / dynamic.request~ / dynamic.this~ / dynamic.patch~).
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

#include <AH_VectorOps.h>
#include <AH_Memory_Swap.h>
#include <AH_Atomic.h>
#include <AH_Max5.h>

#ifndef __APPLE__
#define snprintf _snprintf
#endif 

#ifdef MAX5
#include <jpatcher_api.h>
#endif


// ========================================================================================================================================== //
// Global Varibles
// ========================================================================================================================================== //

void *dynamicserial_class;

static long sig_size;

#define MAX_NUM_PATCHES 4096
#define MAX_ARGS 16

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
	short argc; 
	t_atom *argv;
	
	long index;
	
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
	
	short x_argc;
	t_atom x_argv[MAX_ARGS];
	
	// Pointer to Audio Out Buffers (which are thread dependent)
	
	void **out_ptrs;
	
	// Flags
	
	char patch_valid;
	char patch_on;
	char patch_busy;
		
	// Temporary Memory Variables
	
	long required_temp_mem_size;
	void *temp_mem_ptr;
		
} t_patchspace;


////////////////////////////////////// The object structure //////////////////////////////////////

typedef struct _dynamicserial
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
	
	long declared_sig_ins;
	long declared_sig_outs;
	long declared_temp_buffers;
	long declared_ins;
	long declared_outs;
	
	void **sig_ins;
	void **sig_outs;
	
	void **ins_temp;
	void **temp_buffers1;
	void **temp_buffers2;
	
	t_outvoid *in_table;			// table of non-signal inlets
	t_outvoid *out_table;			// table of non-signal outlets
	long num_proxies;				// number of proxies = MAX(declared_sig_ins, declared_ins)
		
	long temp_buffer_size;
		
	// Temporary Memory Variables
	
	t_safe_mem_swap temp_mem;
	
	
} t_dynamicserial;


// ========================================================================================================================================== //
// Function Prototypes
// ========================================================================================================================================== //


void *dynamicserial_new(t_symbol *s, short argc, t_atom *argv);
void dynamicserial_free(t_dynamicserial *x);
void dynamicserial_assist(t_dynamicserial *x, void *b, long m, long a, char *s);

void dynamicserial_deletepatch (t_dynamicserial *x, t_symbol *msg, short argc, t_atom *argv);
void dynamicserial_deletepatch_internal (t_dynamicserial *x, long index);
void dynamicserial_cleanpatch(t_dynamicserial *x, t_symbol *s, short argc, t_atom *argv);
void dynamicserial_loadexit(t_dynamicserial *x, long replace_symbol_pointers, void *previous, void *previousindex);
void dynamicserial_loadpatch (t_dynamicserial *x, long index,  t_symbol *patch_name_in, short argc, t_atom *argv);
void dynamicserial_user_clear (t_dynamicserial *x);
void dynamicserial_user_loadpatch (t_dynamicserial *x, t_symbol *s, short argc, t_atom *argv);

void dynamicserial_bang(t_dynamicserial *x);
void dynamicserial_int(t_dynamicserial *x, long n);
void dynamicserial_float(t_dynamicserial *x, double f);
void dynamicserial_list(t_dynamicserial *x, t_symbol *s, short argc, t_atom *argv);
void dynamicserial_anything(t_dynamicserial *x, t_symbol *s, short argc, t_atom *argv);
void dynamicserial_target(t_dynamicserial *x, long target_index, long index, t_symbol *msg, short argc, t_atom *argv);
short dynamicserial_targetinlets(t_patcher *p, struct t_args_struct *args);
void dynamicserial_user_target(t_dynamicserial *x, t_symbol *msg, short argc, t_atom *argv);
void dynamicserial_user_target_free(t_dynamicserial *x, t_symbol *msg, short argc, t_atom *argv);

void dynamicserial_free_temp_memory(t_dynamicserial *x, t_symbol *s, short argc, t_atom *argv);
void dynamicserial_sum_float(void **sig_outs, void **current_temp, long declared_sig_outs, long vec_size);
void dynamicserial_sum_double(void **sig_outs, void **current_temp, long declared_sig_outs, long vec_size);
void dynamicserial_perform_common(t_dynamicserial *x);
t_int *dynamicserial_perform(t_int *w);
void dynamicserial_perform64 (t_dynamicserial *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

long dynamicserial_dsp_common(t_dynamicserial *x, long vec_size, long samp_rate);
void dynamicserial_dsp(t_dynamicserial *x, t_signal **sp, short *count);
void dynamicserial_dsp64 (t_dynamicserial *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void dynamicserial_dsp_internal (t_patchspace *patch_space_ptr, long vec_size, long samp_rate);

short dynamicserial_linkinlets(t_patcher *p, t_dynamicserial *x);
short dynamicserial_linkoutlets(t_patcher *p, t_dynamicserial *x);
short dynamicserial_unlinkinlets(t_patcher *p, t_dynamicserial *x);
short dynamicserial_unlinkoutlets(t_patcher *p, t_dynamicserial *x);

void dynamicserial_dblclick(t_dynamicserial *x);
void dynamicserial_open(t_dynamicserial *x, long index);
void dynamicserial_doopen(t_dynamicserial *x, t_symbol *s, short argc, t_atom *argv);
void dynamicserial_pclose(t_dynamicserial *x);
void dynamicserial_wclose(t_dynamicserial *x, long index);
void dynamicserial_dowclose(t_dynamicserial *x, t_symbol *s, short argc, t_atom *argv);

short dynamicserial_patcher_descend(t_patcher *p, fretint fn, void *arg, t_dynamicserial *x);
short dynamicserial_setsubassoc(t_patcher *p, t_dynamicserial *x);
void dynamicserial_pupdate(t_dynamicserial *x, void *b, t_patcher *p);
void *dynamicserial_subpatcher(t_dynamicserial *x, long index, void *arg);
void dynamicserial_parentpatcher(t_dynamicserial *x, t_patcher **parent);

void dynamicserial_init_patch_space (t_patchspace *patch_space_ptrs);
t_patchspace *dynamicserial_new_patch_space(t_dynamicserial *x,long index);
void dynamicserial_free_patch_and_dsp (t_dynamicserial *x, t_patchspace *patch_space_ptrs);

void *dynamicserial_query_declared_sigins(t_dynamicserial *x);
void *dynamicserial_query_declared_sigouts(t_dynamicserial *x);
void *dynamicserial_query_sigins(t_dynamicserial *x);
void *dynamicserial_query_outptrs_ptr(t_dynamicserial *x, long index);
void *dynamicserial_client_get_patch_on (t_dynamicserial *x, long index);
void *dynamicserial_client_get_patch_busy (t_dynamicserial *x, long index);
void dynamicserial_client_set_patch_on (t_dynamicserial *x, long index, long state);
void dynamicserial_client_set_patch_busy (t_dynamicserial *x, long index, long state);
void *dynamicserial_query_temp_mem (t_dynamicserial *x, long index);
void *dynamicserial_client_temp_mem_resize (t_dynamicserial *x, long index, long size);


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


int main(void)
{
	dynamicserial_class = class_new("dynamicserial~",
								 (method)dynamicserial_new, 
								 (method)dynamicserial_free, 
								 sizeof(t_dynamicserial), 
								 NULL, 
								 A_GIMME, 
								 0);
	
	class_addmethod(dynamicserial_class, (method)dynamicserial_dsp, "dsp", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_dsp64, "dsp64", A_CANT, 0);
	
	class_addmethod(dynamicserial_class, (method)dynamicserial_assist, "assist", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_open, "open", A_DEFLONG, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_dblclick, "dblclick", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_wclose, "wclose", A_DEFLONG, 0);
	
	class_addmethod(dynamicserial_class, (method)dynamicserial_pupdate, "pupdate", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_subpatcher, "subpatcher", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_parentpatcher, "parentpatcher", A_CANT, 0);
	
	class_addmethod(dynamicserial_class, (method)dynamicserial_bang, "bang", 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_int, "int", A_LONG, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_float, "float", A_FLOAT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_list, "list", A_GIMME, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_anything, "anything", A_GIMME, 0);
	
	class_addmethod(dynamicserial_class, (method)dynamicserial_user_clear, "clear", 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_user_loadpatch, "loadpatch", A_GIMME, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_deletepatch, "deletepatch", A_GIMME, 0);						// MUST FIX TO GIMME FOR NOW
	class_addmethod(dynamicserial_class, (method)dynamicserial_user_target, "target", A_GIMME, 0);							// MUST FIX TO GIMME FOR NOW
	class_addmethod(dynamicserial_class, (method)dynamicserial_user_target_free, "targetfree", A_GIMME, 0);					// MUST FIX TO GIMME FOR NOW
	
	class_addmethod(dynamicserial_class, (method)dynamicserial_query_declared_sigins, "get_declared_sigins", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_query_declared_sigouts, "get_declared_sigouts", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_query_sigins, "get_sigins", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_query_outptrs_ptr, "get_outptrs_ptr", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_client_get_patch_on, "get_patch_on", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_client_get_patch_busy, "get_patch_busy", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_client_set_patch_on, "set_patch_on", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_client_set_patch_busy, "set_patch_busy", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_query_temp_mem, "get_temp_mem", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_client_temp_mem_resize, "temp_mem_resize", A_CANT, 0);
	
	class_dspinit(dynamicserial_class);
	
	class_register(CLASS_BOX, dynamicserial_class);
	
	ps_getassoc = gensym("getassoc");
	ps_setassoc = gensym("setassoc");
	ps_noedit = gensym("noedit");
	
	ps_dynamicdsp = gensym("___DynamicDSP~___");					// Capitals must stay here (note also that this is currently sharded with dynamic dsp)
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


void *dynamicserial_new(t_symbol *s, short argc, t_atom *argv)
{	
	t_dynamicserial *x = object_alloc (dynamicserial_class);									
	
	t_symbol *patch_name_entered = 0;
	t_symbol *tempsym;
	
	short ac = 0;
	t_atom av[MAX_ARGS];						
	
	long declared_sig_ins = 2;
	long declared_sig_outs = 2;
	long declared_temp_buffers = 0;
	long declared_ins = 2;
	long declared_outs = 2;	
	
	long i;
	
	// Check if there is a patch name given to load
	
	if (argc && argv->a_type == A_SYM && argv->a_w.w_sym != ps_declareio)				
	{
		patch_name_entered = argv->a_w.w_sym;
		argc--; argv++;
	} 
	
	// Check for the declareio symbol and ignore to maintain for old style functionality
	
	if (argc && argv->a_type == A_SYM && argv->a_w.w_sym == ps_declareio)
	{
		argc--; argv++;
	}
	
	// Check if there is a declaration of the number of inputs and outs (message and signal)

	if (argc && argv->a_type == A_LONG)
	{
		if (argv->a_w.w_long >= 0 && argv->a_w.w_long < 256)
			declared_sig_ins = argv->a_w.w_long;
		argc--; argv++;
	}
	if (argc && argv->a_type == A_LONG)
	{
		if (argv->a_w.w_long >= 0 && argv->a_w.w_long < 256)
			declared_sig_outs = argv->a_w.w_long;
		argc--; argv++;
	}
	if (argc && argv->a_type == A_LONG)
	{
		if (argv->a_w.w_long >= 0 && argv->a_w.w_long < 256)
			declared_ins = argv->a_w.w_long;
		argc--; argv++;
	}
	if (argc && argv->a_type == A_LONG)
	{
		if (argv->a_w.w_long >= 0 && argv->a_w.w_long < 256)
			declared_outs = argv->a_w.w_long;
		argc--; argv++;
	}
	
	// Get arguments to patch that is being loaded if there are any
	
	if (argc && argv->a_type == A_SYM) 
	{
		tempsym = argv->a_w.w_sym;
		argc--; argv++;
		if (tempsym == ps_args) 
		{				
			ac = argc;
			if (ac > MAX_ARGS)
				ac = MAX_ARGS;
			for (i = 0; i < ac; i++, argv++)
				av[i] = *argv;
			ac = i;
		}
	}

	// Setup temporary memory and store its current size as a long in the first 32 bits
	//x->temp_mem_size = 0;
	//x->last_temp_mem_size = sizeof(long);
	//x->new_temp_mem_ptr = (t_int32_atomic) (long *) ALIGNED_MALLOC(sizeof(long));
	//*((long *) x->new_temp_mem_ptr) = sizeof(long);	
	
	// Setup temporary memory 
	
	alloc_mem_swap(&x->temp_mem, 0, 0);

	// Set other variables to defaults
	
	x->declared_sig_ins = declared_sig_ins;
	x->declared_sig_outs = declared_sig_outs;
	
	if (declared_sig_ins > declared_sig_outs)
		declared_temp_buffers = declared_sig_ins;
	else
		declared_temp_buffers = declared_sig_outs;
		
	x->declared_temp_buffers = declared_temp_buffers;
	
	x->declared_ins = declared_ins;
	x->declared_outs = declared_outs;
	
	x->patch_spaces_allocated = 0;
	x->target_index = 0;	
	
	x->last_vec_size = 64;
	x->last_samp_rate = 44100;
	
	x->in_table = 0;
	x->out_table = 0;
	
	x->patch_is_loading = 0;
	
	// Create signal in/out buffers and zero
	
	x->sig_ins = (void **) ALIGNED_MALLOC (declared_sig_ins * sizeof(void *));
	x->sig_outs = (void **) ALIGNED_MALLOC (declared_sig_outs * sizeof(void *));
	
	x->ins_temp = (void **) ALIGNED_MALLOC (declared_temp_buffers * sizeof(void *));
	x->temp_buffers1 = (void **) ALIGNED_MALLOC (declared_temp_buffers * sizeof(void *));
	x->temp_buffers2 = (void **) ALIGNED_MALLOC (declared_temp_buffers * sizeof(void *));
	
	for (i = 0; i < declared_sig_ins; i++) 
		x->sig_ins[i] = 0;
	for (i = 0; i < declared_sig_outs; i++) 
		x->sig_outs[i] = 0;
	for (i = 0; i < declared_temp_buffers; i++) 
		x->ins_temp[i] = x->temp_buffers1[i] = x->temp_buffers2[i] = 0;

	// Make non-signal outlets first
	
	if (declared_outs) 
	{
		x->out_table = (t_outvoid *) t_getbytes(declared_outs * sizeof(t_outvoid));
		for (i = declared_outs - 1; i >= 0; i--)
			x->out_table[i] = outlet_new((t_object *)x, 0);	
	}
	
	// Make non-signal inlets 
	
	if (declared_ins) 
	{
		x->in_table = (t_outvoid *)t_getbytes(declared_ins * sizeof(t_outvoid));
		for (i = 0; i < declared_ins; i++)
			x->in_table[i] = outlet_new(0L, 0L);												// make generic unowned inlets
	}
	
	// Make signal ins
	
	x->num_proxies = (declared_sig_ins > declared_ins) ? declared_sig_ins : declared_ins;
	
	dsp_setup((t_pxobject *) x, x->num_proxies);
	x->x_obj.z_misc = Z_NO_INPLACE;																// due to output zeroing!!
	
	// Make signal outs
	
	for (i = 0; i < declared_sig_outs; i++)
		outlet_new((t_object *)x, "signal");
	
	// Initialise patcher symbol
	
	x->parent_patch = (t_patcher *)gensym("#P")->s_thing;										// store reference to parent patcher
	
	// Load patch and initialise
	
	if (patch_name_entered) 
		dynamicserial_loadpatch(x, 0, patch_name_entered, ac, av);
	
	return (x);
}

void dynamicserial_free(t_dynamicserial *x)
{
	t_patchspace *patch_space_ptr;
	long i;
	
	dsp_free((t_pxobject *)x);
	
	// Free patches
	
	for (i = 0; i < x->patch_spaces_allocated; i++)
	{
		patch_space_ptr = x->patch_space_ptrs[i];
		dynamicserial_free_patch_and_dsp (x, patch_space_ptr);
		
		if (patch_space_ptr)
			freebytes((char *) patch_space_ptr, sizeof(t_patchspace));
	}
	
	// Free other resources
	
	free_mem_swap(&x->temp_mem);
	//ALIGNED_FREE(x->temp_mem_ptr);
	
	if (x->declared_sig_ins)
		ALIGNED_FREE(x->sig_ins);
	
	if (x->declared_sig_outs)
		ALIGNED_FREE(x->sig_outs);
	
	if (x->declared_temp_buffers)
	{
		ALIGNED_FREE(x->ins_temp);
		ALIGNED_FREE(x->temp_buffers1);
		ALIGNED_FREE(x->temp_buffers2);
	}
	
	for (i = 0; i < x->declared_ins; i++)
		freeobject(x->in_table[i]);
	
	if (x->in_table)
		freebytes(x->in_table, x->declared_ins * sizeof(t_outvoid));
	
	if (x->out_table)
		freebytes(x->out_table, x->declared_outs * sizeof(t_outvoid));
}

void dynamicserial_assist(t_dynamicserial *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_OUTLET)
	{
		if (a <	x->declared_sig_outs)
			sprintf(s,"Signal Out %ld", a + 1); 
		else
			sprintf(s,"Message Out %ld", a - x->declared_sig_outs + 1); 
	}
	else 
		sprintf(s,"Signal / Message In %ld", a + 1);
}


// ========================================================================================================================================== //
// Patcher Loading / Deleting
// ========================================================================================================================================== //


void dynamicserial_deletepatch (t_dynamicserial *x, t_symbol *msg, short argc, t_atom *argv)
{
	dynamicserial_deletepatch_internal(x, argc ? atom_getlong(argv) - 1 : -1); 
}

void dynamicserial_deletepatch_internal (t_dynamicserial *x, long index)
{
	t_patchspace *patch_space_ptr;
	t_atom a;
	SETLONG(&a, index);
	
	if (index < 0 || index >= x->patch_spaces_allocated)
	{
		AH_error(x, "patch index out of range"); 
		return;
	}
	
	patch_space_ptr = x->patch_space_ptrs[index];
	patch_space_ptr->patch_valid = 0;
	
	defer(x,(method)dynamicserial_cleanpatch, 0L, 1, &a);
}

void dynamicserial_cleanpatch (t_dynamicserial *x, t_symbol *s, short argc, t_atom *argv)
{
	t_patchspace *patch_space_ptr;
	long index = argv->a_w.w_long;
	
	// First close the window
	
	dynamicserial_dowclose(x, 0L, 1, argv);
	
	// Now free
	
	patch_space_ptr = x->patch_space_ptrs[index];
	dynamicserial_free_patch_and_dsp (x, patch_space_ptr);
	dynamicserial_init_patch_space(patch_space_ptr);
}

void dynamicserial_loadexit(t_dynamicserial *x, long replace_symbol_pointers, void *previous, void *previousindex)
{
	if (replace_symbol_pointers)
	{
		ps_dynamicdsp->s_thing = previous;	
		ps_DynamicPatchIndex->s_thing = previousindex;
	}
	ATOMIC_DECREMENT_BARRIER(&x->patch_is_loading);
}

void dynamicserial_loadpatch (t_dynamicserial *x, long index, t_symbol *patch_name_in, short argc, t_atom *argv)
{
	t_patchspace *patch_space_ptr = 0;
	t_object *previous;
	t_object *previousindex;
	
	long validtypes[3];
	
	long num_valid_types;
	long patch_spaces_allocated = x->patch_spaces_allocated;
	long type;
	long i;
	
	short patch_path;
	short saveloadupdate;
	char patch_name[256];
	char windowname[280];
	void *p;

	// Set the valid types to test for (note that JSON will only be used in the Max 5 version)
	
	validtypes[0] = FOUR_CHAR_CODE('maxb');
	validtypes[1] = FOUR_CHAR_CODE('TEXT');
	validtypes[2] = FOUR_CHAR_CODE('JSON');
	
#ifndef MAX5
	num_valid_types = 2;
#else
	num_valid_types = 3;
#endif
		
	// Check that this object is not loading in another thread
	
	if (ATOMIC_INCREMENT_BARRIER(&x->patch_is_loading) > 1)
	{
		AH_error(x, "patch is loading in another thread");
		dynamicserial_loadexit(x, 0, 0, 0);
		return;
	}
	
	// Find a free patch if no index is given
	
	if (index < 0)
	{
		for (index = 0; index < patch_spaces_allocated; index++)
			if (x->patch_space_ptrs[index]->the_patch == 0) 
				break;
	}
	
	// Check that the index is valid
	
	if (index >= MAX_NUM_PATCHES) 
	{
		AH_error(x, "slot out of range");
		dynamicserial_loadexit(x, 0, 0, 0);
		return;
	}
		
	// Create patchspaces up until the last allocated index (if necessary) and store the pointer
	
	for (i = patch_spaces_allocated; i < index + 1; i++)
		dynamicserial_new_patch_space (x, i);
	
	patch_space_ptr = x->patch_space_ptrs[index];
	
	// Free the old patch - the new patch is not yet valid, but we switch it on so it can be switched off at loadbang time
	
	patch_space_ptr->patch_valid = 0;
	dynamicserial_free_patch_and_dsp (x, patch_space_ptr);
	dynamicserial_init_patch_space(patch_space_ptr);
	patch_space_ptr->patch_on = 1;
	
	// Bind to the loading symbols and store the old symbols
	
	previous = ps_dynamicdsp->s_thing;
	previousindex = ps_DynamicPatchIndex->s_thing;
	
	ps_dynamicdsp->s_thing = (t_object *) x;										
	ps_DynamicPatchIndex->s_thing = (t_object *) (index + 1);						
	
	// Try to locate a file of the given name that is of the correct type
	
	strcpy(patch_name, patch_name_in->s_name);
	
	if (locatefile_extended(patch_name, &patch_path, &type, validtypes, num_valid_types))		
	{
		AH_error(x, "no patcher %s", patch_name);
		dynamicserial_loadexit(x, 1, previous, previousindex);
		return;
	}
	
	// Check the number of rarguments (only up to 16 allowed right now)
	
	if (argc > MAX_ARGS)
		argc = MAX_ARGS;
	
	// Load the patch (don't interrupt dsp)
	
	saveloadupdate = dsp_setloadupdate(false);
	p = intload(patch_name, patch_path, 0 , argc, argv, false);
	dsp_setloadupdate(saveloadupdate);
	
	// Check something has loaded
	
	if (!p) 
	{
		AH_error(x, "error loading %s", patch_name);
		dynamicserial_loadexit(x, 1, previous, previousindex);
		return;
	}
	
	// Check that it is a patcher that has loaded
	
	if (!ispatcher(p)) 
	{
		AH_error(x, "%s is not a patcher", patch_name);
		freeobject((t_object *)p);
		dynamicserial_loadexit(x, 1, previous, previousindex);
		return;
	}
	
	// Change the window name
	
	snprintf(windowname, 256, "%s %s%ld%s", patch_name, "(", index + 1, ")");									
#ifndef MAX5
	wind_settitle(((t_patcher *)p)->p_wind, windowname, 0);
#else
	jpatcher_set_title(p, gensym(windowname));
#endif
	
	// Set the relevant associations (for Max 5 the dynamicserial_setsubassoc call covers all of this)
	
#ifndef MAX5
	((t_wind *)((t_patcher *)p)->p_wind)->w_refcount = 1;										// set subpatcher flag: no free on close
#endif
	dynamicserial_patcher_descend((t_patcher *)p, (fretint) dynamicserial_setsubassoc, x, x);			// associate subpatches with this instance
	
	// Link inlets and outlets
	
	if (x->declared_ins) 
		dynamicserial_patcher_descend((t_patcher *)p, (fretint) dynamicserial_linkinlets, x, x);
	if (x->declared_outs) 
		dynamicserial_patcher_descend((t_patcher *)p, (fretint) dynamicserial_linkoutlets, x, x);
	
	// Copy all the relevant data into the patch space
	
	patch_space_ptr->the_patch = p;
	patch_space_ptr->patch_name_in = patch_name_in;
	
	strcpy(patch_space_ptr->patch_name, patch_name);									
	patch_space_ptr->patch_path = patch_path;	
	
	patch_space_ptr->x_argc = argc;
	for (i = 0; i < argc; i++) 
		patch_space_ptr->x_argv[i] = argv[i];
	
	// Compile the dspchain in case dsp is on
	
	dynamicserial_dsp_internal(patch_space_ptr, x->last_vec_size, x->last_samp_rate);
	
	// The patch is valid and ready to go
	
	patch_space_ptr->patch_valid = 1;
	
	// Return to previous state
		
	dynamicserial_loadexit(x, 1, previous, previousindex);
}

void dynamicserial_user_clear (t_dynamicserial *x)
{
	long i;
	
	for (i = 0; i < x->patch_spaces_allocated; i++)
		dynamicserial_deletepatch_internal (x, i);
}

void dynamicserial_user_loadpatch (t_dynamicserial *x, t_symbol *s, short argc, t_atom *argv)
{
	t_symbol *patch_name_entered = 0;
	
	long index = -1;
	
	// Get requested patch index if there is one
	
	if (argc && argv->a_type == A_LONG)
	{
		index = argv->a_w.w_long - 1;
		if (index >= MAX_NUM_PATCHES) 
		{
			AH_error(x, "patch index out of range");
			return;
		}
		argc--; argv++;
	}
	
	// If there is a patch name then try to load the patch
	
	if (argc && argv->a_type == A_SYM)			
	{
		patch_name_entered = argv->a_w.w_sym;
		argc--; argv++;
		
		dynamicserial_loadpatch (x, index, patch_name_entered, argc, argv);
	} 
	else 
		AH_error(x, "no patch specified");
}

// ========================================================================================================================================== //
// Messages in passed on to the patcher via the "in" objects / Voice targeting
// ========================================================================================================================================== //


void dynamicserial_bang(t_dynamicserial *x)
{	
	long index = PROXY_GETINLET((t_object *)x);	
	long target_index = x->target_index;

	if (index >= x->declared_ins)
		return;
	
	if (target_index)
		dynamicserial_target(x, target_index, index, ps_bang, 0, 0);
	else
		outlet_bang(x->in_table[index]);
}

void dynamicserial_int(t_dynamicserial *x, long n)
{
	long index = PROXY_GETINLET((t_object *)x);	// proxy index
	long target_index = x->target_index;

	if (index >= x->declared_ins)
		return;		
	
	if (target_index)
	{
		t_atom n_atom; 
		SETLONG (&n_atom, n);
		dynamicserial_target(x, target_index, index, ps_int, 1, &n_atom);
	}
	else
		outlet_int(x->in_table[index], n);
}

void dynamicserial_float(t_dynamicserial *x, double f)
{
	long index = PROXY_GETINLET((t_object *)x);	// proxy index
	long target_index = x->target_index;

	if (index >= x->declared_ins)
		return;
	
	if (target_index)
	{
		t_atom f_atom; 
		SETFLOAT (&f_atom, f);
		dynamicserial_target(x, target_index, index, ps_float, 1, &f_atom);
	}
	else
		outlet_float(x->in_table[index], f);
}

void dynamicserial_list(t_dynamicserial *x, t_symbol *s, short argc, t_atom *argv)
{
	long index = PROXY_GETINLET((t_object *)x);	// proxy index
	long target_index = x->target_index;

	if (index >= x->declared_ins)
		return;
	
	if (target_index)
		dynamicserial_target(x, target_index, index, ps_list, argc, argv);
	else
		outlet_list(x->in_table[index], ps_list, argc, argv);
}

void dynamicserial_anything(t_dynamicserial *x, t_symbol *s, short argc, t_atom *argv)
{
	long index = PROXY_GETINLET((t_object *)x);	// proxy index
	long target_index = x->target_index;

	if (index >= x->declared_ins)
		return;		
	
	if (target_index)
		dynamicserial_target(x, target_index, index, s, argc, argv);
	else
		outlet_anything(x->in_table[index], s, argc, argv);
}

void dynamicserial_target(t_dynamicserial *x, long target_index, long index, t_symbol *msg, short argc, t_atom *argv)
{	
	struct t_args_struct pass_args;
	
	pass_args.msg = msg;
	pass_args.argc = argc;
	pass_args.argv = argv;
	pass_args.index = index + 1;
	
	if (target_index >= 1 && target_index <= x->patch_spaces_allocated)
	{
		t_patcher *p = x->patch_space_ptrs[target_index - 1]->the_patch;
		
		if (x->patch_space_ptrs[target_index - 1]->patch_valid)
			dynamicserial_patcher_descend(p, (fretint) dynamicserial_targetinlets, &pass_args, x);
	}
}

#ifndef MAX5

// - inlet and outlet linking using the in and out objects

short dynamicserial_targetinlets(t_patcher *p, struct t_args_struct *args)
{
	t_box *b;
	t_inout *io;	
	void *outletptr;
	
	for (b = p->p_box; b; b = b->b_next) 
	{
		if (b->b_firstin && ob_sym(b->b_firstin) == ps_in) 
		{
			io = (t_inout *) b->b_firstin;
			if (io->s_index == args->index)
			{
				outletptr = io->s_obj.o_outlet;
				outlet_anything (outletptr, args->msg, args->argc, args->argv);
			}
		}
    }
	return (0);
}

#else

// - inlet and outlet linking using the in and out objects

short dynamicserial_targetinlets(t_patcher *p, struct t_args_struct *args)
{
	t_box *b;
	t_inout *io;
	void *outletptr;
	
	for (b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b)) 
	{
		if (jbox_get_maxclass(b) == ps_in) 
		{
			io = (t_inout *) jbox_get_object(b);
			if (io->s_index == args->index)
			{
				outletptr = io->s_obj.o_outlet;
				outlet_anything (outletptr, args->msg, args->argc, args->argv);
			}
		}
    }
	return (0);
}

#endif

void dynamicserial_user_target(t_dynamicserial *x, t_symbol *msg, short argc, t_atom *argv)
{
	long target_index = argc ? atom_getlong(argv) : 0;
	
	x->target_index = target_index;
}

void dynamicserial_user_target_free(t_dynamicserial *x, t_symbol *msg, short argc, t_atom *argv)
{
	t_patchspace **patch_space_ptrs = x->patch_space_ptrs;
	
	long val1 = argc > 0 ? atom_getlong(argv + 0) : 0;
	long val2 = argc > 1 ? atom_getlong(argv + 1) : 0;
	long i, from, to;
	long target = 0;
	long patch_spaces_allocated = x->patch_spaces_allocated;
	
	if (val1 > patch_spaces_allocated) 
		val1 = patch_spaces_allocated;
	if (val2 > patch_spaces_allocated) 
		val2 = patch_spaces_allocated;
	
	if (val1 || val2)
	{
		if (val2) 
		{
			from = val1;
			to = val2;
		}
		else
		{
			from = 1;
			to = val1;
		}
	}
	else
	{
		from = 1;
		to = patch_spaces_allocated;
	}
	
	if (from < 1) from = 1;
	
	for (i = from - 1; i < to; i++)
	{
		if (patch_space_ptrs[i]->patch_valid && !patch_space_ptrs[i]->patch_busy)
		{
			target = i + 1;
			break;
		}
	}
	
	if (target) 
		x->target_index = target;
	else 
		x->target_index = -1;
}


// ========================================================================================================================================== //
// Perform and DSP Routines
// ========================================================================================================================================== //


void dynamicserial_free_temp_memory(t_dynamicserial *x, t_symbol *s, short argc, t_atom *argv)
{
	ALIGNED_FREE((void *)s);
}



void dynamicserial_sum_float(void **sig_outs, void **current_temp, long declared_sig_outs, long vec_size)
{
	float *io_pointer1;
	float *io_pointer2;
	long i, j;
	
	// Sum output of threads for each signal outlet
	
	for (i = 0; i < declared_sig_outs; i++)
	{
		for (io_pointer1 = (float *) sig_outs[i], io_pointer2 = (float *) current_temp[i], j = 0; j < vec_size; j++)
			*(io_pointer1++) = *(io_pointer2++);
	}
}


void dynamicserial_sum_double(void **sig_outs, void **current_temp, long declared_sig_outs, long vec_size)
{
	double *io_pointer1;
	double *io_pointer2;
	long i, j;
	
	// Sum output of threads for each signal outlet
	
	for (i = 0; i < declared_sig_outs; i++)
	{
		for (io_pointer1 = (double *) sig_outs[i], io_pointer2 = (double *) current_temp[i], j = 0; j < vec_size; j++)
			*(io_pointer1++) = *(io_pointer2++);
	}
}


void dynamicserial_perform_common(t_dynamicserial *x)
{	
	void **sig_ins = x->sig_ins;
	void **sig_outs = x->sig_outs;
	void **temp_buffers1 = x-> temp_buffers1;
	void **temp_buffers2 = x-> temp_buffers2;
	void **current_temp = temp_buffers1;
	void **ins_temp = x->ins_temp;
	
	long flip = 0;
	long declared_sig_ins = x->declared_sig_ins;
	long declared_sig_outs = x->declared_sig_outs;
	long declared_temp_buffers = x->declared_temp_buffers;
	long vec_size = x->last_vec_size;
	long i, j;
	
	t_patchspace **patch_space_ptrs = x->patch_space_ptrs;
	t_patchspace *next_patch_space_ptr = 0;
	
	t_dspchain *next_dspchain = 0;
	
	long patch_spaces_allocated = x->patch_spaces_allocated; 
		
	void *temp_mem_ptr;
	long temp_mem_size;
	
#if defined( __i386__ ) || defined( __x86_64__ )
	int oldMXCSR;						
	int newMXCSR;
#endif
	
	// Zero Outputs
	
	for (i = 0; i < declared_sig_outs; i++) 
		memset(sig_outs[i], 0, sig_size * vec_size);
	
	if (x->x_obj.z_disabled)
		return;
	
	// Update the temporary memory if relevant
	
	attempt_mem_swap(&x->temp_mem);
	temp_mem_ptr = x->temp_mem.current_ptr;
	temp_mem_size = x->temp_mem.current_size;
	
	// Turn off denormals
	
#if defined( __i386__ ) || defined( __x86_64__ )
	oldMXCSR = _mm_getcsr();						// read the old MXCSR setting 
	newMXCSR = oldMXCSR | 0x8040;					// set DAZ and FZ bits 
	_mm_setcsr( newMXCSR );								// write the new MXCSR setting to the MXCSR 
#endif
	
	// Copy inputs in

	for (i = 0; i < declared_sig_ins; i++)
		memcpy(temp_buffers1[i], sig_ins[i], sig_size * vec_size);
	for (; i < declared_sig_outs; i++)
		memset(temp_buffers1[i], 0, sig_size * vec_size);
	
	// Here we run each patch
	
	for (i = 0; i < patch_spaces_allocated; i++)
	{
		next_patch_space_ptr = patch_space_ptrs[i];
		next_dspchain = next_patch_space_ptr->the_dspchain;
		
		if (next_patch_space_ptr->patch_valid && next_patch_space_ptr->patch_on && next_dspchain && next_patch_space_ptr->required_temp_mem_size <= temp_mem_size)
		{							
			for (j = 0; j < declared_temp_buffers; j++)
				ins_temp[j] = current_temp[j];
				
			// Point to thread buffers and run DSP
			
			if (!flip) 
				current_temp = temp_buffers2;
			else 
				current_temp = temp_buffers1;

			// Clear current output buffers
			
			for (j = 0; j < declared_temp_buffers; j++)
				memset(current_temp[j], 0, sig_size * vec_size);
			
			next_patch_space_ptr->out_ptrs = current_temp;
			next_patch_space_ptr->temp_mem_ptr = temp_mem_ptr;
			
			dspchain_tick(next_dspchain);
			
			flip = 1 - flip;
		}
	}
	
	// Copy outputs out
	
	if (sig_size == sizeof(double))
		dynamicserial_sum_double(sig_outs, current_temp, declared_sig_outs, vec_size);
	else
		dynamicserial_sum_float(sig_outs, current_temp, declared_sig_outs, vec_size);
	
	// Return denormals to previous state 
	
#if defined( __i386__ ) || defined( __x86_64__ )	
	_mm_setcsr(oldMXCSR);	
#endif
}


t_int *dynamicserial_perform(t_int *w)
{
	dynamicserial_perform_common((t_dynamicserial *) w[1]);
	
	return w + 2;	
}


void dynamicserial_perform64 (t_dynamicserial *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{		
	long i;
	
	for (i = 0; i < x->declared_sig_ins; i++)
		x->sig_ins[i] = ins[i];
	for (i = 0; i < x->declared_sig_outs; i++)
		x->sig_outs[i] = outs[i];
	
	dynamicserial_perform_common(x);
}


long dynamicserial_dsp_common(t_dynamicserial *x, long vec_size, long samp_rate)
{
	t_patchspace *patch_space_ptr;
	long mem_fail = 0;
	long i;
			
	// Do internal dsp compile (for each valid patch)
	
	for (i = 0; i < x->patch_spaces_allocated; i++)
	{
		patch_space_ptr = x->patch_space_ptrs[i];
		if (patch_space_ptr->patch_valid)
			dynamicserial_dsp_internal (patch_space_ptr, vec_size, samp_rate);
	}
	
	for (i = 0; i < x->declared_temp_buffers; i++)
	{
		ALIGNED_FREE(x->temp_buffers1[i]);
		ALIGNED_FREE(x->temp_buffers2[i]);
		
		x->temp_buffers1[i] = (void *) ALIGNED_MALLOC(sig_size * vec_size);
		x->temp_buffers2[i] = (void *) ALIGNED_MALLOC(sig_size * vec_size);
		
		if (!x->temp_buffers1[i] || !x->temp_buffers2[i])
			mem_fail = 1;
	}
		
	x->last_vec_size = vec_size;
	x->last_samp_rate = samp_rate;
	
	return mem_fail;
}


void dynamicserial_dsp(t_dynamicserial *x, t_signal **sp, short *count)
{
	long num_proxies = x->num_proxies;
	long i;
	
	// Copy in and out pointers
	
	for (i = 0; i < x->declared_sig_ins; i++) 
		x->sig_ins[i] = sp[i]->s_vec;
	for (i = 0; i < x->declared_sig_outs; i++)
		x->sig_outs[i] = sp[i + num_proxies]->s_vec;
	
	// Add to dsp if common routine successful
	
	if (!dynamicserial_dsp_common(x, sp[0]->s_n, sp[0]->s_sr))
		dsp_add(dynamicserial_perform, 1, x);
}


void dynamicserial_dsp64 (t_dynamicserial *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	// Add to dsp if common routine successful
	
	if (!dynamicserial_dsp_common(x, maxvectorsize, samplerate))
		object_method(dsp64, gensym("dsp_add64"), x, dynamicserial_perform64, 0, NULL);
}


void dynamicserial_dsp_internal (t_patchspace *patch_space_ptr, long vec_size, long samp_rate)
{
	// Free the old dspchain
		
	if (patch_space_ptr->the_dspchain)
		freeobject((t_object *)patch_space_ptr->the_dspchain);
	
	// Recompile 
	
	patch_space_ptr->the_dspchain = dspchain_compile(patch_space_ptr->the_patch, vec_size, samp_rate);	
}


// ========================================================================================================================================== //
// Patcher Link Inlets / Outlets
// ========================================================================================================================================== //

// There are two sets of routines here - one for the Max 4 SDK and one for the Max 5 SDK

#ifndef MAX5

// - inlet and outlet linking using the in and out objects

short dynamicserial_linkinlets(t_patcher *p, t_dynamicserial *x)
{
	t_box *b;
	t_inout *io;
	
	for (b = p->p_box; b; b = b->b_next) 
	{
		if (b->b_firstin && ob_sym(b->b_firstin) == ps_in) 
		{
			io = (t_inout *)b->b_firstin;
			if (io->s_index <= x->declared_ins) outlet_add(x->in_table[io->s_index - 1], (struct inlet *)io->s_obj.o_outlet);
		}
    }
	return (0);
}

short dynamicserial_linkoutlets(t_patcher *p, t_dynamicserial *x)
{
	t_box *b;
	t_inout *io;
	
	for (b = p->p_box; b; b = b->b_next) 
	{
		if (b->b_firstin && ob_sym(b->b_firstin) == ps_out) 
		{
			io = (t_inout *)b->b_firstin;
			if (io->s_index <= x->declared_outs) outlet_add(io->s_outlet, x->out_table[io->s_index - 1]);
		}
    }
	return (0);
}

// - inlet and outlet removal using the in and out objects

short dynamicserial_unlinkinlets(t_patcher *p, t_dynamicserial *x)
{
	t_box *b;
	t_inout *io;
	
	for (b = p->p_box; b; b = b->b_next) 
	{
		if (b->b_firstin && ob_sym(b->b_firstin) == ps_in) 
		{
			io = (t_inout *)b->b_firstin;
			if (io->s_index <= x->declared_ins)
				outlet_rm(x->in_table[io->s_index - 1], (struct inlet *)io->s_obj.o_outlet);
		}
    }
	return (0);
}

short dynamicserial_unlinkoutlets(t_patcher *p, t_dynamicserial *x)
{
	t_box *b;
	t_inout *io;
	
	for (b = p->p_box; b; b = b->b_next) 
	{
		if (b->b_firstin && ob_sym(b->b_firstin) == ps_out) 
		{
			io = (t_inout *)b->b_firstin;
			if (io->s_index <= x->declared_outs)
				outlet_rm(io->s_outlet, x->out_table[io->s_index - 1]);	
		}
    }
	return (0);
}

#else

// - inlet and outlet linking using the in and out objects

short dynamicserial_linkinlets(t_patcher *p, t_dynamicserial *x)
{
	t_box *b;
	t_inout *io;
	
	for (b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b)) 
	{
		if (jbox_get_maxclass(b) == ps_in) 
		{
			io = (t_inout *) jbox_get_object(b);
			if (io->s_index <= x->declared_ins) outlet_add(x->in_table[io->s_index - 1], (struct inlet *)io->s_obj.o_outlet);
		}
    }
	return (0);
}

short dynamicserial_linkoutlets(t_patcher *p, t_dynamicserial *x)
{
	t_box *b;
	t_inout *io;
	
	for (b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
	{
		if (jbox_get_maxclass(b) == ps_out) 
		{
			io = (t_inout *) jbox_get_object(b);
			if (io->s_index <= x->declared_outs) outlet_add(io->s_outlet, x->out_table[io->s_index - 1]);
		}
    }
	return (0);
}

// - inlet and outlet removal using the in and out objects

short dynamicserial_unlinkinlets(t_patcher *p, t_dynamicserial *x)
{
	t_box *b;
	t_inout *io;
	
	for (b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b)) 
	{
		if (jbox_get_maxclass(b) == ps_in) 
		{
			io = (t_inout *) jbox_get_object(b);
			if (io->s_index <= x->declared_ins) outlet_rm(x->in_table[io->s_index - 1], (struct inlet *)io->s_obj.o_outlet);
		}
    }
	return (0);
}

short dynamicserial_unlinkoutlets(t_patcher *p, t_dynamicserial *x)
{
	t_box *b;
	t_inout *io;
	
	for (b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
	{
		if (jbox_get_maxclass(b)  == ps_out) 
		{
			io = (t_inout *) jbox_get_object(b);
			if (io->s_index <= x->declared_outs) outlet_rm(io->s_outlet, x->out_table[io->s_index - 1]);
		}
    }
	return (0);
}

#endif


// ========================================================================================================================================== //
// Patcher Window stuff
// ========================================================================================================================================== //


void dynamicserial_dblclick(t_dynamicserial *x)
{
	long i;
	
	for (i = 0; i < x->patch_spaces_allocated; i++)
	{
		if (x->patch_space_ptrs[i]->the_patch)
		{
			dynamicserial_open(x, i + 1);
			break;
		}
	}
}

void dynamicserial_open(t_dynamicserial *x, long index)
{
	t_atom a;
	SETLONG (&a, index - 1);
	
	if (index < 1) return;
	if (index > x->patch_spaces_allocated) return;
	if (!x->patch_space_ptrs[index - 1]->patch_valid) return;
	
	defer(x,(method)dynamicserial_doopen, 0L, 1, &a);
}

void dynamicserial_doopen(t_dynamicserial *x, t_symbol *s, short argc, t_atom *argv)
{
	long index = argv->a_w.w_long;
	
	if (x->patch_space_ptrs[index]->the_patch) 
		mess0((t_object *)x->patch_space_ptrs[index]->the_patch, ps_front);		// this will always do the right thing
}

void dynamicserial_pclose(t_dynamicserial *x)
{
	// do Nothing
}

void dynamicserial_wclose(t_dynamicserial *x, long index)
{
	t_atom a;
	SETLONG (&a, index - 1);
	
	defer(x,(method)dynamicserial_dowclose, 0L, 1, &a);
}

void dynamicserial_dowclose(t_dynamicserial *x, t_symbol *s, short argc, t_atom *argv)
{
	long index = argv->a_w.w_long;
	
	if (index < 0) return;
	if (index >= x->patch_spaces_allocated) return;
	if (!x->patch_space_ptrs[index]->patch_valid) return;
	
	if (x->patch_space_ptrs[index]->the_patch) 
#ifndef MAX5
		wind_invis((t_wind *)x->patch_space_ptrs[index]->the_patch->p_wind);
#else
	object_method(x->patch_space_ptrs[index]->the_patch, gensym("wclose"), x);
#endif
}


// ========================================================================================================================================== //
// Patcher Utilities (these deal with various updating and necessary behind the scens state stuff)
// ========================================================================================================================================== //


short dynamicserial_patcher_descend(t_patcher *p, fretint fn, void *arg, t_dynamicserial *x)
{
	t_box *b;
	t_patcher *p2;
	long index;
	
#ifndef MAX5
	if (p->p_assoc && p->p_assoc != x) 
		return 0;										// Avoid recursion into a poly / pfft / dynamicserial~
#else
	t_object *assoc = 0;
	object_method(p, ps_getassoc, &assoc);				// Avoid recursion into a poly / pfft / dynamicserial~
	if (assoc && (t_dynamicserial *) assoc != x) 
		return 0;
#endif
	// CHANGED - DO NOT PASS x AS ARG
	if ((*fn)(p, arg))
		return (1);
	
#ifndef MAX5
	for (b = p->p_box; b; b = b->b_next) 
	{
		if (b->b_firstin) 
		{
			index = 0;
			while (p2 = object_subpatcher(b->b_firstin, &index, arg))
			{
				if (dynamicserial_patcher_descend(p2, fn, arg, x))
					return 1;
			}
		}
	}
#else
	for (b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))  
	{
		if (b) 
		{
			index = 0;
			while (p2 = object_subpatcher(jbox_get_object(b), &index, arg))
				if (dynamicserial_patcher_descend(p2, fn, arg, x))
					return 1;
		}
	}
#endif
	return (0);
}

short dynamicserial_setsubassoc(t_patcher *p, t_dynamicserial *x)
{
#ifndef MAX5
	if (!p->p_assoc) 
		p->p_assoc = x;
	p->p_noedit = 1;								
#else
	t_object *assoc;
	
	object_method(p, ps_getassoc, &assoc);
	if (!assoc) 
		object_method(p, ps_setassoc, x);
	object_method(p, ps_noedit, 1);

#endif
	
	return 0;
}

void dynamicserial_pupdate(t_dynamicserial *x, void *b, t_patcher *p)				// broken in Max 4 due to renaming......
{
	t_patchspace *patch_space_ptr;
	long i;
	
	// Reload the patcher when it's updated
	
	for (i = 0; i < x->patch_spaces_allocated; i++)
	{
		patch_space_ptr = x->patch_space_ptrs[i];
		if (patch_space_ptr->the_patch == p) 
			dynamicserial_loadpatch (x, i, patch_space_ptr->patch_name_in, patch_space_ptr->x_argc, patch_space_ptr->x_argv);
	}
}

void *dynamicserial_subpatcher(t_dynamicserial *x, long index, void *arg)
{		
	 if (arg && (long) arg != 1) 
		if (!NOGOOD(arg))								// arg might be good but not a valid object pointer
			if (ob_sym(arg) == ps_dspchain)				// don't report subpatchers to dspchain
				return 0;

	if (index < x->patch_spaces_allocated)
		if (x->patch_space_ptrs[index]->patch_valid) return x->patch_space_ptrs[index]->the_patch;		// the indexed patcher

	return 0;
}


void dynamicserial_parentpatcher(t_dynamicserial *x, t_patcher **parent)
{
	*parent = x->parent_patch;
}

// ========================================================================================================================================== //
// Patchspace Utilities
// ========================================================================================================================================== //


// Make a new patchspace

t_patchspace *dynamicserial_new_patch_space (t_dynamicserial *x,long index)
{
	t_patchspace *patch_space_ptr;
	
	x->patch_space_ptrs[index] = patch_space_ptr = (t_patchspace *)t_getbytes(sizeof(t_patchspace));
	
	dynamicserial_init_patch_space (patch_space_ptr);
	x->patch_spaces_allocated++;
	
	return patch_space_ptr;
}


// Initialise a patchspace 

void dynamicserial_init_patch_space (t_patchspace *patch_space_ptr)
{		
	patch_space_ptr->the_patch = 0;
	patch_space_ptr->patch_name_in = 0;
	patch_space_ptr->patch_path = 0;
	patch_space_ptr->patch_valid = 0;
	patch_space_ptr->patch_on = 0;
	patch_space_ptr->patch_busy = 0;
	patch_space_ptr->the_dspchain = 0;
	patch_space_ptr->x_argc = 0;
	patch_space_ptr->out_ptrs = 0;
		
	patch_space_ptr->required_temp_mem_size = 0;
}

// Free the patch and dspchain

void dynamicserial_free_patch_and_dsp (t_dynamicserial *x, t_patchspace *patch_space_ptr)
{
	// free old patch and dspchain
	
	if (patch_space_ptr->the_dspchain)
		freeobject((t_object *)patch_space_ptr->the_dspchain);
	
	if (patch_space_ptr->the_patch)
	{
		if (x->declared_ins) dynamicserial_patcher_descend(patch_space_ptr->the_patch, (fretint) dynamicserial_unlinkinlets, x, x);
		if (x->declared_outs) dynamicserial_patcher_descend(patch_space_ptr->the_patch, (fretint) dynamicserial_unlinkoutlets, x, x);
		freeobject((t_object *)patch_space_ptr->the_patch);
	}
}


// ========================================================================================================================================== //
// Parent / Child Communication - Routines for owned objects to query the parent
// ========================================================================================================================================== //


// Note that objects wishing to query the parent dynamicserial~ object should call the functions in dynamicserial.h.
// These act as suitable wrappers to send the appropriate message to the parent object and returns values as appropriate


////////////////////////////////////////////////// Signal IO Queries //////////////////////////////////////////////////


void *dynamicserial_query_declared_sigins(t_dynamicserial *x)
{
	return (void *) x->declared_temp_buffers;
}

void *dynamicserial_query_declared_sigouts(t_dynamicserial *x)
{
	return (void *) x->declared_temp_buffers;
}

void *dynamicserial_query_sigins(t_dynamicserial *x)
{
	return (void *) x->ins_temp;
}

void *dynamicserial_query_outptrs_ptr(t_dynamicserial *x, long index)
{
	if (index <= x->patch_spaces_allocated)
		return &x->patch_space_ptrs[index - 1]->out_ptrs;
	else
		return 0;
}


//////////////////////////////////////////////////// State Queries ////////////////////////////////////////////////////


void dynamicserial_client_set_patch_on (t_dynamicserial *x, long index, long state)
{
	if (state) state = 1;
	if (index <= x->patch_spaces_allocated)
		x->patch_space_ptrs[index - 1]->patch_on = state;
}

void dynamicserial_client_set_patch_busy (t_dynamicserial *x, long index, long state)
{
	if (state) state = 1;
	if (index <= x->patch_spaces_allocated)
		x->patch_space_ptrs[index - 1]->patch_busy = state;
}

void *dynamicserial_client_get_patch_on (t_dynamicserial *x, long index)
{
	if (index <= x->patch_spaces_allocated)
		return (void *) (long) x->patch_space_ptrs[index - 1]->patch_on;
	
	return 0;
}

void *dynamicserial_client_get_patch_busy (t_dynamicserial *x, long index)
{
	if (index <= x->patch_spaces_allocated)
		return (void *) (long) x->patch_space_ptrs[index - 1]->patch_busy;
	
	return 0;
}


//////////////////////////////////////////////// Tempory Memory Queries ///////////////////////////////////////////////


// dynamicserial~ provides memory per audio thread for temporary calculations. 
// Objects requiring temporary memory during their perform method request a minimum size during their dsp routine
// The pointer should be requested during the perform routine, and should not be stored
// This reduces memory alloaction, and potentially increases speed by keeping temporary memory in the cache


void *dynamicserial_query_temp_mem (t_dynamicserial *x, long index)
{
	if (index <= x->patch_spaces_allocated)
		return &x->patch_space_ptrs[index - 1]->temp_mem_ptr;
	else
		return 0;
}

void *dynamicserial_client_temp_mem_resize (t_dynamicserial *x, long index, long size)
{	
	schedule_grow_mem_swap(&x->temp_mem, size, size);

	if (index > 0 && index <= x->patch_spaces_allocated)
		x->patch_space_ptrs[index - 1]->required_temp_mem_size = size;
	
	return (void *) 1;
	
}

