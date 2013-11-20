
/*
 *  AH_MAX5
 *
 *	This header file is used to check the SDK version for Max 5 and above, and also to allow cross compilation of code taking advantage of some Max 5 features where available.
 *	Notably the object post and error routines in Max 5 can be used, whilst suitable alternatives are provided for Max 4.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#ifndef _AH_MAX5_
#define _AH_MAX5_ 

#include <ext.h>


// Routines for posting and errors in Max 4 that take the object pointer plus posting string

#define MAX4_ERROR														\
static __inline void AH_error (void *x, char *s, ...) {					\
	va_list ap;															\
	char temp2[2048];													\
	char temp[2048];													\
	va_start(ap, s);													\
	vsnprintf (temp2, 2048, s, ap);										\
	snprintf(temp, 2048, "%s: %s", ob_name((t_object *)x), temp2);		\
	error(temp);														\
	va_end(ap);															\
}

#define MAX4_POST													\
static __inline void AH_post (void *x, char *s, ...) {				\
va_list ap;															\
char temp2[2048];													\
char temp[2048];													\
va_start(ap, s);													\
vsnprintf (temp2, 2048, s, ap);										\
snprintf(temp, 2048, "%s: %s", ob_name((t_object *)x), temp2);		\
post(temp);															\
va_end(ap);															\
}


// Test the version of the SDK

#ifndef _MAX5_TEST_
#define _MAX5_TEST_ 

#ifdef C74_MAX_SDK_VERSION 
#if (C74_MAX_SDK_VERSION >= 0x0500)


// The version of the SDK is 5 or greater

#ifndef MAX5
#define MAX5
#endif	

// Max 5 versions of these routines

static __inline void AH_error (void *x, char *s, ...) {
	char temp[2048];
	va_list ap;
	va_start(ap, s);
	
	vsnprintf (temp, 2048, s, ap);
	object_error(x, temp);
	va_end(ap);
}

static __inline void AH_post (void *x, char *s, ...) {
	char temp[2048];
	va_list ap;
	va_start(ap, s);
	
	vsnprintf (temp, 2048, s, ap);
	object_post(x, temp);
	va_end(ap);
}

// The SDK version is less than 5

#else

// Class attibutes cannot be saved, so we use a macro to prevent this line compiling

#ifndef CLASS_ATTR_SAVE
#define CLASS_ATTR_SAVE(a,b,c)
#endif

// Compile Max 4 routines

MAX4_ERROR
MAX4_POST	

#endif		/* (C74_MAX_SDK_VERSION >= 0x0500) */
#else		/* C74_MAX_SDK_VERSION */

// The SDK Version is undefined

// Class attibutes cannot be saved, so we use a macro to prevent this line compiling

#ifndef CLASS_ATTR_SAVE
#define CLASS_ATTR_SAVE(a,b,c)
#endif

// Compile Max 4 routines

MAX4_ERROR
MAX4_POST	

#endif		/* C74_MAX_SDK_VERSION */
#endif		/* _MAX5_TEST_ */
#endif		/* _AH_MAX5_ */