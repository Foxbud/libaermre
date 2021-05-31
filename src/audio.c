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
#include <string.h>

#include "foxutils/stringmapmacs.h"

#include "aer/audio.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/hld.h"
#include "internal/log.h"
#include "internal/mod.h"

/* ----- PRIVATE GLOBALS ----- */

static FoxMap sampleNames = {0};

static size_t streamNameTableCap = 0;

static char** streamNameTable = NULL;

/* ----- INTERNAL FUNCTIONS ----- */

void AudioManBuildSampleNameTable(void) {
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

    LogInfo("Done initializing audio module.");
    return;
}

void AudioManDestructor(void) {
    LogInfo("Deinitializing audio module...");

    /* Deinitialize name table. */
    FoxMapMDeinit(const char*, int32_t, &sampleNames);
    sampleNames = (FoxMap){0};

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