

#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <string>

#include "SIMDSupport.hpp"
#include <AH_Denormals.h>

enum InputType { kBinary, kScalar1, kScalar2 };
enum CalculationType { kScalar, kVectorOp, kVectorArray };

// Object structure

template<typename Functor, CalculationType Vec32, CalculationType Vec64, bool FirstInputAlwaysSignal = false>
class v_binary
{
    static float fix_denorm(const float a) { return AH_FIX_DENORM_FLOAT(a); }

    template <class T>
    static T fix_denorm(const T a) { return a; }
    
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
                
        post ("%s - using vector version by Alex Harker", object_name);
        
        return 0;
    }
    
    // Free routine
    
    template <class T>
    static void free_object(T *x)
    {
        dsp_free(&x->m_obj);
    }
    
    // New routine
    
    template <class T>
    static void *new_object(double double_val)
    {
        T *x = static_cast<T *>(object_alloc(*getClassPointer<T>()));
        
        dsp_setup(reinterpret_cast<t_pxobject *>(&x->m_obj), 2);
        outlet_new(reinterpret_cast<t_object *>(x),"signal");
        
        x->m_val = double_val;

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
        const static int simd_width = SIMDLimits<float>::max_size;

        long routine = 0;
        long vec_size_val = sp[0]->s_n;

        float *used_input1 = reinterpret_cast<float *>(sp[0]->s_vec);
        float *used_input2 = reinterpret_cast<float *>(sp[1]->s_vec);
        
        // Default to scalar routines
        
        method perform_routine = (method) perform<perform_op<T, 1> >;
        
        // If nothing is connected then don't do anything here....
        
        if (!count[1] && !count[0])
            return;
        
        // Choose scalr routines as needed
        
        if (!count[0])
            routine = FirstInputAlwaysSignal ? 1 : 2;
        else if (!count[1])
            routine = 1;

        // Use SIMD code where possible
        
        if (Vec32 != kScalar && ((sp[0]->s_n / simd_width) > 0))
        {
            if ((t_ptr_uint) used_input1 % 16 || (t_ptr_uint) used_input2 % 16 || (t_ptr_uint) sp[2]->s_vec % 16)
            {
                object_error(reinterpret_cast<t_object *>(x), "handed a misaligned signal vector - update to Max 5.1.3 or later", accessClassName<T>()->c_str());
            }
            else
                routine += (Vec32 == kVectorOp) ? 3 : 6;
        }
        
        switch (routine)
        {
            // Scalar
            
            case 0:     perform_routine = (method) perform<perform_op<T, 1> >;                              break;
            case 1:     perform_routine = (method) perform<perform_single1_op<T, 1> >;                      break;
            case 2:     perform_routine = (method) perform<perform_single2_op<T, 1> >;                      break;
            
            // Vector op
                
            case 3:     perform_routine = (method) perform<perform_op<T, simd_width> >;                     break;
            case 4:     perform_routine = (method) perform<perform_single1_op<T, simd_width> >;             break;
            case 5:     perform_routine = (method) perform<perform_single2_op<T, simd_width> >;             break;
                
            // Vector array
            
            case 6:     perform_routine = (method) perform<perform_array<T> >;                              break;
            case 7:     perform_routine = (method) perform<perform_single1_array<T> >;                      break;
            case 8:     perform_routine = (method) perform<perform_single2_array<T> >;                      break;
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
        
        x->m_functor(reinterpret_cast<float *>(w[4]), reinterpret_cast<float *>(w[2]), reinterpret_cast<float *>(w[3]), static_cast<long>(w[5]), x->m_val, kScalar1);
    }
    
    // 32 bit perform routine with RHS signal-rate input (SIMD - array)

    template <class T>
    static void perform_single2_array(t_int *w)
    {
        T *x = reinterpret_cast<T *>(w[6]);
        
        x->m_functor(reinterpret_cast<float *>(w[4]), reinterpret_cast<float *>(w[2]), reinterpret_cast<float *>(w[3]), static_cast<long>(w[5]), x->m_val, kScalar2);
    }
    
    // 32 bit perform routine with two signal-rate inputs (SIMD - array)
    
    template <class T>
    static void perform_array(t_int *w)
    {
        T *x = reinterpret_cast<T *>(w[6]);
        
        x->m_functor(reinterpret_cast<float *>(w[4]), reinterpret_cast<float *>(w[2]), reinterpret_cast<float *>(w[3]), static_cast<long>(w[5]), x->m_val, kBinary);
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
    
    // 64 bit dsp routine
    
    template <class T>
    static void dsp64(T *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
    {
        method perform_routine = (method) perform64_op<T, 1>;
        long routine = 0;
        const static int simd_width = SIMDLimits<double>::max_size;
        
        // If nothing is connected then don't do anything here....
            
        if (!count[1] && !count[0])
            return;
        
        // Choose scalar routines as needed
        
        if (!count[0])
            routine = FirstInputAlwaysSignal ? 1 : 2;
        else if (!count[1])
            routine = 1;
        
        // Use SIMD code where possible
        
        if (Vec64 != kScalar && ((maxvectorsize / simd_width) > 0))
            routine += (Vec32 == kVectorOp) ? 3 : 6;
        
        switch (routine)
        {
            // Scalar
                
            case 0:     perform_routine = (method) perform64_op<T, 1>;                      break;
            case 1:     perform_routine = (method) perform64_single1_op<T, 1>;              break;
            case 2:     perform_routine = (method) perform64_single2_op<T, 1>;              break;
                
            // Vector op
                
            case 3:     perform_routine = (method) perform64_op<T, simd_width>;             break;
            case 4:     perform_routine = (method) perform64_single1_op<T, simd_width>;     break;
            case 5:     perform_routine = (method) perform64_single2_op<T, simd_width>;     break;
                
            // Vector array
                
            case 6:     perform_routine = (method) perform64_array<T>;                      break;
            case 7:     perform_routine = (method) perform64_single1_array<T>;              break;
            case 8:     perform_routine = (method) perform64_single2_array<T>;              break;
        }
        
        object_method(dsp64, gensym("dsp_add64"), x, perform_routine, 0, 0);
    }
    
    // 64 bit perform routine with one LHS signal-rate input (SIMD - op)

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
    
    // 64 bit perform routine with one RHS signal-rate input (SIMD - op)
    
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
    
    // 64 bit perform routine with two signal-rate inputs (SIMD - op)
    
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
    
    // 64 bit perform routine with one LHS signal-rate input (SIMD - array)
    
    template <class T>
    static void perform64_single1_array(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        x->m_functor(outs[0], ins[0], ins[1], vec_size, x->m_val, kScalar1);
    }
    
    // 64 bit perform routine with one RHS signal-rate input (SIMD - array)
    
    template <class T>
    static void perform64_single2_array(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        x->m_functor(outs[0], ins[0], ins[1], vec_size, x->m_val, kScalar2);
    }
    
    // 64 bit perform routine with two signal-rate inputs (SIMD - array)
    
    template <class T>
    static void perform64_array(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        x->m_functor(outs[0], ins[0], ins[1], vec_size, x->m_val, kBinary);
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
        
    Functor m_functor;
};

