
#ifndef VECTOR_LOOPS_H
#define VECTOR_LOOPS_H

template <class Op, class T, typename... Args>
void vector_loop(T *o, const T *i, long size, Args... args)
{
    using VecType = SIMDType<T, SIMDLimits<T>::max_size>;
    
    const VecType *v_i = reinterpret_cast<const VecType *>(i);
    VecType *v_o = reinterpret_cast<VecType *>(o);
    
    Op op;
    
    // N.B. we can assume that there are an exact number of vectors and that vectors are aligned

    for (; size > 0; size -= SIMDLimits<T>::max_size)
        *v_o++ = op(*v_i++, args...);
}

#endif /* VECTOR_LOOPS_H */
