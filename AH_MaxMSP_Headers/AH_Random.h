
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


// The current implementation in use is the 1024 WELL RNG from http://www.iro.umontreal.ca/~panneton/WELLRNG.html
// The memory requirement is 33 unsigned 32 bit integers
//	Period length is 2^1024 - 1 which shold be more than adequate for most purposes
// The follwing copyright and authorship notice applies only to this section of the file (down till the next three line comment block)


/* ***************************************************************************** */
/* Copyright:      Francois Panneton and Pierre L'Ecuyer, University of Montreal */
/*                 Makoto Matsumoto, Hiroshima University                        */
/* Notice:         This code can be used freely for personal, academic,          */
/*                 or non-commercial purposes. For commercial purposes,          */
/*                 please contact P. L'Ecuyer at: lecuyer@iro.UMontreal.ca       */
/* ***************************************************************************** */

#define M1 3
#define M2 24
#define M3 10

#define MAT0POS(t,v) (v^(v>>t))
#define MAT0NEG(t,v) (v^(v<<(-(t))))
#define Identity(v) (v)

#define V0            STATE[state_i                   ]
#define VM1           STATE[(state_i+M1) & 0x0000001fU]
#define VM2           STATE[(state_i+M2) & 0x0000001fU]
#define VM3           STATE[(state_i+M3) & 0x0000001fU]
#define VRm1          STATE[(state_i+31) & 0x0000001fU]
#define newV0         STATE[(state_i+31) & 0x0000001fU]
#define newV1         STATE[state_i                   ]


typedef struct _rand_gen_
{
	unsigned int state_i;
	unsigned int STATE[32];

}	t_rand_gen;


void InitWELLRNG1024a (t_rand_gen *gen, unsigned int *init)
{
	unsigned int *STATE		= gen->STATE;
	int i;
	
	gen->state_i = 0;
	
	for (i = 0; i < 32; i++)
		STATE[i] = init[i];
}


static __inline unsigned int WELLRNG1024a (t_rand_gen *gen)
{
	unsigned int *STATE		= gen->STATE;
	unsigned int state_i	= gen->state_i;
	
	unsigned int z0    = VRm1;
	unsigned int z1    = Identity(V0)       ^ MAT0POS (8, VM1);
	unsigned int z2    = MAT0NEG (-19, VM2) ^ MAT0NEG(-14,VM3);
	
	newV1 = z1                 ^ z2; 
	newV0 = MAT0NEG (-11,z0)   ^ MAT0NEG(-7,z1)    ^ MAT0NEG(-13,z2) ;
	gen->state_i = state_i = (state_i + 31) & 0x0000001fU;
	
	return STATE[state_i];
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////// The following routines provide an interface with the basic 32 bit UInt RNG ///////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Seed the random number generator randomly using OS specific routines

void rand_seed (t_rand_gen *gen)
{
	unsigned int seed[32];
	long i;
	
#ifdef __APPLE__
	for (i = 0; i < 32; i++)
		seed[i] = arc4random();
#else
	HCRYPTPROV hProvider = 0;
	const DWORD dwLength = 128;
	BYTE *pbBuffer = (BYTE *) seed;

	if (!CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
		return;
		 
	
		 
	CryptGenRandom(hProvider, dwLength, pbBuffer);
	CryptReleaseContext(hProvider, 0);
#endif
	
	InitWELLRNG1024a (gen, seed);// utime.lo);
}


// Generate a single pseudo-random unsigned integer

static __inline unsigned int rand_int (t_rand_gen *gen)
{	
	return WELLRNG1024a(gen);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// The below routines are RNG Agnostic /////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define CONVERT_DOUBLE 2.32830643653869628906e-10

// return an unsigned 32 bit integer

static __inline unsigned int rand_int_n (t_rand_gen *gen, unsigned int n)
{	
	unsigned int used = n;
	unsigned int  i;
	
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


// return an signed 32 bit integer in the range [lo, hi]

static __inline int rand_int_range (t_rand_gen *gen, int lo, int hi)
{	
	return lo + rand_int_n(gen, hi - lo);
}


// return a 32 bit random double in the range [0,1]

static __inline double rand_double (t_rand_gen *gen)
{
	return rand_int(gen) * CONVERT_DOUBLE;
}


// return a 32 bit random double in the range [0, n]

static __inline double rand_double_n (t_rand_gen *gen, double n)
{	
	return rand_double(gen) * n;
}


// return a 32 bit random double in the range [lo, hi]

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


// return a gaussian distribution double with given mean and dev

static __inline double rand_gauss (t_rand_gen *gen, double mean, double dev)
{
	double x, y;
	double R = rand_gauss_basic (gen, &x, &y);
		
	return (R * x) * dev + mean; 
}


// return a gaussian distribution double with given mean and dev and in the range [0, 1]

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
