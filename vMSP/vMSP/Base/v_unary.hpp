
#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <string>

#include "SIMDSupport.hpp"

enum class calculation_type { scalar, vector_op, vector_array };

// Object structure

template<typename Functor, calculation_type Type>
class v_unary
{
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
                       0);
        
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
    static void *new_object()
    {
        T *x = static_cast<T *>(object_alloc(*getClassPointer<T>()));
        
        dsp_setup(reinterpret_cast<t_pxobject *>(&x->m_obj), 1);
        outlet_new(reinterpret_cast<t_object *>(x),"signal");
        
        return x;
    }

    // 64 bit dsp routine
    
    template <typename T, calculation_type C = Type>
    static method dsp_vector_select64(std::enable_if_t<C == calculation_type::scalar, T *>)
    {
        // Scalar
        
        return reinterpret_cast<method>(perform64_op<T, 1>);
    }
    
    template <typename T, calculation_type C = Type>
    static method dsp_vector_select64(std::enable_if_t<C == calculation_type::vector_op, T *>)
    {
        // Vector Op

        constexpr int simd_width = SIMDLimits<double>::max_size;
        return reinterpret_cast<method>(perform64_op<T, simd_width>);
    }
    
    template <typename T, calculation_type C = Type>
    static method dsp_vector_select64(std::enable_if_t<C == calculation_type::vector_array, T *>)
    {
        // Vector Array
    
        return reinterpret_cast<method>(perform64_array<T>);
    }
    
    template <class T>
    static void dsp64(T *x, t_object *dsp64, short *count, double sample_rate, long max_vec, long flags)
    {
        // Default to scalar routine
        
        method current_perform_routine = reinterpret_cast<method>(perform64_op<T, 1>);
        
        // Use SIMD routines if possible

        if ((Type != calculation_type::scalar) && ((max_vec / SIMDLimits<double>::max_size) > 0))
            current_perform_routine = dsp_vector_select64<T>(x);
        
        object_method(dsp64, gensym("dsp_add64"), x, current_perform_routine, 0, 0);
    }
    
    // 64 bit perform routine (SIMD - array)
    
    template <class T>
    static void perform64_array(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
         x->m_functor(outs[0], ins[0], vec_size);
    }
    
    // 64 bit perform routine (SIMD - op)
    
    template <class T, int N>
    static void perform64_op(T *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
    {
        Functor &functor = x->m_functor;

        SIMDType<double, N> *in1 = reinterpret_cast<SIMDType<double, N> *>(ins[0]);
        SIMDType<double, N> *out1 = reinterpret_cast<SIMDType<double, N> *>(outs[0]);
        
        vec_size /= SIMDType<double, N>::size;
            
        while (vec_size--)
            *out1++ = functor(*in1++);
    }
    
    // Assist routine
    
    static void assist(v_unary *x, void *b, long m, long a, char *s)
    {
        if (m == ASSIST_INLET)
            sprintf(s,"(signal) In");
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
    
public:
    
    Functor m_functor;
};
