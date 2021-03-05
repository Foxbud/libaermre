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

/* ----- PRIVATE CONSTANTS ----- */

static const char *ABS_ASSET_PATH_FMT = "assets/mod/%s/%s";

/* ----- PRIVATE GLOBALS ----- */

static bool gamePaused = false;

static char assetPathBuf[8 * 1024];

/* ----- INTERNAL GLOBALS ----- */

CoreStage stage = STAGE_INIT;

/* ----- INTERNAL FUNCTIONS ----- */

const char *CoreGetAbsAssetPath(const char *relAssetPath) {
    assert(relAssetPath);
    assert(ModManHasContext());

    snprintf(assetPathBuf, sizeof(assetPathBuf), ABS_ASSET_PATH_FMT,
             ModManGetMod(ModManPeekContext())->name, relAssetPath);

    return assetPathBuf;
}

__attribute__((constructor)) static void CoreConstructor(void) {
    LogInfo("Action-Event-Response (AER) Mod Runtime Environment (MRE)");

    ConfConstructor();
    OptionConstructor();
    RandConstructor();
    EventManConstructor();
    ObjectManConstructor();
    InstanceManConstructor();

    return;
}

__attribute__((destructor)) static void CoreDestructor(void) {
    InstanceManDestructor();

    SaveManDestructor();
    ModManDestructor();
    ObjectManDestructor();
    EventManDestructor();
    RandDestructor();
    OptionDestructor();
    ConfDestructor();

    return;
}

/* ----- UNLISTED FUNCTIONS ----- */

AER_EXPORT void AERHookInit(HLDVariables vars, HLDFunctions funcs) {
    HLDRecordEngineRefs(&vars, &funcs);

    InstanceManRecordHLDLocals();

    ModManConstructor();
    SaveManConstructor();
    size_t numMods = ModManGetNumMods();

    /* Register sprites. */
    stage = STAGE_SPRITE_REG;
    LogInfo("Registering mod sprites...");
    /*
     * Reverse order so that higher-priority mods' sprite replacements take
     * precedence over those of lower-priority mods.
     */
    for (uint32_t idx = 0; idx < numMods; idx++) {
        int32_t modIdx = (int32_t)(numMods - idx - 1);
        Mod *mod = ModManGetMod(modIdx);
        if (mod->registerSprites) {
            ModManPushContext(modIdx);
            mod->registerSprites();
            ModManPopContext();
        }
    }
    LogInfo("Done.");

    /* Register fonts. */
    stage = STAGE_FONT_REG;
    LogInfo("Registering mod fonts...");
    for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
        Mod *mod = ModManGetMod(modIdx);
        if (mod->registerFonts) {
            ModManPushContext(modIdx);
            mod->registerFonts();
            ModManPopContext();
        }
    }
    LogInfo("Done.");

    /* Build object name table. */
    ObjectManBuildNameTable();

    /* Register objects. */
    stage = STAGE_OBJECT_REG;
    LogInfo("Registering mod objects...");
    for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
        Mod *mod = ModManGetMod(modIdx);
        if (mod->registerObjects) {
            ModManPushContext(modIdx);
            mod->registerObjects();
            ModManPopContext();
        }
    }
    LogInfo("Done.");

    /* Build object inheritance trees and mask event subscribers. */
    ObjectManBuildInheritanceTrees();
    EventManMaskSubscriptionArrays();

    /* Register listeners. */
    stage = STAGE_LISTENER_REG;
    LogInfo("Registering mod event listeners...");
    for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
        Mod *mod = ModManGetMod(modIdx);
        if (mod->registerObjectListeners) {
            ModManPushContext(modIdx);
            mod->registerObjectListeners();
            ModManPopContext();
        }
    }
    LogInfo("Done.");

    /* Sort event subscribers. */
    EventManSortSubscriptionArrays();

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

    /* Check if room changed. */
    int32_t roomIdxCur = *hldvars.roomIndexCurrent;
    if (roomIdxCur != roomIndexPrevious) {
        /* Prune orphaned mod instance locals. */
        InstanceManPruneModLocals();

        /* Call mod room change listeners. */
        ModManExecuteRoomChangeListeners(roomIdxCur, roomIndexPrevious);

        roomIndexPrevious = roomIdxCur;
    }

    /* Call game step listeners. */
    ModManExecuteGameStepListeners();

    return;
}

AER_EXPORT void AERHookEvent(HLDObject *targetObject, HLDEventType eventType,
                             int32_t eventNum) {
    currentEvent = (EventKey){
        .type = eventType, .num = eventNum, .objIdx = targetObject->index};

    return;
}

AER_EXPORT void AERHookLoadData(HLDPrimitive *dataMapId) {
    SaveManLoadData(dataMapId);

    ModManExecuteGameLoadListeners(SaveManGetCurrentSlot());

    return;
}

AER_EXPORT void AERHookSaveData(HLDPrimitive *dataMapId) {
    ModManExecuteGameSaveListeners(SaveManGetCurrentSlot());

    SaveManSaveData(dataMapId);

    return;
}

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT uint32_t AERGetNumSteps(void) {
#define errRet 0
    EnsureStage(STAGE_ACTION);

    return *hldvars.numSteps;
#undef errRet
}

AER_EXPORT bool AERGetPaused(void) {
#define errRet 0
    EnsureStage(STAGE_ACTION);

    return gamePaused;
#undef errRet
}