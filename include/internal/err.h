#ifndef INTERNAL_ERR_H
#define INTERNAL_ERR_H

#include "aer/err.h"



/* ----- INTERNAL MACROS ----- */

#define ErrIf(cond, err, ...) \
	do { \
		if ((cond)) { \
			aererr = (err); \
			return __VA_ARGS__; \
		} \
	} while (0)



#endif /* INTERNAL_ERR_H */
