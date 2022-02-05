
#include "sort.hpp"

#include <sstream>
#include <iostream>
#include <iomanip>

#include <random>
#include <vector>

#include <mach/mach.h>
#include <mach/mach_time.h>

// Output

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 4, bool fixed = true)
{
    std::ostringstream out;
    if (fixed)
        out << std::setprecision(n) << std::fixed << a_value;
    else
        out << std::setprecision(n) << a_value;
    
    return out.str();
}

void tabbedOut(const std::string& name, const std::string& text, int tab = 25)
{
    std::cout << std::setw(tab) << std::setfill(' ');
    std::cout.setf(std::ios::left);
    std::cout.unsetf(std::ios::right);
    std::cout << name;
    std::cout.unsetf(std::ios::left);
    std::cout << text << "\n";
}

// Timing

class Timer
{
public:
    
    Timer() : mStart(0), mStore1(0), mStore2(0) {}
    
    void start()
    {
        mStart = mach_absolute_time();
    };
    
    void stop(const std::string& msg)
    {
        uint64_t end = mach_absolute_time();
        
        mach_timebase_info_data_t info;
        mach_timebase_info(&info);
        
        uint64_t elapsed = ((end - mStart) * info.numer) / info.denom;
        //tabbedOut(msg + " Elapsed ", to_string_with_precision(elapsed / 1000000.0, 2), 35);
        
        mStore2 = mStore1;
        mStore1 = elapsed;
        mStart = mach_absolute_time();
    };
    
    void relative(const std::string& msg)
    {
        tabbedOut(msg + " Comparison ", to_string_with_precision(((double) mStore1 / (double) mStore2), 2), 35);
    }
    
    double reportA()
    {
        return mStore1;
    }
    
    double reportB()
    {
        return mStore2;
    }
    
private:
    
    uint64_t        mStart;
    uint64_t        mStore1;
    uint64_t        mStore2;
};


struct result
{
    result() {}
    result(long index, double distance) : m_index(index), m_distance(distance) {}
    
    friend bool operator < (const result& a, const result& b) { return a.m_distance < b.m_distance; }
    friend bool operator > (const result& a, const result& b) { return a.m_distance > b.m_distance; }

    long m_index;
    double m_distance;
};

std::mt19937 gen32;
//std::default_random_engine engine(std::random_device());

result random_result() { return { gen32(), gen32() / (double) gen32.max() }; }


std::pair<double, double> test_speed(long num_matches, long size)
{
    Timer t;
    
    std::vector<result> results1(size);
    
    for (long i = 0; i < size; i++)
        results1.push_back(random_result());
    
    std::vector<result> results2 = results1;

    num_matches = std::min(num_matches, size);

    t.start();
    
    //std::sort(results2.begin(), results2.end());
    sort(results2, size);
    t.stop(std::string("sort ") + std::to_string(num_matches) + " " + std::to_string(size));
    
    for (long i = 0; i < num_matches; i++)
        std::swap(results1[i], (*std::min_element(results1.begin() + i, results1.begin() + size)));
    t.stop(std::string("insert ") + std::to_string(num_matches) + " " + std::to_string(size));
    
    //t.relative(std::string("compare ") + std::to_string(num_matches) + " " + std::to_string(size));
    return { t.reportA(), t.reportB() };
}

double multiple_tests(long num_matches, long size, long num_tests)
{
    double A = 0.0;
    double B = 0.0;
    
    for (long i = 0; i < num_tests; i++)
    {
        auto r = test_speed(num_matches, size);
        A += r.first;
        B += r.second;
    }
    
    std::string msg(std::string("compare ") + std::to_string(num_matches) + " " + std::to_string(size));
    tabbedOut(msg + " Comparison ", to_string_with_precision(A/B, 2), 35);
    
    return A/B;
}

int main(int argc, const char * argv[]) {
    
    std::cout << "Hello, World!\n";
    
    // if matches < 10
    // or if matches < (log10(full_matches) * 8)
    for (long i = 400000; i < 400001; i += 1)
    {
        long k = 0;
        
        for (long j = 1 ; j < i && k < 7; j += 1)
        {
            if (multiple_tests(j, i, 20) > 1.0)
                k++;
        }
    }
    
    return 0;
}


