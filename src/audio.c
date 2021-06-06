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
#include <string.h>

#include "foxutils/arraymacs.h"
#include "foxutils/stringmapmacs.h"

#include "aer/audio.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/hld.h"
#include "internal/log.h"
#include "internal/mod.h"

/* ----- PRIVATE TYPES ----- */

typedef struct SampleInfo {
    char* name;
    int32_t realIdx;
} SampleInfo;

/* ----- PRIVATE GLOBALS ----- */

static FoxMap sampleNames = {0};

static FoxArray sampleIdxToNameLUT = {0};

/* ----- PRIVATE FUNCTIONS ----- */

static void RecordSampleName(const char* name) {
    int32_t virtIdx = (int32_t)FoxArrayMSize(const char*, &sampleIdxToNameLUT);
    size_t strSize = strlen(name);

    *FoxMapMInsert(const char*, int32_t, &sampleNames, name) = virtIdx;
    *FoxArrayMPush(const char*, &sampleIdxToNameLUT) =
        memcpy(malloc(strSize), name, strSize);

    return;
}

/* ----- INTERNAL FUNCTIONS ----- */

void AudioManBuildSampleNameTables(void) {
    size_t numSamples = hldvars.sampleNameTable->size;
    const char** elements = hldvars.sampleNameTable->elements;
    for (uint32_t sampleIdx = 0; sampleIdx < numSamples; sampleIdx++) {
        *FoxMapMInsert(const char*, int32_t, &sampleNames,
                       elements[sampleIdx]) = sampleIdx;
    }

    return;
}

void AudioManConstructor(void) {
    LogInfo("Initializing audio module...");

    FoxStringMapMInit(int32_t, &sampleNames);
    FoxArrayMInit(const char*, &sampleIdxToNameLUT);

    LogInfo("Done initializing audio module.");
    return;
}

void AudioManDestructor(void) {
    LogInfo("Deinitializing audio module...");

    FoxMapMDeinit(const char*, int32_t, &sampleNames);
    sampleNames = (FoxMap){0};

    size_t numSamples = FoxArrayMSize(const char*, &sampleIdxToNameLUT);
    for (size_t idx = 0; idx < numSamples; idx++) {
        free(FoxArrayMPop(const char*, &sampleIdxToNameLUT));
    }
    FoxArrayMDeinit(const char*, &sampleIdxToNameLUT);
    sampleIdxToNameLUT = (FoxArray){0};

    LogInfo("Done deinitializing audio module.");
    return;
}

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT int32_t AERAudioSampleRegister(const char* filename,
                                          const char* name) {
#define errRet AER_SAMPLE_NULL
    EnsureArg(filename);
    EnsureArg(name);
    LogInfo("Registering audio sample \"%s\" for mod \"%s\"...", filename,
            ModManGetMod(ModManPeekContext())->name);
    EnsureStageStrict(STAGE_SAMPLE_REG);

    int32_t sampleIdx =
        hldfuncs.actionAudioCreateStream(CoreGetAbsAssetPath(filename)) -
        hldconsts.firstStreamIdx + hldvars.sampleTable->size;
    Ensure(HLDSampleLookup(sampleIdx), AER_BAD_FILE);

    /* The engine expects a freeable (dynamically allocated) string for name. */
    char* tmpName = malloc(strlen(name) + 1);
    assert(tmpName);
    ((const char**)hldvars.sampleNameTable->elements)[sampleIdx] =
        strcpy(tmpName, name);

    LogInfo("Successfully registered audio sample to index %i.", sampleIdx);

    Ok(sampleIdx);
#undef errRet
}

AER_EXPORT size_t AERAudioSampleGetNumRegistered(void) {
#define errRet 0
    EnsureStage(STAGE_SAMPLE_REG);

    Ok(hldvars.sampleTable->size);
#undef errRet
}

AER_EXPORT int32_t AERAudioSampleGetByName(const char* name) {
#define errRet AER_SAMPLE_NULL
    EnsureStage(STAGE_SAMPLE_REG);
    EnsureArg(name);

    int32_t* sampleIdx = FoxMapMIndex(const char*, int32_t, &sampleNames, name);
    EnsureLookup(sampleIdx);

    Ok(*sampleIdx);
#undef errRet
}

AER_EXPORT const char* AERAudioSampleGetName(int32_t sampleIdx) {
#define errRet NULL
    EnsureStage(STAGE_ACTION);
    EnsureLookup(HLDSampleLookup(sampleIdx));

    Ok(((const char**)hldvars.sampleNameTable->elements)[sampleIdx]);
#undef errRet
}

AER_EXPORT int32_t AERAudioSamplePlay(int32_t sampleIdx,
                                      double priority,
                                      bool loop) {
#define errRet -1
    EnsureStage(STAGE_ACTION);
    EnsureLookup(HLDSampleLookup(sampleIdx));

    int32_t playbackId =
        hldfuncs.actionAudioPlaySound(sampleIdx, priority, loop);
    assert(playbackId >= FIRST_PLAYBACK_ID);

    Ok(playbackId);
#undef errRet
}