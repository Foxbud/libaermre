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

/* ----- PRIVATE FUNCTIONS ----- */

static inline void MemSwap(size_t size,
                           uint8_t* restrict bufA,
                           uint8_t* restrict bufB) {
    for (size_t idx = 0; idx < size; idx++) {
        /*
         * Temporary variable method of swapping is technically faster than XOR
         * swapping in this situation due to memory accesses.
         */
        uint8_t tmp = bufA[idx];
        bufA[idx] = bufB[idx];
        bufB[idx] = tmp;
    }

    return;
}

static inline void Shuffle(FoxPRNG* prng,
                           size_t elemSize,
                           size_t bufSize,
                           void* elemBuf) {
    for (size_t idx = bufSize - 1; idx > 0; idx--) {
        size_t newIdx = FoxRandUIntRange(prng, 0, idx + 1);
        if (newIdx != idx) {
            MemSwap(elemSize, elemBuf + elemSize * idx,
                    elemBuf + elemSize * newIdx);
        }
    }

    return;
}

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
    Ok(FoxRandUInt((FoxPRNG*)&randPRNG));
}

AER_EXPORT uint64_t AERRandUIntRange(uint64_t min, uint64_t max) {
#define errRet 0
    EnsureMaxExc(min, max);

    Ok(FoxRandUIntRange((FoxPRNG*)&randPRNG, min, max));
#undef errRet
}

AER_EXPORT int64_t AERRandInt(void) {
    Ok(FoxRandInt((FoxPRNG*)&randPRNG));
}

AER_EXPORT int64_t AERRandIntRange(int64_t min, int64_t max) {
#define errRet 0
    EnsureMaxExc(min, max);

    Ok(FoxRandIntRange((FoxPRNG*)&randPRNG, min, max));
#undef errRet
}

AER_EXPORT float AERRandFloat(void) {
    Ok(FoxRandFloat((FoxPRNG*)&randPRNG));
}

AER_EXPORT float AERRandFloatRange(float min, float max) {
#define errRet 0.0f
    EnsureMaxExc(min, max);

    Ok(FoxRandFloatRange((FoxPRNG*)&randPRNG, min, max));
#undef errRet
}

AER_EXPORT double AERRandDouble(void) {
    Ok(FoxRandDouble((FoxPRNG*)&randPRNG));
}

AER_EXPORT double AERRandDoubleRange(double min, double max) {
#define errRet 0.0
    EnsureMaxExc(min, max);

    Ok(FoxRandDoubleRange((FoxPRNG*)&randPRNG, min, max));
#undef errRet
}

AER_EXPORT bool AERRandBool(void) {
    Ok(FoxRandBool((FoxPRNG*)&randPRNG));
}

AER_EXPORT void AERRandShuffle(size_t elemSize, size_t bufSize, void* elemBuf) {
#define errRet
    EnsureArg(elemBuf);
    EnsureMinExc(elemSize, 0);

    Shuffle((FoxPRNG*)&randPRNG, elemSize, bufSize, elemBuf);

    Ok();
#undef errRet
}

AER_EXPORT AERRandGen* AERRandGenNew(uint64_t seed) {
    Ok(FoxXoshiro256SSNew(seed));
}

AER_EXPORT void AERRandGenFree(AERRandGen* gen) {
#define errRet
    EnsureArg(gen);

    FoxXoshiro256SSFree(gen);

    Ok();
#undef errRet
}

AER_EXPORT void AERRandGenSeed(AERRandGen* gen, uint64_t seed) {
#define errRet
    EnsureArg(gen);

    FoxXoshiro256SSSeed(gen, seed);

    Ok();
#undef errRet
}

AER_EXPORT uint64_t AERRandGenUInt(AERRandGen* gen) {
#define errRet 0
    EnsureArg(gen);

    Ok(FoxRandUInt((FoxPRNG*)gen));
#undef errRet
}

AER_EXPORT uint64_t AERRandGenUIntRange(AERRandGen* gen,
                                        uint64_t min,
                                        uint64_t max) {
#define errRet 0
    EnsureArg(gen);
    EnsureMaxExc(min, max);

    Ok(FoxRandUIntRange((FoxPRNG*)gen, min, max));
#undef errRet
}

AER_EXPORT int64_t AERRandGenInt(AERRandGen* gen) {
#define errRet 0
    EnsureArg(gen);

    Ok(FoxRandFloat((FoxPRNG*)gen));
#undef errRet
}

AER_EXPORT int64_t AERRandGenIntRange(AERRandGen* gen,
                                      int64_t min,
                                      int64_t max) {
#define errRet 0
    EnsureArg(gen);
    EnsureMaxExc(min, max);

    Ok(FoxRandIntRange((FoxPRNG*)gen, min, max));
#undef errRet
}

AER_EXPORT float AERRandGenFloat(AERRandGen* gen) {
#define errRet 0.0f
    EnsureArg(gen);

    Ok(FoxRandFloat((FoxPRNG*)gen));
#undef errRet
}

AER_EXPORT float AERRandGenFloatRange(AERRandGen* gen, float min, float max) {
#define errRet 0.0f
    EnsureArg(gen);
    EnsureMaxExc(min, max);

    Ok(FoxRandFloatRange((FoxPRNG*)gen, min, max));
#undef errRet
}

AER_EXPORT double AERRandGenDouble(AERRandGen* gen) {
#define errRet 0.0
    EnsureArg(gen);

    Ok(FoxRandDouble((FoxPRNG*)gen));
#undef errRet
}

AER_EXPORT double AERRandGenDoubleRange(AERRandGen* gen,
                                        double min,
                                        double max) {
#define errRet 0.0
    EnsureArg(gen);
    EnsureMaxExc(min, max);

    Ok(FoxRandDoubleRange((FoxPRNG*)gen, min, max));
#undef errRet
}

AER_EXPORT bool AERRandGenBool(AERRandGen* gen) {
#define errRet false
    EnsureArg(gen);

    Ok(FoxRandBool((FoxPRNG*)gen));
#undef errRet
}

AER_EXPORT void AERRandGenShuffle(AERRandGen* gen,
                                  size_t elemSize,
                                  size_t bufSize,
                                  void* elemBuf) {
#define errRet
    EnsureArg(gen);
    EnsureArg(elemBuf);
    EnsureMinExc(elemSize, 0);

    Shuffle((FoxPRNG*)gen, elemSize, bufSize, elemBuf);

    Ok();
#undef errRet
}