
#ifndef WINDOWFUNCTIONS_HPP
#define WINDOWFUNCTIONS_HPP

#include <algorithm>
#include <cstdint>
#include <cmath>
#include <limits>
#include <vector>

// Coefficients (and the basis for naming) can largely be found in:
//
// Nuttall, A. (1981). Some windows with very good sidelobe behavior.
// IEEE Transactions on Acoustics, Speech, and Signal Processing, 29(1), 84-91.
//
// Similar windows / additional flat-top windows from:
//
// Heinzel, G., Rüdiger, A., & Schilling, R. (2002).
// Spectrum and spectral density estimation by the Discrete Fourier transform (DFT),
// including a comprehensive list of window functions and some new flat-top windows.

namespace window_functions
{
    // Parameter struct
    
    struct params
    {
        constexpr params(double A0 = 0.0, double A1 = 0.0, double A2 = 0.0, double A3 = 0.0, double A4 = 0.0, double exp = 1.0)
        : a0(A0), a1(A1), a2(A2), a3(A3), a4(A4), exponent(exp) {}
        
        constexpr params(const double *param_array, int N, double exp)
        : a0(N > 0 ? param_array[0] : 0.0)
        , a1(N > 1 ? param_array[1] : 0.0)
        , a2(N > 2 ? param_array[2] : 0.0)
        , a3(N > 3 ? param_array[3] : 0.0)
        , a4(N > 4 ? param_array[4] : 0.0)
        , exponent(exp) {}
     
        double a0;
        double a1;
        double a2;
        double a3;
        double a4;
        
        double exponent;
    };
    
    using window_func = double(uint32_t, uint32_t, const params&);

    template <class T>
    using window_generator = void(T*, uint32_t, uint32_t, uint32_t, const params&);
    
    namespace impl
    {
        // Constexpr functions for convenience
        
        constexpr double pi()   { return M_PI; }
        constexpr double pi2()  { return M_PI * 2.0; }
        constexpr double pi4()  { return M_PI * 4.0; }
        constexpr double pi6()  { return M_PI * 6.0; }
        constexpr double pi8()  { return M_PI * 8.0; }
        
        constexpr double div(int x, int y)
        {
            return static_cast<double>(x) / static_cast<double>(y);
        }
        
        // Operators for cosine sum windows
        
        struct constant
        {
            constant(double v) : value(v) {}
            
            inline double operator()(double x) { return value; }
            
            const double value;
        };
        
        struct cosx
        {
            cosx(double c, double mul)
            : coefficient(c), multiplier(mul) {}
            
            inline double operator()(double x)
            {
                return coefficient * cos(x * multiplier);
            }
            
            const double coefficient;
            const double multiplier;
        };
        
        // Normalisation helper
        
        static inline double normalise(uint32_t x, uint32_t N)
        {
            return static_cast<double>(x) / static_cast<double>(N);
        }
        
        // Summing functions for cosine sum windows
        
        template <typename T>
        inline double sum(double x, T op)
        {
            return op(x);
        }
        
        template <typename T, typename ...Ts>
        inline double sum(double x, T op, Ts... ops)
        {
            return sum(x, op) + sum(x, ops...);
        }
        
        template <typename ...Ts>
        inline double sum(uint32_t i, uint32_t N, Ts... ops)
        {
            return sum(normalise(i, N), ops...);
        }
        
        // Specific window calculations
        
        inline double rect(uint32_t i, uint32_t N, const params& p)
        {
            return 1.0;
        }
        
        inline double triangle(uint32_t i, uint32_t N, const params& p)
        {
            return 1.0 - fabs(normalise(i, N) * 2.0 - 1.0);
        }
        
        inline double trapezoid(uint32_t i, uint32_t N, const params& p)
        {
            double a = p.a0;
            double b = p.a1;
            
            if (b < a)
                std::swap(a, b);
            
            const double x = normalise(i, N);
            
            if (x < a)
                return x / a;
            
            if (x > b)
                return 1.0 - ((x - b) / (1.0 - b));
            
            return 1.0;
        }
        
        inline double welch(uint32_t i, uint32_t N, const params& p)
        {
            const double x = 2.0 * normalise(i, N) - 1.0;
            return 1.0 - x * x;
        }
        
        inline double parzen(uint32_t i, uint32_t N, const params& p)
        {
            const double N2 = static_cast<double>(N) * 0.5;
            
            auto w0 = [&](double x)
            {
                x = fabs(x) / N2;
                
                if (x > 0.5)
                {
                    double v = (1.0 - x);
                    return 2.0 * v * v * v;
                }
                
                return 1.0 - 6.0 * x * x * (1.0 - x);
            };
            
            return w0(static_cast<double>(i) - N2);
        }
        
        inline double sine(uint32_t i, uint32_t N, const params& p)
        {
            return sin(pi() * normalise(i, N));
        }
        
        inline double sine_taper(uint32_t i, uint32_t N, const params& p)
        {
            return sin(p.a0 * pi() * normalise(i, N));
        }
        
        inline double tukey(uint32_t i, uint32_t N, const params& p)
        {
            return 0.5 - 0.5 * cos(trapezoid(i, N, p) * pi());
        }
        
        inline double izero(double x2)
        {
            double term = 1.0;
            double bessel = 1.0;
            
            // N.B. - loop based on epsilon for maximum accuracy
            
            for (unsigned long i = 1; term > std::numeric_limits<double>::epsilon(); i++)
            {
                const double i2 = static_cast<double>(i * i);
                term = term * x2 * (1.0 / (4.0 * i2));
                bessel += term;
            }
            
            return bessel;
        }
        
        inline double kaiser(uint32_t i, uint32_t N,  const params& p)
        {
            double x = 2.0 * normalise(i, N) - 1.0;
            return izero((1.0 - x * x) * p.a0 * p.a0) * p.a1;
        }
        
        inline double cosine_2_term(uint32_t i, uint32_t N, const params& p)
        {
            return sum(i, N, constant(p.a0), cosx(-(1.0 - p.a0), pi2()));
        }
        
        inline double cosine_3_term(uint32_t i, uint32_t N, const params& p)
        {
            return sum(i, N, constant(p.a0), cosx(-p.a1, pi2()), cosx(p.a2, pi4()));
        }
        
        inline double cosine_4_term(uint32_t i, uint32_t N, const params& p)
        {
            return sum(i, N, constant(p.a0), cosx(-p.a1, pi2()), cosx(p.a2, pi4()), cosx(-p.a3, pi6()));
        }
        
        inline double cosine_5_term(uint32_t i, uint32_t N, const params& p)
        {
            return sum(i, N, constant(p.a0),
                       cosx(-p.a1, pi2()),
                       cosx(p.a2, pi4()),
                       cosx(-p.a3, pi6()),
                       cosx(p.a4, pi8()));
        }
        
        inline double hann(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_2_term(i, N, params(0.5));
        }
        
        inline double hamming(uint32_t i, uint32_t N, const params& p)
        {
            // N.B. here we use approx alpha of 0.54 (not 25/46 or 0.543478260869565)
            // see equiripple notes on wikipedia
            
            return cosine_2_term(i, N, params(0.54));
        }
        
        inline double blackman(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_3_term(i, N, params(0.42, 0.5, 0.08));
        }
        
        inline double exact_blackman(uint32_t i, uint32_t N, const params& p)
        {
            const params pb(div(7938, 18608), div(9240, 18608), div(1430, 18608));
            return cosine_3_term(i, N, pb);
        }
        
        inline double blackman_harris_62dB(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_3_term(i, N, params(0.44959, 0.49364, 0.05677));
        }
        
        inline double blackman_harris_71dB(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_3_term(i, N, params(0.42323, 0.49755, 0.07922));
        }
        
        inline double blackman_harris_74dB(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_4_term(i, N, params(0.402217, 0.49703, 0.09892, 0.00188));
        }
        
        inline double blackman_harris_92dB(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_4_term(i, N, params(0.35875, 0.48829, 0.14128, 0.01168));
        }
        
        inline double nuttall_1st_64dB(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_3_term(i, N, params(0.40897, 0.5, 0.09103));
        }
        
        inline double nuttall_1st_93dB(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_4_term(i, N, params(0.355768, 0.487396, 0.144232, 0.012604));
        }
        
        inline double nuttall_3rd_47dB(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_3_term(i, N, params(0.375, 0.5, 0.125));
        }
        
        inline double nuttall_3rd_83dB(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_4_term(i, N, params(0.338946, 0.481973, 0.161054, 0.018027));
        }
        
        inline double nuttall_5th_61dB(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_4_term(i, N, params(0.3125, 0.46875, 0.1875, 0.03125));
        }
        
        inline double nuttall_minimal_71dB(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_3_term(i, N, params(0.4243801, 0.4973406, 0.0782793));
        }
        
        inline double nuttall_minimal_98dB(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_4_term(i, N, params(0.3635819, 0.4891775, 0.1365995, 0.0106411));
        }
        
        inline double ni_flat_top(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_3_term(i, N, params(0.2810639, 0.5208972, 0.1980399));
        }
        
        inline double hp_flat_top(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_4_term(i, N, params(1.0, 1.912510941, 1.079173272, 0.1832630879));
        }
        
        inline double stanford_flat_top(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_5_term(i, N, params(1.0, 1.939, 1.29, 0.388, 0.028));
        }
        
        inline double heinzel_flat_top_70dB(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_4_term(i, N, params(1.0, 1.90796, 1.07349, 0.18199));
        }
        
        inline double heinzel_flat_top_90dB(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_5_term(i, N, params(1.0, 1.942604, 1.340318, 0.440811, 0.043097));
        }
        
        inline double heinzel_flat_top_95dB(uint32_t i, uint32_t N, const params& p)
        {
            return cosine_5_term(i, N, params(1.0, 1.9383379, 1.3045202, 0.4028270, 0.0350665));
        }
        
        // Abstract generator
        
        template <window_func Func, bool symmetric, class T>
        void inline generate(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
        {
            auto sq = [&](double x) { return x * x; };
            auto cb = [&](double x) { return x * x * x; };
            auto qb = [&](double x) { return sq(sq(x)); };
            auto toType = [](double x) { return static_cast<T>(x); };
            
            const T *copy_first = nullptr;
            const T *copy_last = nullptr;
            T *out_first = nullptr;
            
            end = std::min(N + 1, end);
            begin = std::min(begin, end);
            
            if (symmetric)
            {
                uint32_t M = (N/2) + 1;
                
                if (begin < M && end > M + 1)
                {
                    uint32_t begin_n = M - begin;
                    uint32_t end_n = (end - begin) - begin_n;
                    
                    if (begin_n > end_n)
                    {
                        copy_last = window + (N+1)/2 - begin;
                        copy_first = copy_last - end_n;
                        out_first = window + begin_n;
                        end = M;
                    }
                    else
                    {
                        copy_first = window + (N+1)/2 + 1 - begin;
                        copy_last = copy_first + (begin_n - 1);
                        out_first = window;
                        window += M - (begin + 1);
                        begin = M - 1;
                    }
                }
            }
            
            if (p.exponent == 1.0)
            {
                for (uint32_t i = begin; i < end; i++)
                    *window++ = toType(Func(i, N, p));
            }
            else if (p.exponent == 0.5)
            {
                for (uint32_t i = begin; i < end; i++)
                    *window++ = toType(std::sqrt(Func(i, N, p)));
            }
            else if (p.exponent == 2.0)
            {
                for (uint32_t i = begin; i < end; i++)
                    *window++ = toType(sq(Func(i, N, p)));
            }
            else if (p.exponent == 3.0)
            {
                for (uint32_t i = begin; i < end; i++)
                    *window++ = toType(cb(Func(i, N, p)));
            }
            else if (p.exponent == 4.0)
            {
                for (uint32_t i = begin; i < end; i++)
                    *window++ = toType(qb(Func(i, N, p)));
            }
            else if (p.exponent > 0 && p.exponent <= std::numeric_limits<int>().max() && p.exponent == std::floor(p.exponent))
            {
                int exponent = static_cast<int>(p.exponent);
                
                for (uint32_t i = begin; i < end; i++)
                    *window++ = toType(std::pow(Func(i, N, p), exponent));
            }
            else
            {
                for (uint32_t i = begin; i < end; i++)
                    *window++ = toType(std::pow(Func(i, N, p), p.exponent));
            }
            
            if (symmetric && out_first)
                std::reverse_copy(copy_first, copy_last, out_first);
        }
    }
    
    // Specific window generators
    
    template <class T>
    void rect(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::rect, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void triangle(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::triangle, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void trapezoid(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::trapezoid, false>(window, N, begin, end, p);
    }
    
    template <class T>
    void welch(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::welch, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void parzen(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::parzen, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void sine(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::sine, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void sine_taper(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        params p1(std::round(p.a0));
        p1.exponent = p.exponent;
        
        impl::generate<impl::sine_taper, false>(window, N, begin, end, p1);
    }
    
    template <class T>
    void tukey(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        params p1(p.a0 * 0.5, 1.0 - (p.a0 * 0.5));
        p1.exponent = p.exponent;
        
        impl::generate<impl::tukey, true>(window, N, begin, end, p1);
    }
    
    template <class T>
    void kaiser(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        params p1(p.a0, 1.0 / impl::izero(p.a0 * p.a0));
        p1.exponent = p.exponent;
        
        impl::generate<impl::kaiser, true>(window, N, begin, end, p1);
    }
    
    template <class T>
    void cosine_2_term(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::cosine_2_term, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void cosine_3_term(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::cosine_3_term, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void cosine_4_term(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::cosine_4_term, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void cosine_5_term(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::cosine_5_term, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void hann(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::hann, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void hamming(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::hamming, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void blackman(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::blackman, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void exact_blackman(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::exact_blackman, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void blackman_harris_62dB(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::blackman_harris_62dB, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void blackman_harris_71dB(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::blackman_harris_71dB, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void blackman_harris_74dB(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::blackman_harris_74dB, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void blackman_harris_92dB(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::blackman_harris_92dB, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void nuttall_1st_64dB(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::nuttall_1st_64dB, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void nuttall_1st_93dB(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::nuttall_1st_93dB, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void nuttall_3rd_47dB(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::nuttall_3rd_47dB, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void nuttall_3rd_83dB(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::nuttall_3rd_83dB, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void nuttall_5th_61dB(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::nuttall_5th_61dB, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void nuttall_minimal_71dB(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::nuttall_minimal_71dB, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void nuttall_minimal_98dB(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::nuttall_minimal_98dB, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void ni_flat_top(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::ni_flat_top, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void hp_flat_top(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::hp_flat_top, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void stanford_flat_top(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::stanford_flat_top, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void heinzel_flat_top_70dB(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::heinzel_flat_top_70dB, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void heinzel_flat_top_90dB(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::heinzel_flat_top_90dB, true>(window, N, begin, end, p);
    }
    
    template <class T>
    void heinzel_flat_top_95dB(T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
    {
        impl::generate<impl::heinzel_flat_top_95dB, true>(window, N, begin, end, p);
    }
    
    template <class T, window_generator<T> ...gens>
    struct indexed_generator
    {
        void operator()(size_t type, T *window, uint32_t N, uint32_t begin, uint32_t end, const params& p)
        {
            return generators[type](window, N, begin, end, p);
        }
        
        window_generator<T> *get(size_t type) { return generators[type]; }
        
        window_generator<T> *generators[sizeof...(gens)] = { gens... };
    };
}

#endif
