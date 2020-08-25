#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "foxutils/arraymacs.h"
#include "foxutils/math.h"
#include "foxutils/stringmapmacs.h"

#include "aer/envconf.h"
#include "internal/envconf.h"
#include "internal/err.h"



/* ----- PRIVATE TYPES ----- */

typedef struct CacheEntry {
	char * origStr;
	char * tokStr;
	FoxArray tokens;
} CacheEntry;



/* ----- PRIVATE CONSTANTS ----- */

static const char * TOKEN_DELIMS = " \t\n\r";



/* ----- PRIVATE GLOBALS ----- */

static FoxMap cache = {0};



/* ----- PRIVATE FUNCTIONS ----- */

static void CacheEntryInit(
		CacheEntry * entry,
		const char * envVarStr
) {
	/* Initialize strings. */
	size_t strSize = strlen(envVarStr) + 1;
	/*
	 * origStr and tokStr could technically be part of the same allocation
	 * to save a little bit of time, but I felt that hurts readablity too
	 * much. Regardless, these functions are only meant to be called during
	 * initialization.
	 */
	char * origStr = malloc(strSize);
	char * tokStr = malloc(strSize);
	assert(origStr);
	assert(tokStr);
	entry->origStr = memcpy(origStr, envVarStr, strSize);
	entry->tokStr = memcpy(tokStr, envVarStr, strSize);

	/* Tokenize string. */
	FoxArrayMInit(const char *, &entry->tokens);
	const char * tok = strtok(tokStr, TOKEN_DELIMS);
	while (tok) {
		*FoxArrayMPush(const char *, &entry->tokens) = tok;
		tok = strtok(NULL, TOKEN_DELIMS);
	}

	return;
}

static bool CacheEntryDeinitCallback(
		CacheEntry * entry,
		void * ctx
) {
	(void)ctx;

	FoxArrayMDeinit(const char *, &entry->tokens);
	free(entry->origStr);
	entry->origStr = NULL;
	free(entry->tokStr);
	entry->tokStr = NULL;

	return true;
}

static CacheEntry * GetCacheEntry(const char * name) {
	/* Check if entry in cache. */
	CacheEntry * result = FoxMapMIndex(const char *, CacheEntry, &cache, name);

	/* Create new cache entry. */
	if (!result) {
		const char * rawVar = getenv(name);
		if (rawVar) {
			result = FoxMapMInsert(const char *, CacheEntry, &cache, name);
			CacheEntryInit(result, rawVar);
		}
	}

	return result;
}



/* ----- INTERNAL FUNCTIONS ----- */

void EnvConfConstructor(void) {
	FoxStringMapMInit(CacheEntry, &cache);

	return;
}

void EnvConfDestructor(void) {
	FoxMapMForEachElement(
			const char *,
			CacheEntry,
			&cache,
			CacheEntryDeinitCallback,
			NULL
	);
	FoxMapMDeinit(const char *, CacheEntry, &cache);

	return;
}



/* ----- PUBLIC FUNCTIONS ----- */

const char * AEREnvConfGetString(const char * name) {
	ErrIf(!name, AER_NULL_ARG, NULL);

	CacheEntry * entry = GetCacheEntry(name);
	ErrIf(!entry, AER_FAILED_LOOKUP, NULL);

	return entry->origStr;
}

size_t AEREnvConfGetStrings(
		const char * name,
		size_t bufSize,
		const char ** strBuf
) {
	ErrIf(!name, AER_NULL_ARG, 0);
	ErrIf(!strBuf, AER_NULL_ARG, 0);

	CacheEntry * entry = GetCacheEntry(name);
	ErrIf(!entry, AER_FAILED_LOOKUP, 0);

	size_t numToks = FoxArrayMSize(const char *, &entry->tokens);
	size_t numToWrite = FoxMin(bufSize, numToks);
	for (unsigned int idx = 0; idx < numToWrite; idx++) {
		strBuf[idx] = *FoxArrayMIndex(const char *, &entry->tokens, idx);
	}

	return numToks;
}
