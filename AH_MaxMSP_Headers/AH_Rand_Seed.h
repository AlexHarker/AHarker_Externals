
/*
 *  AH_Rand Seed
 *
 *	A few externals use the rand() function as a PRNG - this may change later, but in the meantime this header provides OS specific ways of seeding the algorithm
 *
 *  Copyright 2011 Alex Harker. All rights reserved.
 *
 */


#ifndef _AH_RAND_SEED_
#define _AH_RAND_SEED_


// Seed the random number generator randomly using OS specific routines

static __inline unsigned int get_rand_seed ()
{
	unsigned int seed;
	
#ifdef __APPLE__
	seed = arc4random();
#else
	HCRYPTPROV hProvider = 0;
	const DWORD dwLength = 4;
	BYTE *pbBuffer = (BYTE *) &seed;

	if (!CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
		return 0;

	CryptGenRandom(hProvider, dwLength, pbBuffer);
	CryptReleaseContext(hProvider, 0);
#endif
	
	return seed;
}


#endif		/* _AH_RAND_SEED */
