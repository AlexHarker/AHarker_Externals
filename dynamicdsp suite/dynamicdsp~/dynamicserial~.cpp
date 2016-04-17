
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
#include "PatchSet.h"


// ========================================================================================================================================== //
// Global Varibles
// ========================================================================================================================================== //


t_class *dynamicserial_class;

static t_ptr_uint sig_size;

#define MAX_ARGS 16
#define MAX_IO 256


// ========================================================================================================================================== //
// Object Structure
// ========================================================================================================================================== //

typedef struct _dynamicserial
{
    t_pxobject x_obj;
    
	t_patcher *parent_patch;
	
	// Patch Data and Variables 
	
    PatchSet<PatchSlot> *slots;
			
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
	
	long num_proxies;				// number of proxies = MAX(num_sig_ins, num_ins)
				
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
void dynamicserial_clear(t_dynamicserial *x);
void dynamicserial_loadpatch(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv);

void dynamicserial_bang(t_dynamicserial *x);
void dynamicserial_int(t_dynamicserial *x, t_atom_long n);
void dynamicserial_float(t_dynamicserial *x, double f);
void dynamicserial_list(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv);
void dynamicserial_anything(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv);

void dynamicserial_target(t_dynamicserial *x, t_symbol *msg, long argc, t_atom *argv);
void dynamicserial_targetfree(t_dynamicserial *x, t_symbol *msg, long argc, t_atom *argv);

void dynamicserial_perform_common(t_dynamicserial *x);
void dynamicserial_perform_denormal_handled(t_dynamicserial *x);
t_int *dynamicserial_perform(t_int *w);
void dynamicserial_perform64(t_dynamicserial *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

AH_Boolean dynamicserial_dsp_common(t_dynamicserial *x, long vec_size, long samp_rate);
void dynamicserial_dsp(t_dynamicserial *x, t_signal **sp, short *count);
void dynamicserial_dsp64(t_dynamicserial *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

void dynamicserial_dblclick(t_dynamicserial *x);
void dynamicserial_open(t_dynamicserial *x, t_atom_long index);
void dynamicserial_pclose(t_dynamicserial *x);
void dynamicserial_wclose(t_dynamicserial *x, t_atom_long index);

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


t_symbol *ps_dspchain;
t_symbol *ps_args;
t_symbol *ps_declareio;


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
	
	class_addmethod(dynamicserial_class, (method)dynamicserial_clear, "clear", 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_loadpatch, "loadpatch", A_GIMME, 0);
	class_addmethod(dynamicserial_class, (method)dynamicserial_deletepatch, "deletepatch", A_GIMME, 0);						// MUST FIX TO GIMME FOR NOW
	class_addmethod(dynamicserial_class, (method)dynamicserial_target, "target", A_GIMME, 0);                               // MUST FIX TO GIMME FOR NOW
	class_addmethod(dynamicserial_class, (method)dynamicserial_targetfree, "targetfree", A_GIMME, 0);                       // MUST FIX TO GIMME FOR NOW
	
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
	
	ps_dspchain = gensym("dspchain");
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
	
    void *outs[MAX_IO];
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
	
	x->last_vec_size = 64;
	x->last_samp_rate = 44100;
	
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
	
     for (long i = num_outs - 1; i >= 0; i--)
        outs[i] = outlet_new((t_object *)x, NULL);
    	
	// Make signal ins
	
	x->num_proxies = (num_sig_ins > num_ins) ? num_sig_ins : num_ins;
	
    // All proxies (signals or messages must be declared as if signal inlets
    
	dsp_setup((t_pxobject *) x, x->num_proxies);
	x->x_obj.z_misc = Z_NO_INPLACE;
	
	// Make signal outs
	
	for (long i = 0; i < num_sig_outs; i++)
		outlet_new((t_object *)x, "signal");
    
	// Setup slots
    
    x->slots = new PatchSet<PatchSlot>((t_object *)x, num_ins, num_outs, outs);
    
	// Initialise parent patcher
	
	x->parent_patch = (t_patcher *)gensym("#P")->s_thing;
	
	// Load patch
    
	if (patch_name_entered)
        x->slots->load(0, patch_name_entered, ac, av, x->last_vec_size, x->last_samp_rate);
	
	return x;
}

void dynamicserial_free(t_dynamicserial *x)
{
	dsp_free((t_pxobject *)x);
	
	// Free patches
	
    delete x->slots;
	
	// Free other resources
	
	free_mem_swap(&x->temp_mem);
	
    // Free temp buffers
    
	if (x->num_temp_buffers)
	{
        for (long i = 0; i < x->num_temp_buffers; i++)
        {
            ALIGNED_FREE(x->temp_buffers1[i]);
            ALIGNED_FREE(x->temp_buffers2[i]);
        }
	}
    
    // Free buffer handles
    
    if (x->num_sig_ins)
        free(x->sig_ins);
    if (x->num_sig_outs)
        free(x->sig_outs);
    if (x->ins_temp)
        free(x->ins_temp);
    if (x->temp_buffers1)
        free(x->temp_buffers1);
    if (x->temp_buffers2)
        free(x->temp_buffers2);
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
    x->slots->remove(atom_getlong(argv));
}

void dynamicserial_clear(t_dynamicserial *x)
{
    x->slots->clear();
}

void dynamicserial_loadpatch(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv)
{
	t_symbol *patch_name = NULL;
    t_atom_long index = -1;
		
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
	
	// If there is a patch name then try to load the patch
	
	if (argc && atom_gettype(argv) == A_SYM)			
	{
		patch_name = atom_getsym(argv);
		argc--; argv++;
		
        x->slots->load(index, patch_name, argc, argv, x->last_vec_size, x->last_samp_rate);
	}
	else 
		object_error((t_object *) x, "no patch specified");
}

// ========================================================================================================================================== //
// Messages in passed on to the patcher via the "in" objects / Voice targeting
// ========================================================================================================================================== //


void dynamicserial_bang(t_dynamicserial *x)
{	
    x->slots->objBang();
}

void dynamicserial_int(t_dynamicserial *x, t_atom_long n)
{
    x->slots->objInt(n);
}

void dynamicserial_float(t_dynamicserial *x, double f)
{
    x->slots->objFloat(f);
}

void dynamicserial_list(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv)
{
    x->slots->objAnything(s, argc, argv);
}

void dynamicserial_anything(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv)
{
    x->slots->objAnything(s, argc, argv);
}

void dynamicserial_target(t_dynamicserial *x, t_symbol *msg, long argc, t_atom *argv)
{
    x->slots->objTarget(argc, argv);
}

void dynamicserial_targetfree(t_dynamicserial *x, t_symbol *msg, long argc, t_atom *argv)
{
    x->slots->objTargetFree(argc, argv);
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
	
	for (long i = 0; i < x->slots->size(); i++)
	{
		// Copy in pointers
        
        memcpy(ins_temp, flip ? temp_buffers2 : temp_buffers1, num_temp_buffers * sizeof(void *));
        
        // Clear current output buffers
			
        for (long j = 0; j < num_temp_buffers; j++)
            memset(flip ? temp_buffers1[j] : temp_buffers2[j], 0, sig_size * vec_size);
			
        // Process and flip if processing has occurred
            
        if (x->slots->process(i, temp_mem_ptr, flip ? temp_buffers1 : temp_buffers2, temp_mem_size))
            flip = flip ? false : true;
	}
	
	// Copy outputs
	
    for (long i = 0; i < num_sig_outs; i++)
        memcpy(sig_outs[i], flip ? temp_buffers2[i] : temp_buffers1[i], vec_size * sig_size);
}

void dynamicserial_perform_denormal_handled(t_dynamicserial *x)
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
	dynamicserial_perform_denormal_handled((t_dynamicserial *) w[1]);
	
	return w + 2;	
}

void dynamicserial_perform64(t_dynamicserial *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    memcpy(x->sig_ins, ins, x->num_sig_ins * sizeof(void *));
    memcpy(x->sig_outs, outs, x->num_sig_ins * sizeof(void *));
    
	dynamicserial_perform_denormal_handled(x);
}


// ========================================================================================================================================== //
// DSP Routines
// ========================================================================================================================================== //


AH_Boolean dynamicserial_dsp_common(t_dynamicserial *x, long vec_size, long samp_rate)
{
	AH_Boolean mem_fail = false;
			
	// Do internal dsp compile
	
	x->slots->compileDSP(vec_size, samp_rate);
	
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
	// Copy in and out pointers (note that all inlets are declared as if signals)
	
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
// Patcher Window stuff
// ========================================================================================================================================== //


void dynamicserial_dblclick(t_dynamicserial *x)
{
    for (long i = 1; i <= x->slots->size(); i++)
        if (x->slots->openWindow(i))
            break;
}

void dynamicserial_open(t_dynamicserial *x, t_atom_long index)
{
    x->slots->openWindow(index);
}

void dynamicserial_pclose(t_dynamicserial *x)
{
    // Do Nothing
}

void dynamicserial_wclose(t_dynamicserial *x, t_atom_long index)
{
    x->slots->closeWindow(index);
}


// ========================================================================================================================================== //
// Patcher Utilities (these deal with various updating and necessary behind the scenes state stuff)
// ========================================================================================================================================== //


void dynamicserial_pupdate(t_dynamicserial *x, void *b, t_patcher *p)
{
    x->slots->update(p, x->last_vec_size, x->last_samp_rate);
}

void *dynamicserial_subpatcher(t_dynamicserial *x, long index, void *arg)
{
    // Report subpatchers if request by an object that is not a dspchain
    
    if (arg && (t_ptr_uint) arg != 1)
		if (!NOGOOD(arg))
			if (ob_sym(arg) == ps_dspchain)
				return NULL;

    // FIX - only if a valid patcher? Not sure - do tests...
    
    return (void *) x->slots->getPatch(index);
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

// Signals

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
    return x->slots->getOutputHandle(index);
}

// State

void *dynamicserial_client_get_patch_on(t_dynamicserial *x, t_ptr_int index)
{
    return (void *) (t_atom_long) x->slots->getOn(index);
}

void *dynamicserial_client_get_patch_busy(t_dynamicserial *x, t_ptr_int index)
{
    return (void *) (t_atom_long) x->slots->getBusy(index);
}

void dynamicserial_client_set_patch_on(t_dynamicserial *x, t_ptr_int index, t_ptr_int state)
{
    x->slots->setOn(index, state);
}

void dynamicserial_client_set_patch_busy(t_dynamicserial *x, t_ptr_int index, t_ptr_int state)
{
    x->slots->setBusy(index, state);
}

// Temporary memory

// dynamicserial~ provides memory per audio thread for temporary calculations.
// Objects requiring temporary memory during their perform method request a minimum size during their dsp routine
// The pointer should be requested during the perform routine, and should not be stored
// This reduces memory allocation, and potentially increases speed by keeping temporary memory in the cache

void *dynamicserial_query_temp_mem(t_dynamicserial *x, t_ptr_int index)
{
    return x->slots->getTempMemHandle(index);
}

void *dynamicserial_client_temp_mem_resize(t_dynamicserial *x, t_ptr_int index, t_ptr_uint size)
{	
	schedule_grow_mem_swap(&x->temp_mem, size, size);

    x->slots->setTempMemSize(index, size);

	return (void *) 1;
}