
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
#include <jpatcher_api.h>

#include <AH_Atomic.h>
#include <AH_Memory_Swap.h>
#include <AH_Types.h>
#include <AH_VectorOps.h>

#include "PatchSlot.h"

#ifndef __APPLE__
#define snprintf _snprintf
#endif 

// FIX - hack for descending (could use traverse jpatcher system also..)

typedef int (*intfunc) (t_patcher *p, void *arg);

// ========================================================================================================================================== //
// Global Varibles
// ========================================================================================================================================== //


t_class *dynamicserial_class;

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

////////////////////////////////////// The object structure //////////////////////////////////////

typedef struct _dynamicserial
{
    t_pxobject x_obj;
    
	t_patcher *parent_patch;
	
	// Patch Data and Variables 
	
	PatchSlot *patch_slots[MAX_NUM_PATCHES];
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
    long num_temp_buffers;
	
	void **sig_ins;
	void **sig_outs;
	
	void **ins_temp;
	void **temp_buffers1;
	void **temp_buffers2;
	
	t_outvoid *in_table;			// table of non-signal inlets
	t_outvoid *out_table;			// table of non-signal outlets
	long num_proxies;				// number of proxies = MAX(num_sig_ins, num_ins)
		
	long temp_buffer_size;
		
	// Temporary Memory Variables
	
	t_safe_mem_swap temp_mem;
	
	
} t_dynamicserial;


// ========================================================================================================================================== //
// Function Prototypes
// ========================================================================================================================================== //


void *dynamicserial_new(t_symbol *s, long argc, t_atom *argv);
void dynamicserial_free(t_dynamicserial *x);
void dynamicserial_assist(t_dynamicserial *x, void *b, long m, long a, char *s);

void dynamicserial_deletepatch(t_dynamicserial *x, t_symbol *msg, long argc, t_atom *argv);
void dynamicserial_deletepatch_internal(t_dynamicserial *x, t_atom_long index);
void dynamicserial_cleanpatch(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv);
void dynamicserial_loadpatch(t_dynamicserial *x, long index,  t_symbol *patch_name_in, long argc, t_atom *argv);
void dynamicserial_user_clear(t_dynamicserial *x);
void dynamicserial_user_loadpatch(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv);

void dynamicserial_bang(t_dynamicserial *x);
void dynamicserial_int(t_dynamicserial *x, t_atom_long n);
void dynamicserial_float(t_dynamicserial *x, double f);
void dynamicserial_list(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv);
void dynamicserial_anything(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv);
void dynamicserial_target(t_dynamicserial *x, long target_index, long inlet, t_symbol *msg, long argc, t_atom *argv);
int dynamicserial_targetinlets(t_patcher *p, struct t_args_struct *args);
void dynamicserial_user_target(t_dynamicserial *x, t_symbol *msg, long argc, t_atom *argv);
void dynamicserial_user_target_free(t_dynamicserial *x, t_symbol *msg, long argc, t_atom *argv);

void dynamicserial_perform_common(t_dynamicserial *x);
void dynamicserial_denormal_handled_perform(t_dynamicserial *x);
t_int *dynamicserial_perform(t_int *w);
void dynamicserial_perform64(t_dynamicserial *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

AH_Boolean dynamicserial_dsp_common(t_dynamicserial *x, long vec_size, long samp_rate);
void dynamicserial_dsp(t_dynamicserial *x, t_signal **sp, short *count);
void dynamicserial_dsp64(t_dynamicserial *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

int dynamicserial_linkinlets(t_patcher *p, t_dynamicserial *x);
int dynamicserial_linkoutlets(t_patcher *p, t_dynamicserial *x);
int dynamicserial_unlinkinlets(t_patcher *p, t_dynamicserial *x);
int dynamicserial_unlinkoutlets(t_patcher *p, t_dynamicserial *x);

void dynamicserial_dblclick(t_dynamicserial *x);
void dynamicserial_open(t_dynamicserial *x, t_atom_long index);
void dynamicserial_doopen(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv);
void dynamicserial_pclose(t_dynamicserial *x);
void dynamicserial_wclose(t_dynamicserial *x, t_atom_long index);
void dynamicserial_dowclose(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv);

int dynamicserial_patcher_descend(t_patcher *p, intfunc fn, void *arg, t_dynamicserial *x);
void dynamicserial_pupdate(t_dynamicserial *x, void *b, t_patcher *p);
void *dynamicserial_subpatcher(t_dynamicserial *x, long index, void *arg);
void dynamicserial_parentpatcher(t_dynamicserial *x, t_patcher **parent);

void *dynamicserial_query_num_sigins(t_dynamicserial *x);
void *dynamicserial_query_num_sigouts(t_dynamicserial *x);
void *dynamicserial_query_sigins(t_dynamicserial *x);
void *dynamicserial_query_sigouts(t_dynamicserial *x, t_ptr_int index);
void *dynamicserial_client_get_patch_on(t_dynamicserial *x, t_ptr_int index);
void *dynamicserial_client_get_patch_busy(t_dynamicserial *x, t_ptr_int index);
void dynamicserial_client_set_patch_on(t_dynamicserial *x, t_ptr_int index, t_ptr_int state);
void dynamicserial_client_set_patch_busy(t_dynamicserial *x, t_ptr_int index, t_ptr_int state);
void *dynamicserial_query_temp_mem(t_dynamicserial *x, t_ptr_int index);
void *dynamicserial_client_temp_mem_resize(t_dynamicserial *x, t_ptr_int index, t_ptr_uint size);


// ========================================================================================================================================== //
// Symbols
// ========================================================================================================================================== //


t_symbol *ps_dynamicdsp;
t_symbol *ps_DynamicPatchIndex;
t_symbol *ps_dspchain;
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

// ========================================================================================================================================== //
// Main
// ========================================================================================================================================== //


int C74_EXPORT main(void)
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
	
	class_addmethod(dynamicserial_class, (method)dynamicserial_query_num_sigins, "get_num_sigins", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_query_num_sigouts, "get_num_sigouts", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_query_sigins, "get_sigins", A_CANT, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_query_sigouts, "get_sigouts", A_CANT, 0);
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
	
	ps_dynamicdsp = gensym("___DynamicDSP~___");					// Capitals must stay here (note also that this is currently sharded with dynamic dsp)
	ps_DynamicPatchIndex = gensym("___DynamicPatchIndex___");		// Capitals must stay here
	ps_dspchain = gensym("dspchain");
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


void *dynamicserial_new(t_symbol *s, long argc, t_atom *argv)
{	
	t_dynamicserial *x = (t_dynamicserial *) object_alloc(dynamicserial_class);
	
	t_symbol *patch_name_entered = NULL;
	t_symbol *tempsym;
	
	long ac = 0;
	t_atom av[MAX_ARGS];						
	
	long num_sig_ins = 2;
	long num_sig_outs = 2;
	long num_ins = 2;
	long num_outs = 2;	
    long num_temp_buffers = 0;
	
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

	// Setup temporary memory 
	
	alloc_mem_swap(&x->temp_mem, 0, 0);

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
	
	x->patch_spaces_allocated = 0;
	x->target_index = 0;	
	
	x->last_vec_size = 64;
	x->last_samp_rate = 44100;
	
	x->in_table = 0;
	x->out_table = 0;
	
	x->patch_is_loading = 0;
	
	// Create signal in/out buffers and zero
	
	x->sig_ins = (void **) malloc(num_sig_ins * sizeof(void *));
	x->sig_outs = (void **) malloc(num_sig_outs * sizeof(void *));
	
	x->ins_temp = (void **) malloc(num_temp_buffers * sizeof(void *));
	x->temp_buffers1 = (void **) malloc(num_temp_buffers * sizeof(void *));
	x->temp_buffers2 = (void **) malloc(num_temp_buffers * sizeof(void *));
	
	for (long i = 0; i < num_sig_ins; i++)
		x->sig_ins[i] = NULL;
	for (long i = 0; i < num_sig_outs; i++)
		x->sig_outs[i] = NULL;
	for (long i = 0; i < num_temp_buffers; i++)
		x->ins_temp[i] = x->temp_buffers1[i] = x->temp_buffers2[i] = NULL;

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
	x->x_obj.z_misc = Z_NO_INPLACE;																// due to output zeroing!!
	
	// Make signal outs
	
	for (long i = 0; i < num_sig_outs; i++)
		outlet_new((t_object *)x, "signal");
	
	// Initialise patcher symbol
	
	x->parent_patch = (t_patcher *)gensym("#P")->s_thing;										// store reference to parent patcher
	
    // Initialise slotr
    
    for (long i = 0; i < MAX_NUM_PATCHES; i++)
        x->patch_slots[i] = NULL;
    
	// Load patch and initialise
	
	if (patch_name_entered) 
		dynamicserial_loadpatch(x, 0, patch_name_entered, ac, av);
	
	return x;
}

void dynamicserial_free(t_dynamicserial *x)
{
	dsp_free((t_pxobject *)x);
	
	// Free patches
	
	for (long i = 0; i < x->patch_spaces_allocated; i++)
	{
        if (x->patch_slots[i])
            delete x->patch_slots[i];
	}
	
	// Free other resources
	
	free_mem_swap(&x->temp_mem);
	
	if (x->num_sig_ins)
		free(x->sig_ins);
	
	if (x->num_sig_outs)
		free(x->sig_outs);
	
	if (x->num_temp_buffers)
	{
        for (long i = 0; i < x->num_temp_buffers; i++)
        {
            ALIGNED_FREE(x->temp_buffers1[i]);
            ALIGNED_FREE(x->temp_buffers2[i]);
        }
        
		free(x->ins_temp);
		free(x->temp_buffers1);
		free(x->temp_buffers2);
	}
	
	for (long i = 0; i < x->num_ins; i++)
		freeobject((t_object *)x->in_table[i]);
	
	if (x->in_table)
		freebytes(x->in_table, x->num_ins * sizeof(t_outvoid));
	
	if (x->out_table)
		freebytes(x->out_table, x->num_outs * sizeof(t_outvoid));
}

void dynamicserial_assist(t_dynamicserial *x, void *b, long m, long a, char *s)
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


void dynamicserial_deletepatch(t_dynamicserial *x, t_symbol *msg, long argc, t_atom *argv)
{
	dynamicserial_deletepatch_internal(x, argc ? atom_getlong(argv) - 1 : -1); 
}

void dynamicserial_deletepatch_internal(t_dynamicserial *x, t_atom_long index)
{
	t_atom a;
	
	if (index < 0 || index >= x->patch_spaces_allocated)
	{
		object_error((t_object *) x, "patch index out of range"); 
		return;
	}

    atom_setlong(&a, index);
	x->patch_slots[index]->setInvalid();
	defer(x,(method)dynamicserial_cleanpatch, 0L, 1, &a);
}

void dynamicserial_cleanpatch(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom_long index = atom_getlong(argv);
	
    if (index < 0 || index >= x->patch_spaces_allocated)
        return;

	// Free slot
	
	delete x->patch_slots[index];
    x->patch_slots[index] = new PatchSlot();
}

void dynamicserial_loadpatch(t_dynamicserial *x, long index, t_symbol *patch_name_in, long argc, t_atom *argv)
{
	long patch_spaces_allocated = x->patch_spaces_allocated;
	
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
			if (x->patch_slots[index]->isEmpty())
				break;
	}
	
	// Check that the index is valid
	
	if (index >= MAX_NUM_PATCHES) 
	{
		object_error((t_object *) x, "slot out of range");
		ATOMIC_DECREMENT_BARRIER(&x->patch_is_loading);
		return;
	}
		
	// Create patch slots up until the last allocated index (if necessary) and store the pointer
	
	for (long i = patch_spaces_allocated; i <= index; i++)
        x->patch_slots[i] = new PatchSlot();
	
    if (index + 1 > patch_spaces_allocated)
        x->patch_spaces_allocated = index + 1;
    
    // Store the old loading symbols
    
    t_object *previous = ps_dynamicdsp->s_thing;
    t_object *previous_index = ps_DynamicPatchIndex->s_thing;
   
    // Bind to the loading symbols
    
    ps_dynamicdsp->s_thing = (t_object *) x;
    ps_DynamicPatchIndex->s_thing = (t_object *) (index + 1);
    
    //PatchSlot::LoadError error =
    x->patch_slots[index]->load((t_object *) x, index + 1, patch_name_in, argc, argv, x->last_vec_size, x->last_samp_rate);
	
    // FIX - do errors!

    // Return to previous state
    
    ps_dynamicdsp->s_thing = previous;
    ps_DynamicPatchIndex->s_thing = previous_index;
    
    // Check whether the patch loaded correctly
    
    if (!x->patch_slots[index]->isEmpty())
    {
        // Link inlets and outlets
        
        if (x->num_ins) 
            dynamicserial_patcher_descend(x->patch_slots[index]->getPatch(), (intfunc) dynamicserial_linkinlets, x, x);
        if (x->num_outs) 
            dynamicserial_patcher_descend(x->patch_slots[index]->getPatch(), (intfunc) dynamicserial_linkoutlets, x, x);
    }
	
    ATOMIC_DECREMENT_BARRIER(&x->patch_is_loading);
}

void dynamicserial_user_clear(t_dynamicserial *x)
{
	for (long i = 0; i < x->patch_spaces_allocated; i++)
		dynamicserial_deletepatch_internal (x, i);
}

void dynamicserial_user_loadpatch(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv)
{
	t_symbol *patch_name_entered = NULL;
    t_atom_long index = -1;
		
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
	
	// If there is a patch name then try to load the patch
	
	if (argc && atom_gettype(argv) == A_SYM)			
	{
		patch_name_entered = atom_getsym(argv);
		argc--; argv++;
		
		dynamicserial_loadpatch(x, index, patch_name_entered, argc, argv);
	} 
	else 
		object_error((t_object *) x, "no patch specified");
}

// ========================================================================================================================================== //
// Messages in passed on to the patcher via the "in" objects / Voice targeting
// ========================================================================================================================================== //


void dynamicserial_bang(t_dynamicserial *x)
{	
	long inlet = proxy_getinlet((t_object *)x);
	long target_index = x->target_index;

	if (inlet >= x->num_ins)
		return;
	
	if (target_index)
		dynamicserial_target(x, target_index, inlet, ps_bang, 0, 0);
	else
		outlet_bang(x->in_table[inlet]);
}

void dynamicserial_int(t_dynamicserial *x, t_atom_long n)
{
	long inlet = proxy_getinlet((t_object *)x);
	long target_index = x->target_index;

	if (inlet >= x->num_ins)
		return;		
	
	if (target_index)
	{
		t_atom n_atom; 
		atom_setlong(&n_atom, n);
		dynamicserial_target(x, target_index, inlet, ps_int, 1, &n_atom);
	}
	else
		outlet_int(x->in_table[inlet], n);
}

void dynamicserial_float(t_dynamicserial *x, double f)
{
    long inlet = proxy_getinlet((t_object *)x);
	long target_index = x->target_index;

	if (inlet >= x->num_ins)
		return;
	
	if (target_index)
	{
		t_atom f_atom; 
		atom_setfloat(&f_atom, f);
		dynamicserial_target(x, target_index, inlet, ps_float, 1, &f_atom);
	}
	else
		outlet_float(x->in_table[inlet], f);
}

void dynamicserial_list(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv)
{
    long inlet = proxy_getinlet((t_object *)x);
	long target_index = x->target_index;

	if (inlet >= x->num_ins)
		return;
	
	if (target_index)
		dynamicserial_target(x, target_index, inlet, ps_list, argc, argv);
	else
		outlet_list(x->in_table[inlet], ps_list, argc, argv);
}

void dynamicserial_anything(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv)
{
    long inlet = proxy_getinlet((t_object *)x);
	long target_index = x->target_index;

	if (inlet >= x->num_ins)
		return;		
	
	if (target_index)
		dynamicserial_target(x, target_index, inlet, s, argc, argv);
	else
		outlet_anything(x->in_table[inlet], s, argc, argv);
}

void dynamicserial_target(t_dynamicserial *x, long target_index, long inlet, t_symbol *msg, long argc, t_atom *argv)
{	
	struct t_args_struct pass_args;
	
	pass_args.msg = msg;
	pass_args.argc = argc;
	pass_args.argv = argv;
	pass_args.inlet_index = inlet + 1;
	
	if (target_index >= 1 && target_index <= x->patch_spaces_allocated)
	{
		t_patcher *p = x->patch_slots[target_index - 1]->getPatch();
		
		if (x->patch_slots[target_index - 1]->getValid())
			dynamicserial_patcher_descend(p, (intfunc) dynamicserial_targetinlets, &pass_args, x);
	}
}

// - inlet and outlet linking using the in and out objects

int dynamicserial_targetinlets(t_patcher *p, struct t_args_struct *args)
{
	for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
	{
		if (jbox_get_maxclass(b) == ps_in) 
		{
			t_inout *io = (t_inout *) jbox_get_object(b);
			if (io->s_index == args->inlet_index)
			{
				void *outletptr = io->s_obj.o_outlet;
				outlet_anything (outletptr, args->msg, args->argc, args->argv);
			}
		}
    }
    
	return 0;
}

void dynamicserial_user_target(t_dynamicserial *x, t_symbol *msg, long argc, t_atom *argv)
{
	t_atom_long target_index = argc ? atom_getlong(argv) : 0;
	
    if (target_index >= 0 || target_index <= x->patch_spaces_allocated)
        x->target_index = target_index;
    else
        x->target_index = -1;

}

void dynamicserial_user_target_free(t_dynamicserial *x, t_symbol *msg, long argc, t_atom *argv)
{
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
        if (x->patch_slots[i]->getValid() && !x->patch_slots[i]->getBusy())
        {
            x->target_index = i + 1;
            return;
        }
    }
    
    x->target_index = -1;
}


// ========================================================================================================================================== //
// Perform and DSP Routines
// ========================================================================================================================================== //


void dynamicserial_perform_common(t_dynamicserial *x)
{	
	void **sig_ins = x->sig_ins;
	void **sig_outs = x->sig_outs;
	void **temp_buffers1 = x->temp_buffers1;
	void **temp_buffers2 = x->temp_buffers2;
	void **ins_temp = x->ins_temp;
	
    long num_sig_ins = x->num_sig_ins;
    long num_sig_outs = x->num_sig_outs;
    long num_temp_buffers = x->num_temp_buffers;
    long vec_size = x->last_vec_size;
    long patch_spaces_allocated = x->patch_spaces_allocated;
    
	AH_Boolean flip = false;
	
	// Zero Outputs
	
	for (long i = 0; i < num_sig_outs; i++)
		memset(sig_outs[i], 0, sig_size * vec_size);
	
	if (x->x_obj.z_disabled)
		return;
	
	// Update the temporary memory if relevant
	
	attempt_mem_swap(&x->temp_mem);
	void *temp_mem_ptr = x->temp_mem.current_ptr;
	t_ptr_uint temp_mem_size = x->temp_mem.current_size;
	
	// Copy inputs in and zero output temp buffers

	for (long i = 0; i < num_sig_ins; i++)
		memcpy(temp_buffers1[i], sig_ins[i], sig_size * vec_size);
	for (long i = num_sig_ins; i < num_sig_outs; i++)
		memset(temp_buffers1[i], 0, sig_size * vec_size);
	
	// Loop over patches
	
	for (long i = 0; i < patch_spaces_allocated; i++)
	{
		// Copy in pointers
        
        memcpy(ins_temp, flip ? temp_buffers2 : temp_buffers1, num_temp_buffers * sizeof(void *));
        
        // Clear current output buffers
			
        for (long j = 0; j < num_temp_buffers; j++)
            memset(flip ? temp_buffers1[j] : temp_buffers2[j], 0, sig_size * vec_size);
			
        // Process and flip if processing has occurred
            
        if (x->patch_slots[i]->process(temp_mem_ptr, flip ? temp_buffers1 : temp_buffers2, temp_mem_size))
            flip = flip ? false : true;
	}
	
	// Copy outputs
	
    for (long i = 0; i < num_sig_outs; i++)
        memcpy(sig_outs[i], flip ? temp_buffers2[i] : temp_buffers1[i], vec_size * sig_size);
}

void dynamicserial_denormal_handled_perform(t_dynamicserial *x)
{
    // Turn off denormals
    
#if defined( __i386__ ) || defined( __x86_64__ )
    int oldMXCSR = _mm_getcsr();						// read the old MXCSR setting
    _mm_setcsr(oldMXCSR | 0x8040);						// write the new MXCSR setting setting DAZ and FZ bits
#endif
    
    // Process
    
    dynamicserial_perform_common(x);
    
    // Return denormals to previous state

#if defined( __i386__ ) || defined( __x86_64__ )
    _mm_setcsr(oldMXCSR);
#endif
}

t_int *dynamicserial_perform(t_int *w)
{
	dynamicserial_denormal_handled_perform((t_dynamicserial *) w[1]);
	
	return w + 2;	
}

void dynamicserial_perform64(t_dynamicserial *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    memcpy(x->sig_ins, ins, x->num_sig_ins * sizeof(void *));
    memcpy(x->sig_outs, outs, x->num_sig_ins * sizeof(void *));
    
	dynamicserial_denormal_handled_perform(x);
}


// ========================================================================================================================================== //
// DSP Routines
// ========================================================================================================================================== //


AH_Boolean dynamicserial_dsp_common(t_dynamicserial *x, long vec_size, long samp_rate)
{
	AH_Boolean mem_fail = false;
			
	// Do internal dsp compile (for each valid patch)
	
	for (long i = 0; i < x->patch_spaces_allocated; i++)
        x->patch_slots[i]->compileDSP(vec_size, samp_rate);
	
	for (long i = 0; i < x->num_temp_buffers; i++)
	{
		ALIGNED_FREE(x->temp_buffers1[i]);
		ALIGNED_FREE(x->temp_buffers2[i]);
		
		x->temp_buffers1[i] = (void *) ALIGNED_MALLOC(sig_size * vec_size);
		x->temp_buffers2[i] = (void *) ALIGNED_MALLOC(sig_size * vec_size);
		
		if (!x->temp_buffers1[i] || !x->temp_buffers2[i])
			mem_fail = true;
	}
		
	x->last_vec_size = vec_size;
	x->last_samp_rate = samp_rate;
	
	return mem_fail;
}

void dynamicserial_dsp(t_dynamicserial *x, t_signal **sp, short *count)
{
	// Copy in and out pointers
	
	for (long i = 0; i < x->num_sig_ins; i++)
		x->sig_ins[i] = sp[i]->s_vec;
	for (long i = 0; i < x->num_sig_outs; i++)
		x->sig_outs[i] = sp[i + x->num_proxies]->s_vec;
	
	// Add to dsp if common routine successful
	
	if (!dynamicserial_dsp_common(x, sp[0]->s_n, sp[0]->s_sr))
		dsp_add(dynamicserial_perform, 1, x);
}

void dynamicserial_dsp64(t_dynamicserial *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	// Add to dsp if common routine successful
	
	if (!dynamicserial_dsp_common(x, maxvectorsize, samplerate))
		object_method(dsp64, gensym("dsp_add64"), x, dynamicserial_perform64, 0, NULL);
}


// ========================================================================================================================================== //
// Patcher Link Inlets / Outlets
// ========================================================================================================================================== //

// - inlet and outlet linking using the in and out objects

int dynamicserial_linkinlets(t_patcher *p, t_dynamicserial *x)
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

int dynamicserial_linkoutlets(t_patcher *p, t_dynamicserial *x)
{
	for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
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

int dynamicserial_unlinkinlets(t_patcher *p, t_dynamicserial *x)
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

int dynamicserial_unlinkoutlets(t_patcher *p, t_dynamicserial *x)
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


void dynamicserial_dblclick(t_dynamicserial *x)
{
    for (long i = 0; i < x->patch_spaces_allocated; i++)
    {
        if (!x->patch_slots[i]->isEmpty())
        {
            dynamicserial_open(x, i + 1);
            break;
        }
    }
}

void dynamicserial_open(t_dynamicserial *x, t_atom_long index)
{
    t_atom a;
    atom_setlong (&a, index - 1);
    defer(x,(method)dynamicserial_doopen, 0L, 1, &a);
}

void dynamicserial_doopen(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv)
{
    t_atom_long index = atom_getlong(argv);
    
    if ((index < 0) || (index >= x->patch_spaces_allocated) || (!x->patch_slots[index]->getValid()))
        return;

    if (x->patch_slots[index])
        x->patch_slots[index]->openWindow();
}

void dynamicserial_pclose(t_dynamicserial *x)
{
    // Do Nothing
}

void dynamicserial_wclose(t_dynamicserial *x, t_atom_long index)
{
    t_atom a;
    atom_setlong (&a, index - 1);
    defer(x,(method)dynamicserial_dowclose, 0L, 1, &a);
}

void dynamicserial_dowclose(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv)
{
    t_atom_long index = atom_getlong(argv);
    
    if ((index < 0) || (index >= x->patch_spaces_allocated) || (!x->patch_slots[index]->getValid()))
        return;
    
    if (x->patch_slots[index])
        x->patch_slots[index]->closeWindow();
}


// ========================================================================================================================================== //
// Patcher Utilities (these deal with various updating and necessary behind the scenes state stuff)
// ========================================================================================================================================== //


int dynamicserial_patcher_descend(t_patcher *p, intfunc fn, void *arg, t_dynamicserial *x)
{
	t_patcher *p2;
	t_object *assoc = 0;
    
	object_method(p, ps_getassoc, &assoc);				// Avoid recursion into a poly / pfft / dynamicserial~
	if (assoc && (t_dynamicserial *) assoc != x) 
		return 0;

	// CHANGED - DO NOT PASS x AS ARG
	if ((*fn)(p, arg))
		return (1);
	
	for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
	{
		if (b) 
		{
			long index = 0;
			while ((p2 = (t_patcher *) object_subpatcher(jbox_get_object(b), &index, arg)))
				if (dynamicserial_patcher_descend(p2, fn, arg, x))
					return 1;
		}
	}

	return 0;
}

void dynamicserial_pupdate(t_dynamicserial *x, void *b, t_patcher *p)
{
	// Reload the patcher when it's updated
	
	for (long i = 0; i < x->patch_spaces_allocated; i++)
	{
		if (x->patch_slots[i]->getPatch() == p)
            x->patch_slots[i]->load((t_object *) x, x->last_vec_size, x->last_samp_rate);
	}
}

void *dynamicserial_subpatcher(t_dynamicserial *x, long index, void *arg)
{		
    if (arg && (t_ptr_uint) arg != 1)
		if (!NOGOOD(arg))                                       // arg might be good but not a valid object pointer
			if (ob_sym(arg) == ps_dspchain)                     // don't report subpatchers to dspchain
				return NULL;

	if (index < x->patch_spaces_allocated)
		if (x->patch_slots[index]->getValid())
            return x->patch_slots[index]->getPatch();           // the indexed patcher
    
    return NULL;
}

void dynamicserial_parentpatcher(t_dynamicserial *x, t_patcher **parent)
{
	*parent = x->parent_patch;
}


// ========================================================================================================================================== //
// Parent / Child Communication - Routines for owned objects to query the parent
// ========================================================================================================================================== //


// Note that objects wishing to query the parent dynamicserial~ object should call the functions in dynamicdsp.h
// These act as suitable wrappers to send the appropriate message to the parent object and returns values as appropriate


////////////////////////////////////////////////// Signal IO Queries //////////////////////////////////////////////////


void *dynamicserial_query_num_sigins(t_dynamicserial *x)
{
	return (void *)x->num_temp_buffers;
}

void *dynamicserial_query_num_sigouts(t_dynamicserial *x)
{
	return (void *)x->num_temp_buffers;
}

void *dynamicserial_query_sigins(t_dynamicserial *x)
{
	return (void *)x->ins_temp;
}

void *dynamicserial_query_sigouts(t_dynamicserial *x, t_ptr_int index)
{
	if (x->patch_slots[index - 1])
		return x->patch_slots[index - 1]->getOutputsHandle();
	else
		return NULL;
}


//////////////////////////////////////////////////// State Queries ////////////////////////////////////////////////////

// FIX - safety!

void *dynamicserial_client_get_patch_on(t_dynamicserial *x, t_ptr_int index)
{
	if (x->patch_slots[index - 1])
		return (void *) (t_atom_long) x->patch_slots[index - 1]->getOn();
	
	return 0;
}

void *dynamicserial_client_get_patch_busy(t_dynamicserial *x, t_ptr_int index)
{
	if (x->patch_slots[index - 1])
		return (void *) (t_atom_long) x->patch_slots[index - 1]->getBusy();
	
	return 0;
}

void dynamicserial_client_set_patch_on(t_dynamicserial *x, t_ptr_int index, t_ptr_int state)
{
    if (x->patch_slots[index - 1])
        x->patch_slots[index - 1]->setOn(state ? true : false);
}

void dynamicserial_client_set_patch_busy(t_dynamicserial *x, t_ptr_int index, t_ptr_int state)
{
    if (x->patch_slots[index - 1])
        x->patch_slots[index - 1]->setBusy(state ? true : false);
}


//////////////////////////////////////////////// Temporary Memory Queries ///////////////////////////////////////////////


// dynamicserial~ provides memory per audio thread for temporary calculations.
// Objects requiring temporary memory during their perform method request a minimum size during their dsp routine
// The pointer should be requested during the perform routine, and should not be stored
// This reduces memory alloaction, and potentially increases speed by keeping temporary memory in the cache


void *dynamicserial_query_temp_mem(t_dynamicserial *x, t_ptr_int index)
{
	if (x->patch_slots[index - 1])
		return x->patch_slots[index - 1]->getTempMemHandle();
	else
		return NULL;
}

void *dynamicserial_client_temp_mem_resize(t_dynamicserial *x, t_ptr_int index, t_ptr_uint size)
{	
	schedule_grow_mem_swap(&x->temp_mem, size, size);

	if (x->patch_slots[index - 1])
		x->patch_slots[index - 1]->setTempMemSize(size);
	
	return (void *) 1;
}