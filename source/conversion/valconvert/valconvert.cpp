
/*
 *  valconvert and valconvert~
 *
 *  valconvert and its audio rate counterpart (valconvert~) are general pupose scaling objects.
 *  They inclued several useful musical scalings (log / exp / pitch / amp / scale).
 *  The max version scales ints (float output), floats and lists, the audio rate version scales signals.
 *
 *  Scaling is set by a parameter message.
 *  Scaling type refers to interpretation of the *input* rather than the output.
 *  E.g. the log mode treats the input as logarithmic and scales exponentially (e.g to control frequency).
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

#ifdef MSP_VERSION
#include <simd_support.hpp>
#include <SIMDExtended.hpp>


// Useful Defines

using max_object_base = t_pxobject;
const char *object_name = "valconvert~";
method free_routine = (method) dsp_free;
#else
using max_object_base = t_object;
const char *object_name = "valconvert";
method free_routine = nullptr;
#endif

// Globals, Enums and Object Structure

t_class *this_class;

t_symbol *ps_scale = gensym("scale");
t_symbol *ps_log = gensym("log");
t_symbol *ps_amp = gensym("amp");
t_symbol *ps_pitch = gensym("pitch");
t_symbol *ps_exp = gensym("exp");
t_symbol *ps_iamp = gensym("iamp");
t_symbol *ps_ipitch = gensym("ipitch");
t_symbol *ps_none = gensym("none");

t_symbol *ps_list = gensym("list");

enum class conversion_mode
{
    none,
    linear,
    log_in,
    exp_in,
};

struct t_valconvert
{
    max_object_base a_obj;
    
    conversion_mode mode;
    
    double mul;
    double sub;
    double lo;
    double hi;
    
    void *the_outlet;
};

// Function Protoypes

void *valconvert_new(t_symbol *msg, long argc, t_atom *argv);

#ifdef MSP_VERSION

void valconvert_perform64(t_valconvert *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void valconvert_perform_SIMD64(t_valconvert *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void valconvert_dsp64(t_valconvert *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags);

#else

double valconvert_scale(t_valconvert *x, double input);
void valconvert_int(t_valconvert *x, t_atom_long i_in);
void valconvert_float(t_valconvert *x, double f_in);
void valconvert_list(t_valconvert *x, t_symbol *msg, long argc, t_atom *argv);

#endif

void valconvert_anything(t_valconvert *x, t_symbol *msg, long argc, t_atom *argv);
void valconvert_assist(t_valconvert *x, void *b, long m, long a, char *s);

// Lowercase Conversion

t_symbol *symbol_to_lowercase(t_symbol *sym)
{
    std::string str(sym->s_name);
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
    
    return gensym(str.c_str());
}

// Main

int C74_EXPORT main()
{
    this_class = class_new(object_name,
                          (method) valconvert_new,
                          free_routine,
                          sizeof(t_valconvert),
                          (method) nullptr,
                          A_GIMME,
                          0);
    
#ifdef MSP_VERSION
    class_addmethod(this_class, (method) valconvert_dsp64, "dsp64", A_CANT, 0);
    class_dspinit(this_class);
#else
    class_addmethod(this_class, (method) valconvert_int, "int", A_LONG, 0);
    class_addmethod(this_class, (method) valconvert_float, "float", A_FLOAT, 0);
    class_addmethod(this_class, (method) valconvert_list, "list", A_GIMME, 0);
#endif
    
    class_addmethod(this_class, (method) valconvert_anything, "anything", A_GIMME, 0);
    class_addmethod(this_class, (method) valconvert_assist, "assist", A_CANT, 0);
    
    class_register(CLASS_BOX, this_class);
        
    return 0;
}

// New

void *valconvert_new(t_symbol *msg, long argc, t_atom *argv)
{
    t_valconvert *x = (t_valconvert *) object_alloc(this_class);
    
    x->mode = conversion_mode::none;
    x->lo = 0.0;
    x->hi = 1.0;
    x->mul = 1.0;
    x->sub = 0.0;
    
    if (argc)
    {
        if (argc < 5 && symbol_to_lowercase(atom_getsym(argv)) != ps_none)
        {
            object_error((t_object *) x, "not enough arguments for parameter set up");
        }
        else
        {
            if (atom_gettype(argv) == A_SYM)
                valconvert_anything (x, atom_getsym(argv), argc - 1, argv + 1);
        }
    }
    
#ifdef MSP_VERSION
    dsp_setup((t_pxobject *) x, 1);
    outlet_new((t_object *) x, "signal");
#else
    x->the_outlet = outlet_new(x, nullptr);
#endif
    
    return x;
}

// Clip Helper

double clip(double value, double min_val, double max_val)
{
    return std::max(min_val, std::min(max_val, value));
}

// Log Helper

double safe_log(double x)
{
    return log(x < 0 ? 0 : x);
}

#ifdef MSP_VERSION

// Signal-specific

// Perform / DSP

void valconvert_perform64(t_valconvert *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    double *in = ins[0];
    double *out = outs[0];
    
    double mul = x->mul;
    double sub = x->sub;
    double lo = x->lo;
    double hi = x->hi;
    
    switch (x->mode)
    {
        case conversion_mode::none:
            while (vec_size--)
                *out++ = *in++;
            break;
            
        case conversion_mode::linear:
            while (vec_size--)
                *out++ = clip((*in++ * mul) - sub, lo, hi);
            break;
            
        case conversion_mode::log_in:
            while (vec_size--)
                *out++ = clip(exp((*in++ * mul) - sub), lo, hi);
            break;
            
        case conversion_mode::exp_in:
            while (vec_size--)
                *out++ = clip((safe_log(*in++) * mul) - sub, lo, hi);
            break;
    }
}

void valconvert_perform_SIMD64(t_valconvert *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    using vec_type = htl::simd_type<double,  htl::simd_limits<double>::max_size>;
    
    double *out = outs[0];

    const vec_type *in_vec = reinterpret_cast<const vec_type *>(ins[0]);
    vec_type *out_vec = reinterpret_cast<vec_type *>(out);
    
    long num_vecs = vec_size /  htl::simd_limits<double>::max_size;
    
    vec_type mul(x->mul);
    vec_type sub(x->sub);
    vec_type lo(x->lo);
    vec_type hi(x->hi);
    
    switch (x->mode)
    {
        case conversion_mode::none:
            while (num_vecs--)
                *out_vec++ = *in_vec++;
            break;
            
        case conversion_mode::linear:
            while (num_vecs--)
                *out_vec++ = min(max(lo, (*in_vec++ * mul) - sub), hi);
            break;
            
        case conversion_mode::log_in:
            for (long i = 0; i < num_vecs; i++)
                out_vec[i] = (in_vec[i] * mul) - sub;
            exp_array(out, out, vec_size);
            for (long i = 0; i < num_vecs; i++)
                out_vec[i] = min(max(lo, out_vec[i]), hi);
            break;
            
        case conversion_mode::exp_in:
            for (long i = 0; i < num_vecs; i++)
                out_vec[i] = max(in_vec[i], double(0));
            log_array(out, out, vec_size);
            for (long i = 0; i < num_vecs; i++)
                out_vec[i] = min(max(lo, (out_vec[i] * mul - sub)), hi);
            break;
    }
}

void valconvert_dsp64(t_valconvert *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags)
{
    if (max_vec >= htl::simd_limits<double>::max_size)
        object_method(dsp64, gensym("dsp_add64"), x, valconvert_perform_SIMD64, 0, nullptr);
    else
        object_method(dsp64, gensym("dsp_add64"), x, valconvert_perform64, 0, nullptr);
}

// Signal-specific Assist

void valconvert_assist(t_valconvert *x, void *b, long m, long a, char *s)
{
    static constexpr int maxAssist = 256;

    if (m == ASSIST_OUTLET)
        snprintf(s, maxAssist, "(signal) Scaled Output");
    else
        snprintf(s, maxAssist, "(signal) Input / (message) Parameters");
}

#else

// Message Scaling

double valconvert_scale(t_valconvert *x, double input)
{
    switch (x->mode)
    {
        case conversion_mode::linear:   return clip((input * x->mul) - x->sub, x->lo, x->hi);
        case conversion_mode::log_in:   return clip(exp((input * x->mul) - x->sub), x->lo, x->hi);
        case conversion_mode::exp_in:   return clip((safe_log(input) * x->mul) - x->sub, x->lo, x->hi);
        default:                        return input;
    }
}

void valconvert_float(t_valconvert *x, double f_in)
{
    outlet_float(x->the_outlet, valconvert_scale(x, f_in));
}

void valconvert_int(t_valconvert *x, t_atom_long l_in)
{
    outlet_float(x->the_outlet, valconvert_scale(x, static_cast<double>(l_in)));
}

void valconvert_list(t_valconvert *x, t_symbol *msg, long argc, t_atom *argv)
{
    std::vector<t_atom> list(argc);
    
    for (long i = 0; i < argc; i++)
        atom_setfloat(list.data() + i ,valconvert_scale(x, atom_getfloat(argv++)));
    
    outlet_list(x->the_outlet, ps_list, static_cast<short>(argc), list.data());
}

// Message Assist

void valconvert_assist(t_valconvert *x, void *b, long m, long a, char *s)
{
    static constexpr int maxAssist = 256;

    if (m == ASSIST_OUTLET)
        snprintf(s, maxAssist, "Scaled Output");
    else
        snprintf(s, maxAssist, "Input / Parameters");
}

#endif

// Methods to Set Scaling

void convert_power(double &a, double&b, double base, double divisor)
{
    a = pow(base, a / divisor);
    b = pow(base, b / divisor);
}

void convert_log(double &a, double&b)
{
    a = safe_log(a);
    b = safe_log(b);
}

void valconvert_anything(t_valconvert *x, t_symbol *msg, long argc, t_atom *argv)
{
    // Here we convert captialised symbols, so as to allow capitalisation
    
    msg = symbol_to_lowercase(msg);
    
    if (msg == ps_none)
    {
        x->mode = conversion_mode::none;
        return;
    }
    
    if (argc < 4)
    {
        object_error((t_object *) x, "not enough values for parameter change");
        return;
    }
    
    double min_in = atom_getfloat(argv++);
    double max_in = atom_getfloat(argv++);
    double min_out = atom_getfloat(argv++);
    double max_out = atom_getfloat(argv++);
    
    if (msg == ps_log || msg == ps_amp || msg == ps_pitch)
        x->mode = conversion_mode::log_in;
    else if (msg == ps_exp  || msg == ps_iamp || msg == ps_ipitch)
        x->mode = conversion_mode::exp_in;
    else
    {
        if (msg != ps_scale)
            object_error ((t_object *) x, "unknown conversion type - defaulting to scale");
        x->mode = conversion_mode::linear;
    }
    
    if (msg == ps_amp)
        convert_power(min_out, max_out, 10.0, 20.0);
    else if (msg == ps_pitch)
        convert_power(min_out, max_out, 2.0, 12.0);
    else if (msg == ps_iamp)
        convert_power(min_in, max_in, 10.0, 20.0);
    else if (msg == ps_ipitch)
        convert_power(min_in, max_in, 2.0, 12.0);
    
    x->lo = min_out < max_out ? min_out : max_out;
    x->hi = min_out < max_out ? max_out : min_out;
    
    if (x->mode == conversion_mode::log_in)
        convert_log(min_out, max_out);
    else if (x->mode == conversion_mode::exp_in)
        convert_log(min_in, max_in);
    
    x->mul = (min_in == max_in) ? 0.0 : (max_out - min_out) / (max_in - min_in);
    x->sub = (min_in * x->mul) - min_out;
}
