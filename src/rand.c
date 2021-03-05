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
#define errRet 0
    EnsureMaxExc(min, max);

    return FoxRandUIntRange((FoxPRNG *)&randPRNG, min, max);
#undef errRet
}

AER_EXPORT int64_t AERRandInt(void) { return FoxRandInt((FoxPRNG *)&randPRNG); }

AER_EXPORT int64_t AERRandIntRange(int64_t min, int64_t max) {
#define errRet 0
    EnsureMaxExc(min, max);

    return FoxRandIntRange((FoxPRNG *)&randPRNG, min, max);
#undef errRet
}

AER_EXPORT float AERRandFloat(void) {
    return FoxRandFloat((FoxPRNG *)&randPRNG);
}

AER_EXPORT float AERRandFloatRange(float min, float max) {
#define errRet 0.0f
    EnsureMaxExc(min, max);

    return FoxRandFloatRange((FoxPRNG *)&randPRNG, min, max);
#undef errRet
}

AER_EXPORT double AERRandDouble(void) {
    return FoxRandDouble((FoxPRNG *)&randPRNG);
}

AER_EXPORT double AERRandDoubleRange(double min, double max) {
#define errRet 0.0
    EnsureMaxExc(min, max);

    return FoxRandDoubleRange((FoxPRNG *)&randPRNG, min, max);
#undef errRet
}

AER_EXPORT bool AERRandBool(void) { return FoxRandBool((FoxPRNG *)&randPRNG); }

AER_EXPORT AERRandGen *AERRandGenNew(uint64_t seed) {
    return FoxXoshiro256SSNew(seed);
}

AER_EXPORT void AERRandGenFree(AERRandGen *gen) {
#define errRet
    EnsureArg(gen);

    FoxXoshiro256SSFree(gen);

    return;
#undef errRet
}

AER_EXPORT void AERRandGenSeed(AERRandGen *gen, uint64_t seed) {
#define errRet
    EnsureArg(gen);

    FoxXoshiro256SSSeed(gen, seed);

    return;
#undef errRet
}

AER_EXPORT uint64_t AERRandGenUInt(AERRandGen *gen) {
#define errRet 0
    EnsureArg(gen);

    return FoxRandUInt((FoxPRNG *)&gen);
#undef errRet
}

AER_EXPORT uint64_t AERRandGenUIntRange(AERRandGen *gen, uint64_t min,
                                        uint64_t max) {
#define errRet 0
    EnsureArg(gen);
    EnsureMaxExc(min, max);

    return FoxRandUIntRange((FoxPRNG *)&gen, min, max);
#undef errRet
}

AER_EXPORT int64_t AERRandGenInt(AERRandGen *gen) {
#define errRet 0
    EnsureArg(gen);

    return FoxRandFloat((FoxPRNG *)&gen);
#undef errRet
}

AER_EXPORT int64_t AERRandGenIntRange(AERRandGen *gen, int64_t min,
                                      int64_t max) {
#define errRet 0
    EnsureArg(gen);
    EnsureMaxExc(min, max);

    return FoxRandIntRange((FoxPRNG *)&gen, min, max);
#undef errRet
}

AER_EXPORT float AERRandGenFloat(AERRandGen *gen) {
#define errRet 0.0f
    EnsureArg(gen);

    return FoxRandFloat((FoxPRNG *)&gen);
#undef errRet
}

AER_EXPORT float AERRandGenFloatRange(AERRandGen *gen, float min, float max) {
#define errRet 0.0f
    EnsureArg(gen);
    EnsureMaxExc(min, max);

    return FoxRandFloatRange((FoxPRNG *)&gen, min, max);
#undef errRet
}

AER_EXPORT double AERRandGenDouble(AERRandGen *gen) {
#define errRet 0.0
    EnsureArg(gen);

    return FoxRandDouble((FoxPRNG *)&gen);
#undef errRet
}

AER_EXPORT double AERRandGenDoubleRange(AERRandGen *gen, double min,
                                        double max) {
#define errRet 0.0
    EnsureArg(gen);
    EnsureMaxExc(min, max);

    return FoxRandDoubleRange((FoxPRNG *)&gen, min, max);
#undef errRet
}

AER_EXPORT bool AERRandGenBool(AERRandGen *gen) {
#define errRet false
    EnsureArg(gen);

    return FoxRandBool((FoxPRNG *)&gen);
#undef errRet
}