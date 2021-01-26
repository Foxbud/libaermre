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
#include <time.h>

#include "foxutils/rand.h"
#include "foxutils/xoshiro256ss.h"

#include "aer/rand.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/rand.h"

/* ----- PRIVATE GLOBALS ----- */

static FoxXoshiro256SS randPRNG = {0};

/* ----- INTERNAL FUNCTIONS ----- */

void RandConstructor(void) {
    LogInfo("Initializing random module...");

    FoxXoshiro256SSInit(&randPRNG, time(NULL));

    LogInfo("Done initializing random module.");
    return;
}

void RandDestructor(void) {
    LogInfo("Deinitializing random module...");

    FoxXoshiro256SSDeinit(&randPRNG);

    LogInfo("Done deinitializing random module.");
    return;
}

/* ------ PUBLIC FUNCTIONS ----- */

AER_EXPORT uint64_t AERRandUInt(void) {
    return FoxRandUInt((FoxPRNG *)&randPRNG);
}

AER_EXPORT uint64_t AERRandUIntRange(uint64_t min, uint64_t max) {
    ErrIf(min >= max, AER_BAD_VAL, 0);

    return FoxRandUIntRange((FoxPRNG *)&randPRNG, min, max);
}

AER_EXPORT int64_t AERRandInt(void) { return FoxRandInt((FoxPRNG *)&randPRNG); }

AER_EXPORT int64_t AERRandIntRange(int64_t min, int64_t max) {
    ErrIf(min >= max, AER_BAD_VAL, 0);

    return FoxRandIntRange((FoxPRNG *)&randPRNG, min, max);
}

AER_EXPORT float AERRandFloat(void) {
    return FoxRandFloat((FoxPRNG *)&randPRNG);
}

AER_EXPORT float AERRandFloatRange(float min, float max) {
    ErrIf(min >= max, AER_BAD_VAL, 0.0f);

    return FoxRandFloatRange((FoxPRNG *)&randPRNG, min, max);
}

AER_EXPORT double AERRandDouble(void) {
    return FoxRandDouble((FoxPRNG *)&randPRNG);
}

AER_EXPORT double AERRandDoubleRange(double min, double max) {
    ErrIf(min >= max, AER_BAD_VAL, 0.0);

    return FoxRandDoubleRange((FoxPRNG *)&randPRNG, min, max);
}

AER_EXPORT bool AERRandBool(void) { return FoxRandBool((FoxPRNG *)&randPRNG); }

AER_EXPORT AERRandGen *AERRandGenNew(uint64_t seed) {
    return FoxXoshiro256SSNew(seed);
}

AER_EXPORT void AERRandGenFree(AERRandGen *gen) {
    ErrIf(!gen, AER_NULL_ARG);

    FoxXoshiro256SSFree(gen);

    return;
}

AER_EXPORT void AERRandGenSeed(AERRandGen *gen, uint64_t seed) {
    ErrIf(!gen, AER_NULL_ARG);

    FoxXoshiro256SSSeed(gen, seed);

    return;
}

AER_EXPORT uint64_t AERRandGenUInt(AERRandGen *gen) {
    ErrIf(!gen, AER_NULL_ARG, 0);

    return FoxRandUInt((FoxPRNG *)&randPRNG);
}

AER_EXPORT uint64_t AERRandGenUIntRange(AERRandGen *gen, uint64_t min,
                                        uint64_t max) {
    ErrIf(!gen, AER_NULL_ARG, 0);
    ErrIf(min >= max, AER_BAD_VAL, 0);

    return FoxRandUIntRange((FoxPRNG *)&randPRNG, min, max);
}

AER_EXPORT int64_t AERRandGenInt(AERRandGen *gen) {
    ErrIf(!gen, AER_NULL_ARG, 0);

    return FoxRandFloat((FoxPRNG *)&randPRNG);
}

AER_EXPORT int64_t AERRandGenIntRange(AERRandGen *gen, int64_t min,
                                      int64_t max) {
    ErrIf(!gen, AER_NULL_ARG, 0);
    ErrIf(min >= max, AER_BAD_VAL, 0);

    return FoxRandIntRange((FoxPRNG *)&randPRNG, min, max);
}

AER_EXPORT float AERRandGenFloat(AERRandGen *gen) {
    ErrIf(!gen, AER_NULL_ARG, 0.0f);

    return FoxRandFloat((FoxPRNG *)&randPRNG);
}

AER_EXPORT float AERRandGenFloatRange(AERRandGen *gen, float min, float max) {
    ErrIf(!gen, AER_NULL_ARG, 0.0f);
    ErrIf(min >= max, AER_BAD_VAL, 0.0f);

    return FoxRandFloatRange((FoxPRNG *)&randPRNG, min, max);
}

AER_EXPORT double AERRandGenDouble(AERRandGen *gen) {
    ErrIf(!gen, AER_NULL_ARG, 0.0);

    return FoxRandDouble((FoxPRNG *)&randPRNG);
}

AER_EXPORT double AERRandGenDoubleRange(AERRandGen *gen, double min,
                                        double max) {
    ErrIf(!gen, AER_NULL_ARG, 0.0);
    ErrIf(min >= max, AER_BAD_VAL, 0.0);

    return FoxRandDoubleRange((FoxPRNG *)&randPRNG, min, max);
}

AER_EXPORT bool AERRandGenBool(AERRandGen *gen) {
    ErrIf(!gen, AER_NULL_ARG, false);

    return FoxRandBool((FoxPRNG *)&randPRNG);
}