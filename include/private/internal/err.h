#ifndef INTERNAL_ERR_H
#define INTERNAL_ERR_H

#include "foxutils/arraymacs.h"

#include "aer/err.h"
#include "internal/log.h"
#include "internal/modman.h"



/* ----- INTERNAL MACROS ----- */

#define ErrIf(cond, err, ...) \
	do { \
		if ((cond)) { \
			aererr = (err); \
			if (FoxArrayMEmpty(Mod *, &modman.context)) { \
				LogWarn( \
						"Potentially recoverable error \"%s\" occurred during internal " \
						"call to function \"%s.\"", \
						#err, \
						__func__ \
				); \
			} else { \
				LogWarn( \
						"Potentially recoverable error \"%s\" occurred during call to " \
						"function \"%s\" by mod \"%s.\"", \
						#err, \
						__func__, \
						(*FoxArrayMPeek(Mod *, &modman.context))->name \
				); \
			} \
			return __VA_ARGS__; \
		} \
	} while (0)



#endif /* INTERNAL_ERR_H */
