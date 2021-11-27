
/*
 *  ibufconcatenate~
 *
 *  The ibufconcatenate~ object is used to handle data to do with sections of a buffer.
 *
 *  It can be used in one of two ways:
 *
 *      1 - entries - store start and end points for items in a buffer by entering them manually.
 *      2 - concatenation - concatenate multiple msp buffers into one buffer, storing start and end points.
 *
 *  The bounds of each chunk can be retrieved either as a message, or as a signal in a sample-accurate manner, according to the mode of the object.
 *  The ibufconcatedrive~ object can also be used to give sample-accurate playback of only one item at a time.
 *
 *  See the helpfile documentation for more on how this object can be used in practice.
 *
 *  Copyright 2010-21 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <algorithm>

#include <ibuffer_access.hpp>
#include "ibufconcatenate_info.hpp"


t_class *this_class;


constexpr int BUFFER_GROW_SIZE = 1048576;


struct t_ibufconcatenate
{
    t_pxobject x_obj;
    
    t_ibufconcatenate_info *attachment;
    
    t_atom_long max_mode;
    
    void *data_out;
    void *last_added_out;
};


void *ibufconcatenate_new(t_symbol *buffer_name, t_atom_long max_mode);
void ibufconcatenate_free(t_ibufconcatenate *x);
void ibufconcatenate_assist(t_ibufconcatenate *x, void *b, long m, long a, char *s);

void ibufconcatenate_set(t_ibufconcatenate *x, t_symbol *buffer_name);
void ibufconcatenate_clear(t_ibufconcatenate *x);
void ibufconcatenate_append(t_ibufconcatenate *x, t_symbol *source_name);
void ibufconcatenate_entry(t_ibufconcatenate *x, double beg, double end);
void ibufconcatenate_output(t_ibufconcatenate *x, long item, double beg, double end);

static inline t_ptr_int mstosamps(double ms, double sr);
void ibufconcatenate_int(t_ibufconcatenate *x, long in);

t_int *ibufconcatenate_perform(t_int *w);
void ibufconcatenate_perform64(t_ibufconcatenate *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

void ibufconcatenate_dsp(t_ibufconcatenate *x, t_signal **sp, short *count);
void ibufconcatenate_dsp64(t_ibufconcatenate *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);


int C74_EXPORT main()
{
    this_class = class_new ("ibufconcatenate~",
                            (method) ibufconcatenate_new,
                            (method)ibufconcatenate_free,
                            sizeof(t_ibufconcatenate),
                            0L,
                            A_SYM,
                            A_DEFLONG,
                            0);
    
    class_addmethod (this_class, (method)ibufconcatenate_int, "int", A_LONG, 0L);
    class_addmethod (this_class, (method)ibufconcatenate_set, "set", A_SYM, 0L);
    class_addmethod (this_class, (method)ibufconcatenate_clear, "clear", 0L);
    class_addmethod (this_class, (method)ibufconcatenate_append, "append", A_SYM, 0L);
    class_addmethod (this_class, (method)ibufconcatenate_entry, "entry", A_DEFFLOAT, A_DEFFLOAT, 0L);
    class_addmethod (this_class, (method)ibufconcatenate_assist, "assist", A_CANT, 0L);
    class_addmethod(this_class, (method)ibufconcatenate_dsp, "dsp", A_CANT, 0);
    class_addmethod(this_class, (method)ibufconcatenate_dsp64, "dsp64", A_CANT, 0);
    
    class_register(CLASS_BOX, this_class);
    
    class_dspinit(this_class);
    
    return 0;
}

void *ibufconcatenate_new(t_symbol *buffer_name, t_atom_long max_mode)
{
    t_ibufconcatenate *x = (t_ibufconcatenate *)object_alloc(this_class);
    
    x->last_added_out = listout(x);
    
    if (!max_mode)
    {
        dsp_setup((t_pxobject *)x, 1);
        outlet_new((t_object *)x, "signal");
        outlet_new((t_object *)x, "signal");
        outlet_new((t_object *)x, "signal");
        outlet_new((t_object *)x, "signal");
    }
    else
    {
        x->data_out = listout(x);
    }
    
    x->attachment = attach_ibufconcatenate_info(buffer_name);
    x->max_mode = max_mode;
    
    return x;
}

void ibufconcatenate_free(t_ibufconcatenate *x)
{
    if (!x->max_mode)
        dsp_free(&x->x_obj);
    detach_ibufconcatenate_info(x->attachment);
}

void ibufconcatenate_assist(t_ibufconcatenate *x, void *b, long m, long a, char *s)
{
    if (!x->max_mode)
    {
        if (m == ASSIST_OUTLET)
        {
            switch (a)
            {
                case 0:
                    sprintf(s,"(signal) Start (ms)");
                    break;
                    
                case 1:
                    sprintf(s,"(signal) Start High Resolution (ms)");
                    break;
                    
                case 2:
                    sprintf(s,"(signal) End (ms)");
                    break;
                    
                case 3:
                    sprintf(s,"(signal) End High Resolution (ms)");
                    break;
                    
                case 4:
                    sprintf(s,"Last Item Added");
                    break;
            }
        }
        else
        {
            sprintf(s,"(signal) Item / Entries / Concatenation Instructions");
        }
    }
    else
    {
        if (m == ASSIST_OUTLET)
        {
            switch (a)
            {
                case 0:
                    sprintf(s,"Item Info Out");
                    break;
                    
                case 1:
                    sprintf(s,"Last Item Added");
                    break;
            }
        }
        else
            sprintf(s,"Entries / Concatenation Instructions / Item (int)");
    }
}

void ibufconcatenate_set(t_ibufconcatenate *x, t_symbol *buffer_name)
{
    detach_ibufconcatenate_info(x->attachment);
    x->attachment = attach_ibufconcatenate_info(buffer_name);
}

void ibufconcatenate_clear(t_ibufconcatenate *x)
{
    t_ibufconcatenate_info::write_access access(x->attachment);
    access.clear();
}

long ibufconcatenate_append_internal(t_ibufconcatenate *x, t_symbol *source_name, double &beg, double &end)
{
    t_ibufconcatenate_info::write_access access(x->attachment);
    
    ibuffer_data target(access.buffername());
    ibuffer_data source(source_name);
    
    if (target.get_type() != kBufferMaxBuffer)
        return -1;
    
    t_ptr_int offset = mstosamps(access.get_max_end(), target.get_sample_rate()) + 2;
    t_ptr_int required_length = offset + source.get_length() + 1;
    
    // Check we have enough memory in the buffer
    
    if (required_length >= target.get_length())
    {
        struct temp_buffer
        {
            temp_buffer(t_ptr_int size) : m_ptr(static_cast<float*>(malloc(sizeof(float) * size))) {}
            ~temp_buffer() { free(m_ptr); }
            float *m_ptr;
        };
        
        // Calculate memory to allocate
        
        t_ptr_int old_size = target.get_length() * target.get_num_chans();
        t_ptr_int new_size = std::max(required_length * target.get_num_chans(), old_size + BUFFER_GROW_SIZE);
        
        // Allocate temporary memory
        
        temp_buffer temp(old_size);
        
        if (temp.m_ptr)
        {
            // Copy from current buffer to temporary memory
            
            std::copy_n(static_cast<float *>(target.get_samples()), old_size, temp.m_ptr);
            
            // Set buffer to new size and copy back from temporary memory
            
            target.set_size_in_samples(new_size);
            
            if (required_length >= target.get_length())
                std::copy_n(temp.m_ptr, old_size, static_cast<float *>(target.get_samples()));
        }
        
        if (required_length < target.get_length())
        {
            error ("ibufconcatenate: no room left in buffer");
            return -1;
        }
    }
    
    float *out_samps = static_cast<float *>(target.get_samples()) + offset * target.get_num_chans();
    
    // Copy samples
    
    if (target.get_num_chans() == 1)
    {
        // Single channel case
        
        ibuffer_get_samps(source, out_samps, 0, source.get_length(), 0);
    }
    else
    {
        // Multichannel case (copies to temporary block and reads source with modulo channels
        
        const t_ptr_int target_chans = target.get_num_chans();
        const t_ptr_int block_size = 256;
        float samp_block[block_size];
        
        for (t_ptr_int i = 0; i < source.get_length(); i += block_size)
        {
            for (t_ptr_int j = 0; j < target_chans; j++)
            {
                t_ptr_int loop_size = std::min(source.get_length() - j, block_size);
                ibuffer_get_samps(source, samp_block, 0, loop_size, j % source.get_num_chans());
                
                for (t_ptr_int k = 0; k < loop_size; k++)
                    out_samps[i * target_chans + j + k] = samp_block[k];
            }
        }
    }
    
    // Copy samples at either end to ensure playback is within range
    
    std::copy_n(out_samps, target.get_num_chans(), out_samps - target.get_num_chans());
    out_samps += (source.get_length() - 1) * target.get_num_chans();
    std::copy_n(out_samps, target.get_num_chans(), out_samps + target.get_num_chans());
    
    // Store data
    
    double sr_const = 1000.0 / target.get_sample_rate();
    
    beg = offset * sr_const;
    end = (offset + source.get_length() - 1) * sr_const;
    
    long item = access.add_item(beg, end);
    
    // Set the buffer as dirty
    
    target.set_dirty();
    
    // We are done with the buffers
    
    target.release();
    source.release();
    
    return item;
}

void ibufconcatenate_append(t_ibufconcatenate *x, t_symbol *source_name)
{
    double beg = 0.0;
    double end = 0.0;
    
    long item = ibufconcatenate_append_internal(x, source_name, beg, end);
    
    if (item != -1)
        ibufconcatenate_output(x, item, beg, end);
}

void ibufconcatenate_entry(t_ibufconcatenate *x, double beg, double end)
{
    // Scope to release lock before calling the output
    
    auto add = [&]()
    {
        t_ibufconcatenate_info::write_access access(x->attachment);
        return access.add_item(beg, end);
    };
    
    long item = add();
    ibufconcatenate_output(x, item, beg, end);
}

void ibufconcatenate_output(t_ibufconcatenate *x, long item, double beg, double end)
{
    t_atom list[3];
    
    // Output
    
    atom_setlong(list, item);
    atom_setfloat(list + 1, beg);
    atom_setfloat(list + 2, end);
    
    outlet_list(x->last_added_out, 0, 3, list);
}

static inline t_ptr_int mstosamps(double ms, double sr)
{
    return (t_ptr_int) std::round((ms * sr) / 1000.0);
}

double ibuffer_full_length(t_symbol *name)
{
    ibuffer_data buffer(name);
    
    if (buffer.get_length())
        return (buffer.get_length() / buffer.get_sample_rate()) * 1000.0;
    else
        return 0.0;
}

void ibufconcatenate_int(t_ibufconcatenate *x, long item)
{
    t_ibufconcatenate_info::read_access access(x->attachment);
    
    double full_length = ibuffer_full_length(access.buffername());
    double sr = 0;
    
    t_atom atom_out[3];
    
    if (!x->max_mode)
        return;
    
    // Default Output Values
    
    double beg = 0.0;
    double end = 0.0;
    
    item--;
    
    if (item == -1)
        end = full_length;
    else
        access.get_item(item - 1, beg, end);
    
    atom_setlong(atom_out + 0, mstosamps(beg, sr));
    atom_setlong(atom_out + 1, mstosamps(end, sr));
    atom_setfloat(atom_out + 2, sr);
    
    outlet_list(x->data_out, 0, 3, atom_out);
}

void store(float *lo_res, float *hi_res, double output)
{
    *lo_res = static_cast<float>(output);
    *hi_res = output - *lo_res;
}

void store(double *lo_res, double *hi_res, double output)
{
    *lo_res = output;
    *hi_res = 0.0;
}

template <class T>
void ibufconcatenate_perform_core(t_ibufconcatenate *x, T **ins, T **outs, long vec_size)
{
    // Set pointers
    
    T *in = ins[0];
    T *beg_lo_res = outs[0];
    T *beg_hi_res = outs[1];
    T *end_lo_res = outs[2];
    T *end_hi_res = outs[3];
    
    t_ibufconcatenate_info::read_access access(x->attachment);
    
    double full_length = ibuffer_full_length(access.buffername());
    
    while (vec_size--)
    {
        // Get item
        
        long item = (long) *in++;
        
        // Default Output Values
        
        double beg = 0.0;
        double end = 0.0;
        
        if (item == -1)
            end = full_length;
        else
            access.get_item(item - 1, beg, end);
        
        // Chunk Output Values
        
        store(beg_lo_res++, beg_hi_res++, beg);
        store(end_lo_res++, end_hi_res++, end);
    }
}

t_int *ibufconcatenate_perform(t_int *w)
{
    // Set pointers
    
    float *ins[1];
    float *outs[4];
    
    ins[0] = (float *) w[1];
    
    outs[0] = (float *) w[2];
    outs[1] = (float *) w[3];
    outs[2] = (float *) w[4];
    outs[3] = (float *) w[5];
    
    long vec_size = (long) w[6];
    t_ibufconcatenate *x = (t_ibufconcatenate *) w[7];
    
    ibufconcatenate_perform_core(x, ins, outs, vec_size);
    
    return w + 8;
}

void ibufconcatenate_perform64(t_ibufconcatenate *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    ibufconcatenate_perform_core(x, ins, outs, vec_size);
}

void ibufconcatenate_dsp(t_ibufconcatenate *x, t_signal **sp, short *count)
{
    if (!x->max_mode)
        dsp_add(ibufconcatenate_perform, 7, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[0]->s_n, x);
}

void ibufconcatenate_dsp64(t_ibufconcatenate *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    if (!x->max_mode)
        object_method(dsp64, gensym("dsp_add64"), x, ibufconcatenate_perform64, 0, nullptr);
}

