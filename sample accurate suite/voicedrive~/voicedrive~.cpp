
/*
 *  voicedrive~
 *
 *  voicedrive~ is a (sub)sample-accurate (usually non-interrupting) ramp generator.
 *  It has with outputs for driving grains, playback voices or other similar processes.
 *
 *  Typically voicedrive~ is used in connection with the voicemanager~ object.
 *  In this case it is used as the drive unit for each individual voice of a process.
 *  It can also be used for other sample-accurate timing purposes such as random length ramping.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>


// Globals and Object Structure

t_class *this_class;

struct t_voicedrive
{
    t_pxobject x_obj;
    
    bool interrupt_on;
    bool precision;
    
    double drive;
    double length;
    double length_val;
    double out_offset;
    double sr_val;
};

// Function Prototypes

void *voicedrive_new(t_atom_long precision, t_atom_long interrupt_on);
void voicedrive_free(t_voicedrive *x);
void voicedrive_assist(t_voicedrive *x, void *b, long m, long a, char *s);

t_int *voicedrive_perform(t_int *w);
t_int *voicedrive_perform_hr(t_int *w);

void voicedrive_perform64(t_voicedrive *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

void voicedrive_dsp(t_voicedrive *x, t_signal **sp, short *count);
void voicedrive_dsp64(t_voicedrive *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags);

// Main

int C74_EXPORT main()
{
    this_class = class_new("voicedrive~",
                           (method) voicedrive_new,
                           (method) voicedrive_free,
                           sizeof(t_voicedrive),
                           (method) nullptr,
                           A_DEFLONG,
                           A_DEFLONG,
                           0);
    
    class_addmethod(this_class, (method) voicedrive_assist, "assist", A_CANT, 0);
    class_addmethod(this_class, (method) voicedrive_dsp, "dsp", A_CANT, 0);
    class_addmethod(this_class, (method) voicedrive_dsp64, "dsp64", A_CANT, 0);
    
    class_dspinit(this_class);
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

// New / Free

void *voicedrive_new(t_atom_long precision, t_atom_long interrupt_on)
{
    t_voicedrive *x = (t_voicedrive *) object_alloc(this_class);
    
    dsp_setup((t_pxobject *) x, 4);
    outlet_new((t_object *) x, "signal");
    outlet_new((t_object *) x, "signal");
    outlet_new((t_object *) x, "signal");
    outlet_new((t_object *) x, "signal");
    if (precision)
        outlet_new((t_object *) x, "signal");
    
    x->interrupt_on = interrupt_on ? true : false;
    x->precision = precision ? true : false;
    x->drive = 1.0;
    x->length = 1.0;
    x->length_val = 1.0;
    x->out_offset = 0.0;
    
    return x;
}

void voicedrive_free(t_voicedrive *x)
{
    dsp_free(&x->x_obj);
}

// Perform

// Standard Perform Routine

t_int *voicedrive_perform(t_int *w)
{
    // Set pointers
    
    float *in1 = (float *) w[1];
    float *in2 = (float *) w[2];
    float *in3 = (float *) w[3];
    float *in4 = (float *) w[4];
    float *out1 = (float *) w[5];
    float *out2 = (float *) w[6];
    float *out3 = (float *) w[7];
    float *out4 = (float *) w[8];
    long vec_size = (long) w[9];
    t_voicedrive *x = (t_voicedrive *) w[10];
    
    bool interrupt_on = x->interrupt_on;
    
    float trigger, busy;
    
    double pos, norm;
    double length = x->length;
    double drive = x->drive;
    double length_val = x->length_val;
    double out_offset = x->out_offset;
    double sr_val = x->sr_val;
    double msr_recip = 1.0 / sr_val;
    
    while (vec_size--)
    {
        // Check for new trigger conditions
        
        if (*in1 && *in2 > 0.f && *in3 < 1.f && (interrupt_on || drive >= length))
        {
            trigger = 1.f;
            length = *in2 * sr_val;
            length_val = 1.0 / length;
            drive = *in3;
            out_offset = *in4;
            
            if (drive < 0.0)
                drive = 0.0;
        }
        else
            trigger = 0.f;
        
        // Calculate current drive position and related parameters
        
        drive += 1.0;
        
        if (drive >= length)
        {
            drive = length;
            busy = 0.f;
        }
        else
            busy = 1.f;
        
        norm = drive * length_val;
        pos = drive * msr_recip + out_offset;
        
        // Advance input pointers
        
        in1++;
        in2++;
        in3++;
        in4++;
        
        // Output
        
        *out1++ = (float) pos;
        *out2++ = (float) norm;
        *out3++ = trigger;
        *out4++ = busy;
    }
    
    x->drive = drive;
    x->length = length;
    x->length_val = length_val;
    x->out_offset = out_offset;
    
    return w + 11;
}

// High-Resolution Perform Routine

t_int *voicedrive_perform_hr(t_int *w)
{
    // Set pointers
    
    float *in1 = (float *) w[1];
    float *in2 = (float *) w[2];
    float *in3 = (float *) w[3];
    float *in4 = (float *) w[4];
    float *out1 = (float *) w[5];
    float *out2 = (float *) w[6];
    float *out3 = (float *) w[7];
    float *out4 = (float *) w[8];
    float *out5 = (float *) w[9];
    long vec_size = (long) w[10];
    t_voicedrive *x = (t_voicedrive *) w[11];
    
    bool interrupt_on = x->interrupt_on;
    
    float trigger, busy, fpos;
    
    double pos, norm;
    double length = x->length;
    double drive = x->drive;
    double length_val = x->length_val;
    double out_offset = x->out_offset;
    double sr_val = x->sr_val;
    double msr_recip = 1.0 / sr_val;
    
    while (vec_size--)
    {
        // Check for new trigger conditions
        
        if (*in1 && *in2 > 0.f && *in3 < 1.f && (interrupt_on || drive >= length))
        {
            trigger = 1.f;
            length = *in2 * sr_val;
            length_val = 1.0 / length;
            drive = *in3;
            out_offset = *in4;
            
            if (drive < 0.0)
                drive = 0.0;
        }
        else
            trigger = 0.f;
        
        // Calculate current drive position and related parameters
        
        drive += 1.0;
        
        if (drive >= length)
        {
            drive = length;
            busy = 0.f;
        }
        else
            busy = 1.f;
        
        norm = drive * length_val;
        pos = drive * msr_recip + out_offset;
        
        // Advance input pointers
        
        in1++;
        in2++;
        in3++;
        in4++;
        
        // Output
        
        *out1++ = fpos = (float) pos;
        *out2++ = (float) pos - fpos;
        *out3++ = (float) norm;
        *out4++ = trigger;
        *out5++ = busy;
    }
    
    x->length = length;
    x->length_val = length_val;
    x->drive = drive;
    x->out_offset = out_offset;
    
    return w + 12;
}

// 64 bit Perform Routine

void voicedrive_perform64(t_voicedrive *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    bool precision = x->precision;
    
    // Set pointers
    
    double *in1 = ins[0];
    double *in2 = ins[1];
    double *in3 = ins[2];
    double *in4 = ins[3];
    double *out1 = outs[0];
    double *out2 = precision ? outs[2] : outs[1];
    double *out3 = precision ? outs[3] : outs[2];
    double *out4 = precision ? outs[4] : outs[3];
    double *hr_out = precision ? outs[1] : nullptr;
    
    long interrupt_on = x->interrupt_on;
    long i;
    
    double trigger, pos, norm, busy;
    double length = x->length;
    double drive = x->drive;
    double length_val = x->length_val;
    double out_offset = x->out_offset;
    double sr_val = x->sr_val;
    double msr_recip = 1.0 / sr_val;
    
    for (i = 0; i < vec_size; i++)
    {
        // Check for new trigger conditions
        
        if (*in1 && *in2 > 0.0 && *in3 < 1.0 && (interrupt_on || drive >= length))
        {
            trigger = 1.0;
            length = *in2 * sr_val;
            length_val = 1.0 / length;
            drive = *in3;
            out_offset = *in4;
            
            if (drive < 0.0)
                drive = 0.0;
        }
        else
            trigger = 0.0;
        
        // Calculate current drive position and related parameters
        
        drive += 1.0;
        
        if (drive >= length)
        {
            drive = length;
            busy = 0.0;
        }
        else
            busy = 1.0;
        
        pos = drive * msr_recip + out_offset;
        norm = drive * length_val;
        
        // Advance input pointers
        
        in1++;
        in2++;
        in3++;
        in4++;
        
        // Output
        
        *out1++ = pos;
        *out2++ = norm;
        *out3++ = trigger;
        *out4++ = busy;
    }
    
    // Zero the hr output if in precision mode
    
    if (hr_out)
        for (i = 0; i < vec_size; i++)
            hr_out[i] = 0.0;
    
    x->length = length;
    x->length_val = length_val;
    x->drive = drive;
    x->out_offset = out_offset;
}

// DSP

void voicedrive_dsp(t_voicedrive *x, t_signal **sp, short *count)
{
    x->sr_val = sp[0]->s_sr / 1000.0;
    
    if (x->precision)
        dsp_add(voicedrive_perform_hr, 11, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec, sp[7]->s_vec, sp[8]->s_vec, sp[0]->s_n, x);
    else
        dsp_add(voicedrive_perform, 10, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec, sp[7]->s_vec, sp[0]->s_n, x);
}

void voicedrive_dsp64(t_voicedrive *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags)
{
    x->sr_val = sample_rate / 1000.0;
    object_method(dsp64, gensym("dsp_add64"), x, voicedrive_perform64, 0, nullptr);
}

// Assist

void voicedrive_assist(t_voicedrive *x, void *b, long m, long a, char *s)
{
    
    if (m == ASSIST_INLET)
    {
        switch (a)
        {
            case 0:
                sprintf(s,"(signal) Triggers");
                break;
                
            case 1:
                sprintf(s,"(signal) Lengths");
                break;
                
            case 2:
                sprintf(s,"(signal) Subsample Offset");
                break;
                
            case 3:
                sprintf(s,"(signal) Output Offset");
                break;
        }
    }
    else
    {
        if (!x->precision && a > 0)
            a++;
        
        switch (a)
        {
            case 0:
                sprintf(s,"(signal) Position (ms)");
                break;
                
            case 1:
                sprintf(s,"(signal) Position Hi-Res (ms)");
                break;
                
            case 2:
                sprintf(s,"(signal) Normalised Position (0-1)");
                break;
                
            case 3:
                sprintf(s,"(signal) Triggers");
                break;
                
            case 4:
                sprintf(s,"(signal) Busy");
                break;
        }
    }
}
