
/*
 *  v_binary.hpp
 *
 *  A header file that provides a base class for binary vMSP objects.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <string>
#include <type_traits>

#include "SIMDSupport.hpp"
#include <AH_Denormals.h>


// Type Enums

enum class calculation_type { scalar, vector_op, vector_array };
enum class inputs { binary, scalar1, scalar2 };

// Binary Class

template<typename Functor, calculation_type Vec32, calculation_type Vec64, bool FirstInputAlwaysSignal = false>
class v_binary
{
    // Denormal Fixing
    
    static float fix_denorm(const float a)
    {
        float b = a;
        FIX_DENORM_FLOAT(b);
        return b;
    }

    template <class T>
    static T fix_denorm(const T a) { return a; }
    
public:
    
    // Main
    
    template <typename T>
    static int setup(const char *object_name)
    {
        t_class **C = get_class_pointer<T>();
        
        *C = class_new(object_name,
                       (method) new_object<T>,
                       (method) free_object<T>,
                       sizeof(T),
                       (method) nullptr,
                       A_DEFFLOAT,
                       0);
        
        class_addmethod(*C, (method) float_in<T>, "float", A_FLOAT, 0);
        class_addmethod(*C, (method) int_in<T>, "int", A_LONG, 0);
        class_addmethod(*C, (method) dsp<T>, "dsp", A_CANT, 0);
        class_addmethod(*C, (method) dsp64<T>, "dsp64", A_CANT, 0);
        class_addmethod(*C, (method) assist, "assist", A_CANT, 0);
        
        class_dspinit(*C);
        class_register(CLASS_BOX, *C);
                
        post("%s - using vector version by Alex Harker", object_name);
        
        return 0;
    }
    
    // Free
    
    template <class T>
    static void free_object(T *x)
    {
        dsp_free(&x->m_obj);
    }
    
    // New
    
    template <class T>
    static void *new_object(double double_val)
    {
        T *x = static_cast<T *>(object_alloc(*get_class_pointer<T>()));
        
        dsp_setup(reinterpret_cast<t_pxobject *>(&x->m_obj), 2);
        outlet_new(reinterpret_cast<t_object *>(x),"signal");
        
        float_in(x, double_val);

        return x;
    }
    
    // Float Input
    
    template <class T>
    static void float_in(T *x, double value)
    {
        x->m_val = value;
    }
    
    // Int Input
    
    template <class T>
    static void int_in(T *x, t_atom_long value)
    {
        x->m_val = static_cast<double>(value);
    }
    
    // 32 bit DSP
   
    template <typename T, calculation_type C = Vec32>
    static method dsp_vector_select(std::enable_if_t<C == calculation_type::scalar, int> routine)
    {
        // Scalar
        
        switch (routine)
        {
            case 1:     return reinterpret_cast<method>(perform<perform_single1_op<T, 1>>);
            case 2:     return reinterpret_cast<method>(perform<perform_single2_op<T, 1>>);
            default:    return reinterpret_cast<method>(perform<perform_op<T, 1>>);
        }
    }
    
    template <typename T, calculation_type C = Vec32>
    static method dsp_vector_select(std::enable_if_t<C == calculation_type::vector_op, int> routine)
    {
        // Vector Op

        constexpr int simd_width = SIMDLimits<float>::max_size;
        
        switch (routine)
        {
            case 1:     return reinterpret_cast<method>(perform<perform_single1_op<T, simd_width>>);
            case 2:     return reinterpret_cast<method>(perform<perform_single2_op<T, simd_width>>);
            default:    return reinterpret_cast<method>(perform<perform_op<T, simd_width>>);
        }
    }
    
    template <typename T, calculation_type C = Vec32>
    static method dsp_vector_select(std::enable_if_t<C == calculation_type::vector_array, int> routine)
    {
        // Vector Array
    
        switch (routine)
        {
            case 1:     return reinterpret_cast<method>(perform<perform_single1_array<T>>);
            case 2:     return reinterpret_cast<method>(perform<perform_single2_array<T>>);
            default:    return reinterpret_cast<method>(perform<perform_array<T>>);
        }
    }
    
    template <class T>
    static void dsp(T *x, t_signal **sp, short *count)
    {
        int routine = 0;
        bool use_vec = false;
        long vec_size = sp[0]->s_n;

        float *used_input1 = reinterpret_cast<float *>(sp[0]->s_vec);
        float *used_input2 = reinterpret_cast<float *>(sp[1]->s_vec);
        
        // Default to scalar routines
        
        method perform_routine = (method) perform<perform_op<T, 1>>;
        
        // If nothing is connected then don't do anything here....
        
        if (!count[1] && !count[0])
            return;
        
        // Choose scalar routines as needed
        
        if (!count[0])
            routine = FirstInputAlwaysSignal ? 1 : 2;
        else if (!count[1])
            routine = 1;

        // Use SIMD code where possible
        
        if (Vec32 != calculation_type::scalar && ((vec_size / SIMDLimits<float>::max_size) > 0))
        {
            if ((t_ptr_uint) used_input1 % 16 || (t_ptr_uint) used_input2 % 16 || (t_ptr_uint) sp[2]->s_vec % 16)
            {
                object_error((t_object *) x, "handed a misaligned signal vector - update to Max 5.1.3 or later");
            }
            else
                use_vec = true;
        }
        
        if (use_vec)
            perform_routine = dsp_vector_select<T>(routine);
        else
            perform_routine = dsp_vector_select<T, calculation_type::scalar>(routine);
        
        dsp_add(denormals_perform, 6, perform_routine, used_input1, used_input2, sp[2]->s_vec, vec_size, x);
    }
    
    // 32 bit Perform Return Wrapper
    
    template<void perform_routine(t_int *w)>
    static t_int *perform(t_int *w)
    {
        perform_routine(w);
        
        return w + 7;
    }
    
    // 32 bit Perform with One LHS Signal Input (SIMD - array)

    template <class T>
    static void perform_single1_array(t_int *w)
    {
        T *x = reinterpret_cast<T *>(w[6]);
        
        x->m_functor(reinterpret_cast<float *>(w[4]), reinterpret_cast<float *>(w[2]), reinterpret_cast<float *>(w[3]), static_cast<long>(w[5]), x->m_val, inputs::scalar1);
    }
    
    // 32 bit Perform with One RHS Signal Input (SIMD - array)

    template <class T>
    static void perform_single2_array(t_int *w)
    {
        T *x = reinterpret_cast<T *>(w[6]);
        
        x->m_functor(reinterpret_cast<float *>(w[4]), reinterpret_cast<float *>(w[2]), reinterpret_cast<float *>(w[3]), static_cast<long>(w[5]), x->m_val, inputs::scalar2);
    }
    
    // 32 bit Perform with Two Signal Inputs (SIMD - array)
    
    template <class T>
    static void perform_array(t_int *w)
    {
        T *x = reinterpret_cast<T *>(w[6]);
        
        x->m_functor(reinterpret_cast<float *>(w[4]), reinterpret_cast<float *>(w[2]), reinterpret_cast<float *>(w[3]), static_cast<long>(w[5]), x->m_val, inputs::binary);
    }
    
    // 32 bit Perform with One LHS Signal Input (SIMD - op)
    
    template <class T, int N>
    static void perform_single1_op(t_int *w)
    {
        SIMDType<float, N> *in1 = reinterpret_cast< SIMDType<float, N> *>(w[2]);
        SIMDType<float, N> *out1 = reinterpret_cast< SIMDType<float, N> *>(w[4]);
        long vec_size = static_cast<long>(w[5]);
        T *x = reinterpret_cast<T *>(w[6]);

        Functor &functor = x->m_functor;

        SIMDType<float, N> float_val(static_cast<float>(x->m_val));
        
        vec_size /= SIMDType<float, N>::size;

        while (vec_size--)
            *out1++ = fix_denorm(functor(*in1++, float_val));
    }
    
    // 32 bit Perform with One RHS Signal Input (SIMD - op)
    
    template <class T, int N>
    static void perform_single2_op(t_int *w)
    {
        SIMDType<float, N> *in2 = reinterpret_cast< SIMDType<float, N> *>(w[3]);
        SIMDType<float, N> *out1 = reinterpret_cast< SIMDType<float, N> *>(w[4]);
        long vec_size = static_cast<long>(w[5]);
        T *x = reinterpret_cast<T *>(w[6]);
        
        Functor &functor = x->m_functor;

        SIMDType<float, N> float_val(static_cast<float>(x->m_val));

        vec_size /= SIMDType<float, N>::size;
    
        while (vec_size--)
            *out1++ = fix_denorm(functor(float_val, *in2++));
    }
    
    // 32 bit Perform with Two Signal Inputs (SIMD - op)
    
    template <class T, int N>
    static void perform_op(t_int *w)
    {
        SIMDType<float, N> *in1 = reinterpret_cast<SIMDType<float, N> *>(w[2]);
        SIMDType<float, N> *in2 = reinterpret_cast<SIMDType<float, N> *>(w[3]);
        SIMDType<float, N> *out1 = reinterpret_cast<SIMDType<float, N> *>(w[4]);
        long vec_size = static_cast<long>(w[5]);
        T *x = reinterpret_cast<T *>(w[6]);

        Functor &functor = x->m_functor;

        vec_size /= SIMDType<float, N>::size;

        while (vec_size--)
            *out1++ = fix_denorm(functor(*in1++, *in2++));
    }
    
    // 64 bit DSP
    
    template <typename T, calculation_type C = Vec64>
    static method dsp_vector_select64(std::enable_if_t<C == calculation_type::scalar, int> routine)
    {
        // Scalar
        
        switch (routine)
        {
            case 1:     return reinterpret_cast<method>(perform64_single1_op<T, 1>);
            case 2:     return reinterpret_cast<method>(perform64_single2_op<T, 1>);
            default:    return reinterpret_cast<method>(perform64_op<T, 1>);
        }
    }
    
    template <typename T, calculation_type C = Vec64>
    static method dsp_vector_select64(std::enable_if_t<C == calculation_type::vector_op, int> routine)
    {
        // Vector Op

        constexpr int simd_width = SIMDLimits<double>::max_size;
        
        switch (routine)
        {
            case 1:     return reinterpret_cast<method>(perform64_single1_op<T, simd_width>);
            case 2:     return reinterpret_cast<method>(perform64_single2_op<T, simd_width>);
            default:    return reinterpret_cast<method>(perform64_op<T, simd_width>);
        }
    }
    
    template <typename T, calculation_type C = Vec64>
    static method dsp_vector_select64(std::enable_if_t<C == calculation_type::vector_array, int> routine)
    {
        // Vector Array
    
        switch (routine)
        {
            case 1:     return reinterpret_cast<method>(perform64_single1_array<T>);
            case 2:     return reinterpret_cast<method>(perform64_single2_array<T>);
            default:    return reinterpret_cast<method>(perform64_array<T>);
        }
    }
    
    template <class T>
    static void dsp64(T *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags)
    {
        method perform_routine = (method) perform64_op<T, 1>;
        int routine = 0;
        
        // If nothing is connected then don't do anything here....
            
        if (!count[1] && !count[0])
            return;
        
        // Choose scalar routines as needed
        
        if (!count[0])
            routine = FirstInputAlwaysSignal ? 1 : 2;
        else if (!count[1])
            routine = 1;
        
        // Use SIMD code where possible
        
        if (Vec64 != calculation_type::scalar && ((max_vec / SIMDLimits<double>::max_size) > 0))
            perform_routine = dsp_vector_select64<T>(routine);
        else
            perform_routine = dsp_vector_select64<T, calculation_type::scalar>(routine);
        
        object_method(dsp64, gensym("dsp_add64"), x, perform_routine, 0, 0);
    }
    
    // 64 bit Perform with One LHS Signal Input (SIMD - op)

    template <class T, int N>
    static void perform64_single1_op(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        Functor &functor = x->m_functor;

        SIMDType<double, N> *in1 = reinterpret_cast<SIMDType<double, N> *>(ins[0]);
        SIMDType<double, N> *out1 = reinterpret_cast<SIMDType<double, N> *>(outs[0]);
        
        SIMDType<double, N> double_val(x->m_val);
        
        vec_size /= SIMDType<double, N>::size;
        
        while (vec_size--)
            *out1++ = fix_denorm(functor(*in1++, double_val));
    }
    
    // 64 bit Perform with One RHS Signal Input (SIMD - op)
    
    template <class T, int N>
    static void perform64_single2_op(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        Functor &functor = x->m_functor;
        
        SIMDType<double, N> *in2 = reinterpret_cast<SIMDType<double, N> *>(ins[1]);
        SIMDType<double, N> *out1 = reinterpret_cast<SIMDType<double, N> *>(outs[0]);
        
        SIMDType<double, N> double_val(x->m_val);

        vec_size /= SIMDType<double, N>::size;

        while (vec_size--)
            *out1++ = fix_denorm(functor(double_val, *in2++));
    }
    
    // 64 bit Perform with Two Signal Inputs (SIMD - op)
    
    template <class T, int N>
    static void perform64_op(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        Functor &functor = x->m_functor;

        SIMDType<double, N> *in1 = reinterpret_cast<SIMDType<double, N> *>(ins[0]);
        SIMDType<double, N> *in2 = reinterpret_cast<SIMDType<double, N> *>(ins[1]);
        SIMDType<double, N> *out1 = reinterpret_cast<SIMDType<double, N> *>(outs[0]);

        vec_size /= SIMDType<double, N>::size;

        while (vec_size--)
            *out1++ = fix_denorm(functor(*in1++, *in2++));
    }
    
    // 64 bit Peform with One LHS Signal Input (SIMD - array)
    
    template <class T>
    static void perform64_single1_array(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        x->m_functor(outs[0], ins[0], ins[1], vec_size, x->m_val, inputs::scalar1);
    }
    
    // 64 bit Perform with One RHS Signal Input (SIMD - array)
    
    template <class T>
    static void perform64_single2_array(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        x->m_functor(outs[0], ins[0], ins[1], vec_size, x->m_val, inputs::scalar2);
    }
    
    // 64 bit Perform with Two Signal Inputs (SIMD - array)
    
    template <class T>
    static void perform64_array(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        x->m_functor(outs[0], ins[0], ins[1], vec_size, x->m_val, inputs::binary);
    }
    
    // Assist
    
    static void assist(v_binary *x, void *b, long m, long a, char *s)
    {
        if (m == ASSIST_INLET)
        {
            switch (a)
            {
                case 0:
                    sprintf(s,"(signal, float) In 1");
                    break;
                case 1:
                    sprintf(s,"(signal, float) In 2");
                    break;
            }
        }
        else
            sprintf(s,"(signal) Out");
    }
    
private:
    
    // Static Class Pointer
    
    template <class T>
    static t_class **get_class_pointer()
    {
        static t_class *C;
        
        return &C;
    }
    
    // Data
    
    t_pxobject m_obj;
    
    double m_val;
        
    Functor m_functor;
};

