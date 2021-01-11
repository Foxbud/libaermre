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
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "foxutils/arraymacs.h"
#include "foxutils/mapmacs.h"
#include "foxutils/stringmapmacs.h"

#include "aer/mre.h"
#include "internal/confman.h"
#include "internal/confvars.h"
#include "internal/err.h"
#include "internal/event.h"
#include "internal/export.h"
#include "internal/instance.h"
#include "internal/log.h"
#include "internal/mod.h"
#include "internal/mre.h"
#include "internal/object.h"
#include "internal/rand.h"

/* ----- PRIVATE CONSTANTS ----- */

static const char *ABS_ASSET_PATH_FMT = "assets/mod/%s/%s";

/* ----- PRIVATE GLOBALS ----- */

static char assetPathBuf[1024];

/* ----- INTERNAL GLOBALS ----- */

AERMRE mre = {0};

/* ----- PRIVATE FUNCTIONS ----- */

static void BuildInstanceLocals(void) {
  size_t numLocals = hldvars.instanceLocalTable->size;
  const char **names = hldvars.instanceLocalTable->elements;
  for (uint32_t idx = 0; idx < numLocals; idx++) {
    *FoxMapMInsert(const char *, int32_t, mre.instLocals, names[idx]) = idx;
  }

  return;
}

static void InitMRE(HLDVariables varRefs, HLDFunctions funcRefs) {
  LogInfo("Initializing mod runtime environment...");
  hldvars = varRefs;
  hldfuncs = funcRefs;

  mre = (AERMRE){.roomIndexPrevious = 0,
                 .instLocals = FoxStringMapMNew(int32_t),
                 .stage = STAGE_INIT};

  BuildInstanceLocals();

  LogInfo("Done initializing mod runtime environment.");

  return;
}

static void InitMods(void) {
  size_t numMods = ModManGetNumMods();

  /* Register sprites. */
  mre.stage = STAGE_SPRITE_REG;
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

  /* Register objects. */
  mre.stage = STAGE_OBJECT_REG;
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
  mre.stage = STAGE_LISTENER_REG;
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

  mre.stage = STAGE_ACTION;

  return;
}

/* ----- INTERNAL FUNCTIONS ----- */

const char *MREGetAbsAssetPath(const char *relAssetPath) {
  assert(relAssetPath);
  assert(ModManHasContext());

  snprintf(assetPathBuf, sizeof(assetPathBuf), ABS_ASSET_PATH_FMT,
           ModManGetMod(ModManPeekContext())->name, relAssetPath);

  return assetPathBuf;
}

/* ----- UNLISTED FUNCTIONS ----- */

AER_EXPORT void AERHookInit(HLDVariables varRefs, HLDFunctions funcRefs) {
  LogInfo("Checking engine variables...");
  HLDVariablesCheck(&varRefs);
  LogInfo("Done checking engine variables.");

  InitMRE(varRefs, funcRefs);
  ModManConstructor();
  InitMods();

  return;
}

AER_EXPORT void AERHookStep(void) {
  /* Check if room changed. */
  int32_t roomIdxCur = *hldvars.roomIndexCurrent;
  int32_t roomIdxPrev = mre.roomIndexPrevious;
  if (roomIdxCur != roomIdxPrev) {
    mre.roomIndexPrevious = roomIdxCur;

    /* Prune orphaned mod instance locals. */
    InstanceManPruneModLocals();

    /* Call room change listeners. */
    ModManExecuteRoomChangeListeners(roomIdxCur, roomIdxPrev);
  }

  /* Call room step listeners. */
  ModManExecuteRoomStepListeners();

  return;
}

AER_EXPORT void AERHookEvent(HLDObject *targetObject, HLDEventType eventType,
                             int32_t eventNum) {
  currentEvent = (EventKey){
      .type = eventType, .num = eventNum, .objIdx = targetObject->index};

  return;
}

/* ----- LIBRARY MANAGEMENT ----- */

__attribute__((constructor)) void AERConstructor(void) {
  LogInfo("Action-Event-Response (AER) Mod Runtime Environment (MRE)");

  ConfManConstructor();
  ConfVarsConstructor();
  RandConstructor();
  EventManConstructor();
  ObjectManConstructor();
  InstanceManConstructor();

  return;
}

__attribute__((destructor)) void AERDestructor(void) {
  ModManDestructor();

  LogInfo("Deinitializing mod runtime environment...");

  FoxMapMFree(const char *, int32_t, mre.instLocals);
  mre.instLocals = NULL;

  LogInfo("Done deinitializing mod runtime environment.");

  InstanceManDestructor();
  ObjectManDestructor();
  EventManDestructor();
  RandDestructor();
  ConfVarsDestructor();
  ConfManDestructor();

  return;
}

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT uint32_t AERGetNumSteps(void) {
  ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, 0);

  return *hldvars.numSteps;
}

AER_EXPORT const bool *AERGetKeysPressed(void) {
  ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

  return *hldvars.keysPressedTable;
}

AER_EXPORT const bool *AERGetKeysHeld(void) {
  ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

  return *hldvars.keysHeldTable;
}

AER_EXPORT const bool *AERGetKeysReleased(void) {
  ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

  return *hldvars.keysReleasedTable;
}

AER_EXPORT const bool *AERGetMouseButtonsPressed(void) {
  ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

  return *hldvars.mouseButtonsPressedTable;
}

AER_EXPORT const bool *AERGetMouseButtonsHeld(void) {
  ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

  return *hldvars.mouseButtonsHeldTable;
}

AER_EXPORT const bool *AERGetMouseButtonsReleased(void) {
  ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

  return *hldvars.mouseButtonsReleasedTable;
}

AER_EXPORT void AERGetMousePosition(uint32_t *x, uint32_t *y) {
  ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
  ErrIf(!(x || y), AER_NULL_ARG);

  if (x)
    *x = *hldvars.mousePosX;
  if (y)
    *y = *hldvars.mousePosY;

  return;
}
