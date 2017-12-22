
#ifndef TABLEREADER_H
#define TABLEREADER_H

#include "SIMDSupport.hpp"
#include "Interpolation.hpp"

// Base class for table fetchers

template <class T> struct table_fetcher
{
    typedef T fetch_type;
    table_fetcher(double scale_val) : scale(scale_val) {}
    
    const double scale;
};

// Generic intepolation readers

template <class T, class U, class Table, typename Interp> struct interp_2_reader
{
    interp_2_reader(Table fetcher) : fetch(fetcher) {}
    
    T operator()(const double*& positions)
    {
        typename T::scalar_type fract_array[T::size];
        typename U::scalar_type array[T::size * 2];
        
        for (int i = 0; i < T::size; i++)
        {
            double position = *positions++;
            intptr_t offset = static_cast<intptr_t>(position);
            fract_array[i] = static_cast<typename T::scalar_type >(position - static_cast<double>(offset));
            
            array[i] = fetch(offset);
            array[i + T::size] = fetch(offset + 1);
        }
        
        const T y0 = U(array);
        const T y1 = U(array + T::size);
        
        return interpolate(T(fract_array), y0, y1);
    }
    
    Table fetch;
    Interp interpolate;
};

template <class T, class U, class Table, typename Interp> struct interp_4_reader
{
    interp_4_reader(Table fetcher) : fetch(fetcher) {}
    
    T operator()(const double*& positions)
    {
        typename U::scalar_type array[T::size * 4];
        typename T::scalar_type fract_array[T::size];

        for (int i = 0; i < T::size; i++)
        {
            double position = *positions++;
            intptr_t offset = static_cast<intptr_t>(position);
            fract_array[i] = static_cast<typename T::scalar_type >(position - static_cast<double>(offset));
            
            array[i] = fetch(offset - 1);
            array[i + T::size] = fetch(offset);
            array[i + T::size * 2] = fetch(offset + 1);
            array[i + T::size * 3] = fetch(offset + 2);
        }
        
        const T y0 = U(array);
        const T y1 = U(array + T::size);
        const T y2 = U(array + (T::size * 2));
        const T y3 = U(array + (T::size * 3));
        
        return interpolate(T(fract_array), y0, y1, y2, y3);
    }
    
    Table fetch;
    Interp interpolate;
};

// Readers with specific interpolation types

template <class T, class U, class Table> struct no_interp_reader
{
    no_interp_reader(Table fetcher) : fetch(fetcher) {}
    
    T operator()(const double*& positions)
    {
        typename U::scalar_type array[T::size];
        
        for (int i = 0; i < T::size; i++)
            array[i] = fetch(static_cast<intptr_t>(*positions++));
        
        return U(array);
    }
    
    Table fetch;
};

template <class T, class U, class Table>
struct linear_reader : public interp_2_reader<T, U, Table, linear_interp<T> >
{
    linear_reader(Table fetcher) : interp_2_reader<T, U, Table, linear_interp<T> >(fetcher) {}
};

template <class T, class U, class Table>
struct cubic_bspline_reader : public interp_4_reader<T, U, Table, cubic_bspline_interp<T> >
{
    cubic_bspline_reader(Table fetcher) : interp_4_reader<T, U, Table, cubic_bspline_interp<T> >(fetcher) {}
};

template <class T, class U, class Table>
struct cubic_hermite_reader : public interp_4_reader<T, U, Table, cubic_hermite_interp<T> >
{
    cubic_hermite_reader(Table fetcher) : interp_4_reader<T, U, Table, cubic_hermite_interp<T> >(fetcher) {}
};

template <class T, class U, class Table>
struct cubic_lagrange_reader : public interp_4_reader<T, U, Table, cubic_lagrange_interp<T> >
{
    cubic_lagrange_reader(Table fetcher) : interp_4_reader<T, U, Table, cubic_lagrange_interp<T> >(fetcher) {}
};

// Reading loop

template <class T, class U, class Table, template <class V, class W, class Tb> class Reader>
void table_read_loop(Table fetcher, typename T::scalar_type *out, const double *positions, intptr_t n_samps, typename U::scalar_type mul)
{
    Reader<T, U, Table> reader(fetcher);
    
    T *v_out = reinterpret_cast<T *>(out);
    T scale = mul * reader.fetch.scale;
    
    for (intptr_t i = 0; i < (n_samps / T::size); i++)
        *v_out++ = scale * reader(positions);
}

// Template to determine vector/scalar types

template <template <class T, class U, class Tb> class Reader, class Table, class V>
void table_read(Table fetcher, V *out, const double *positions, intptr_t n_samps, double mul)
{
    typedef typename Table::fetch_type fetch_type;
#if (SIMD_COMPILER_SUPPORT_LEVEL >= SIMD_COMPILER_SUPPORT_AVX256)
    const int vec_size = 32 / sizeof(V);
#else
    const int vec_size = 16 / sizeof(V);
#endif
    
    intptr_t n_vsample = (n_samps / SIMDType<V, vec_size>::size) * SIMDType<V, vec_size>::size;
    
    table_read_loop<SIMDType<V, vec_size>, SIMDType<fetch_type, vec_size>, Table, Reader>(fetcher, out, positions, n_vsample, mul);
    table_read_loop<Scalar<V>, Scalar<fetch_type>, Table, Reader>(fetcher, out + n_vsample, positions + n_vsample, n_samps - n_vsample, mul);
}

// Main reading call that switches between different types of interpolation

template <class T, class Table>
void table_read(Table fetcher, T *out, const double *positions, intptr_t n_samps, T mul, InterpType interp)
{
    switch(interp)
    {
        case kInterpNone:           table_read<no_interp_reader>(fetcher, out, positions, n_samps, mul);        break;
        case kInterpLinear:         table_read<linear_reader>(fetcher, out, positions, n_samps, mul);           break;
        case kInterpCubicHermite:   table_read<cubic_hermite_reader>(fetcher, out,positions, n_samps, mul);     break;
        case kInterpCubicLagrange:  table_read<cubic_lagrange_reader>(fetcher, out, positions, n_samps, mul);   break;
        case kInterpCubicBSpline:   table_read<cubic_bspline_reader>(fetcher, out, positions, n_samps, mul);    break;
    }
}

#endif /* TableReader_h */
