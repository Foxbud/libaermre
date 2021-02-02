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
#ifndef INTERNAL_ERR_H
#define INTERNAL_ERR_H

#include "aer/err.h"
#include "internal/log.h"
#include "internal/mod.h"

/* ----- INTERNAL MACROS ----- */

#define Ensure(cond, err)                                                      \
    do {                                                                       \
        if (!(cond)) {                                                         \
            aererr = (err);                                                    \
            if (ModManHasContext()) {                                          \
                LogWarn("Potentially recoverable error \"%s\" occurred "       \
                        "during call to "                                      \
                        "function \"%s\" by mod \"%s\".",                      \
                        #err, __func__,                                        \
                        ModManGetMod(ModManPeekContext())->name);              \
            } else {                                                           \
                LogWarn("Potentially recoverable error \"%s\" occurred "       \
                        "during internal "                                     \
                        "call to function \"%s\".",                            \
                        #err, __func__);                                       \
            }                                                                  \
            return errRet;                                                     \
        }                                                                      \
    } while (0)

#define EnsureArg(arg) Ensure((arg), AER_NULL_ARG)

#define EnsureArgBuf(buf, size) Ensure(((buf) || (size) == 0), AER_NULL_ARG)

#define EnsureLookup(item) Ensure((item), AER_FAILED_LOOKUP)

#define EnsureRange(val, min, max)                                             \
    do {                                                                       \
        typeof(val) EnsureRange_val = (val);                                   \
        Ensure((EnsureRange_val >= (typeof(val))(min) &&                       \
                EnsureRange_val <= (typeof(val))(max)),                        \
               AER_BAD_VAL);                                                   \
    } while (0)

#define EnsureProba(val) EnsureRange((val), 0.0f, 1.0f)

#define EnsureStage(curStage) Ensure((stage >= (curStage)), AER_SEQ_BREAK)

#define EnsureStageStrict(curStage) Ensure((stage == (curStage)), AER_SEQ_BREAK)

#define EnsureStagePast(pastStage) Ensure((stage > (pastStage)), AER_SEQ_BREAK)

#define ErrIf(cond, err, ...)                                                  \
    do {                                                                       \
        if ((cond)) {                                                          \
            aererr = (err);                                                    \
            if (ModManHasContext()) {                                          \
                LogWarn("Potentially recoverable error \"%s\" occurred "       \
                        "during call to "                                      \
                        "function \"%s\" by mod \"%s\".",                      \
                        #err, __func__,                                        \
                        ModManGetMod(ModManPeekContext())->name);              \
            } else {                                                           \
                LogWarn("Potentially recoverable error \"%s\" occurred "       \
                        "during internal "                                     \
                        "call to function \"%s\".",                            \
                        #err, __func__);                                       \
            }                                                                  \
            return __VA_ARGS__;                                                \
        }                                                                      \
    } while (0)

#endif /* INTERNAL_ERR_H */
