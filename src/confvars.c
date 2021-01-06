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
#include <stdlib.h>

#include "aer/confman.h"
#include "aer/err.h"
#include "internal/confvars.h"
#include "internal/log.h"



/* ----- PRIVATE CONSTANTS ----- */

static const char * MOD_NAMES_KEY = "mods";



/* ----- INTERNAL GLOBALS ----- */

size_t confNumModNames = 0;

const char ** confModNames = NULL;



/* ----- PRIVATE FUNCTIONS ----- */

static void CheckErrors(const char * key) {
	switch (aererr) {
		case AER_OK:
			break;

		case AER_FAILED_LOOKUP:
			LogErr("Configuration key \"%s\" is undefined.", key);
			abort();
			break;

		case AER_FAILED_PARSE:
			LogErr("Could not parse configuration key \"%s\".", key);
			abort();
			break;

		default:
			LogErr(
					"Unknown error while trying to read configuration key \"%s\".",
					key
			);
			abort();
	}

	return;
}



/* ----- INTERNAL FUNCTIONS ----- */

void ConfVarsConstructor(void) {
	/* Mod names. */
	aererr = AER_OK;
	confNumModNames = AERConfManGetStrings(MOD_NAMES_KEY, 0, NULL);
	CheckErrors(MOD_NAMES_KEY);
	confModNames = malloc(confNumModNames * sizeof(const char *));
	assert(confModNames);
	AERConfManGetStrings(MOD_NAMES_KEY, confNumModNames, confModNames);
	CheckErrors(MOD_NAMES_KEY);

	return;
}

void ConfVarsDestructor(void) {
	/* Mod names. */
	free(confModNames);
	confModNames = NULL;
	confNumModNames = 0;

	return;
}
