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
#include <stdlib.h>
#include <string.h>

#include "aer/sprite.h"
#include "internal/core.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/hld.h"
#include "internal/log.h"
#include "internal/mod.h"

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT int32_t AERSpriteRegister(const char *name, const char *filename,
                                     size_t numFrames, uint32_t origX,
                                     uint32_t origY) {
#define errRet AER_SPRITE_NULL
    EnsureArg(name);
    LogInfo("Registering sprite \"%s\" for mod \"%s\"...", name,
            ModManGetMod(ModManPeekContext())->name);
    EnsureStageStrict(STAGE_SPRITE_REG);
    EnsureArg(filename);

    int32_t spriteIdx = hldfuncs.actionSpriteAdd(
        CoreGetAbsAssetPath(filename), numFrames, 0, 0, 0, 0, origX, origY);
    HLDSprite *sprite = HLDSpriteLookup(spriteIdx);
    Ensure(sprite, AER_BAD_FILE);

    /* The engine expects a freeable (dynamically allocated) string for name. */
    char *tmpName = malloc(strlen(name) + 1);
    Ensure(tmpName, AER_OUT_OF_MEM);
    sprite->name = strcpy(tmpName, name);

    LogInfo("Successfully registered sprite to index %i.", spriteIdx);

    return spriteIdx;
#undef errRet
}

AER_EXPORT void AERSpriteReplace(int32_t spriteIdx, const char *filename,
                                 size_t numFrames, uint32_t origX,
                                 uint32_t origY) {
#define errRet
    HLDSprite *oldSprite = HLDSpriteLookup(spriteIdx);
    EnsureLookup(oldSprite);
    LogInfo("Replacing sprite \"%s\" for mod \"%s\"...", oldSprite->name,
            ModManGetMod(ModManPeekContext())->name);
    EnsureStageStrict(STAGE_SPRITE_REG);
    EnsureArg(filename);

    hldfuncs.actionSpriteReplace(spriteIdx, CoreGetAbsAssetPath(filename),
                                 numFrames, 0, 0, 0, 0, origX, origY);
    /* TODO Check if replacement was successful. */

    LogInfo("Successfully replaced sprite at index %i.", spriteIdx);

    return;
#undef errRet
}

AER_EXPORT size_t AERSpriteGetNumRegistered(void) {
#define errRet 0
    EnsureStagePast(STAGE_SPRITE_REG);

    return hldvars.spriteTable->size;
#undef errRet
}

AER_EXPORT const char *AERSpriteGetName(int32_t spriteIdx) {
#define errRet NULL
    EnsureStagePast(STAGE_SPRITE_REG);

    HLDSprite *sprite = HLDSpriteLookup(spriteIdx);
    EnsureLookup(sprite);

    return sprite->name;
#undef errRet
}
