
#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <string>

#include "SIMDSupport.hpp"
#include <AH_Denormals.h>

enum CalculationType { kScalar, kVectorOp, kVectorArray };

// Object structure

template<typename Functor, CalculationType Vec32, CalculationType Vec64>
class v_unary
{
    
public:
    
    // Main routine
    
    template <typename T>
    static int setup(const char *object_name)
    {
        t_class **C = getClassPointer<T>();
        *accessClassName<T>() = object_name;
        
        *C = class_new (object_name, (method)new_object<T>, (method)free_object<T>, sizeof(T), NULL, 0);
        
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
    static void *new_object()
    {
        T *x = static_cast<T *>(object_alloc(*getClassPointer<T>()));
        
        dsp_setup(reinterpret_cast<t_pxobject *>(&x->m_obj), 1);
        outlet_new(reinterpret_cast<t_object *>(x),"signal");
        
        return (x);
    }
    
    // 32 bit dsp routine
    
    template <class T>
    static void dsp(T *x, t_signal **sp, short *count)
    {
        const static int simd_width = SIMDLimits<float>::max_size;
        
        // Default to scalar routine
        
        bool vector = (Vec32 != kScalar)  || ((sp[0]->s_n / simd_width) > 0);
        method current_perform_routine = reinterpret_cast<method>(perform<perform_op<T, 1> >);
        long vec_size_val = sp[0]->s_n;

        // Use SIMD routines if possible
        
        if (vector)
        {
            vec_size_val /= simd_width;

            if (Vec32 == kVectorArray)
                current_perform_routine = reinterpret_cast<method>(perform<perform_array<T> >);
            else
            {
                // Check memory alignment of all relevant vectors

                if ((t_ptr_uint) sp[0]->s_vec % 16 || (t_ptr_uint) sp[1]->s_vec % 16)
                {
                    current_perform_routine = reinterpret_cast<method>(perform<perform_misaligned<T, simd_width> >);
                    post ("%s: handed a misaligned signal vector - update to Max 5.1.3 or later", accessClassName<T>()->c_str());
                }
                else
                    current_perform_routine = reinterpret_cast<method>(perform<perform_op<T, simd_width> >);
            }
        }
        
        dsp_add(denormals_perform, 5, current_perform_routine, sp[0]->s_vec, sp[1]->s_vec, vec_size_val, x);
    }
    
    // 32 bit perform return wrapper
    
    template<void perform_routine(t_int *w)>
    static t_int *perform(t_int *w)
    {
        perform_routine(w);
        
        return w + 6;
    }
    
    // 32 bit perform routine (SIMD - array)
    
    template <class T>
    static void perform_array(t_int *w)
    {
        T *x = reinterpret_cast<T *>(w[5]);

        x->m_functor(reinterpret_cast<float *>(w[3]), reinterpret_cast<float *>(w[2]), static_cast<long>(w[4]));
    }
    
    // 32 bit perform routine (SIMD - op)
    
    template <class T, int N>
    static void perform_op(t_int *w)
    {
        SIMDType<float, N> *in1 = reinterpret_cast<SIMDType<float, N> *>(w[2]);
        SIMDType<float, N> *out1 = reinterpret_cast<SIMDType<float, N> *>(w[3]);
        long vec_size = w[4];
        T *x = reinterpret_cast<T *>(w[5]);

        Functor &functor = x->m_functor;

        while (vec_size--)
            *out1++ = functor(*in1++);
    }
    
    // 32 bit perform routine (misaligned SIMD - op - early versions of Max 5 only - slightly slower)
    
    template <class T, int N>
    static void perform_misaligned(t_int *w)
    {
        float *in1 =  reinterpret_cast<float *>(w[2]);
        float *out1 = reinterpret_cast<float *>(w[3]);
        long vec_size = w[4];
        T *x = reinterpret_cast<T *>(w[5]);
        
        Functor &functor = x->m_functor;

        for ( ; vec_size--; in1 += N, out1 += N)
            functor(SIMDType<float, N>(in1)).store(out1);
    }
    
    // 64 bit dsp routine
    
    template <class T>
    static void dsp64(T *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
    {
        const static int simd_width = SIMDLimits<double>::max_size;
        
        // Default to scalar routine
        
        bool vector = !(Vec64 != kScalar) || ((maxvectorsize / simd_width) > 0);
        method current_perform_routine = reinterpret_cast<method>(perform64_op<T, 1>);
        
        // Use SIMD routines if possible

        if (vector)
        {
            if (Vec64 == kVectorArray)
                current_perform_routine = reinterpret_cast<method>(perform64_array<T>);
            else
                current_perform_routine = reinterpret_cast<method>(perform64_op<T, simd_width>);
        }
        
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
