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

#include "internal/hld.h"
#include "internal/log.h"

/* ----- PRIVATE MACROS ----- */

#define CheckVar(var)                                                          \
  do {                                                                         \
    if (!(var)) {                                                              \
      LogErr("Engine variable failed consistency check \"%s\".", #var);        \
      abort();                                                                 \
    }                                                                          \
  } while (0)

/* ----- INTERNAL GLOBALS ----- */

HLDVariables hldvars = {0};

HLDFunctions hldfuncs = {0};

/* ----- INTERNAL FUNCTIONS ----- */

HLDSprite *HLDSpriteLookup(int32_t spriteIdx) {
  HLDSprite *result = NULL;

  if (spriteIdx >= 0 && (uint32_t)spriteIdx < hldvars.spriteTable->size) {
    result = ((HLDSprite **)hldvars.spriteTable->elements)[spriteIdx];
  }

  return result;
}

HLDRoom *HLDRoomLookup(int32_t roomIdx) {
  HLDRoom *result = NULL;

  if (roomIdx >= 0 && (uint32_t)roomIdx < hldvars.roomTable->size) {
    result = ((HLDRoom **)hldvars.roomTable->elements)[roomIdx];
  }

  return result;
}

void *HLDOpenHashTableLookup(HLDOpenHashTable *table, int32_t key) {
  assert(table != NULL);
  void *result = NULL;

  if (key >= 0) {
    uint32_t idx = key & table->keyMask;
    HLDOpenHashItem *item = table->slots[idx].first;
    while (item) {
      if (item->key == key) {
        result = item->value;
        break;
      } else {
        item = item->next;
      }
    }
  }

  return result;
}

void *HLDClosedHashTableLookup(HLDClosedHashTable *table, int32_t key) {
  assert(table != NULL);
  void *result = NULL;
  uint32_t keyMask = table->keyMask;

  if (key >= 0) {
    uint32_t origIdx = key & keyMask;
    uint32_t idx = origIdx;
    HLDClosedHashSlot *slot = table->slots + idx;
    while (slot->keyNext) {
      if (slot->key == key) {
        result = slot->value;
        break;
      } else {
        idx = (idx + 1) & keyMask;
        if (idx == origIdx)
          break;
        slot = table->slots + idx;
      }
    }
  }

  return result;
}

HLDEvent *HLDEventNew(HLDNamedFunction *handler) {
  assert(handler);

  HLDEvent *event = malloc(sizeof(HLDEvent));
  assert(event);

  event->classDef = hldvars.eventClass;
  event->eventNext = NULL;
  event->field_8 = 1;
  event->field_C = 1;
  event->field_10 = hldvars.unknownEventAddress;
  event->field_14 = 0;
  event->field_18 = 0;
  event->field_1C = 0;
  event->field_20 = 0;
  event->field_24 = 0;
  event->field_28 = 0;
  event->field_2C = 0;
  event->field_30 = 0;
  event->field_34 = 0;
  event->field_38 = 0;
  event->field_3C = 0;
  event->field_40 = 0;
  event->field_44 = 0;
  event->field_48 = 0;
  event->field_4C = 0;
  event->field_50 = 0;
  event->field_54 = 0;
  event->field_58 = hldvars.unknownEventAddress;
  event->name = handler->name;
  event->handlerIndex = 0;
  event->handler = handler;
  event->field_68 = 0;
  event->field_6C = 0;
  event->field_70 = 0;
  event->field_74 = 0;
  event->field_78 = 0;
  event->field_7C = 0x11;

  return event;
}

HLDEventWrapper *HLDEventWrapperNew(HLDEvent *event) {
  assert(event);

  HLDEventWrapper *wrapper = malloc(sizeof(HLDEventWrapper));
  assert(wrapper);

  wrapper->classDef = hldvars.eventWrapperClass;
  wrapper->event = event;
  wrapper->field_08 = hldvars.unknownEventAddress;
  wrapper->field_0C = 0x81;

  return wrapper;
}

void HLDRecordEngineRefs(HLDVariables *vars, HLDFunctions *funcs) {
  LogInfo("Checking engine variables...");

  CheckVar(vars->numSteps);

  CheckVar(vars->keysPressedTable);
  CheckVar(vars->keysHeldTable);
  CheckVar(vars->keysReleasedTable);

  CheckVar(vars->mouseButtonsPressedTable);
  CheckVar(vars->mouseButtonsHeldTable);
  CheckVar(vars->mouseButtonsReleasedTable);

  CheckVar(vars->mousePosX);
  CheckVar(vars->mousePosY);

  CheckVar(vars->roomTable);
  CheckVar(vars->roomTable->elements);
  CheckVar(vars->roomTable->size == 0x114);

  CheckVar(vars->roomIndexCurrent);

  CheckVar(vars->roomCurrent);
  CheckVar(*vars->roomCurrent);

  CheckVar(vars->spriteTable);
  CheckVar(vars->spriteTable->elements);
  CheckVar(vars->spriteTable->size == 0xd2b);

  CheckVar(vars->objectTableHandle);
  CheckVar(*vars->objectTableHandle);
  CheckVar((*vars->objectTableHandle)->slots);
  CheckVar((*vars->objectTableHandle)->numItems == 0x1fe);
  CheckVar((*vars->objectTableHandle)->keyMask == 0x1ff);

  CheckVar(vars->instanceTable);
  CheckVar(vars->instanceTable->slots);

  CheckVar(vars->instanceLocalTable);
  CheckVar(vars->instanceLocalTable->elements);
  CheckVar(vars->instanceLocalTable->size == 0xdf4);

  CheckVar(vars->alarmEventSubscriberCounts);
  CheckVar(vars->alarmEventSubscribers);

  CheckVar(vars->stepEventSubscriberCounts);
  CheckVar(vars->stepEventSubscribers);

  CheckVar(vars->eventClass);
  CheckVar(vars->eventWrapperClass);
  CheckVar(vars->unknownEventAddress);

  LogInfo("Done checking engine variables.");

  LogInfo("Recording engine references...");

  hldvars = *vars;
  hldfuncs = *funcs;

  LogInfo("Done recording engine references...");
  return;
}
