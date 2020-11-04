#include <assert.h>
#include <stdlib.h>

#include "aer/envconf.h"
#include "aer/err.h"
#include "internal/confvars.h"
#include "internal/log.h"



/* ----- PRIVATE CONSTANTS ----- */

static const char * MOD_NAMES_VAR = "AER_MODS";



/* ----- INTERNAL GLOBALS ----- */

size_t confNumModNames = 0;

const char ** confModNames = NULL;



/* ----- PRIVATE FUNCTIONS ----- */

static void CheckErrors(const char * envVar) {
	switch (aererr) {
		case AER_OK:
			break;

		case AER_FAILED_LOOKUP:
			LogErr("Environment variable \"%s\" is undefined.", envVar);
			abort();
			break;

		case AER_FAILED_PARSE:
			LogErr("Could not parse environment variable \"%s.\"", envVar);
			abort();
			break;

		default:
			LogErr(
					"Unknown error while trying to read environment variable \"%s.\"",
					envVar
			);
			abort();
	}

	return;
}



/* ----- INTERNAL FUNCTIONS ----- */

void ConfVarsConstructor(void) {
	/* Mod names. */
	aererr = AER_OK;
	confNumModNames = AEREnvConfGetStrings(MOD_NAMES_VAR, 0, NULL);
	CheckErrors(MOD_NAMES_VAR);
	confModNames = malloc(confNumModNames * sizeof(const char *));
	assert(confModNames);
	AEREnvConfGetStrings(MOD_NAMES_VAR, confNumModNames, confModNames);
	CheckErrors(MOD_NAMES_VAR);

	return;
}

void ConfVarsDestructor(void) {
	/* Mod names. */
	free(confModNames);
	confModNames = NULL;
	confNumModNames = 0;

	return;
}
