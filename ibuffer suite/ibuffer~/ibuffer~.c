
/*
 *  ibuffer~
 *
 *	ibuffer~ is an audio file buffer that loads audio files in their stored integer format (or float format if relevant).
 *
 *	This object is designed primarily for applications with large memory requirements where loading in 16 bit or 24 bit formats saves vital space in memory.
 *	It supports 16, 24 and 32 integer formats, as well as 32 bit float and can load either all channels or specified channels.
 *	There are a corresponding set of playback and other objects that will also function with standard MSP buffers.
 *
 *	ibuffer~ is dependent on libsndfile by Erik de Castro Lopo (http://www.mega-nerd.com/libsndfile/) for interfacing with the soundfiles as stored on disk.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include "file_reading/file_reading.h"

#include <AH_Atomic.h>
#include <ibuffer.h>

void *this_class;

#define DEFAULT_WORK_CHUNK 10000

void *ibuffer_new (t_symbol *name, t_symbol *path_sym);
void ibuffer_name (t_ibuffer *x, t_symbol *s, short argc, t_atom *argv);
void ibuffer_name_internal (t_ibuffer *x, t_symbol *name, short argc, t_atom *argv);
void ibuffer_free (t_ibuffer *x);
void ibuffer_assist (t_ibuffer *x, void *b, long m, long a, char *s);

void ibuffer_name (t_ibuffer *x, t_symbol *s, short argc, t_atom *argv);
void *ibuffer_valid (t_ibuffer *x);
void ibuffer_load (t_ibuffer *x, t_symbol *s, short argc, t_atom *argv);
void ibuffer_doload (t_ibuffer *x, t_symbol *s, short argc, t_atom *argv);


t_symbol *ps_null;


C74_EXPORT int main (void)
{
	this_class = class_new ("ibuffer~", (method) ibuffer_new, (method)ibuffer_free, (short)sizeof(t_ibuffer), 0L, A_DEFSYM, A_DEFSYM, 0);
	
	class_addmethod (this_class, (method)ibuffer_name, "name", A_GIMME, 0);
	class_addmethod (this_class, (method)ibuffer_name, "set", A_GIMME, 0);
	class_addmethod (this_class, (method)ibuffer_load, "open", A_GIMME, 0);
	class_addmethod (this_class, (method)ibuffer_load, "replace", A_GIMME, 0);
	class_addmethod (this_class, (method)ibuffer_valid, "valid", A_CANT, 0);
	class_addmethod (this_class, (method)ibuffer_assist, "assist", A_CANT, 0);
	
	class_dspinit (this_class);
	class_register (CLASS_BOX, this_class);
	
	ps_null = gensym("");
	
	return 0;
}


void *ibuffer_new (t_symbol *name, t_symbol *path_sym)
{
	t_atom temp_atom;
	
    t_ibuffer *x = (t_ibuffer *) object_alloc (this_class);
	
	dsp_setup((t_pxobject *)x, 0);
	
	x->name = 0;
	x->valid = 0;
	x->thebuffer = 0;
	x->frames = 0;
	x->channels = 0;
	x->format = 0;
	x->sr = 0;
	x->inuse = 0;
	
	x->bang_out = bangout(x);
	
	if (name && name != ps_null) 
	{
		atom_setsym(&temp_atom, name);
		ibuffer_name (x, 0, 1, &temp_atom);
	}
	
	if (path_sym && path_sym != ps_null) 
	{
		atom_setsym(&temp_atom, path_sym);
		ibuffer_load (x, 0, 1, &temp_atom);
	}
	
    return (x);
}


void ibuffer_free (t_ibuffer *x)
{
	x->valid = 0;
	
	dsp_free(&x->x_obj);
	free (x->thebuffer);
	
	if (x->name) 
		x->name->s_thing = 0;
}


void ibuffer_assist (t_ibuffer *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET) 
		sprintf(s,"(bang) Buffer Loaded");
    else 
		sprintf(s,"File Operations");
}


void ibuffer_name (t_ibuffer *x, t_symbol *s, short argc, t_atom *argv)
{
	// Send name of buffer as the calling symbol
	
	if (argc)
		defer(x, (method) ibuffer_name_internal, atom_getsym(argv), 0, 0);
}


void ibuffer_name_internal (t_ibuffer *x, t_symbol *name, short argc, t_atom *argv)
{	
	if (!name || name == ps_null || name == x->name) 
		return;
	
	if (name->s_thing)
		object_error ((t_object *) x, "ibuffer~: name %s already in use!", name->s_name);
	else
	{
		if (x->name) 
			x->name->s_thing = 0;
		name->s_thing = (t_object *) x;
		x->name = name;
	}
}


void *ibuffer_valid (t_ibuffer *x)
{	
	return (void *) &x->valid;
}


void ibuffer_load (t_ibuffer *x, t_symbol *s, short argc, t_atom *argv)
{
	if (argc)
		defer(x, (method) ibuffer_doload, s, argc, argv);
}


void ibuffer_doload (t_ibuffer *x, t_symbol *s, short argc, t_atom *argv)
{
	double sr;
	
	char prev_valid = x->valid;
	char endianness_swap = 0;
	char null_char = 0;
	
	char filename[2048];
	char foldname[2048];
	char fullname[2048];
	char *name_ptr = foldname;
	
	long channel_order [4];
	long load_all_channels = 1;
	long channels_to_load = 1;
	long format = PCM_FLOAT;
	long sample_size = 0;
	long colon = 0;
	long channels; 
	long frames;
	long work_chunk;
	long i, j, k;
	
    t_fourcc type = 0;
    
	short path = 0;
	short err;
	
	UInt8 swap;
	UInt8 *data;
	UInt8 *load_temp;
	UInt8 *channels_swap;
	
	t_sndfile *the_sndfile = NULL;
    t_sndfile_info info;
    
	// Get path
	
	t_symbol *path_sym = atom_getsym (argv++);
	argc--;
	
	// Set the ibuffer as invalid
	
	x->valid = 0;

	// Check if in use (can't replace when in use)

	if (ATOMIC_INCREMENT_BARRIER(&x->inuse) > 1) 
	{
		object_error ((t_object *) x, "ibuffer~: in use - cannot replace contents");
		x->valid = prev_valid;
		return;
	}
	
	if (path_sym)
	{
		// Find the file and get the correct filename ss well as that of the enclosing folder (making sure the later is in slash style)
		
		strcpy(filename, path_sym->s_name);
		err = locatefile_extended(filename, &path , &type, &type,-1);
		err |= path_topathname(path, &null_char, foldname);
		err |= path_nameconform (foldname, fullname, PATH_STYLE_NATIVE, PATH_TYPE_ABSOLUTE);
		
		// If we now how a valid filename and folder name copy the strings into a fullname in the correct format 
		
		if (!err)
		{
			
#ifdef __APPLE__
			
			// Start with the root '/Volumes/'
			
			strcpy(fullname, "/Volumes/");
		
			// Now copy the rest of the folder name but stripping the first colon
			
			for (i = 9; *name_ptr; name_ptr++) 
			{
				// colon will equal 1 only for the first colon (which we want to strip)
				
				if (*name_ptr == ':' || colon == 1)
					colon++;
				
				if (*name_ptr && colon != 1)
					fullname[i++] = *name_ptr;
			}
#else
			// Copy string
			
			strcpy(fullname, foldname);		
			i = strlen(foldname);
#endif

			// Add a slash seperator and then copy the file name 
			
			fullname[i++] = '/';

			strcpy(fullname + i, filename);

			// Try to open the file using libsndfile
			
			the_sndfile  = sndfile_open(fullname, &info, NULL);
        }
	}
		
	// Load the format data and if we have a valid format load the sample
	
	if (the_sndfile)
	{		
		// Get sample info 
		
		frames = info.frames;
		channels = info.channels;
        sample_size = info.depth;
		sr = info.sample_rate;
		        
		switch (sample_size)
		{
			case 2:
				format = PCM_INT_16;
				break;
				
			case 3:
				format = PCM_INT_24;
				break;
				
			case 4:
				format = PCM_INT_32;
				break;
				
            default:
				sample_size = 0;
				break;
		}
        
        if (info.num_format == NUMFORMAT_FLOAT)
        {
            format = PCM_FLOAT;
            if (sample_size != 4)
                sample_size = 0;
        }
		
        // Bail if incorrect format
        
        if (!sample_size)
        {
            object_error ((t_object *) x, "ibuffer~: incorrect sample format");
            ATOMIC_DECREMENT_BARRIER(&x->inuse);
            sndfile_close (the_sndfile);
            return;
        }
        
		// Sort channels to load (assume all)
		// Only allow 4 channels max
		
		for (i = 0, channels_to_load = channels; i < ((argc <= 4) ? argc : 4); i++)
		{
			load_all_channels = 0;
			channel_order[i] = (atom_getlong(argv + i) - 1) % channels;
			if (channel_order[i] < 0) 
				channel_order[i] = 0;
			channels_to_load = i + 1;
		}
		
		// Free previous memory and allocate memory to store the sample
		
		free (x->thebuffer);
		x->thebuffer = calloc (sample_size, (frames * channels_to_load + 64));
		x->samples = (void *) ((char *) x->thebuffer + (16 * sample_size));
		data = x->samples;
		
		// Bail if no memory
		
		if (!x->thebuffer)
		{
			object_error ((t_object *) x, "ibuffer~: could not allocate memory to load file");
			ATOMIC_DECREMENT_BARRIER(&x->inuse);
			sndfile_close (the_sndfile);
			return;
		}
		
		// Load the audio data raw and close the file
		
		if (load_all_channels)
			sndfile_read_raw (the_sndfile, x->samples, frames);
		else 
		{
			// Here we load in chunks to some temporary memory and then copy out ony the relevant channels
			
			load_temp = malloc (DEFAULT_WORK_CHUNK * sample_size * channels);
			
			if (!load_temp) 
			{
				object_error ((t_object *) x, "ibuffer~: could not allocate memory to load file");
				ATOMIC_DECREMENT_BARRIER(&x->inuse);
				sndfile_close (the_sndfile);
				return;
			}
			
			for (i = 0; i < channels_to_load; i++)
				channel_order[i] *= sample_size;
			
			for (i = 0; i < (frames + DEFAULT_WORK_CHUNK + 1) / DEFAULT_WORK_CHUNK; i++)
			{				
				// Read chunk
				
				work_chunk = (i + 1) * DEFAULT_WORK_CHUNK > frames ?  frames - (i * DEFAULT_WORK_CHUNK): DEFAULT_WORK_CHUNK;
				sndfile_read_raw (the_sndfile, load_temp, work_chunk);
				
				// Copy channels
				
				for (j = 0, channels_swap = load_temp; j < work_chunk; j++, channels_swap += channels * sample_size)
				{
					for (k = 0; k < channels_to_load; k++, data += sample_size)
						memcpy (data, channels_swap + channel_order[k], sample_size);
				}
			}
			
			// Free temp memory and store relevant variables
			
			free (load_temp);
			channels = channels_to_load;
			data = x->samples;
		}
		
		// If the samples are in the wrong endianness then reverse the byte order for each sample 
		
#if (TARGET_RT_LITTLE_ENDIAN || defined (WIN_VERSION))	
		if (info.endianness == ENDIANNESS_BIG)
			endianness_swap = 1;
#else
		if (info.endianness == ENDIANNESS_LITTLE)
			endianness_swap = 1;
#endif
		
		if (endianness_swap)
		{
			switch (format)
			{
				case PCM_INT_16:
					
					for (i = 0; i < frames * channels; i++)
					{
						swap = data[1];
						data[1] = data[0];
						data[0] = swap;
						data += 2;
					}
					break;
					
				case PCM_INT_24:
					
					for (i = 0; i < frames * channels; i++)
					{
						swap = data[2];
						data[2] = data[0];
						data[0] = swap;
						data += 3;
					}
					break;
					
				case PCM_INT_32:
				case PCM_FLOAT:
					
					for (i = 0; i < frames * channels; i++)
					{
						swap = data[3];
						data[3] = data[0];
						data[0] = swap; 
						swap = data[2];
						data[2] = data[1];
						data[1] = swap;
						data += 4;
					}
					break;
			}
		}
		
		// Store sample info into object
		
		x->sr = sr;
		x->frames = frames;
		x->channels = channels;
		x->format = format;
		
		// File is now loaded - set the buffer to valid and bang (must be in this order!)
		
		x->valid = 1;
		outlet_bang(x->bang_out);		
	}
	else 
	{
		object_error ((t_object *) x, "ibuffer~: could not find / open named file");
	}
	
	ATOMIC_DECREMENT_BARRIER(&x->inuse);
	sndfile_close (the_sndfile);
}

