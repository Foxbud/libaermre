#ifndef XORSHIFT_H
#define XORSHIFT_H

#include "utilmacs.h"



/* ----- PUBLIC MACROS ----- */

/* Parameters recommended by Marsaglia in "Xorshift RNGs." */

#define XS32_PARAM_A 13

#define XS32_PARAM_B 17

#define XS32_PARAM_C 5

/* 
 * Parameters recommended by Vigna in "An experimental exploration of
 * Marsaglia's xorshift generators, scrambled."
 */

#define XS64_PARAM_A 12

#define XS64_PARAM_B 25

#define XS64_PARAM_C 17

#define XS64_STAR_COEF 0x2545f4914f6cdd1d

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



#endif /* XORSHIFT_H */
