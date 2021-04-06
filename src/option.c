/**
 * @copyright 2021 the libaermre authors
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

#include "aer/conf.h"
#include "aer/err.h"
#include "internal/log.h"
#include "internal/option.h"

/* ----- INTERNAL GLOBALS ----- */

Options opts = {0};

/* ----- INTERNAL FUNCTIONS ----- */

void OptionConstructor(void) {
    LogInfo("Initializing options...");
    const char* key = NULL;

    /* Required keys. */

    key = "mods";
    aererr = AER_TRY;
    opts.numModNames = AERConfGetStrings(key, 0, NULL);
    opts.modNames = malloc(opts.numModNames * sizeof(const char*));
    assert(opts.modNames);
    aererr = AER_TRY;
    AERConfGetStrings(key, opts.numModNames, opts.modNames);
    switch (aererr) {
        case AER_OK:
            LogInfo("Found required configuration key \"%s\".", key);
            break;
        case AER_FAILED_PARSE:
            LogErr(
                "Required configuration key \"%s\" must be an array of "
                "strings!",
                key);
            abort();
        default:
            LogErr("Required configuration key \"%s\" is undefined!", key);
            abort();
    }

    /* Optional keys. */

    key = "error.promote_unhandled";
    aererr = AER_TRY;
    opts.promoteUnhandledErrors = AERConfGetBool(key);
    switch (aererr) {
        case AER_OK:
            LogInfo(
                "Found optional configuration key \"%s\" with value \"%i\".",
                key, opts.promoteUnhandledErrors);
            break;
        case AER_FAILED_PARSE:
            LogErr("Optional configuration key \"%s\" must be a boolean!", key);
            abort();
        default:
            LogInfo(
                "Optional configuration key \"%s\" is undefined. Using default "
                "value \"%i\".",
                key, (opts.promoteUnhandledErrors = false));
    }

    LogInfo("Done initializing options.");
    return;
}

void OptionDestructor(void) {
    LogInfo("Deinitializing options...");

    /* Mod names. */
    free(opts.modNames);
    opts = (Options){0};

    LogInfo("Done deinitializing options.");
    return;
}
