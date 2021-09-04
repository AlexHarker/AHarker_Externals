
/*
 *  ibufplayer~
 *
 *  ibufplayer~ is an efficient playback object for ibuffer~ and standard buffer~ objects.
 *
 *  ibufplayer~ features SIMD optimisation, individual channel level controls, reduced CPU usage for integer playback speeds and fixed speed, or varispeed (with a base speed) playback.
 *  It is also features three different kinds of cubic interpolation which can be requested as desired.
 *  It should at least (roughly) as fast as groove~ for worst case scenarios, and faster in various other situations.
 *
 *  Copyright 2010-21 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <AH_Denormals.h>
#include <SIMDSupport.hpp>
#include <ibuffer_access.hpp>

#include <limits>
#include <algorithm>

t_class *this_class;


enum t_transport_flag
{
    FLAG_NONE,
    FLAG_PLAY,
    FLAG_STOP,
};

const int max_num_chans = 64;

// Main object struct

struct t_ibufplayer
{
    t_pxobject x_obj;
    
    t_symbol *buffer_name;
    
    t_transport_flag transport_flag;
    
    InterpType interp_type;
    
    double drive;
    double speed;
    double start_samp;
    double min_samp;
    double max_samp;
    double sr_div;
    
    bool sig_control;
    bool input_connected;
    bool playing;
    
    double vols[max_num_chans];
    
    long obj_n_chans;
    
    float *float_outs[max_num_chans];
    
    void *done_clock;
    void *bang_outlet;
};

// Phase info helper

struct phase_info
{
    phase_info(bool sig_control, double speed, double start, double min, double max, double length)
    : m_sig_control(sig_control), m_speed(speed), m_start(start)
    , m_min(std::min(min, length)), m_max(std::min(max, length))
    , m_length_norm((m_speed < 0 ? -1.0 : 1.0) / (m_max - m_min))
    {}
    
    long fixed_speed_loop_size(double drive, long n_samps) const
    {
        if (m_speed > 0)
            return ((n_samps * m_speed) + drive > m_max) ? (m_max - drive) / m_speed : n_samps;
        
        return ((n_samps * m_speed) + drive < m_min) ? (m_min - drive) / m_speed : n_samps;
    }
    
    bool speed_is_int() const { return m_speed == std::round(m_speed) && !m_sig_control; }
    
    bool out_of_range(double drive) const { return drive > m_max || drive < m_min; }
    
    bool sig_control() const { return m_sig_control; }
    
    double speed() const { return m_speed; }
    double start() const { return m_start; }
    double min() const { return m_min; }
    double max() const { return m_max; }
    double length_norm() const { return m_length_norm; }
    
private:
    
    bool m_sig_control;
    
    double m_speed;
    double m_start;
    double m_min;
    double m_max;
    double m_length_norm;
};

void *ibufplayer_new(t_symbol *s, long argc, t_atom *argv);
void ibufplayer_free(t_ibufplayer *x);
void ibufplayer_assist(t_ibufplayer *x, void *b, long m, long a, char *s);

void ibufplayer_set(t_ibufplayer *x, t_symbol *msg, long argc, t_atom *argv);
void ibufplayer_set_internal(t_ibufplayer *x, t_symbol *s);
void ibufplayer_vols(t_ibufplayer *x, t_symbol *s, long argc, t_atom *argv);
void ibufplayer_play(t_ibufplayer *x, t_symbol *s, long argc, t_atom *argv);
void ibufplayer_stop(t_ibufplayer *x);

void ibufplayer_done_bang(t_ibufplayer *x);

t_int *ibufplayer_perform(t_int *w);
void ibufplayer_dsp(t_ibufplayer *x, t_signal **sp, short *count);

void ibufplayer_perform64(t_ibufplayer *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void ibufplayer_dsp64(t_ibufplayer *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);


int C74_EXPORT main()
{
    this_class = class_new ("ibufplayer~",
                            (method) ibufplayer_new,
                            (method) ibufplayer_free,
                            sizeof (t_ibufplayer),
                            NULL,
                            A_GIMME,
                            0);
    
    class_addmethod(this_class, (method)ibufplayer_set, "set", A_GIMME, 0);
    class_addmethod(this_class, (method)ibufplayer_vols, "vols", A_GIMME, 0);
    class_addmethod(this_class, (method)ibufplayer_play, "play", A_GIMME, 0);
    class_addmethod(this_class, (method)ibufplayer_stop, "stop", 0);
    class_addmethod(this_class, (method)ibufplayer_assist, "assist", A_CANT, 0);
    class_addmethod(this_class, (method)ibufplayer_dsp, "dsp", A_CANT, 0);
    class_addmethod(this_class, (method)ibufplayer_dsp64, "dsp64", A_CANT, 0);
    
    // Add Attributes
    
    add_ibuffer_interp_attribute<t_ibufplayer, kInterpCubicHermite>(this_class, "interp");
    
    class_dspinit(this_class);
    class_register(CLASS_BOX, this_class);
    
    return 0;
}

void *ibufplayer_new(t_symbol *s, long argc, t_atom *argv)
{
    t_ibufplayer *x = (t_ibufplayer *)object_alloc(this_class);
    
    // Arguments
    
    long non_attr_argc = attr_args_offset(argc, argv);
    
    t_atom_long obj_n_chans = non_attr_argc > 0 ? atom_getlong(argv + 0) : 1;
    obj_n_chans = std::min(static_cast<t_atom_long>(max_num_chans), std::max(static_cast<t_atom_long>(1), obj_n_chans));
    
    // Setup DSP
    
    dsp_setup((t_pxobject *)x, 1);
    
    // Creat bang outlet
    
    x->bang_outlet = bangout(x);
    
    // Create signal outlets
    
    for (long i = 0; i < obj_n_chans + 1; i++)
        outlet_new((t_object *)x, "signal");
    
    // Default variables
    
    x->drive = 0;
    x->speed = 1;
    x->sig_control = false;
    x->playing = false;
    x->input_connected = false;
    x->transport_flag = FLAG_NONE;
    x->obj_n_chans = obj_n_chans;
    x->buffer_name = NULL;
    
    for (long i = 0 ; i < max_num_chans; i++)
        x->vols[i] = 1.0;
    
    x->interp_type = kInterpCubicHermite;
    
    x->done_clock = clock_new(x, (method) ibufplayer_done_bang);
    
    // Set attributes from arguments
    
    attr_args_process(x, argc, argv);
    
    return x;
}

void ibufplayer_free(t_ibufplayer *x)
{
    dsp_free(&x->x_obj);
    freeobject(static_cast<t_object *>(x->done_clock));
}

void ibufplayer_assist(t_ibufplayer *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
    {
        if (a < x->obj_n_chans)
        {
            sprintf(s,"(signal) Output %ld", a + 1);
        }
        else
        {
            switch (a - x->obj_n_chans)
            {
                case 0:
                    sprintf(s,"(signal) Position Output (0-1)");
                    break;
                case 1:
                    sprintf(s,"(bang) Done Playing");
                    break;
            }
        }
    }
    else
    {
        sprintf(s,"(signal) Speed Input");
    }
}

void ibufplayer_set(t_ibufplayer *x, t_symbol *msg, long argc, t_atom *argv)
{
    ibufplayer_set_internal(x, argc ? atom_getsym(argv) : 0);
}

void ibufplayer_set_internal(t_ibufplayer *x, t_symbol *s)
{
    ibuffer_data buffer(s);
    
    if (buffer.get_type() != kBufferNone)
    {
        if (s != x->buffer_name)
            x->drive = -1.0;
        x->buffer_name = s;
        
    }
    else
    {
        x->buffer_name = NULL;
        if (s)
            object_error((t_object *)x, "no buffer %s", s->s_name);
    }
}

void ibufplayer_vols(t_ibufplayer *x,  t_symbol *s, long argc, t_atom *argv)
{
    for (long i = 0; i < max_num_chans && i < argc; i++)
        x->vols[i] = atom_getfloat(argv++);
}

void ibufplayer_stop(t_ibufplayer *x)
{
    x->transport_flag = FLAG_STOP;
}

double ibuffer_sample_rate(t_symbol *name)
{
    return ibuffer_data(name).get_sample_rate();
}

void ibufplayer_play(t_ibufplayer *x, t_symbol *s, long argc, t_atom *argv)
{
    double sr_ms = 44.1;
    double start_samp;
    double end_samp;
    double start = 0;
    double end = -1;
    double speed = 1;
    t_atom_long sig_control = 0;
    
    // Set buffer if there is one named
    
    if (argc && atom_gettype(argv) == A_SYM)
    {
        ibufplayer_set_internal(x, atom_getsym(argv++));
        argc--;
    }
    
    // Get the following parameters (if given)
    
    if (argc >= 1)
        speed = atom_getfloat(argv++);
    if (argc >= 2)
        start = atom_getfloat(argv++);
    if (argc >= 3)
        end = atom_getfloat(argv++);
    if (argc >= 4)
        sig_control = atom_getlong(argv++);
    
    // If we can't play (no buffer or zero speed) then we report done, so as to free resources depending on this
    
    if (!x->buffer_name || !speed)
    {
        ibufplayer_done_bang(x);
        return;
    }
    
    // Calculate parameters and set the play flag to be picked up in the perform routine
    
    sr_ms = 0.001 * ibuffer_sample_rate(x->buffer_name);
    if (start < 0)
        start = 0;
    if (end < 0)
        end = std::numeric_limits<double>::max();
    
    speed = fabs(speed);
    start_samp = start * sr_ms;
    end_samp = end * sr_ms;
    x->start_samp = start_samp;
    
    // Consider playback direction
    
    if (start < end)
    {
        x->speed = speed;
        x->min_samp = start_samp;
        x->max_samp = end_samp;
    }
    else
    {
        x->speed = -speed;
        x->min_samp = end_samp;
        x->max_samp = start_samp;
    }
    
    x->sig_control = sig_control ? true : false;
    x->transport_flag = FLAG_PLAY;
}

void ibufplayer_done_bang(t_ibufplayer *x)
{
    outlet_bang(x->bang_outlet);
}

// Core Perform Routines

template <class T, class U>
void ibufplayer_update_phase(U *&positions, T *&phases, U &pos, const U &speed, const phase_info &info)
{
    *positions++ = pos;
    *phases++ = (pos - info.start()) * info.length_norm();
    pos += speed;
}

template <int N, class T>
void ibufplayer_phase_fixed(double *positions, T *phases, double& pos, const phase_info &info, long n_vecs)
{
    double pos_array[N];
    
    for (int i = 0; i < N; i++, pos += info.speed())
        pos_array[i] = pos;
    
    SIMDType<double, N> v_pos(pos_array);
    SIMDType<double, N> v_speed(info.speed() * N);
    SIMDType<double, N> *v_positions = reinterpret_cast<SIMDType<double, N> *>(positions);
    SIMDType<T, N>* v_phases = reinterpret_cast<SIMDType<T, N> *>(phases);
    
    for (long i = 0; i < n_vecs; i++)
        ibufplayer_update_phase(v_positions, v_phases, v_pos, v_speed, info);
    
    v_pos.store(pos_array);
    pos = pos_array[0];
}

template <class T>
void perform_core(t_ibufplayer *x, T *in, T **outs, T *phase_out, double *positions, long vec_size)
{
    const int N = SIMDLimits<double>::max_size;
    
    ibuffer_data buffer(x->buffer_name);
    
    long obj_n_chans = x->obj_n_chans;
    long to_do = 0;
    
    // Set default position output
    
    std::fill_n(phase_out, vec_size, 1.0);
    
    // Check on playback state / new play instruction and decide whether to output
    
    if (buffer.get_length())
    {
        double per_samp = x->speed * buffer.get_sample_rate() * x->sr_div;
        
        phase_info info(x->sig_control, per_samp, x->start_samp, x->min_samp, x->max_samp, buffer.get_length());
        
        if (x->transport_flag == FLAG_PLAY)
            x->drive = x->start_samp;
        
        if (x->transport_flag == FLAG_STOP)
            x->drive = -1.0;
        
        if (!info.out_of_range(x->drive))
        {
            x->playing = true;
            
            InterpType interp_type = info.speed_is_int() ? kInterpNone : x->interp_type;
            
            // Calculate the phasor block
            
            double drive = x->drive;
            
            if (info.sig_control())
            {
                for (to_do = 0; to_do < vec_size; to_do++)
                {
                    if (info.out_of_range(drive))
                        break;
                    
                    ibufplayer_update_phase(positions, phase_out, drive, *in++ * info.speed(), info);
                }
            }
            else
            {
                to_do = info.fixed_speed_loop_size(drive, vec_size);
                const long v_count = ((t_ptr_uint) positions % 16 || (t_ptr_uint) phase_out % 16) ? 0 : to_do / N;
                const long S = v_count * N;
                
                ibufplayer_phase_fixed<N>(positions + 0, phase_out + 0, drive, info, v_count);
                ibufplayer_phase_fixed<1>(positions + S, phase_out + S, drive, info, to_do - S);
            }
            
            x->drive = to_do < vec_size ? -1.0 : drive;
            
            // Now get samples interpolating as relevant
            
            for (long i = 0; i < obj_n_chans; i++)
            {
                double vol = x->vols[i];
                
                long chan_to_do = (vol && buffer.get_num_chans() > i) ? to_do : 0;
                
                if (chan_to_do)
                    ibuffer_read(buffer, outs[i], positions, chan_to_do, i, vol, interp_type);
                std::fill_n(outs[i] + chan_to_do, (vec_size - chan_to_do), 0);
            }
        }
    }
    
    // Zero outputs if needed
    
    if (!to_do)
    {
        for (long i = 0; i < obj_n_chans; i++)
            std::fill_n(outs[i], vec_size, 0);
    }
    
    // Calculate playing flag
    
    bool playing = !(to_do < vec_size);
    if (!playing && x->playing)
        clock_delay (x->done_clock, 0);
    x->playing = playing;
    x->transport_flag = FLAG_NONE;
}

// Perform and DSP for 32-bit signals

t_int *ibufplayer_perform(t_int *w)
{
    // Ignore the copy of this function pointer (due to denormal fixer)
    
    // Set pointers
    
    float *in = reinterpret_cast<float *>(w[2]);
    float **outs = reinterpret_cast<float **>(w[3]);
    float *phase_out = reinterpret_cast<float *>(w[4]);
    long vec_size = w[5];
    t_ibufplayer *x = reinterpret_cast<t_ibufplayer *>(w[6]);
    
    if (!x->x_obj.z_disabled)
        perform_core(x, in, outs, phase_out, NULL, vec_size);
    
    return w + 7;
}

void ibufplayer_dsp(t_ibufplayer *x, t_signal **sp, short *count)
{
    x->sr_div = 1.0 / sp[0]->s_sr;
    
    // Set buffer again in case it is no longer valid / extant
    
    ibufplayer_set_internal(x, x->buffer_name);
    
    // Check if input is connected
    
    x->input_connected = count[0];
    
    for (long i = 0; i < x->obj_n_chans; i++)
        x->float_outs[i] = reinterpret_cast<float *>(sp[i + 1]->s_vec);
    
    dsp_add(denormals_perform, 6, ibufplayer_perform, sp[0]->s_vec, x->float_outs, sp[x->obj_n_chans]->s_vec, sp[0]->s_n, x);
}

// Perform and DSP for 64-bit signals

void ibufplayer_perform64(t_ibufplayer *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    perform_core(x, ins[0], outs, outs[numouts - 1], outs[numouts - 2], vec_size);
}

void ibufplayer_dsp64(t_ibufplayer *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    x->sr_div = 1.0 / samplerate;
    
    // Set buffer again in case it is no longer valid / extant
    
    ibufplayer_set_internal(x, x->buffer_name);
    
    // Check if input is connected
    
    x->input_connected = count[0];
    
    object_method(dsp64, gensym("dsp_add64"), x, ibufplayer_perform64);
}
