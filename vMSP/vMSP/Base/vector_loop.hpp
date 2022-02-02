
#ifndef _VECTOR_LOOP_HPP_
#define _VECTOR_LOOP_HPP_

template <class Op, class T, typename... Args>
void vector_loop(T *o, const T *i, long size, Args... args)
{
    using vec_type = SIMDType<T, SIMDLimits<T>::max_size>;
    
    const vec_type *v_i = reinterpret_cast<const vec_type *>(i);
    vec_type *v_o = reinterpret_cast<vec_type *>(o);
    
    Op op;
    
    // N.B. we can assume that there are an exact number of vectors and that vectors are aligned

    for (; size > 0; size -= SIMDLimits<T>::max_size)
        *v_o++ = op(*v_i++, args...);
}

#endif /* _VECTOR_LOOP_HPP_ */
