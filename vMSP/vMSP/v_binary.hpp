
/*
 *  Template_Binary
 *
 *    This is a template file for a SIMD binary (either commutative or non-commutative) operator MSP object.
 *    It should only be necessary to provide a few macro definitions before this code to comiple a fully functional object.
 *    See other source files for examples (e.g. vdiv~.c).
 *
 *    The object will support all block sizes, using SIMD where possible.
 *    The object will also accept float / int arguments, and inputs for one of the two values (according to the connections to the object)
 *
 *    The following *MUST* be defined:
 *
 *    OBJNAME_STR            - The object name as a string (i.e. - with quotes)
 *    OBJNAME_FIRST(a)    - A macro to combine the object name followed by a given string
 *
 *        Should be defined like:
 *        #define OBJNAME_FIRST(a) objname ## a
 *
 *    OBJNAME_SECOND(a)    - A macro to combine the object name followed by a given string
 *
 *        Should be defined like:
 *        #define OBJNAME_SECOND(a) a ## objname
 * *
 *    F32_VEC_OP                - A binary 32 bit floating point SIMD operator (vFloat, vFloat)
 *    F32_VEC_ARRAY            - A binary 32 bit floating point SIMD array function (float *out, float *in1, float *in2, long length)
 *    F32_SCALAR_OP            - A binary 32 bit floating point scalar operator (float, float)
 *
 *    F64_VEC_OP                - A binary 64 bit floating point SIMD operator (vDouble, vDouble)
 *    F64_VEC_ARRAY            - A binary 64 bit floating point SIMD array function (double *out, double *in1, double *in2, long length)
 *    F64_SCALAR_OP            - A binary 64 bit floating point scalar operator (double, double)
 *
 *
 *  To use the array version over the op define USE_F32_VEC_ARRAY or USE_F64_VEC_ARRAY as appropriate
 *    To avoid using vector processing at all define NO_F32_SIMD or NO_F64_SIMD (according to which are available)
 *    It is best to define these based on the available functions (preferring the operation, then the array version, then no SIMD)
 *
 *    Optionally for objects requiring constants you can define:
 *
 *    SET_CONSTANTS        - Set constants in main routine (if necessary)
 * *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <string>
#include <algorithm>

#include "SIMDSupport.hpp"
#include <AH_Denormals.h>

enum CalculationType { kScalar, kVectorOp, kVectorArray };

// Object structure

template<typename Functor, CalculationType Vec32, CalculationType Vec64, bool Reverse = false>
class v_binary
{
    static float fix_denorm(const float a) { return AH_FIX_DENORM_FLOAT(a); }

    template <class T>
    static T fix_denorm(const T a) { return a; }
    
#if (SIMD_COMPILER_SUPPORT_LEVEL >= SIMD_COMPILER_SUPPORT_AVX256)
    const static int width32 = 8;
    const static int width64 = 4;
#else
    const static int width32 = 4;
    const static int width64 = 2;
#endif
    
public:
    
    // Main routine
    
    template <typename T>
    static int setup(const char *object_name)
    {
        t_class **C = getClassPointer<T>();
        *accessClassName<T>() = object_name;
        
        *C = class_new (object_name, (method)new_object<T>, (method)free_object<T>, sizeof(T), NULL, A_DEFFLOAT, 0);
        
        class_addmethod(*C, (method)float_in<T>, "float", A_FLOAT, 0);
        class_addmethod(*C, (method)int_in<T>, "int", A_LONG, 0);
        class_addmethod(*C, (method)dsp<T>, "dsp", A_CANT, 0);
        class_addmethod(*C, (method)dsp64<T>, "dsp64", A_CANT, 0);
        class_addmethod(*C, (method)assist, "assist", A_CANT, 0);
        
        class_dspinit(*C);
        class_register(CLASS_BOX, *C);
        
        // Constants are set here (if they cannot be set in global code
        
        post ("%s - using vector version by Alex Harker", object_name);
        
        return 0;
    }
    
    // Free routine
    
    template <class T>
    static void free_object(T *x)
    {
        dsp_free(&x->m_obj);
        
        if (x->m_temp_memory)
            deallocate_aligned(x->m_temp_memory);
    }
    
    // New routine
    
    template <class T>
    static void *new_object(double double_val)
    {
        T *x = static_cast<T *>(object_alloc(*getClassPointer<T>()));
        
        dsp_setup(reinterpret_cast<t_pxobject *>(&x->m_obj), 2);
        outlet_new(reinterpret_cast<t_object *>(x),"signal");
        
        x->m_val = double_val;
        x->m_temp_memory = NULL;

        return (x);
    }
    
    // Float input routine
    
    template <class T>
    static void float_in(T *x, double value)
    {
        x->m_val = value;
    }
    
    // Int input routine
    
    template <class T>
    static void int_in(T *x, t_atom_long value)
    {
        x->m_val = static_cast<double>(value);
    }
    
    // 32 bit dsp routine
    
    template <class T>
    static void dsp(T *x, t_signal **sp, short *count)
    {
        long routine = 0;
        long vec_size_val = sp[0]->s_n;

        float *used_input1 = reinterpret_cast<float *>(Reverse ? sp[1]->s_vec : sp[0]->s_vec);
        float *used_input2 = reinterpret_cast<float *>(Reverse ? sp[0]->s_vec : sp[1]->s_vec);
        
        // Default to scalar routines
        
        method perform_routine = (method) perform<perform_op<T, 1> >;
        
        if (x->m_temp_memory)
        {
            deallocate_aligned(x->m_temp_memory);
            x->m_temp_memory = NULL;
        }
        
        // If nothing is connected then don't do anything here....
        
        if (!count[1] && !count[0])
            return;
        
        // Only pass the second input (twice)
        
        if (!count[0])
        {
            used_input1 = used_input2;
            routine = Reverse ? 1 : 2;
        }
        
        // Only pass the first input (twice)

        if (!count[1])
        {
            used_input2 = used_input1;
            routine = Reverse ? 2 : 1;
        }
        
        // Use SIMD code where possible
        
        if (Vec32 != kScalar && ((sp[0]->s_n / width32) > 0))
        {
            if (Vec32 == kVectorOp)
            {
                routine += 3;
                vec_size_val = sp[0]->s_n / width32;
                
                if ((t_ptr_uint) used_input1 % 16 || (t_ptr_uint) used_input2 % 16 || (t_ptr_uint) sp[2]->s_vec % 16)
                {
                    post("%s: handed a misaligned signal vector - update to Max 5.1.3 or later", accessClassName<T>()->c_str());
                    routine += 3;
                }
            }
            else
            {
                // Make temporary array for array-based vector routines
                
                routine += 9;
                
                if (routine != 9)
                    x->m_temp_memory = allocate_aligned<float>(sp[0]->s_n);
            }
        }
        
        switch (routine)
        {
            // Scalar
            
            case 0:     perform_routine = (method) perform<perform_op<T, 1> >;                          break;
            case 1:     perform_routine = (method) perform<perform_single1_op<T, 1> >;                  break;
            case 2:     perform_routine = (method) perform<perform_single2_op<T, 1> >;                  break;
            
            // Vector aligned
                
            case 3:     perform_routine = (method) perform<perform_op<T, width32> >;                    break;
            case 4:     perform_routine = (method) perform<perform_single1_op<T, width32> >;            break;
            case 5:     perform_routine = (method) perform<perform_single2_op<T, width32> >;            break;
            
            // Vector mis-aligned
                
            case 6:     perform_routine = (method) perform<perform_misaligned<T, width32> >;            break;
            case 7:     perform_routine = (method) perform<perform_single1_misaligned<T, width32> >;    break;
            case 8:     perform_routine = (method) perform<perform_single2_misaligned<T, width32> >;    break;
            
            // Vector array
            
            case 9:     perform_routine = (method) perform<perform_array<T> >;                          break;
            case 10:    perform_routine = (method) perform<perform_single1_array<T> >;                  break;
            case 11:    perform_routine = (method) perform<perform_single2_array<T> >;                  break;
        }
        
        dsp_add(denormals_perform, 6, perform_routine, used_input1, used_input2, sp[2]->s_vec, vec_size_val, x);
    }
    
    // 32 bit perform return wrapper
    
    template<void perform_routine(t_int *w)>
    static t_int *perform(t_int *w)
    {
        perform_routine(w);
        
        return w + 7;
    }
    
    // 32 bit perform routine with LHS signal-rate input (SIMD - array)

    template <class T>
    static void perform_single1_array(t_int *w)
    {
        T *x = reinterpret_cast<T *>(w[6]);
        long vec_size = static_cast<long>(w[5]);

        float *temp_memory = (float *)x->m_temp_memory;
        
        std::fill_n(((float *)temp_memory), vec_size, static_cast<float>(x->m_val));
        x->m_functor(reinterpret_cast<float *>(w[4]), reinterpret_cast<float *>(w[2]), temp_memory, vec_size);
    }
    
    // 32 bit perform routine with RHS signal-rate input (SIMD - array)

    template <class T>
    static void perform_single2_array(t_int *w)
    {
        T *x = reinterpret_cast<T *>(w[6]);
        long vec_size = static_cast<long>(w[5]);

        float *temp_memory = (float *)x->m_temp_memory;
        
        std::fill_n(((float *)temp_memory), vec_size, static_cast<float>(x->m_val));
        x->m_functor(reinterpret_cast<float *>(w[4]), temp_memory, reinterpret_cast<float *>(w[2]), vec_size);
    }
    
    // 32 bit perform routine with two signal-rate inputs (SIMD - array)
    
    template <class T>
    static void perform_array(t_int *w)
    {
        T *x = reinterpret_cast<T *>(w[6]);
        
        x->m_functor(reinterpret_cast<float *>(w[4]), reinterpret_cast<float *>(w[2]), reinterpret_cast<float *>(w[4]), static_cast<long>(w[5]));
    }
    
    // 32 bit perform routine with first operand only at signal-rate (SIMD - op)
    
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
    
    // 32 bit perform routine with second operand only at signal-rate (SIMD - op)
    
    template <class T, int N>
    static void perform_single2_op(t_int *w)
    {
        SIMDType<float, N> *in1 = reinterpret_cast< SIMDType<float, N> *>(w[2]);
        SIMDType<float, N> *out1 = reinterpret_cast< SIMDType<float, N> *>(w[4]);
        long vec_size = static_cast<long>(w[5]);
        T *x = reinterpret_cast<T *>(w[6]);
        
        Functor &functor = x->m_functor;

        SIMDType<float, N> float_val(static_cast<float>(x->m_val));

        vec_size /= SIMDType<float, N>::size;
        
        while (vec_size--)
            *out1++ = fix_denorm(functor(float_val, *in1++));
    }
    
    // 32 bit perform routine with two signal-rate inputs (SIMD - op)
    
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
    
    
    // 32 bit perform routine with first operand only at signal-rate (misaligned SIMD - early versions of Max 5 only - slightly slower)
    
    template <class T, int N>
    static void perform_single1_misaligned(t_int *w)
    {
        float *in1 = reinterpret_cast<float *>(w[2]);
        float *out1 = reinterpret_cast<float *>(w[4]);
        long vec_size = static_cast<long>(w[5]);
        T *x = reinterpret_cast<T *>(w[6]);

        SIMDType<float, N> float_val(static_cast<float>(x->m_val));
        
        Functor &functor = x->m_functor;
        
        vec_size /= SIMDType<float, N>::size;

        for ( ; vec_size--; in1 += N, out1 += N)
            functor(SIMDType<float, N>(in1), float_val).store(out1);
    }
    
    // 32 bit perform routine with second operand only at signal-rate (misaligned SIMD - early versions of Max 5 only - slightly slower)
    
    template <class T, int N>
    static void perform_single2_misaligned(t_int *w)
    {
        float *in1 = reinterpret_cast<float *>(w[2]);
        float *out1 = reinterpret_cast<float *>(w[4]);
        long vec_size = static_cast<long>(w[5]);
        T *x = reinterpret_cast<T *>(w[6]);

        Functor &functor = x->m_functor;

        SIMDType<float, N> float_val(static_cast<float>(x->m_val));

        vec_size /= SIMDType<float, N>::size;

        for ( ; vec_size--; in1 += N, out1 += N)
            functor(float_val, SIMDType<float, N>(in1)).store(out1);
    }
    
    // 32 bit perform routine with two signal-rate inputs (misaligned SIMD - early versions of Max 5 only - slightly slower)
    
    template <class T, int N>
    static void perform_misaligned(t_int *w)
    {
        float *in1 = reinterpret_cast<float *>(w[2]);
        float *in2 = reinterpret_cast<float *>(w[3]);
        float *out1 = reinterpret_cast<float *>(w[4]);
        long vec_size = static_cast<long>(w[5]);
        T *x = reinterpret_cast<T *>(w[6]);

        Functor &functor = x->m_functor;

        vec_size /= SIMDType<float, N>::size;

        for ( ; vec_size--; in1 += N, out1 += N)
            functor(SIMDType<float, N>(in1), SIMDType<float, N>(in2)).store(out1);
    }
    
    // 64 bit dsp routine
    
    template <class T>
    static void dsp64(T *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
    {
        long routine = 0;
        long userparam = Reverse ? 1 : 0;
        
        method perform_routine = (method) perform64_op<T, 1>;
        
        if (x->m_temp_memory)
        {
            deallocate_aligned(x->m_temp_memory);
            x->m_temp_memory = NULL;
        }
            
        // If nothing is connected then don't do anything here....
            
        if (!count[1] && !count[0])
            return;
        
        // Use single routine
        
        if (!count[0])
        {
            userparam = Reverse ? 0 : 1;
            routine = Reverse ? 1 : 2;
        }
        
        if (!count[1])
        {
            userparam = Reverse ? 1 : 0;
            routine = Reverse ? 2 : 1;
        }
        
        // Use SIMD code where possible
        
        if (Vec64 != kScalar && ((maxvectorsize / width64) > 0))
        {
            if (Vec64 == kVectorOp)
                routine += 3;
            else
            {
                // Make temporary array for array-based vector routines
            
                routine += 6;

                if (routine != 6)
                    x->m_temp_memory = allocate_aligned<double>(maxvectorsize);
            }
        }
        
        switch (routine)
        {
            // Scalar
                
            case 0:     perform_routine = (method) perform64_op<T, 1>;                  break;
            case 1:     perform_routine = (method) perform64_single1_op<T, 1>;          break;
            case 2:     perform_routine = (method) perform64_single2_op<T, 1>;          break;
                
            // Vector aligned op
                
            case 3:     perform_routine = (method) perform64_op<T, width64>;            break;
            case 4:     perform_routine = (method) perform64_single1_op<T, width64>;    break;
            case 5:     perform_routine = (method) perform64_single2_op<T, width64>;    break;
                
            // Vector aligned array
                
            case 6:     perform_routine = (method) perform64_array<T>;                  break;
            case 7:     perform_routine = (method) perform64_single1_array<T>;          break;
            case 8:     perform_routine = (method) perform64_single2_array<T>;          break;
        }
        
        object_method(dsp64, gensym("dsp_add64"), x, perform_routine, 0, userparam);
    }
    
    // 64 bit perform routine with one LHS signal-rate input (SIMD - op)

    template <class T, int N>
    static void perform64_single1_op(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        Functor &functor = x->m_functor;

        SIMDType<double, N> *in1 = reinterpret_cast<SIMDType<double, N> *>(ins[userparam ? 1 : 0]);
        SIMDType<double, N> *out1 = reinterpret_cast<SIMDType<double, N> *>(outs[0]);
        
        SIMDType<double, N> double_val(x->m_val);
        
        vec_size /= SIMDType<double, N>::size;
        
        while (vec_size--)
            *out1++ = fix_denorm(functor(*in1++, double_val));
    }
    
    // 64 bit perform routine with one RHS signal-rate input (SIMD - op)
    
    template <class T, int N>
    static void perform64_single2_op(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        Functor &functor = x->m_functor;
        
        SIMDType<double, N> *in1 = reinterpret_cast<SIMDType<double, N> *>(ins[userparam ? 1 : 0]);
        SIMDType<double, N> *out1 = reinterpret_cast<SIMDType<double, N> *>(outs[0]);
        
        SIMDType<double, N> double_val(x->m_val);

        vec_size /= SIMDType<double, N>::size;

        while (vec_size--)
            *out1++ = fix_denorm(functor(double_val, *in1++));
    }
    
    // 64 bit perform routine with two signal-rate inputs (SIMD - op)
    
    template <class T, int N>
    static void perform64_op(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        Functor &functor = x->m_functor;

        SIMDType<double, N> *in1 = reinterpret_cast<SIMDType<double, N> *>(ins[userparam ? 1 : 0]);
        SIMDType<double, N> *in2 = reinterpret_cast<SIMDType<double, N> *>(ins[userparam ? 0 : 1]);
        SIMDType<double, N> *out1 = reinterpret_cast<SIMDType<double, N> *>(outs[0]);

        vec_size /= SIMDType<double, N>::size;

        while (vec_size--)
            *out1++ = fix_denorm(functor(*in1++, *in2++));
    }
    
    // 64 bit perform routine with one LHS signal-rate input (SIMD - array)
    
    template <class T>
    static void perform64_single1_array(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        double *temp_memory = (double *)x->m_temp_memory;
        
        std::fill_n(((double *)temp_memory), vec_size, x->m_val);
        x->m_functor(outs[0], ins[userparam ? 1 : 0], temp_memory, vec_size);
    }
    
    // 64 bit perform routine with one RHS signal-rate input (SIMD - array)
    
    template <class T>
    static void perform64_single2_array(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        double *temp_memory = (double *)x->m_temp_memory;
        
        std::fill_n(((double *)temp_memory), vec_size, x->m_val);
        x->m_functor(outs[0], temp_memory, ins[userparam ? 1 : 0], vec_size);
    }
    
    // 64 bit perform routine with two signal-rate inputs (SIMD - array)
    
    template <class T>
    static void perform64_array(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        x->m_functor(outs[0], ins[userparam ? 1 : 0], ins[userparam ? 0 : 1], vec_size);
    }
    
    // Assist routine
    
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
    
    // Static Items
    
    template <class T> static t_class **getClassPointer()
    {
        static t_class *C;
        
        return &C;
    }
    
    template <class T> static std::string *accessClassName()
    {
        static std::string str;
        
        return &str;
    }
    
    // Data
    
    t_pxobject m_obj;
    
    double m_val;
    
    void *m_temp_memory;
    
    Functor m_functor;
};

