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
#ifndef INTERNAL_EVENTTRAP_H
#define INTERNAL_EVENTTRAP_H

#include "foxutils/array.h"

#include "aer/eventtrap.h"
#include "internal/hld.h"
#include "internal/modman.h"

/* ----- INTERNAL TYPES ----- */

typedef struct EventTrap {
  FoxArray modListeners;
  void (*origListener)(HLDInstance *target, HLDInstance *other);
  HLDEventType eventType;
} EventTrap;

typedef struct EventTrapIter {
  AEREventTrapIter base;
  EventTrap *trap;
  uint32_t nextIdx;
} EventTrapIter;

/* ----- INTERNAL FUNCTIONS ----- */

void EventTrapInit(EventTrap *trap, HLDEventType eventType,
                   void (*origListener)(HLDInstance *target,
                                        HLDInstance *other));

void EventTrapDeinit(EventTrap *trap);

void EventTrapAddListener(EventTrap *trap, ModListener listener);

void EventTrapIterInit(EventTrapIter *iter, EventTrap *trap);

void EventTrapIterDeinit(EventTrapIter *iter);

bool EventTrapIterNext(EventTrapIter *iter, HLDInstance *target,
                       HLDInstance *other);

#endif /* INTERNAL_EVENTTRAP_H */
