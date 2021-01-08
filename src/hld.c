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

/* Sanity check to make sure all engine variables are as expected. */
void HLDVariablesCheck(HLDVariables *hldvars) {
  CheckVar(hldvars->numSteps);

  CheckVar(hldvars->keysPressedTable);
  CheckVar(hldvars->keysHeldTable);
  CheckVar(hldvars->keysReleasedTable);

  CheckVar(hldvars->mouseButtonsPressedTable);
  CheckVar(hldvars->mouseButtonsHeldTable);
  CheckVar(hldvars->mouseButtonsReleasedTable);

  CheckVar(hldvars->mousePosX);
  CheckVar(hldvars->mousePosY);

  CheckVar(hldvars->roomTable);
  CheckVar(hldvars->roomTable->elements);
  CheckVar(hldvars->roomTable->size == 0x114);

  CheckVar(hldvars->roomIndexCurrent);

  CheckVar(hldvars->roomCurrent);
  CheckVar(*hldvars->roomCurrent);

  CheckVar(hldvars->spriteTable);
  CheckVar(hldvars->spriteTable->elements);
  CheckVar(hldvars->spriteTable->size == 0xd2b);

  CheckVar(hldvars->objectTableHandle);
  CheckVar(*hldvars->objectTableHandle);
  CheckVar((*hldvars->objectTableHandle)->slots);
  CheckVar((*hldvars->objectTableHandle)->numItems == 0x1fe);
  CheckVar((*hldvars->objectTableHandle)->keyMask == 0x1ff);

  CheckVar(hldvars->instanceTable);
  CheckVar(hldvars->instanceTable->slots);

  CheckVar(hldvars->instanceLocalTable);
  CheckVar(hldvars->instanceLocalTable->elements);
  CheckVar(hldvars->instanceLocalTable->size == 0xdf4);

  CheckVar(hldvars->alarmEventSubscriberCounts);
  CheckVar(hldvars->alarmEventSubscribers);

  CheckVar(hldvars->stepEventSubscriberCounts);
  CheckVar(hldvars->stepEventSubscribers);

  CheckVar(hldvars->eventClass);
  CheckVar(hldvars->eventWrapperClass);
  CheckVar(hldvars->unknownEventAddress);

  return;
}
