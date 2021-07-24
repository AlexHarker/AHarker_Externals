
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

#include <SIMDSupport.hpp>

#include "PatchSlot.hpp"
#include "PatchSet.hpp"

#include "dynamic_host.hpp"


/*****************************************/
// Global Varibles
/*****************************************/

t_class *this_class;

t_symbol *ps_args;
t_symbol *ps_declareio;

static t_ptr_uint sig_size;

#define MAX_ARGS 16
#define MAX_IO 256


/*****************************************/
// Object Structure
/*****************************************/

struct t_dynamicserial
{
    t_pxobject x_obj;
    
	t_patcher *parent_patch;
	
	// Patch Data and Variables 
	
    PatchSet<PatchSlot> *patch_set;
			
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
};


/*****************************************/
// Function Prototypes
/*****************************************/

void *dynamicserial_new(t_symbol *s, long argc, t_atom *argv);
void dynamicserial_free(t_dynamicserial *x);
void dynamicserial_assist(t_dynamicserial *x, void *b, long m, long a, char *s);

void dynamicserial_loadpatch(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv);

void dynamicserial_perform_common(t_dynamicserial *x);
void dynamicserial_perform_denormal_handled(t_dynamicserial *x);
t_int *dynamicserial_perform(t_int *w);
void dynamicserial_perform64(t_dynamicserial *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

bool dynamicserial_dsp_common(t_dynamicserial *x, long vec_size, long samp_rate);
void dynamicserial_dsp(t_dynamicserial *x, t_signal **sp, short *count);
void dynamicserial_dsp64(t_dynamicserial *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

/*****************************************/
// Main
/*****************************************/

int C74_EXPORT main()
{
    using handler = DynamicHost<t_dynamicserial>;
    
	this_class = class_new("dynamicserial~",
								 (method)dynamicserial_new, 
								 (method)dynamicserial_free, 
								 sizeof(t_dynamicserial), 
								 NULL, 
								 A_GIMME, 
								 0);
	
	class_addmethod(this_class, (method)dynamicserial_dsp, "dsp", A_CANT, 0);
	class_addmethod(this_class, (method)dynamicserial_dsp64, "dsp64", A_CANT, 0);
	
	class_addmethod(this_class, (method)dynamicserial_assist, "assist", A_CANT, 0);
    
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
	
    class_addmethod(this_class, (method)handler::clear, "clear", 0);
	class_addmethod(this_class, (method)dynamicserial_loadpatch, "loadpatch", A_GIMME, 0);
    class_addmethod(this_class, (method)handler::deletepatch, "deletepatch", A_GIMME, 0);						// MUST FIX TO GIMME FOR NOW
	class_addmethod(this_class, (method)handler::target, "target", A_GIMME, 0);                               // MUST FIX TO GIMME FOR NOW
	class_addmethod(this_class, (method)handler::targetfree, "targetfree", A_GIMME, 0);                       // MUST FIX TO GIMME FOR NOW
	
    class_addmethod(this_class, (method)handler::loading_index, "loading_index", A_CANT, 0);
	class_addmethod(this_class, (method)handler::query_num_sigins, "query_num_sigins", A_CANT, 0);
	class_addmethod(this_class, (method)handler::query_num_sigouts, "query_num_sigouts", A_CANT, 0);
	class_addmethod(this_class, (method)handler::query_sigins, "query_sigins", A_CANT, 0);
	class_addmethod(this_class, (method)handler::query_sigouts, "query_sigouts", A_CANT, 0);
	class_addmethod(this_class, (method)handler::client_get_patch_on, "client_get_patch_on", A_CANT, 0);
	class_addmethod(this_class, (method)handler::client_get_patch_busy, "client_get_patch_busy", A_CANT, 0);
	class_addmethod(this_class, (method)handler::client_set_patch_on, "client_set_patch_on", A_CANT, 0);
	class_addmethod(this_class, (method)handler::client_set_patch_busy, "client_set_patch_busy", A_CANT, 0);
	
    CLASS_ATTR_OBJ(this_class, "ownsdspchain", ATTR_SET_OPAQUE | ATTR_SET_OPAQUE_USER, t_dynamicserial, x_obj);
    CLASS_ATTR_ACCESSORS(this_class, "ownsdspchain", (method) patchset_get_ownsdspchain, (method) 0);
    CLASS_ATTR_INVISIBLE(this_class, "ownsdspchain", 0);
    
	class_dspinit(this_class);
	
	class_register(CLASS_BOX, this_class);
	
	ps_args = gensym("args");
	ps_declareio = gensym("declareio");
	
	sig_size = ((maxversion() & 0x3FFF) >= 0x600) ? sizeof(double) : sizeof(float);

	return 0;
}


/*****************************************/
// Object Creation / Freeing / Assisstance
/*****************************************/

void *dynamicserial_new(t_symbol *s, long argc, t_atom *argv)
{	
	t_dynamicserial *x = (t_dynamicserial *) object_alloc(this_class);
    
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
    
    // Get parent patcher
    
    x->parent_patch = (t_patcher *)gensym("#P")->s_thing;

	// Setup slots
    
    x->patch_set = new PatchSet<PatchSlot>((t_object *)x, x->parent_patch, num_ins, num_outs, outs);
    
	// Load patch
    
	if (patch_name_entered)
        x->patch_set->load(0, patch_name_entered, ac, av, x->last_vec_size, x->last_samp_rate);
	
	return x;
}

void dynamicserial_free(t_dynamicserial *x)
{
	dsp_free((t_pxobject *)x);
	
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


/*****************************************/
// Patcher Loading
/*****************************************/

void dynamicserial_loadpatch(t_dynamicserial *x, t_symbol *s, long argc, t_atom *argv)
{
	t_symbol *patch_name = NULL;
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


/*****************************************/
// Perform and DSP Routines
/*****************************************/

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
    
	bool flip = false;
	
	// Zero Outputs
	
	for (long i = 0; i < num_sig_outs; i++)
		memset(sig_outs[i], 0, sig_size * vec_size);
	
	// Copy inputs in and zero output temp buffers

	for (long i = 0; i < num_sig_ins; i++)
		memcpy(temp_buffers1[i], sig_ins[i], sig_size * vec_size);
	for (long i = num_sig_ins; i < num_sig_outs; i++)
		memset(temp_buffers1[i], 0, sig_size * vec_size);
	
	// Loop over patches
	
	for (long i = 1; i <= x->patch_set->size(); i++)
	{
		// Copy in pointers
        
        memcpy(ins_temp, flip ? temp_buffers2 : temp_buffers1, num_temp_buffers * sizeof(void *));
        
        // Clear current output buffers
			
        for (long j = 0; j < num_temp_buffers; j++)
            memset(flip ? temp_buffers1[j] : temp_buffers2[j], 0, sig_size * vec_size);
			
        // Process and flip if processing has occurred
            
        if (x->patch_set->process(i, flip ? temp_buffers1 : temp_buffers2))
            flip = !flip;
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
    if (!((t_dynamicserial *) (w[1]))->x_obj.z_disabled)
        dynamicserial_perform_denormal_handled((t_dynamicserial *) w[1]);
	
	return w + 2;	
}

void dynamicserial_perform64(t_dynamicserial *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    memcpy(x->sig_ins, ins, x->num_sig_ins * sizeof(void *));
    memcpy(x->sig_outs, outs, x->num_sig_ins * sizeof(void *));
    
	dynamicserial_perform_denormal_handled(x);
}


/*****************************************/
// DSP Routines
/*****************************************/

bool dynamicserial_dsp_common(t_dynamicserial *x, long vec_size, long samp_rate)
{
	bool mem_fail = false;
			
	// Do internal dsp compile
	
	x->patch_set->compileDSP(vec_size, samp_rate);
	
	for (long i = 0; i < x->num_temp_buffers; i++)
	{
		deallocate_aligned(x->temp_buffers1[i]);
		deallocate_aligned(x->temp_buffers2[i]);
		
		x->temp_buffers1[i] = allocate_aligned<u_int8_t>(sig_size * vec_size);
		x->temp_buffers2[i] = allocate_aligned<u_int8_t>(sig_size * vec_size);
		
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
