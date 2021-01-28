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

/* ----- PRIVATE FUNCTIONS ----- */

static void CheckErrors(const char *key) {
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
        LogErr("Unknown error while trying to read configuration key \"%s\".",
               key);
        abort();
    }

    return;
}

/* ----- INTERNAL FUNCTIONS ----- */

void OptionConstructor(void) {
    LogInfo("Initializing options...");

    /* Mod names. */
    aererr = AER_OK;
    opts.numModNames = AERConfGetStrings("mods", 0, NULL);
    CheckErrors("mods");
    opts.modNames = malloc(opts.numModNames * sizeof(const char *));
    assert(opts.modNames);
    AERConfGetStrings("mods", opts.numModNames, opts.modNames);
    CheckErrors("mods");

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
