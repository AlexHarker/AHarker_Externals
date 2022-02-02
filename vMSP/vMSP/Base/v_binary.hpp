

#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <string>

#include "SIMDSupport.hpp"

enum class calculation_type { scalar, vector_op, vector_array };
enum class inputs { binary, scalar1, scalar2 };

// Object structure

template<typename Functor, calculation_type Type, bool FirstInputAlwaysSignal = false>
class v_binary
{
    static float fix_denorm(const float a)
    {
        float b = a;
        FIX_DENORM_FLOAT(b);
        return b;
    }

    template <class T>
    static T fix_denorm(const T a) { return a; }
    
public:
    
    // Main routine
    
    template <typename T>
    static int setup(const char *object_name)
    {
        t_class **C = getClassPointer<T>();
        *accessClassName<T>() = object_name;
        
        *C = class_new(object_name,
                       (method) new_object<T>,
                       (method) free_object<T>,
                       sizeof(T),
                       (method) nullptr,
                       A_DEFFLOAT,
                       0);
        
        class_addmethod(*C, (method) float_in<T>, "float", A_FLOAT, 0);
        class_addmethod(*C, (method) int_in<T>, "int", A_LONG, 0);
        class_addmethod(*C, (method) dsp64<T>, "dsp64", A_CANT, 0);
        class_addmethod(*C, (method) assist, "assist", A_CANT, 0);
        
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

        return x;
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
    
    // 64 bit dsp routine
    
    template <class T>
    static void dsp64(T *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags)
    {
        method perform_routine = (method) perform64_op<T, 1>;
        long routine = 0;
        constexpr int simd_width = SIMDLimits<double>::max_size;
        
        // If nothing is connected then don't do anything here....
            
        if (!count[1] && !count[0])
            return;
        
        // Choose scalar routines as needed
        
        if (!count[0])
            routine = FirstInputAlwaysSignal ? 1 : 2;
        else if (!count[1])
            routine = 1;
        
        // Use SIMD code where possible
        
        if (Type != calculation_type::scalar && ((max_vec / simd_width) > 0))
            routine += (Type == calculation_type::vector_op) ? 3 : 6;
        
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
        x->m_functor(outs[0], ins[0], ins[1], vec_size, x->m_val, inputs::scalar1);
    }
    
    // 64 bit perform routine with one RHS signal-rate input (SIMD - array)
    
    template <class T>
    static void perform64_single2_array(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        x->m_functor(outs[0], ins[0], ins[1], vec_size, x->m_val, inputs::scalar2);
    }
    
    // 64 bit perform routine with two signal-rate inputs (SIMD - array)
    
    template <class T>
    static void perform64_array(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        x->m_functor(outs[0], ins[0], ins[1], vec_size, x->m_val, inputs::binary);
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

