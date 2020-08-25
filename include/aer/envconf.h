/**
 * @file
 */
#ifndef AER_ENVCONF_H
#define AER_ENVCONF_H

#include <stddef.h>



/* ----- PUBLIC FUNCTIONS ----- */

const char * AEREnvConfGetString(const char * name);

size_t AEREnvConfGetStrings(
		const char * name,
		size_t bufSize,
		const char ** strBuf
);



#endif /* AER_ENVCONF_H */
