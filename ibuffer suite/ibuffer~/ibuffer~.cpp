
/*
 *  ibuffer~
 *
 *  ibuffer~ is an audio file buffer that loads audio files in their stored integer format (or float format if relevant).
 *
 *  This object is designed primarily for applications with large memory requirements where loading in 16 bit or 24 bit formats saves vital space in memory.
 *  It supports 16, 24 and 32 integer formats, as well as 32 bit float and can load either all channels or specified channels.
 *  There are a corresponding set of playback and other objects that will also function with standard MSP buffers.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <algorithm>

#include "AudioFile/IAudioFile.h"

#include <ibuffer.hpp>


t_class *this_class;


void *ibuffer_new(t_symbol *name, t_symbol *path_sym);
void ibuffer_name(t_ibuffer *x, t_symbol *s, short argc, t_atom *argv);
void ibuffer_name_internal(t_ibuffer *x, t_symbol *name, short argc, t_atom *argv);
void ibuffer_free(t_ibuffer *x);
void ibuffer_assist(t_ibuffer *x, void *b, long m, long a, char *s);

void ibuffer_name(t_ibuffer *x, t_symbol *s, short argc, t_atom *argv);
void *ibuffer_valid(t_ibuffer *x);
void ibuffer_load(t_ibuffer *x, t_symbol *s, short argc, t_atom *argv);
void ibuffer_doload(t_ibuffer *x, t_symbol *s, short argc, t_atom *argv);


t_symbol *ps_null;


int C74_EXPORT main()
{
    this_class = class_new("ibuffer~",
                           (method) ibuffer_new,
                           (method) ibuffer_free,
                           sizeof(t_ibuffer),
                           (method) nullptr,
                           A_DEFSYM,
                           A_DEFSYM,
                           0);
    
    class_addmethod(this_class, (method) ibuffer_name, "name", A_GIMME, 0);
    class_addmethod(this_class, (method) ibuffer_name, "set", A_GIMME, 0);
    class_addmethod(this_class, (method) ibuffer_load, "open", A_GIMME, 0);
    class_addmethod(this_class, (method) ibuffer_load, "replace", A_GIMME, 0);
    
    class_addmethod(this_class, (method) ibuffer_valid, "valid", A_CANT, 0);
    class_addmethod(this_class, (method) ibuffer_assist, "assist", A_CANT, 0);
    
    class_dspinit(this_class);
    class_register(CLASS_BOX, this_class);
    
    ps_null = gensym("");
    
    return 0;
}


void *ibuffer_new(t_symbol *name, t_symbol *path_sym)
{
    t_atom temp_atom;
    
    t_ibuffer *x = (t_ibuffer *) object_alloc(this_class);
    
    dsp_setup((t_pxobject *) x, 0);
    
    x->name = 0;
    x->memory = nullptr;
    x->samples = nullptr;
    x->frames = 0;
    x->channels = 0;
    x->format = PCM_FLOAT;
    x->sr = 0;
    x->inuse = 0;
    x->valid = 1;
    
    x->bang_out = bangout(x);
    
    if (name && name != ps_null)
    {
        atom_setsym(&temp_atom, name);
        ibuffer_name(x, 0, 1, &temp_atom);
    }
    
    if (path_sym && path_sym != ps_null)
    {
        atom_setsym(&temp_atom, path_sym);
        ibuffer_load(x, 0, 1, &temp_atom);
    }
    
    return x;
}

void ibuffer_free(t_ibuffer *x)
{
    x->valid = 0;
    
    dsp_free(&x->x_obj);
    free(x->memory);
    
    if (x->name)
        x->name->s_thing = 0;
}

void ibuffer_assist(t_ibuffer *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
        sprintf(s,"(bang) Buffer Loaded");
    else
        sprintf(s,"File Operations");
}

void ibuffer_name(t_ibuffer *x, t_symbol *s, short argc, t_atom *argv)
{
    // Send name of buffer as the calling symbol
    
    if (argc)
        defer(x, (method) ibuffer_name_internal, atom_getsym(argv), 0, 0);
}

void ibuffer_name_internal(t_ibuffer *x, t_symbol *name, short argc, t_atom *argv)
{
    if (!name || name == ps_null || name == x->name)
        return;
    
    if (name->s_thing)
        object_error((t_object *) x, "ibuffer~: name %s already in use!", name->s_name);
    else
    {
        if (x->name)
            x->name->s_thing = 0;
        name->s_thing = (t_object *) x;
        x->name = name;
    }
}

void *ibuffer_valid(t_ibuffer *x)
{
    return (void *) &x->valid;
}

void ibuffer_load(t_ibuffer *x, t_symbol *s, short argc, t_atom *argv)
{
    if (argc)
        defer(x, (method) ibuffer_doload, s, argc, argv);
}

struct ibuffer_lock
{
    ibuffer_lock(t_ibuffer *x, HISSTools::Utility::IAudioFile *file) : m_ibuffer(x), m_file(file)
    {
        // Set invalid and wait till we can become the only user
        
        while (!ATOMIC_COMPARE_SWAP32(1, 0, &x->valid));
        while (!ATOMIC_COMPARE_SWAP32(0, 1, &x->inuse));
    }
    
    ~ibuffer_lock()
    {
        destroy();
    }
    
    void destroy()
    {
        // Set the buffer as valid and decrement the usage count
        
        if (m_ibuffer)
        {
            while (!ATOMIC_COMPARE_SWAP32(0, 1, &m_ibuffer->valid));
            ATOMIC_DECREMENT_BARRIER(&m_ibuffer->inuse);
        }
        
        if (m_file)
            m_file->close();
        
        m_ibuffer = nullptr;
        m_file = nullptr;
    }
    
    t_ibuffer *m_ibuffer;
    HISSTools::Utility::IAudioFile *m_file;
};

#ifdef __APPLE__
void form_os_name(char *filename, char *foldname, char *fullname)
{
    char *name_ptr = foldname;
    long offset;
    long colon = 0;
    
    // Start with the root '/Volumes/'
    
    strcpy(fullname, "/Volumes/");
    
    // Now copy the rest of the folder name but stripping the first colon
    
    for (offset = 9; *name_ptr; name_ptr++)
    {
        // colon will equal 1 only for the first colon (which we want to strip)
        
        if (*name_ptr == ':' || colon == 1)
            colon++;
        
        if (*name_ptr && colon != 1)
            fullname[offset++] = *name_ptr;
    }
    
    // Add a slash seperator and then copy the file name
    
    fullname[offset++] = '/';
    strcpy(fullname + offset, filename);
}
#else
void form_os_name(char *filename, char *foldname, char *fullname)
{
    size_t offset = strlen(foldname);
    
    // Copy folder name, add a slash seperator and then copy the file name
    
    strcpy(fullname, foldname);
    fullname[offset++] = '/';
    strcpy(fullname + offset, filename);
}
#endif

void ibuffer_switch_endianness(t_ibuffer *x)
{
    uint8_t *data = reinterpret_cast<uint8_t*>(x->samples);
    
    if (x->format == PCM_INT_16)
    {
        for (t_ptr_int i = 0; i < x->frames * x->channels; i++, data += 2)
            std::swap(data[0], data[1]);
    }
    else if (x->format == PCM_INT_24)
    {
        for (t_ptr_int i = 0; i < x->frames * x->channels; i++, data += 3)
            std::swap(data[0], data[2]);
    }
    else
    {
        // (x->format == PCM_INT_32 or PCM_FLOAT)
        
        for (t_ptr_int i = 0; i < x->frames * x->channels; i++, data += 4)
        {
            std::swap(data[0], data[3]);
            std::swap(data[1], data[2]);
        }
    }
}

void ibuffer_doload(t_ibuffer *x, t_symbol *s, short argc, t_atom *argv)
{
    HISSTools::Utility::IAudioFile file;
    
    // Get path
    
    if (argc)
    {
        char filename[2048];
        char foldname[2048];
        char fullname[2048];
        
        char null_char = 0;
        
        short path = 0;
        t_fourcc type = 0;
        
        // Find the file and get the correct filename ss well as that of the enclosing folder (making sure the later is in slash style)
        
        strcpy(filename, atom_getsym(argv++)->s_name);
        argc--;
        
        short err = locatefile_extended(filename, &path , &type, &type,-1);
        err |= path_topathname(path, &null_char, foldname);
        err |= path_nameconform(foldname, fullname, PATH_STYLE_NATIVE, PATH_TYPE_ABSOLUTE);
        
        // If we now how a valid filename and folder name copy the strings into a fullname in the correct format and try to open the file
        
        if (!err)
        {
            form_os_name(filename, foldname, fullname);
            file.open(fullname);
        }
    }
    
    // Lock the ibuffer (automatically releases on return)
    
    ibuffer_lock lock(x, &file);
    
    if (file.isOpen())
    {
        // Load the format data and if we have a valid format load the sample
        
        x->frames = file.getFrames();
        x->channels = file.getChannels();
        x->sr = file.getSamplingRate();
        
        switch (file.getPCMFormat())
        {
            case HISSTools::Utility::BaseAudioFile::kAudioFileInt16:    x->format = PCM_INT_16;    break;
            case HISSTools::Utility::BaseAudioFile::kAudioFileInt24:    x->format = PCM_INT_24;    break;
            case HISSTools::Utility::BaseAudioFile::kAudioFileInt32:    x->format = PCM_INT_32;    break;
            case HISSTools::Utility::BaseAudioFile::kAudioFileFloat32:  x->format = PCM_FLOAT;     break;
                
            default:
                object_error((t_object *) x, "ibuffer~: incorrect sample format");
                return;
        }
        
        // Sort channels to load (assume all if argc is zero)
        
        std::vector<long> channel_order(argc);
        
        for (long i = 0; i < channel_order.size(); i++)
        {
            t_atom_long channel = atom_getlong(argv + i) - 1;
            channel_order[i] = channel < 0 ? 0 : ((channel > x->channels - 1) ? x->channels - 1 : channel);
        }
        
        // Free previous memory and allocate memory to store the sample
        
        long num_chans_to_load = channel_order.size() ? channel_order.size() : x->channels;
        long sample_size = file.getByteDepth();
        
        free(x->memory);
        x->memory = calloc(sample_size, (x->frames * num_chans_to_load + 64));
        x->samples = (void *)((char *) x->memory + (16 * sample_size));
        
        // Bail if no memory
        
        if (!x->memory)
        {
            object_error((t_object *) x, "ibuffer~: could not allocate memory to load file");
            return;
        }
        
        // Load the raw audio data and close the file
        
        if (!channel_order.size())
            file.readRaw(x->samples, x->frames);
        else
        {
            constexpr t_ptr_int default_work_chunk = 8192;
            
            // Here we load in chunks to some temporary memory and then copy out ony the relevant channels
            
            uint8_t *data = (uint8_t *) x->samples;
            uint8_t *load_temp = (uint8_t *) malloc(default_work_chunk * sample_size * x->channels);
            
            if (!load_temp)
            {
                object_error((t_object *) x, "ibuffer~: could not allocate memory to load file");
                return;
            }
            
            for (long i = 0; i < x->frames + default_work_chunk; i += default_work_chunk)
            {
                // Read chunk
                
                t_ptr_int work_chunk = std::min(default_work_chunk, x->frames - i);
                file.readRaw(load_temp, work_chunk);
                
                // Copy channels
                
                uint8_t*channels_swap = load_temp;
                
                for (long j = 0; j < work_chunk; j++, channels_swap += x->channels * sample_size)
                    for (long k = 0; k < channel_order.size(); k++, data += sample_size)
                        memcpy(data, channels_swap + (channel_order[k] * sample_size), sample_size);
            }
            
            // Free temp memory and store relevant variables
            
            free(load_temp);
            x->channels = static_cast<long>(channel_order.size());
        }
        
        // If the samples are in the wrong endianness then reverse the byte order for each sample
        
        if (file.getAudioEndianness() == HISSTools::Utility::BaseAudioFile::kAudioFileBigEndian)
            ibuffer_switch_endianness(x);
        
        // File is now loaded - destroy the lock and bang (must be in this order!)
        
        lock.destroy();
        outlet_bang(x->bang_out);
    }
    else
    {
        object_error((t_object *) x, "ibuffer~: could not find / open named file");
    }
}
