
/*
 *  AH_Random
 *
 *	This header file offers high quality pseduo-random number generator facilities for integer and double precision numbers (the latter also in gaussian and windowed gaussian distribution).
 *	A t_rand_gen struct should be declared, usually in within the structure of each MaxMSP object and routines are called with a pointer to this structure.
 *	It is not thread-safe to call a random number generator from more than one thread. When using mulitple threads, each should have its own generator.
 *
 *	The design of this file is such that it should be easy to replace the basic RNG without touching most of the code with which the user interfaces.
 *
 *  Copyright 2010 Alex Harker (except where noted otherwise). All rights reserved.
 *
 */

#ifndef _AH_RANDOM_
#define _AH_RANDOM_

#include <AH_Types.h>

// Include lower tail quantile for standard normal distribution function, adapted from http://home.online.no/~pjacklam/notes/invnorm/impl/sprouse/ltqnorm.c
// This facilities faster generation of windowed gaussian numbers, which are slow for large deviations when using rejection sampling

#include <AH_LTQ_Norm.h>

#ifndef _APPLE_
#include <erf.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// The following routines provide a basic 32 bit UInt RNG /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// This random number generator may be any pseudorandom number generator with the following properties
//
//	1 - generates 32 bit unsigned integers - all bits "random"
//	2 - high quality, long period length
//	3 - small state memory usage (for thread safety each object stores its own state - and the object size in MaxMSP is not unlimited)
//
//	The RNG state is stored in a struct t_rand_gen, which *MUST* be defined here


// The current implementation in use is a complementary modulo with carry algorithm (proposed by George Marsaglia)
// Details can be found in Marsaglia, G. (2003). "Random number generators". Journal of Modern Applied Statistical Methods 2
// See - http://digitalcommons.wayne.edu/cgi/viewcontent.cgi?article=1725&context=jmasm)

// The memory requirement is currently 34 unsigned 32 bit integers (can be altered using CMWC_LAG_SIZE
// The period length is currently circa 2^1054 - 1 which shold be more than adequate for most purposes

// N.B. CMWC_LAG_SIZE must be a power of two
// N.B. CMWC_A_VALUE should be a suitable value according to CMWC_LAG_SIZE


#define CMWC_LAG_SIZE 32U
#define CMWC_A_VALUE 987655670ULL


typedef struct _rand_gen_
{
    AH_UInt32 increment;
	AH_UInt32 carry;
	AH_UInt32 STATE[CMWC_LAG_SIZE];

}	t_rand_gen;


void initCMWC (t_rand_gen *gen, AH_UInt32 *init)
{
	gen->increment = (CMWC_LAG_SIZE - 1);
    gen->carry = 123;
	
	for (long i = 0; i < CMWC_LAG_SIZE; i++)
		gen->STATE[i] = init[i];
}


static __inline AH_UInt32 CMWC (t_rand_gen *gen)
{
    AH_UInt32 i = gen->increment;
    AH_UInt32 c = gen->carry;
    AH_UInt32 x;

    AH_UInt64 t;
    
    i = (i + 1) & (CMWC_LAG_SIZE - 1);
    t = (AH_UInt64) CMWC_A_VALUE * gen->STATE[i] + c;
    c = (t >> 32);
    x = t + c;
    
    if (x < c)
    {
        x++;
        c++;
    }
    
    gen->STATE[i] = (0xFFFFFFFE - x);
    gen->increment = i;
    gen->carry = c;
    
	return gen->STATE[i];
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////// The following routines provide an interface with the basic 32 bit UInt RNG ///////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Seed the random number generator randomly using OS specific routines

void rand_seed (t_rand_gen *gen)
{
	AH_UInt32 seed[CMWC_LAG_SIZE];
	
#ifdef __APPLE__
	for (long i = 0; i < CMWC_LAG_SIZE; i++)
		seed[i] = arc4random();
#else
	HCRYPTPROV hProvider = 0;
	const DWORD dwLength = 4 * CMWC_LAG_SIZE;
	BYTE *pbBuffer = (BYTE *) seed;

	if (!CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
		return;
		  
	CryptGenRandom(hProvider, dwLength, pbBuffer);
	CryptReleaseContext(hProvider, 0);
#endif
	
	initCMWC (gen, seed);
}


// Generate a single pseudo-random unsigned integer

static __inline AH_UInt32 rand_int (t_rand_gen *gen)
{	
	return CMWC(gen);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////// The following routine provides an interface to retrieve a single using OS provided routines (for seeding etc.) /////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static __inline AH_UInt32 rand_int_os()
{
    AH_UInt32 rand;
    
#ifdef __APPLE__
    rand = arc4random();
#else
    HCRYPTPROV hProvider = 0;
    const DWORD dwLength = 4;
    BYTE *pbBuffer = (BYTE *) &rand;
    
    if (!CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
        return 0;
    
    CryptGenRandom(hProvider, dwLength, pbBuffer);
    CryptReleaseContext(hProvider, 0);
#endif
    
    return rand;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// The below routines are RNG Agnostic /////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define CONVERT_DOUBLE 2.32830643653869628906e-10

// Return an unsigned 32 bit integer

static __inline AH_UInt32 rand_int_n (t_rand_gen *gen, AH_UInt32 n)
{	
	AH_UInt32 used = n;
	AH_UInt32 i;
	
	used |= used >> 1;
	used |= used >> 2;
	used |= used >> 4;
	used |= used >> 8;
	used |= used >> 16;
	
	do
		i = rand_int(gen) & used;			// toss unused bits shortens search
	while (i > n);
	
	return i;
}


// Return an signed 32 bit integer in the range [lo, hi]

static __inline AH_SInt32 rand_int_range (t_rand_gen *gen, AH_SInt32 lo, AH_SInt32 hi)
{	
	return lo + rand_int_n(gen, hi - lo);
}


// Return a 32 bit random double in the range [0,1]

static __inline double rand_double (t_rand_gen *gen)
{
	return rand_int(gen) * CONVERT_DOUBLE;
}


// Return a 32 bit random double in the range [0, n]

static __inline double rand_double_n (t_rand_gen *gen, double n)
{	
	return rand_double(gen) * n;
}


// Return a 32 bit random double in the range [lo, hi]

static __inline double rand_double_range (t_rand_gen *gen, double lo, double hi)
{	
	return lo + rand_double(gen) * (hi - lo); 
}


// This is for internal use only (unless you need two independent gaussian numbers at the same time...)

static __inline double rand_gauss_basic (t_rand_gen *gen, double *ret_x, double *ret_y)
{
	double x, y;
	double R = 0;
		
	while (R >= 1 || R == 0)
	{
		x = rand_double_range (gen, -1.0, 1.0);
		y = rand_double_range (gen, -1.0, 1.0);
		R = (x * x) + (y * y);
	}
		
	R = sqrt ((-2.0 * log(R)) / R);
			
	*ret_x = x;
	*ret_y = y;
	
	return R;
}


// Return a gaussian distribution double with given mean and dev

static __inline double rand_gauss (t_rand_gen *gen, double mean, double dev)
{
	double x, y;
	double R = rand_gauss_basic (gen, &x, &y);
		
	return (R * x) * dev + mean; 
}


// Return a gaussian distribution double with given mean and dev and in the range [0, 1]

static __inline double rand_windgauss_bm (t_rand_gen *gen, double mean, double dev)
{
	double x, y;
	double R = -1.;
	
	while (R < 0 || R > 1)
	{
		R = rand_gauss_basic (gen, &x, &y);
		R = (R * x) * dev + mean;
		
		if (R >= 0 && R <= 1)
			break;
		
		R = (R * y) * dev + mean;
	}

	return R; 
}


// N.B. This direct method is faster than the box muller for high values of dev, hence it is default

static __inline double rand_windgauss (t_rand_gen *gen, double mean, double dev)
{
	double dev_recip = 1. / (dev * sqrt(2.));
	double mmean_t_dev = -mean * dev_recip;
		   
	return ltqnorm(0.5 + 0.5 * rand_double_range(gen, erf(mmean_t_dev), erf(dev_recip + mmean_t_dev))) * dev + mean; 
}

#endif		/* _AH_RANDOM_ */
