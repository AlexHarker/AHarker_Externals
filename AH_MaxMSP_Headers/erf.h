
/*
 * Copied from OpenBSD project (src/lib/libm/src/s_erf.c)
 * Specialized for 32-bit little endian architectures.
 */
/* Real math libraries provide erf(), CUDA also provides an implementation. */

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

double erf(double x);
double erfc(double x);