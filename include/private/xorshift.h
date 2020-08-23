#ifndef PRIVATE_XORSHIFT_H
#define PRIVATE_XORSHIFT_H

#include "private/utilmacs.h"



/* ----- PUBLIC MACROS ----- */

/* Parameters recommended by Marsaglia in "Xorshift RNGs." */

#define XS32_PARAM_A 13u

#define XS32_PARAM_B 17u

#define XS32_PARAM_C 5u

/* 
 * Parameters recommended by Vigna in "An experimental exploration of
 * Marsaglia's xorshift generators, scrambled."
 */

#define XS64_PARAM_A 12u

#define XS64_PARAM_B 25u

#define XS64_PARAM_C 17u

#define XS64_STAR_COEF 0x2545f4914f6cdd1dul

#define XS32Round(state) \
	MacWrap( \
		(state) ^= (state) << XS32_PARAM_A; \
		(state) ^= (state) >> XS32_PARAM_B; \
		(state) ^= (state) << XS32_PARAM_C; \
	)

#define XS64Round(state) \
	MacWrap( \
		(state) ^= (state) << XS64_PARAM_A; \
		(state) ^= (state) >> XS64_PARAM_B; \
		(state) ^= (state) << XS64_PARAM_C; \
	)



#endif /* PRIVATE_XORSHIFT_H */
