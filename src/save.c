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
#include <stdio.h>

#include "foxutils/math.h"

#include "aer/save.h"
#include "internal/core.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/hld.h"
#include "internal/mod.h"

/* ----- PRIVATE CONSTANTS ----- */

static const char *ABS_KEY_FMT = "AER%s%s";

/* ----- PRIVATE GLOBALS ----- */

static char keyBuf[1024];

/* ----- PRIVATE FUNCTIONS ----- */

static size_t GetAbsKey(const char *relKey) {
    assert(ModManHasContext());

    return FoxMin(sizeof(keyBuf),
                  snprintf(keyBuf, sizeof(keyBuf), ABS_KEY_FMT,
                           ModManGetMod(ModManPeekContext())->name, relKey) +
                      1);
}

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT double AERSaveReadValue(const char *key) {
#define errRet 0.0
    EnsureStage(STAGE_ACTION);
    EnsureArg(key);

    HLDPrimitiveMakeString(argKey, keyBuf, GetAbsKey(key));
    HLDPrimitive result = HLDScriptCall(hldfuncs.gmlScriptValueCheck, &argKey);
    EnsureLookup(result.type == HLD_PRIMITIVE_REAL);

    return result.value.r;
#undef errRet
}

AER_EXPORT void AERSaveWriteValue(const char *key, double value) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(key);

    HLDPrimitiveMakeString(argKey, keyBuf, GetAbsKey(key));
    HLDPrimitiveMakeReal(argValue, value);
    HLDScriptCall(hldfuncs.gmlScriptValueRecord, &argKey, &argValue);

    return;
#undef errRet
}