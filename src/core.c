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

#include "aer/core.h"
#include "aer/object.h"
#include "aer/room.h"
#include "internal/conf.h"
#include "internal/core.h"
#include "internal/err.h"
#include "internal/event.h"
#include "internal/export.h"
#include "internal/hld.h"
#include "internal/input.h"
#include "internal/instance.h"
#include "internal/log.h"
#include "internal/mod.h"
#include "internal/object.h"
#include "internal/option.h"
#include "internal/rand.h"
#include "internal/room.h"
#include "internal/save.h"
#include "internal/sprite.h"

/* ----- PRIVATE CONSTANTS ----- */

static const char* ABS_ASSET_PATH_FMT = "assets/mod/%s/%s";

/* ----- PRIVATE GLOBALS ----- */

static bool gamePaused = false;

static char assetPathBuf[8 * 1024];

/* ----- INTERNAL GLOBALS ----- */

CoreStage stage = STAGE_INIT;

/* ----- INTERNAL FUNCTIONS ----- */

const char* CoreGetAbsAssetPath(const char* modName, const char* relAssetPath) {
    assert(modName);
    assert(relAssetPath);

    snprintf(assetPathBuf, sizeof(assetPathBuf), ABS_ASSET_PATH_FMT, modName,
             relAssetPath);

    return assetPathBuf;
}

__attribute__((constructor)) static void CoreConstructor(void) {
    LogInfo("Action-Event-Response (AER) Mod Runtime Environment (MRE)");

    ModManConstructor();
    ConfConstructor();
    OptionConstructor();
    RandConstructor();
    EventManConstructor();
    SpriteManConstructor();
    ObjectManConstructor();
    RoomManConstructor();
    InstanceManConstructor();

    return;
}

__attribute__((destructor)) static void CoreDestructor(void) {
    InstanceManDestructor();
    SaveManDestructor();
    ModManUnloadMods();
    RoomManDestructor();
    ObjectManDestructor();
    SpriteManDestructor();
    EventManDestructor();
    RandDestructor();
    OptionDestructor();
    ConfDestructor();
    ModManDestructor();

    return;
}

/* ----- UNLISTED FUNCTIONS ----- */

AER_EXPORT void AERHookInit(HLDVariables vars, HLDFunctions funcs) {
    HLDRecordEngineRefs(&vars, &funcs);

    InstanceManRecordHLDLocals();
    ModManLoadMods();
    size_t numMods = ModManGetNumMods();
    SaveManConstructor();

    /* Build sprite name table. */
    SpriteManBuildNameTable();

    /* Register sprites. */
    stage = STAGE_SPRITE_REG;
    LogInfo("Registering mod sprites...");
    /*
     * Reverse order so that higher-priority mods' sprite replacements take
     * precedence over those of lower-priority mods.
     */
    for (uint32_t idx = 0; idx < numMods; idx++) {
        int32_t modIdx = (int32_t)(numMods - idx - 1);
        Mod* mod = ModManGetMod(modIdx);
        if (mod->registerSprites) {
            mod->registerSprites();
        }
    }
    LogInfo("Done.");

    /* Register fonts. */
    stage = STAGE_FONT_REG;
    LogInfo("Registering mod fonts...");
    for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
        Mod* mod = ModManGetMod(modIdx);
        if (mod->registerFonts) {
            mod->registerFonts();
        }
    }
    LogInfo("Done.");

    /* Build object name table. */
    ObjectManBuildNameTable();

    /* Register objects. */
    stage = STAGE_OBJECT_REG;
    LogInfo("Registering mod objects...");
    for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
        Mod* mod = ModManGetMod(modIdx);
        if (mod->registerObjects) {
            mod->registerObjects();
        }
    }
    LogInfo("Done.");

    /* Build object inheritance trees, record draw event targets, and mask event
     * subscribers. */
    ObjectManBuildInheritanceTrees();
    EventManRecordDrawTargets();
    EventManMaskSubscriptionArrays();

    /* Register listeners. */
    stage = STAGE_LISTENER_REG;
    LogInfo("Registering mod event listeners...");
    for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
        Mod* mod = ModManGetMod(modIdx);
        if (mod->registerObjectListeners) {
            mod->registerObjectListeners();
        }
    }
    LogInfo("Done.");

    /* Sort event subscribers. */
    EventManSortSubscriptionArrays();

    /* Build room name table. */
    RoomManBuildNameTable();

    stage = STAGE_ACTION;

    return;
}

AER_EXPORT void AERHookStep(void) {
    /* Record user input. */
    InputManRecordUserInput();

    /* Check if game pause state changed. */
    bool paused = HLDObjectLookup(AER_OBJECT_MENUS)->numInstances > 0;
    if (paused != gamePaused) {
        gamePaused = paused;

        /* Call game pause listeners. */
        ModManExecuteGamePauseListeners(paused);
    }

    /* Call game step listeners. */
    ModManExecuteGameStepListeners();

    return;
}

AER_EXPORT void AERHookEvent(HLDObject* targetObject,
                             HLDEventType eventType,
                             int32_t eventNum) {
    currentEvent = (EventKey){
        .type = eventType,
        .num = eventNum,
        .objIdx = targetObject->index,
    };

    return;
}

AER_EXPORT void AERHookLoadData(HLDPrimitive* dataMapId) {
    SaveManLoadData(dataMapId);

    ModManExecuteGameLoadListeners(SaveManGetCurrentSlot());

    return;
}

AER_EXPORT void AERHookSaveData(HLDPrimitive* dataMapId) {
    ModManExecuteGameSaveListeners(SaveManGetCurrentSlot());

    SaveManSaveData(dataMapId);

    return;
}

AER_EXPORT void AERHookRoomStart(HLDEventType type, int32_t num) {
    (void)type;
    (void)num;

    /* Skip first room change. */
    if (*hldvars.roomIndexCurrent == AER_ROOM__INIT)
        return;

    /* Prune orphaned mod instance locals. */
    InstanceManPruneModLocals();

    /* Record that room change is done. */
    int32_t roomIndexPrev = roomIndexAux;
    roomIndexAux = AER_ROOM_NULL;

    /* Call room start listeners. */
    ModManExecuteRoomStartListeners(*hldvars.roomIndexCurrent, roomIndexPrev);

    return;
}

AER_EXPORT void AERHookRoomEnd(HLDEventType type, int32_t num) {
    (void)type;
    (void)num;

    /* Call room end listeners. */
    ModManExecuteRoomEndListeners(roomIndexAux, *hldvars.roomIndexCurrent);

    /* Swap room indicies. */
    roomIndexAux = *hldvars.roomIndexCurrent;

    return;
}

AER_EXPORT void AERHookRoomChange(int32_t newRoomIdx) {
    /* Skip first room change. */
    if (*hldvars.roomIndexCurrent == AER_ROOM__INIT)
        return;

    /* Record that room change is happening. */
    roomIndexAux = newRoomIdx;

    return;
}

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT uint32_t AERGetNumSteps(void) {
#define errRet 0
    EnsureStage(STAGE_ACTION);

    Ok(*hldvars.numSteps);
#undef errRet
}

AER_EXPORT double AERGetDeltaTime(void) {
#define errRet 0.0
    EnsureStage(STAGE_ACTION);

    /* Convert microseconds to seconds. */
    Ok(*hldvars.deltaTime * 0.000001);
#undef errRet
}

AER_EXPORT bool AERGetPaused(void) {
#define errRet 0
    EnsureStage(STAGE_ACTION);

    Ok(gamePaused);
#undef errRet
}