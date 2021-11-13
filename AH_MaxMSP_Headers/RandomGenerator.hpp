
#ifndef _RANDOMGENERATOR_HPP_
#define _RANDOMGENERATOR_HPP_

#include <cstdint>

namespace random_generators
{
    // Basic CMWC Generator

    class cmwc
    {
        static const uint32_t CMWC_LAG_SIZE = 32;
        static const uint64_t CMWC_A_VALUE = 987655670LL;
        
    public:
        
        inline uint32_t operator()()
        {
            uint32_t i = m_increment;
            uint32_t c = m_carry;
            uint32_t x;
            
            uint64_t t;
            
            i = (i + 1) & (CMWC_LAG_SIZE - 1);
            t = (uint64_t) CMWC_A_VALUE * m_state[i] + c;
            c = (t >> 32);
            x = (uint32_t) ((t + c) & 0xFFFFFFFF);
            
            if (x < c)
            {
                x++;
                c++;
            }
            
            m_state[i] = (0xFFFFFFFE - x);
            m_increment = i;
            m_carry = c;
            
            return m_state[i];
        }
        
        // Seeding (specific / OS-specific random values)
        
        void seed(uint32_t *init)
        {
            m_increment = (CMWC_LAG_SIZE - 1);
            m_carry = 123;
            
            for (uint32_t i = 0; i < CMWC_LAG_SIZE; i++)
                m_state[i] = init[i];
        }
        
        void rand_seed()
        {
            uint32_t seeds[CMWC_LAG_SIZE];
            
        #ifdef __APPLE__
            for (uint32_t i = 0; i < CMWC_LAG_SIZE; i++)
                seeds[i] = arc4random();
        #elif defined (__linux__)
            srandom(time(nullptr));
            for (uint32_t i = 0; i < CMWC_LAG_SIZE; i++)
                seeds[i] = random();
        #else
            HCRYPTPROV hProvider = 0;
            const DWORD dwLength = 4 * CMWC_LAG_SIZE;
            BYTE *pbBuffer = (BYTE *) seeds;
            
            if (!CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
                return;
            
            CryptGenRandom(hProvider, dwLength, pbBuffer);
            CryptReleaseContext(hProvider, 0);
        #endif
            seed(seeds);
        }
        
        // State
        
        uint32_t m_increment;
        uint32_t m_carry;
        uint32_t m_state[CMWC_LAG_SIZE];
    };
}

template <typename Generator = random_generators::cmwc>
class random_generator
{
public:
    
    random_generator()                  { m_generator.rand_seed(); }
    random_generator(uint32_t *init)    { m_generator.seed(init); }

    // Seeding (specific / random values)
    
    void seed(uint32_t *init)   { m_generator,seed(init); }
    void rand_seed()            { m_generator.rand_seed(); }
    
    // Generate a Single Pseudo-random Unsigned Integer (full range /  in the range [0, n] / in the range [lo, hi])
    
    uint32_t rand_int()
    {
        return m_generator();
    }
    
    uint32_t rand_int(uint32_t n)
    {
        uint32_t used = n;
        uint32_t i;
        
        used |= used >> 1;
        used |= used >> 2;
        used |= used >> 4;
        used |= used >> 8;
        used |= used >> 16;
        
        do
            i = rand_int() & used;   // toss unused bits shortens search
        while (i > n);
        
        return i;
    }
    
    int32_t rand_int(int32_t lo, int32_t hi)
    {
        return lo + rand_int(hi - lo);
    }
    
    // Generate a 32 bit Random Double (in the range [0,1] / in the range [0, n] / in the range [lo, hi])
    
    double rand_double()                        { return rand_int() * 2.32830643653869628906e-10; }
    double rand_double(double n)                { return rand_double() * n; }
    double rand_double(double lo, double hi)    { return lo + rand_double() * (hi - lo); }

    // Generate a 32 bit Random Double of Gaussian Distribution with given Mean / Deviation
    
    double rand_gaussian(double mean, double dev)
    {
        double x, y, R;
        
        rand_gaussians(x, y, R);
        
        return (R * x) * dev + mean;
    }
    
    // Generate two independent gaussians (Mean 0 and Deviation 1)
    
    void rand_gaussians(double& x, double& y)
    {
        double R;
        
        rand_gaussians(x, y, R);
        
        x *= R;
        y *= R;
    }
    
private:

    // Gaussian Helper
    
    void rand_gaussians(double& x, double& y, double& R)
    {
        x = 0.0;
        y = 0.0;
        R = 0.0;
        
        while (R >= 1.0 || R == 0.0)
        {
            x = rand_double(-1.0, 1.0);
            y = rand_double(-1.0, 1.0);
            R = (x * x) + (y * y);
        }
        
        R = sqrt((-2.0 * log(R)) / R);
    }
    
    // State
    
    Generator m_generator;
};

#endif
