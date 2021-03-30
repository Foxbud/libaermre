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

#include "cclosure.h"

#include "aer/iter.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/iter.h"

/* ----- INTERNAL FUNCTIONS ----- */

void *IterCreate(void *callback, void *env, void (*destructor)(void *env)) {
    assert(callback);
    assert(env);

    ((IterBaseEnv *)env)->destructor = destructor;
    void *iter = CClosureNew(callback, env, false);
    assert(iter);

    return iter;
}

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT void AERIterDestroy(void *iter) {
#define errRet
    Ensure(CClosureCheck(iter), AER_BAD_VAL);

    IterBaseEnv *env = CClosureFree(iter);
    env->destructor(env);

    Ok();
#undef errRet
}