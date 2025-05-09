
/*
 *  voicemanager~
 *
 *  voicemanager~ is a (sub)sample-accurate voice manager for use with the voicedrive~ object.
 *
 *  Typically the voicemanager~ is located outside of a poly~ / dynamic~ host object.
 *  The voicedrive~ objects are then hosted in each voice of the host object.
 *
 *  The trigger input causes a voice to be assigned with the given length and subsample offset.
 *  These values must be known before the voice starts.
 *  voicemanager~ keeps track of which voices are free/busy (assuming the output is passed on correctly).
 *
 *  Outgoing triggers are numbered according to voice.
 *  These can be tested using an audio rate comparison to derive triggers for individual voices.
 * *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <algorithm>


// Globals and Object Structure

t_class *this_class;

constexpr long max_voices = 1000000;

struct t_voicemanager
{
    t_pxobject x_obj;
    
    double *free_times;
    
    double current_time;
    double sr_val;
    
    long num_voices;
    long active_voices;
    long active_connected;
    
    bool reset;
};

// Function Prototypes

void *voicemanager_new(t_atom_long num_voices);
void voicemanager_free(t_voicemanager *x);
void voicemanager_assist(t_voicemanager *x, void *b, long m, long a, char *s);

void voicemanager_reset(t_voicemanager *x);
void voicemanager_active(t_voicemanager *x, t_symbol *msg, long argc, t_atom *argv);

void voicemanager_perform64(t_voicemanager *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void voicemanager_dsp64(t_voicemanager *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags);

// Main

int C74_EXPORT main()
{
    this_class = class_new("voicemanager~",
                           (method) voicemanager_new,
                           (method) voicemanager_free,
                           sizeof(t_voicemanager),
                           (method) nullptr,
                           A_DEFLONG,
                           0);
    
    class_addmethod(this_class, (method) voicemanager_reset, "reset", 0);
    class_addmethod(this_class, (method) voicemanager_active, "active", A_GIMME, 0);
    class_addmethod(this_class, (method) voicemanager_assist, "assist", A_CANT, 0);
    class_addmethod(this_class, (method) voicemanager_dsp64, "dsp64", A_CANT, 0);
    
    class_dspinit(this_class);
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / Free

void *voicemanager_new(t_atom_long num_voices)
{
    t_voicemanager *x = (t_voicemanager *) object_alloc(this_class);
    
    dsp_setup((t_pxobject *) x, 4);
    outlet_new((t_object *) x, "signal");
    outlet_new((t_object *) x, "signal");
    outlet_new((t_object *) x, "signal");
    
    x->num_voices = static_cast<long>(std::max(1L, std::min(static_cast<long>(num_voices), max_voices)));
    x->free_times = reinterpret_cast<double *>(malloc(sizeof(double) * x->num_voices));
    x->active_voices = x->num_voices;
    x->reset = true;
    
    return x;
}

void voicemanager_free(t_voicemanager *x)
{
    dsp_free(&x->x_obj);
    free(x->free_times);
}

// Message Handlers

void voicemanager_reset(t_voicemanager *x)
{
    x->reset = true;
}

void voicemanager_active(t_voicemanager *x, t_symbol *msg, long argc, t_atom *argv)
{
    long active = x->num_voices;
    
    if (argc)
        active = static_cast<long>(atom_getlong(argv));
    
    x->active_voices = std::max(0L, std::min(x->num_voices, active));
}

// Perform

void voicemanager_perform64(t_voicemanager *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    // Set pointers
    
    double *in1 = ins[0];
    double *in2 = ins[1];
    double *in3 = ins[2];
    double *in4 = ins[3];
    double *out1 = outs[0];
    double *out2 = outs[1];
    double *out3 = outs[2];
    
    long num_voices = x->num_voices;
    long active_voices = x->active_voices;
    long active_connected = x->active_connected;
    long voice_idx;
    
    double *free_times = x->free_times;
    
    double length;
    double subsample_offset;
    double current_time = x->current_time;
    double sr_val = x->sr_val;
    
    // Reset the data for all voices
    
    if (x->reset)
    {
        for (long i = 0; i < num_voices; i++)
            free_times[i] = current_time;
        
        current_time = 0.0;
        x->reset = false;
    }
    
    // Sample loop
    
    while (vec_size--)
    {
        // Reset variables
        
        voice_idx = 0;
        length = 0.0;
        subsample_offset = 0.0;
        
        active_voices = active_connected ? (long) *in4 : active_voices;
        active_voices  = active_voices > num_voices ? num_voices : active_voices;
        
        if (*in1 && (*in3  < 1.0))
        {
            // Search for a free and active voice
            
            for (voice_idx = 1; free_times[voice_idx - 1] > current_time && voice_idx <= active_voices; voice_idx++);
            
            if (voice_idx <= active_voices)
            {
                // Get data for trigger
                
                length = *in2;
                subsample_offset = *in3;
                
                if (subsample_offset < 0.0)
                    subsample_offset = 0.0;
                
                free_times[voice_idx - 1] = current_time + (sr_val * length) - subsample_offset;
            }
            else
                voice_idx = 0;
        }
        
        // Advance input pointers
        
        in1++;
        in2++;
        in3++;
        in4++;
        
        // Output parameters
        
        *out1++ = voice_idx;
        *out2++ = length;
        *out3++ = subsample_offset;
        
        current_time += 1.0;
    }
    
    x->current_time = current_time;
}

// DSP

void voicemanager_dsp64(t_voicemanager *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags)
{
    x->sr_val = sample_rate / 1000.0;
    x->active_connected = count[3];
    
    object_method(dsp64, gensym("dsp_add64"), x, voicemanager_perform64, 0, nullptr);
}

// Assist

void voicemanager_assist(t_voicemanager *x, void *b, long m, long a, char *s)
{
    static constexpr int maxAssist = 256;

    switch (a)
    {
        case 0:
            snprintf(s, maxAssist, "(signal) Triggers");
            break;
            
        case 1:
            snprintf(s, maxAssist, "(signal) Lengths");
            break;
            
        case 2:
            snprintf(s, maxAssist, "(signal) Subsample Offset");
            break;
            
        case 3:
            snprintf(s, maxAssist, "(signal) Number Active Voices");
            break;
    }
}
