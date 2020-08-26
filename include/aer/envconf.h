/**
 * @file
 */
#ifndef AER_ENVCONF_H
#define AER_ENVCONF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



/* ----- PUBLIC FUNCTIONS ----- */

bool AEREnvConfGetBool(const char * name);

size_t AEREnvConfGetBools(
		const char * name,
		size_t bufSize,
		bool * boolBuf
);

int32_t AEREnvConfGetInt(const char * name);

size_t AEREnvConfGetInts(
		const char * name,
		size_t bufSize,
		int32_t * intBuf
);

float AEREnvConfGetFloat(const char * name);

size_t AEREnvConfGetFloats(
		const char * name,
		size_t bufSize,
		float * floatBuf
);

const char * AEREnvConfGetString(const char * name);

size_t AEREnvConfGetStrings(
		const char * name,
		size_t bufSize,
		const char ** strBuf
);



#endif /* AER_ENVCONF_H */
