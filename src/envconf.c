/**
 * @copyright 2020 the libaermre authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "foxutils/arraymacs.h"
#include "foxutils/math.h"
#include "foxutils/stringmapmacs.h"

#include "aer/envconf.h"
#include "internal/envconf.h"
#include "internal/err.h"
#include "internal/export.h"



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

static bool StringToBool(const char * str, bool * success) {
	bool result = false;

	switch (str[0]) {
		case '0':
		case 'f':
		case 'F':
		case 'n':
		case 'N':
			result = false;
			if (success) *success = true;
			break;

		case '1':
		case 't':
		case 'T':
		case 'y':
		case 'Y':
			result = true;
			if (success) *success = true;
			break;

		default:
			if (success) *success = false;
	}

	return result;
}

static int32_t StringToInt(const char * str, bool * success) {
	int32_t result = 0;
	int32_t numParsed = sscanf(str, "%i", &result);
	if (success) *success = (numParsed > 0);

	return result;
}

static float StringToFloat(const char * str, bool * success) {
	float result = 0;
	int32_t numParsed = sscanf(str, "%f", &result);
	if (success) *success = (numParsed > 0);

	return result;
}

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

AER_EXPORT bool AEREnvConfGetBool(const char * name) {
	ErrIf(!name, AER_NULL_ARG, false);

	CacheEntry * entry = GetCacheEntry(name);
	ErrIf(!entry, AER_FAILED_LOOKUP, false);

	bool success;
	bool result = StringToBool(
			*FoxArrayMIndex(const char *, &entry->tokens, 0),
			&success
	);
	ErrIf(!success, AER_FAILED_PARSE, false);

	return result;
}

AER_EXPORT size_t AEREnvConfGetBools(
		const char * name,
		size_t bufSize,
		bool * boolBuf
) {
	ErrIf(!name, AER_NULL_ARG, 0);
	ErrIf(!boolBuf && bufSize > 0, AER_NULL_ARG, 0);

	CacheEntry * entry = GetCacheEntry(name);
	ErrIf(!entry, AER_FAILED_LOOKUP, 0);

	size_t numToks = FoxArrayMSize(const char *, &entry->tokens);
	size_t numToWrite = FoxMin(bufSize, numToks);
	for (unsigned int idx = 0; idx < numToWrite; idx++) {
		bool success;
		boolBuf[idx] = StringToBool(
				*FoxArrayMIndex(const char *, &entry->tokens, idx),
				&success
		);
		ErrIf(!success, AER_FAILED_PARSE, 0);
	}

	return numToks;
}

AER_EXPORT int32_t AEREnvConfGetInt(const char * name) {
	ErrIf(!name, AER_NULL_ARG, 0);

	CacheEntry * entry = GetCacheEntry(name);
	ErrIf(!entry, AER_FAILED_LOOKUP, 0);

	bool success;
	int32_t result = StringToInt(
			*FoxArrayMIndex(const char *, &entry->tokens, 0),
			&success
	);
	ErrIf(!success, AER_FAILED_PARSE, 0);

	return result;
}

AER_EXPORT size_t AEREnvConfGetInts(
		const char * name,
		size_t bufSize,
		int32_t * intBuf
) {
	ErrIf(!name, AER_NULL_ARG, 0);
	ErrIf(!intBuf && bufSize > 0, AER_NULL_ARG, 0);

	CacheEntry * entry = GetCacheEntry(name);
	ErrIf(!entry, AER_FAILED_LOOKUP, 0);

	size_t numToks = FoxArrayMSize(const char *, &entry->tokens);
	size_t numToWrite = FoxMin(bufSize, numToks);
	for (unsigned int idx = 0; idx < numToWrite; idx++) {
		bool success;
		intBuf[idx] = StringToInt(
				*FoxArrayMIndex(const char *, &entry->tokens, idx),
				&success
		);
		ErrIf(!success, AER_FAILED_PARSE, 0);
	}

	return numToks;
}

AER_EXPORT float AEREnvConfGetFloat(const char * name) {
	ErrIf(!name, AER_NULL_ARG, 0.0f);

	CacheEntry * entry = GetCacheEntry(name);
	ErrIf(!entry, AER_FAILED_LOOKUP, 0.0f);

	bool success;
	float result = StringToFloat(
			*FoxArrayMIndex(const char *, &entry->tokens, 0),
			&success
	);
	ErrIf(!success, AER_FAILED_PARSE, 0.0f);

	return result;
}

AER_EXPORT size_t AEREnvConfGetFloats(
		const char * name,
		size_t bufSize,
		float * floatBuf
) {
	ErrIf(!name, AER_NULL_ARG, 0);
	ErrIf(!floatBuf && bufSize > 0, AER_NULL_ARG, 0);

	CacheEntry * entry = GetCacheEntry(name);
	ErrIf(!entry, AER_FAILED_LOOKUP, 0);

	size_t numToks = FoxArrayMSize(const char *, &entry->tokens);
	size_t numToWrite = FoxMin(bufSize, numToks);
	for (unsigned int idx = 0; idx < numToWrite; idx++) {
		bool success;
		floatBuf[idx] = StringToFloat(
				*FoxArrayMIndex(const char *, &entry->tokens, idx),
				&success
		);
		ErrIf(!success, AER_FAILED_PARSE, 0);
	}

	return numToks;
}

AER_EXPORT const char * AEREnvConfGetString(const char * name) {
	ErrIf(!name, AER_NULL_ARG, NULL);

	CacheEntry * entry = GetCacheEntry(name);
	ErrIf(!entry, AER_FAILED_LOOKUP, NULL);

	return entry->origStr;
}

AER_EXPORT size_t AEREnvConfGetStrings(
		const char * name,
		size_t bufSize,
		const char ** strBuf
) {
	ErrIf(!name, AER_NULL_ARG, 0);
	ErrIf(!strBuf && bufSize > 0, AER_NULL_ARG, 0);

	CacheEntry * entry = GetCacheEntry(name);
	ErrIf(!entry, AER_FAILED_LOOKUP, 0);

	size_t numToks = FoxArrayMSize(const char *, &entry->tokens);
	size_t numToWrite = FoxMin(bufSize, numToks);
	for (unsigned int idx = 0; idx < numToWrite; idx++) {
		strBuf[idx] = *FoxArrayMIndex(const char *, &entry->tokens, idx);
	}

	return numToks;
}
